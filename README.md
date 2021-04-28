# DSP Effects Glove

## Project Abstract

The goal of the Hand Tracking Glove is to track joint bends in multiple fingers and hand orientation of the right hand. The glove produces messages containing sensor data that can be transferred to a user-end application via Bluetooth. My user-end application to demonstrate the glove’s capabilities is an interactive audio effects rack utilizing the hardware platform from the Real-Time Digital Signal Processing course (EEL4750). The user wears the glove to control parameters of high-pass and low-pass filters, pitch shifting, and volume.  


**Figure 1: Hand Tracking Glove device.**



**Figure 2: Charger PCB (left) attached to the glove PCB (right) for battery charging.**

## Features

In this section, I will discuss the core features and objectives of this project. This project features two PCBs - one for the glove and one for the battery charger. Additionally, the hardware platform from Real-Time Digital Signal Processing (EEL4750) is used in this project. The features for each platform are separated into three lists below. 

#### Glove PCB Features

- Capture bends in 3 fingers using bend-sensitive resistors as a proof of concept. The current hardware design allows for easily adding 5 more flex sensors with minimal changes required in software/hardware.
- Finger sensor bend resolution is high enough to detect noticeable differences in finger movements. Testing shows that I can achieve ~130 readings per flex sensor after ADC filtering (14-bit to 8-bit bit shifting).
- Provides pitch and roll orientation.
- The glove must operate wirelessly. To support this requirement, the device is run on a single-cell 3.7V Li-Po battery and transmits data to/from the DSP Effects Rack platform via Bluetooth.
- The device should be lightweight and comfortable to wear.
- Provide a text-based user interface on the glove using an LCD. This display shows the battery’s charge percentage, Bluetooth connection status, flex sensor readings, and pitch/roll. 

#### C2000 / Codec Platform

- Audio received from the input line is processed and output to a DAW running on a desktop.
- Implements audio effects - high-pass filter, low-pass filter, and pitch shifting. 
- Maps data received from the glove to tunable parameters of the audio effects - cutoff frequencies, volume, pitch.

#### Charger PCB Features
- A battery charger circuit that safely charges the Li-ion battery to max capacity. This circuit implements Constant Current (CC) and Constant Voltage (CV) circuits. The circuit automatically switches between CC, CV, and open circuit outputs based on the battery’s current and voltage thresholds.
- The charger PCB plays a 4 note tune when the glove is connected or disconnected. The tune is  played forwards when connected and backwards when disconnected.
- The charger PCB automatically detects when the glove PCB is connected or disconnected even if the Li-ion battery on the glove PCB is fully depleted.

**Figure 3: Hardware diagram for Glove, Charger, and DSP Effects Rack.**

## Concept & Technology Selection

In this section, I will discuss the technologies and designs used for the implementation of this project. Much of the content in this section is copied from my Preliminary Design Report from the beginning of this semester; however, the details have been updated to reflect the design changes made throughout the semester.

#### Capturing Joint Bends

I used bend sensitive resistors attached above and below each joint, which change resistance as the joint bends on a scale of 20k - 100k Ohms. The flex sensor is used in a voltage divider circuit and measured through the MSP432’s 14-bit ADC. The bottom 6 bits are removed, leaving 8 bits per flex sensor that are used in the Bluetooth message.

The primary issue with using multiple analog sensors is that the MSP432 only has 8 ADC pins. With 2 of those pins being required for the State of Charge estimation, only 6 pins remain. One of my goals is to create a circuit that can easily extend to 10-15 bend sensitive resistors. The MSP432 does not have enough analog pins for this type of application. There is also some circuitry to amplify the flex sensor voltages before being read by the ADC. This hardware would have to be copied for each individual flex sensor voltage divider. To solve this problem, I used a 4051 8x1 analog multiplexer IC to switch between finger voltages to output. This allows the microcontroller to select which finger to take ADC measurements on using three GPIO as mux selects.

