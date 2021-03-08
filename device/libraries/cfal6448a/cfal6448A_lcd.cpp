//===========================================================================
//
//  Code written for Seeeduino v4.2 runnung at 3.3v
//
//  CRYSTALFONTZ CFAL6448A-066B 64x48 OLED in in 4-wire, 8-bit SPI mode
//
//  ref: https://www.crystalfontz.com/product/cfal6448a066bw
//
//  2017 - 06 - 23 Brent A. Crosby
//===========================================================================
//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//For more information, please refer to <http://unlicense.org/>
//===========================================================================
//#include <SPI.h>
//#include "bitmap.h"
//============================================================================
// The CFAL6448A-066B is a 3.3v device. You need a 3.3v Arduino to operate this
// code properly. We used a seeedunio v4.2 set to 3.3v:
//https://www.crystalfontz.com/product/cfapn15062-seeeduino-arduino-clone-microprocessor
//or
// http://www.seeedstudio.com/item_detail.html?p_id=2517
// 
// LCD SPI & control lines
//   ARD      | Port | LCD
// -----------+------+-------------------------
//  #8/D8     |  PB0 | LCD_RS
//  #9/D9     |  PB1 | LCD_RESET
// #10/D10    |  PB2 | LCD_CS_NOT (or SPI SS)
// #11/D11    |  PB3 | LCD_MOSI   (hardware SPI)
// #12/D12    |  PB4 | not used   (would be MISO)
// #13/D13    |  PB5 | LCD_SCK    (hardware SPI)
//
// GND => BS0 (Selects 4-wire, 8-bit SPI)
// GND => BS1 (Selects 4-wire, 8-bit SPI)
// 3.3v => VBAT
// 3.3v => VDD
//

/*
// ARDUINO IMPLEMENTATION
#define CLR_RS    (PORTB &= ~(0x01))
#define SET_RS    (PORTB |=  (0x01))
#define CLR_RESET (PORTB &= ~(0x02))
#define SET_RESET (PORTB |=  (0x02))
#define CLR_CS    (PORTB &= ~(0x04))
#define SET_CS    (PORTB |=  (0x04))
#define CLR_MOSI  (PORTB &= ~(0x08))
#define SET_MOSI  (PORTB |=  (0x08))
#define CLR_SCK   (PORTB &= ~(0x20))
#define SET_SCK   (PORTB |=  (0x20))
*/


/*
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 * MSP432 PORT (03/01/2021)
 * Author: Daniel Hamilton
 * Goal: Updating drivers to support Texas Instruments MSP432.
 * +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
 */

#include "target_hw_common.h"
#include "spi_if.h"
#include "lcd_64x48_bitmap.h"

/*
 * LCD SPI & control lines
 *   HEADER   | Color  | LCD
 * -----------+--------+-------------------------
 *  P1.6      | Brown  | LCD_MOSI   (hardware SPI)
 *  P1.5      | Red    | LCD_SCK    (hardware SPI)
 *  P2.3      | Orange | LCD_RS     (gpio)
 *  P5.1      | Yellow | LCD_RESET  (gpio)
 *  P3.5      | Green  | LCD_CS_NOT (GPIO or SPI SS)
 *  P1.7      | N/A    | not used   (would be MISO)
 *
 */

#define _delay_ms(ms) delayMs(ms)

#define CLR_MOSI  GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN6)  // P1.6
#define SET_MOSI  GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN6)
#define CLR_SCK   GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN5)  // P1.5
#define SET_SCK   GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN5)
#define CLR_RS    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN3)  // P2.3
#define SET_RS    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN3)
#define CLR_RESET GPIO_setOutputLowOnPin(GPIO_PORT_P5, GPIO_PIN1); _delay_ms(1)  // P5.1
#define SET_RESET GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1); _delay_ms(1)
#define CLR_CS    GPIO_setOutputLowOnPin(GPIO_PORT_P3, GPIO_PIN5); _delay_ms(1)  // P3.5
#define SET_CS    GPIO_setOutputHighOnPin(GPIO_PORT_P3, GPIO_PIN5); _delay_ms(1)

