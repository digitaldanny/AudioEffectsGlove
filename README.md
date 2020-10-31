# Gesture Control

## 1. Overview

The goal of this project is to create a glove that collects information about the user's wrist orientation and finger movements. Once collected, this data can be wirelessly transmitted to an external application. My current plan is to use this to control a video game made in the Unity Engine. 

## 2. Hardware

#### 2.1 - Prototype v1.0.0 using RPi 3 B+

The first prototype doesn't quite look like a glove yet; however, it has allowed a large portion of the software to be completed before ordering additional parts and beginning PCB design. Below is a description of some of the component groups, which are marked in the picture as colored boxes.

- **Red Box:** Flex sensors are variable resistors that change resistance based on how much they are bent, which makes them a perfect sensor for capturing finger movements. The 10 potentiometers shown in the image are modeling the 10 [flex sensors](https://en.wikipedia.org/wiki/Flex_sensor) that will measure how each finger is bent. 10 sensors are used for 5 fingers because 2 joints will be captured per finger - [the MCP joint and the PIP joint](https://en.wikipedia.org/wiki/Interphalangeal_joints_of_the_hand), and the third joint (DIP) near the tip of the finger can be approximated based on the PIP joint.
- **Pink Box:** This is the HC-05 Bluetooth module that was meant to transfer data wirelessly to the desktop application. There are some compatability issues with this chip and the Unity Bluetooth libraries, so this is not used yet.
- **Purple Box:** This dev board contains the accelerometer/gyroscope/magnetometer sensors in one component. These are the sensors that will be used to capture wrist orientation.
- **Orange Box:** The RPi only has digital pins, so an external ADC was required to capture analog values from the flex sensor models.
- **Green Box:** These 8:1 Analog Mux chips are used to select between the flex sensors. Using these, both ADC values required to capture the joint info for one finger can be calculated at the same time. Unfortunately, the external ADC (in orange) cannot capture both ADC values at the same time, so this optimization cannot be utilized until future prototypes.
- **Blue Box:** 5V to 3.3V regulator circuit. 

<img src="/images/prototype_v1.jpg" alt="prototype v1 w/ RPi" width="700"/>

#### 2.2 - Prototype v2.0.0 using C2000 LaunchPad

This prototype will look very similar to v1.0.0 where the primary change is switching microcontrollers from the Raspberry Pi to [Texas Instrument's C2000 LaunchPad](https://www.ti.com/tool/LAUNCHXL-F28379D). This dev board was chosen because it contains a dual-core TMS320 CPU, which uses a 200 MHz clock and 200 MFLOPS FPU. The FPU is an important component for this project because there will be significant amount of floating point math in the accelerometer/gyroscope/magnetometer sensor fusion algorithm. Due to this change, I will also be removing the external ADC IC. The TMS320 comes with multiple ADC pins, so the extra hardware would just bloat the PCB. 
