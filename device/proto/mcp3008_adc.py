import spidev
import inspect
from time import sleep

class Mcp3008:
    '''
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    Description: Mcp3008
    This class handles communication to MCP3008 8-channel ADC IC.
    This module communicates with the chip over SPI.

    Chip documentation is available here:
    https://ww1.microchip.com/downloads/en/DeviceDoc/21295d.pdf
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    '''

    SPI_BUS         = 0
    SPI_DEVICE      = 1
    SPI_CLK_HZ      = 10000 # Min sampling frequency is 10 kHz
    SPI_POLARITY    = 0b0   # CS rests at low
    SPI_PHASE       = 0b1   # rising edge used to sample data
    SPI_BITS_PER_WORD   = 8
    SPI_CS_ACTIVE_HIGH  = False
    SPI_LSB_FIRST   = True

    def __init__(self):
        self.spi = spidev.SpiDev()

    def __enter__(self):

        # Open spi device /dev/spidev<bus>.<device>
        self.spi.open(self.SPI_BUS, self.SPI_DEVICE)

        # Configure SPI interface to ADC chip.
        self.spi.max_speed_hz    = self.SPI_CLK_HZ
        self.spi.mode            = (self.SPI_POLARITY << 1) | (self.SPI_PHASE << 0)
        self.spi.bits_per_word   = self.SPI_BITS_PER_WORD
        self.spi.cshigh          = self.SPI_CS_ACTIVE_HIGH
        #spi.lsbfirst = SPI_LSB_FIRST # Errors out due to this problem with spidev: 
        #                               https://github.com/doceme/py-spidev/issues/18
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.spi.close()

    def errorPrint(self, msg):
        print("**ERROR** - {}.{}: {}".format(self.__class__.__name__, inspect.currentframe().f_code.co_name, msg))

    def readAdc(self, adc_channel):
        '''
        +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
        Description: readAdc
        This function starts an ADC conversion on a specified channel and returns
        the results in integer form.

        INPUTS:
        @(int) adc_channel
            Determines which ADC channel to start the conversion on. Must be a
            value between 0-7, corresponding to the 8 ADC channels.
        
        RETURN: (int) ADC reading with value from 0-1023. To convert this to
            a voltage value, use the formula referenceVoltage*(returned val)/1024.
        +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
        '''

        # Check that the requested channel is within operational range.
        if adc_channel < 0 or adc_channel > 7:
            self.errorPrint("Invalid channel number passed ({}).".format(adc_channel))
            return None
        
        # Create message to transfer over SPI.
        # 
        # The MCP3008 expects a 17 bit message, but this module expands it to 24 bits since
        # the SPI module is set up to transfer 8 bit words.
        # 
        # The message format is shown below.
        # Byte 2                            Byte 1                                              Byte 0
        # [Zero Padding (7bits) : Start Bit][SNGLEND/~DIFF : Channel 2-0 : Zero Padding (4bits)][Zero Padding[8bits]]
        startTx = 0x1
        singleEndMode = 0x8
        garbage = 0x00
        msg = [startTx, (singleEndMode | adc_channel) << 4, garbage]

        print("Sending 0x{:02x}{:02x}{:02x}".format(msg[0], msg[1], msg[2]))
        rx = self.spi.xfer2(msg)
        return (rx[1] << 8) | (rx[2] << 0)

if __name__ == "__main__":
    '''
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    Description: __main__
    This script infinitely reads from ADC channels 0/1 and prints out the 
    voltage level assuming a voltage reference of 3.3V.
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    '''
    with Mcp3008() as s:
        while True:
            for channel in range(0, 2):
                result = s.readAdc(channel)
                print("ADC Channel #{}: {:.2f}".format(channel, 3.3*result/1024))
                sleep(0.5)