/*
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
 * DESCRIPTION: Spi_c
 * This class simply calls the SPI functions in "spi_if." The need for this class
 * comes from porting the Arduino drivers for this LCD to the MSP432. In an attempt
 * to modify as little code as possible, this file requires an object instantiation
 * called "SPI"
 * +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
*/
class Spi_c
{
public:
    void begin() { Spi::init(); }
    uint8_t transfer(uint8_t data) { return Spi::transferByte(data); }
};
static Spi_c SPI; // <- Instantiation here.

//============================================================================
#define NUMBER_OF_SCREENS (13)
#define GEAR_START_SCREEN (5)
const SCREEN_IMAGE *const screens[NUMBER_OF_SCREENS] =
  {
  &Logo_6448,
  &Dimensions_6448,
  &Left_Double_6448,
  &Left_6448,
  &OLED_6448,
  &Gear_0_6448,
  &Gear_1_6448,
  &Gear_2_6448,
  &Gear_3_6448,
  &Gear_4_6448,
  &Gear_5_6448,
  &Gear_6_6448,
  &Gear_7_6448
  };
//============================================================================
void SPI_sendCommand(uint8_t command)
  {
  // Select the LCD's command register
  CLR_RS;
  // Select the LCD controller
  CLR_CS;
  //Send the command via SPI:
  SPI.transfer(command);
  // Deselect the LCD controller
  SET_CS;
  }
//============================================================================
void SPI_sendData(uint8_t data)
  {
  // Select the LCD's data register
  SET_RS;
  // Select the LCD controller
  CLR_CS;
  //Send the data via SPI:
  SPI.transfer(data);
  // Deselect the LCD controller
  SET_CS;
  }
//============================================================================
#define SSD1306B_00_SET_LOWER_COLUMN_ADDRESS_BIT (0x00)
#define SSD1306B_10_SET_UPPER_COLUMN_ADDRESS_BIT (0x10)
#define SSD1306B_B0_SET_PAGE_START_ADDRESS_BIT   (0xB0)
#define SSD1306B_DCDC_CONFIG_PREFIX_8D           (0x8D)
#define SSD1306B_DCDC_CONFIG_7p5v_14             (0x14)
#define SSD1306B_DCDC_CONFIG_6p0v_15             (0x15)
#define SSD1306B_DCDC_CONFIG_8p5v_94             (0x94)
#define SSD1306B_DCDC_CONFIG_9p0v_95             (0x95)
#define SSD1306B_DISPLAY_OFF_YES_SLEEP_AE        (0xAE)
#define SSD1306B_DISPLAY_ON_NO_SLEEP_AF          (0xAF)
#define SSD1306B_CLOCK_DIVIDE_PREFIX_D5          (0xD5)
#define SSD1306B_MULTIPLEX_RATIO_PREFIX_A8       (0xA8)
#define SSD1306B_DISPLAY_OFFSET_PREFIX_D3        (0xD3)
#define SSD1306B_DISPLAY_START_LINE_40           (0x40)
#define SSD1306B_SEG0_IS_COL_0_A0                (0xA0)
#define SSD1306B_SEG0_IS_COL_127_A1              (0xA1)
#define SSD1306B_SCAN_DIR_UP_C0                  (0xC0)
#define SSD1306B_SCAN_DIR_DOWN_C8                (0xC8)
#define SSD1306B_COM_CONFIG_PREFIX_DA            (0xDA)
#define SSD1306B_COM_CONFIG_SEQUENTIAL_LEFT_02   (0x02)
#define SSD1306B_COM_CONFIG_ALTERNATE_LEFT_12    (0x12)
#define SSD1306B_COM_CONFIG_SEQUENTIAL_RIGHT_22  (0x22)
#define SSD1306B_COM_CONFIG_ALTERNATE_RIGHT_32   (0x32)
#define SSD1306B_CONTRAST_PREFIX_81              (0x81)
#define SSD1306B_PRECHARGE_PERIOD_PREFIX_D9      (0xD9)
#define SSD1306B_VCOMH_DESELECT_PREFIX_DB        (0xDB)
#define SSD1306B_VCOMH_DESELECT_0p65xVCC_00      (0x00)
#define SSD1306B_VCOMH_DESELECT_0p71xVCC_10      (0x10)
#define SSD1306B_VCOMH_DESELECT_0p77xVCC_20      (0x20)
#define SSD1306B_VCOMH_DESELECT_0p83xVCC_30      (0x30)
#define SSD1306B_ENTIRE_DISPLAY_FORCE_ON_A5      (0xA5)
#define SSD1306B_ENTIRE_DISPLAY_NORMAL_A4        (0xA4)
#define SSD1306B_INVERSION_NORMAL_A6             (0xA6)
#define SSD1306B_INVERSION_INVERTED_A7           (0xA7)
//============================================================================
void Set_Address(uint8_t column, uint8_t page)
  {
  //Set column-lower nibble
  SPI_sendCommand(SSD1306B_00_SET_LOWER_COLUMN_ADDRESS_BIT|(column&0x0F));
  //Set column-upper nibble
  SPI_sendCommand(SSD1306B_10_SET_UPPER_COLUMN_ADDRESS_BIT|((column>>4)&0x0F));
  //Set page address, limiting from 0 to 7
  SPI_sendCommand(SSD1306B_B0_SET_PAGE_START_ADDRESS_BIT|(page&0x0F));
  }