Another issue with using flex sensors is pricing. The popular Adafruit flex sensors are $10 per sensor. This is affordable for my prototype, but does not scale well with 10-15 flex sensors. I attempted to solve this problem by making homemade flex sensors from pressure-sensitive material and copper tape; however, these sensors were too sensitive to strap to the gloves. They would max out just by applying tape to them. If I were to continue working on this project, I would look for other methods of detecting finger bends.

Finally, I built a 10 Hz low-pass filter on the mux output to reduce noise on the ADC readings. However, this created a problem during the software development stage when switching between the finger voltage readings. The LPF’s capacitor discharge time was so long that each finger’s voltage reading was related to the previous finger’s voltage reading. My solution was to remove the LPF hardware from the PCB since my software essentially implemented a low-pass filter by removing the bottom 6 bits of the ADC readings.

#### Hand Orientation (Pitch and Roll)
My first idea to capture hand rotation around pitch, yaw, and roll axes was to simply use a gyroscope. In theory, the angular velocity data could be integrated to determine rotational position (orientation). However, high-precision gyroscopes are expensive ($200 - $1000+ range) and cheaper gyroscopes become highly inaccurate due to drift. The alternative I used was to combine data from a cheap accelerometer and gyroscope using a sensor fusion algorithm. The accelerometer data is used to find orientation based on gravity’s vector, and the gyroscope data is used to correct the accelerometer’s sensitivity to non-gravity related acceleration (i.e. the user’s hand moving).

#### Bluetooth Message Transmission

I chose to use Bluetooth for wireless data transmission because Bluetooth allows the device to be connected to most computers or phones without additional hardware. For the breakout board, I am using the HC-05 UART to Bluetooth module. I chose this board because (1) it can send and read messages through a UART interface, (2) it has Arduino drivers that I could modify and port to the MSP432, and (3) it can be set up as a master for the glove application and a slave for the DSP Effects Rack application. Below is a table showing how individual data packets look when transferred from the glove application to the DSP Effects Rack.

Op Code (1 Byte)
Flex [0:3] (4 Bytes)
Pitch [4B] (4 Bytes)
Roll [4B] (4 Bytes)

#### Battery Power

I used a buck-boost converter on the glove PCB to step down the 3.7V single celled Li-Po battery to 3.3V in the 3.3-4.2V voltage range and step it up to 3.3V in the 3.0-3.29V range. Additionally, the buck-boost has very high power conversion efficiency (~93%). This allows the PCB to get the most charge out of the battery before requiring a recharge.

#### Battery Charge Monitoring

The battery’s charge or “State of Charge (SOC)” is estimated using a method called “Coulomb Counting.” This method involves estimating the state of charge by subtracting the current consumption from the initial capacity. The battery percentage can be estimated with the following formula:

(Qprevious - icurrent * deltaTime) / Qtotal * 100%

The initial charge measurement is calculated by mapping the Open Circuit Voltage (OCV) of the battery to its charge in the battery’s discharge curve. To capture this OCV, the microcontroller disables all other hardware on the PCB and sets itself into low-power mode. The microcontroller starts an ADC reading and wakes back up when it has captured the battery’s voltage. This voltage is very close to the OCV since the microcontroller only pulls a few microamps in low-power mode.

The load current is a static value that is estimated based on the typical current draw values provided in each hardware component’s datasheet. Most of the current variation in the glove PCB is from the flex sensors. Since this current draw is in a scale of microamps, and the other hardware’s current draw is in a scale of milliamps, the variation will not make much difference. This is why runtime current draw estimation using a shunt resistor / in-amp is not necessary even though it is in the original design.

#### Battery Charger Circuit (CCCV)

Recharging Lithium-ion batteries involves a 2-step process - Constant Current and Constant Voltage (CCCV). The first stage provides the battery with a constant flow of the max charging current until the battery reaches a 4.2V threshold. At this threshold, a Schmitt trigger switches the CC circuit off and the CV circuit on so that a constant voltage of 4.2V is provided to the battery. When the battery’s current draw reaches a minimum threshold of around 5 mA, a comparator circuit shuts the circuit off completely.


**Figure 4: Charge characteristics curve showing expected circuit outputs.**

