# Gesture Control

## 1. Overview

The goal of this project is to create a glove that collects information about the user's wrist orientation and finger movements. Once collected, this data can be wirelessly transmitted to an external application. My current plan is to use this to control a video game made in the Unity Engine. 

## 2. Hardware

#### 2.1 - Prototype v1.0.0 using RPi 3 B+

The first prototype doesn't quite look like a glove yet; however, it has allowed some software to be completed before ordering additional parts and beginning PCB design. Below is a description of some of the component groups, which are marked in the picture as colored boxes.

- **Stretch Sensors (Red):** Stretch sensors are variable resistors that change resistance based on how much they are stretched, which makes them a perfect sensor for capturing finger movements. The 10 potentiometers shown in the image are modeling the 10 stretch sensors that will measure how each finger is bent. 10 sensors are used for 5 fingers because 2 joints will be captured per finger - [the MCP joint and the PIP joint](https://en.wikipedia.org/wiki/Interphalangeal_joints_of_the_hand), and the third joint (DIP) near the tip of the finger can be approximated based on the PIP joint in software.
- **Serial to Bluetooth Module (Pink):** This is the HC-05 Bluetooth module that was meant to transfer data wirelessly to the desktop application. There are some compatability issues with this chip and the Unity Bluetooth libraries, so this is not used yet.
- **MIMU (Purple):** This dev board contains the accelerometer/gyroscope/magnetometer sensors in one component. These are the sensors that will be used to capture wrist orientation.
- **ADC to SPI IC (Orange):** The RPi only has digital pins, so an external ADC was required to capture analog values from the flex sensor models.
- **Analog Muxes (Green):** These 8:1 Analog Mux chips are used to select between the flex sensors. Using these, both ADC values required to capture the joint info for one finger can be calculated at the same time. Unfortunately, the external ADC (in orange) cannot capture both ADC values at the same time, so this optimization cannot be utilized until future prototypes.
- **Linear Regulator (Blue):** 5V to 3.3V regulator circuit. 

<img src="/images/prototype_v1.jpg" alt="prototype v1 w/ RPi" width="700"/>

**Figure 1:** Image of Raspberry Pi prototype and potentiometers in place of stretch-sensitive resistors.

#### 2.2 - Prototype v2.0.0 using C2000 LaunchPad

This prototype looks similar to v1.0.0 where the primary change is switching microcontrollers from the Raspberry Pi to [Texas Instrument's C2000 LaunchPad](https://www.ti.com/tool/LAUNCHXL-F28379D). This dev board was chosen because it contains a dual-core TMS320 CPU, which uses a 200 MHz clock and 200 MFLOPS FPU. At the time of selecting this, I was planning to implement the sensor fusion algorithm on the processor. Since this would involve a lot of floating point math that would be CPU intensive with software emulation, the FPU would have been critical. I decided against using this later, but I was able to remove the external ADC IC and test the parallel ADC capture using the TMS320's ADC pins. 

#### 2.3 - Prototype v3.0.0 using MSP432 LaunchPad

The hardware is currently being built, so an image of the high-level hardware block diagram is shown in Figure 2 instead. The following changes will be included in this prototype.

- Switching from TMS320 DSP to MSP432 MCU. The sensor fusion algorithm will now be implemented on the desktop, so the DSP's high power FPU is no longer required. The MSP432 is much smaller, reducing the amount of space the PCB will take up on the user's wrist.
- Powered with 3.3V Li-ion rechargeable battery.
- Transistor based battery charger circuit that can implement the Constant Current, Constant Voltage (CCCV) steps for recharging Li-ion cells. This circuit will be placed on a separate PCB and will connect to the glove's PCB with 2 pins (power / ground).
- MCU controllable 4-frequency triangle wave generator using a 555 timer IC. This will be used to play a 4 note tune when the glove is plugged/unplugged for charging.
- Voltage amplifier to control speaker volume. This will use a negative feedback op-amp that will scale voltage from 0V - 3.3V.
- Class AB amplifier to amplify power output for driving the 8 Ohm speaker. This is required since the timer and voltage amplifier will not be able to output enough current to power the speaker.

<img src="/images/HW_Diagram_v3.PNG" alt="HW Diagram v3" width="1000"/>

**Figure 2:** High-level hardware diagram of the battery charger PCB (left) and the glove hardware (right).

## 3. Software

Below is a high level flowchart of what the glove's software will do and how it will connect with the desktop application.

<img src="/images/SW_Flowchart_v2.PNG" alt="SW Flowchart v2" width="1000"/>

**Figure 2:** High-level software flowchart for the MSP432 and the Unity 3d application.