//============================================================================
void show_64_x_48_bitmap(const SCREEN_IMAGE *OLED_image)
  {
  uint8_t
    column;
  uint8_t
    row;

  for(row=0;row<6;row++)
    {
    //Display is offest by 32 columns
    //(only 64 used of a possible 128, 32 discarded each side)
    Set_Address(32,row);

    // Select the LCD's data register
    SET_RS;
    // Select the LCD controller
    CLR_CS;
    for(column=0;column<64;column++)
      {
      //Read this byte from the program memory / flash,
      //Send the data via SPI:
      //SPI.transfer(pgm_read_byte( &(OLED_image->bitmap_data[row][column]) )); // Arduino
      SPI.transfer(OLED_image->bitmap_data[row][column]); // MSP432
      }
    // Deselect the LCD controller
    SET_CS;
    }
  }
//============================================================================
void Initialize_CFAL6448A(void)
  {
  //Thump the reset.  
  _delay_ms(1);
  CLR_RESET;
  _delay_ms(1);
  SET_RESET;
  _delay_ms(1);

  //Set the display to sleep mode for the rest of the init.
  SPI_sendCommand(SSD1306B_DISPLAY_OFF_YES_SLEEP_AE);

  //Set the clock speed, nominal ~105FPS
  //Low nibble is divide ratio
  //High level is oscillator frequency
  SPI_sendCommand(SSD1306B_CLOCK_DIVIDE_PREFIX_D5);
  SPI_sendCommand(0x80);//177Hz measured

  //Set the multiplex ratio to 1/48
  //Default is 0x3F (1/64 Duty), we need 0x2F (1/48 Duty)
  SPI_sendCommand(SSD1306B_MULTIPLEX_RATIO_PREFIX_A8);
  SPI_sendCommand(0x2F);  

  //Set the display offset to 0 (default)
  SPI_sendCommand(SSD1306B_DISPLAY_OFFSET_PREFIX_D3);
  SPI_sendCommand(0x00);

  //Set the display RAM display start line to 0 (default)
  //Bits 0-5 can be set to 0-63 with a bitwise or
  SPI_sendCommand(SSD1306B_DISPLAY_START_LINE_40);

  //Enable DC/DC converter, 9.0v
  SPI_sendCommand(SSD1306B_DCDC_CONFIG_PREFIX_8D);
  SPI_sendCommand(SSD1306B_DCDC_CONFIG_9p0v_95);

  //Map the columns correctly for our OLED glass layout
  SPI_sendCommand(SSD1306B_SEG0_IS_COL_127_A1);

  //Set COM output scan correctly for our OLED glass layout
  SPI_sendCommand(SSD1306B_SCAN_DIR_DOWN_C8);

  //Set COM pins correctly for our OLED glass layout
  SPI_sendCommand(SSD1306B_COM_CONFIG_PREFIX_DA);
  SPI_sendCommand(SSD1306B_COM_CONFIG_ALTERNATE_LEFT_12);

  //Set Contrast Control / SEG Output Current / Iref
  SPI_sendCommand(SSD1306B_CONTRAST_PREFIX_81);
  SPI_sendCommand(0xFF);  //magic # from factory

  //Set precharge (low nibble) / discharge (high nibble) timing
  //precharge = 15 clocks
  //discharge = 15 clocks
  SPI_sendCommand(SSD1306B_PRECHARGE_PERIOD_PREFIX_D9); //Set Pre-Charge period        
  SPI_sendCommand(0xFF); 

  //Set VCOM Deselect Level
  SPI_sendCommand(SSD1306B_VCOMH_DESELECT_PREFIX_DB);
  SPI_sendCommand(SSD1306B_VCOMH_DESELECT_0p65xVCC_00); 

  //Make sure Entire Display On is disabled (default)
  SPI_sendCommand(SSD1306B_ENTIRE_DISPLAY_NORMAL_A4);

  //Make sure display is not inverted (default)
  SPI_sendCommand(SSD1306B_INVERSION_NORMAL_A6);

  //Get out of sleep mode, into normal operation
  SPI_sendCommand(SSD1306B_DISPLAY_ON_NO_SLEEP_AF);
  }