Initially, I planned to charge the battery with a 5V power supply. However, I tried multiple designs for the CC and CV circuits that worked poorly due to high voltage dropout or instability. Eventually, I decided to upgrade my power supply to 10V, which allowed me to use an LM317 regulator as constant voltage and constant current power supplies.

#### Waveform Generator & Amplification

This circuit plays a 4-note tune either forwards or backwards when triggered. I've attached a high level diagram of how this circuit will work in Figure 8 below. I've also listed an explanation of the steps starting from the bottom after the charger PCB is connected to the glove PCB. Throughout this explanation, “connection” or “disconnection” refers to the charger PCB connecting/disconnecting from the glove PCB.

- When connected, the Connected pin is set low. When connected, the pin will be high. 
- Both positive and negative edges are detected using a single circuit, which utilizes an XOR gate and RC circuit to detect when there is a change in voltage at the Connected pin.
- The DFF is used to control whether the 4029 counter will count up or down. This is because I want the tune to be played forward or backwards depending on if the glove is connected or disconnected. The user can audibly tell the difference between a connection or disconnection.
- The edge detectors will trigger a monostable 555 timer, which will enable an astable mode 555 timer long enough to output 4 pulses.
- The 4029 counter will count from 0, 1, 2, 3 or 0, 3, 2, 1 depending on if it is in up or down mode.
- The 4051 demux decodes the 0-3 binary values and selects a resistor value to be used in the next stage's 555 timer for frequency selection. Some experimentation on the breadboard will determine if any BJTs or MOSFETs are necessary for this stage.
- The 3rd 555 timer and RC circuit will output a triangle wave at the frequency selected in the previous stage. This output is what will be played through the speaker, so the resistors need to be carefully tuned to notes in the song. 
- The volume controller amplifies the signal in the range 0V - 5V. 
- The AB amplifier will keep a 1:1 voltage gain but allows for more current to be drawn from the power supply rather than the volume control amplifier. Output is fed into the speaker to produce the audio.


**Figure 5: High-level block diagram of hardware used in the waveform generator and speaker amplifier.**

This circuit could have been easily implemented with a microcontroller and an AB amplifier; however, I wanted this board to work without having to be programmed. Additionally, this whole circuit can be built cheaper than the $13 worth of hardware required for the microcontroller (microcontroller, crystal, inductors, etc).

#### Glove & Charger Connection Detection

The circuit for detecting connection between the charger PCB and the glove PCB uses a pull-up resistor and a disconnectable ground wire looping between the two PCBs. When the PCBs are disconnected, the pull-up resistor will pull the Connected pin high. When the PCBs are connected, the ground signal will pull the Connected pin low. 


**Figure 6: Connection detection circuit that will trigger 4 note tune circuit.**

#### Interactive Audio Effect Rack - Demo Application

The goal of this project is to create an expressive and easy-to-use controller. However, a controller is not an interesting project without an application to control. I used the glove’s outputs to control several audio effects on a DSP/codec platform. Figure 10 shows the components used in this application from a high level.

**Figure 7: Hardware platform for the demo application.**

The C2000 receives flex sensor data, pitch, and roll from the glove application through the slave HC-05 module. The microcontroller decodes the message data according to the established message protocol.
The C2000 implements several frequency domain effects listed in the table below. The microcontroller software maps various sensor readings from the glove to control “knobs” of the effects as described in the table below.

	
| Effect | Tunable Parameters | Controlled by |
| --- | --- | --- |
| Low-Pass Filter | Cutoff Frequency | Pitch axis |
| Audio Pitch | # frequency bins to shift left or right | Roll axis |
| Audio Volume | Gain | Pointer finger |
| High-Pass Filter | Cutoff Frequency | Middle finger |
| Toggle effect rack enable | Enable / Disable | Pinky finger |


Audio input comes from the codec via I2S, sampled at 48 kHz. The audio effects discussed previously are applied to this data and output back to the codec via SPI. Data transportation during the input, processing, and output stages are handled using 2 DMA channels and ping-pong buffers, which allows data to be sampled and output in real time.