//============================================================================
uint8_t
  gear_dir;
//----------------------------------------------------------------------------
void lcd_setup( void )
  {
  // LCD SPI & control lines
  //   ARD      | Port | LCD
  // -----------+------+-------------------------
  //  #8/D8     |  PB0 | LCD_RS
  //  #9/D9     |  PB1 | LCD_RESET
  // #10/D10    |  PB2 | LCD_CS_NOT (or SPI SS)
  // #11/D11    |  PB3 | LCD_MOSI   (hardware SPI)
  // #12/D12    |  PB4 | not used   (would be MISO)
  // #13/D13    |  PB5 | LCD_SCK    (hardware SPI)

  //Set the control lines to output
  //DDRB |= 0x2F;
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN6);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN5);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN3);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
  MAP_GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN5);

  //Drive the control lines to a reasonable starting state.
  CLR_RESET;
  CLR_RS;
  SET_CS;
  CLR_MOSI;
  CLR_SCK;

  // Initialize SPI. By default the clock is 4MHz.
  SPI.begin();
  //Bump the clock to 8MHz. Appears to be the maximum.
  //SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));


  //Fire up the SPI OLED
  Initialize_CFAL6448A();

  gear_dir=0;
  }
//----------------------------------------------------------------------------
void  lcd_loop(void)
  {
  uint8_t
    current_screen;
  //Put up some bitmaps from flash
  for(current_screen=0;current_screen<GEAR_START_SCREEN;current_screen++)
    {
    //show_64_x_48_bitmap((SCREEN_IMAGE *)pgm_read_word(&screens[current_screen])); // Arduino
    show_64_x_48_bitmap((SCREEN_IMAGE*)&screens[current_screen]); // MSP432
    //Wait a bit . . .
    _delay_ms(1500);
    if(current_screen < 2)
      {
      //Wait a little extra for the logo & dimensions
      _delay_ms(750);
      }
    }
  //Loop the gear animation.
  uint8_t
    repeat;
  if(0==gear_dir)
    {
    for(repeat=0;repeat<25;repeat++)
      {
      for(current_screen=GEAR_START_SCREEN;current_screen<NUMBER_OF_SCREENS;current_screen++)
        {
          show_64_x_48_bitmap((SCREEN_IMAGE*)&screens[current_screen]); // MSP432
        _delay_ms(20);        
        }
      }
    gear_dir=1;  
    }
  else
    {
    for(repeat=0;repeat<25;repeat++)
      {
      for(current_screen=NUMBER_OF_SCREENS-1;GEAR_START_SCREEN<=current_screen;current_screen--)
        {
          show_64_x_48_bitmap((SCREEN_IMAGE*)&screens[current_screen]); // MSP432
        _delay_ms(20);        
        }
      }
    gear_dir=0;  
    }
  }
//============================================================================

void lcd_test()
{
    //Display is offest by 32 columns
    //(only 64 used of a possible 128, 32 discarded each side)
    Set_Address(32,123);

    // Select the LCD's data register
    SET_RS;
    // Select the LCD controller
    CLR_CS;
    for(int column=0;column<256;column++)
      {
      //Read this byte from the program memory / flash,
      //Send the data via SPI:
      //SPI.transfer(pgm_read_byte( &(OLED_image->bitmap_data[row][column]) )); // Arduino
      uint8_t d = 0x3C;
      SPI.transfer(d); // MSP432

      uint8_t c = 0xAF;
      SPI_sendCommand(c);

      c = 0xAE;
      SPI_sendCommand(c);
      }
    // Deselect the LCD controller
    SET_CS;
}
