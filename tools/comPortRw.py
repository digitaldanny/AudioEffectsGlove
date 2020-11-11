'''
# +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
DESCRIPTION: comPortRw.py
The goal of this script is to provide an easy-to-modify tool that can be used
to debug the Unity Bluetooth script and the C2000 / HC-05 connection.
# +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
'''

# +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
#                             SCRIPT CONFIGURATIONS
# +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+
BAUDRATE        = 115200
TIMEOUT_SECONDS = 0.5
COM_WRITE       = "COM7"
COM_READ        = "COM5"
# +-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+-----+s

import serial

portWrite   = serial.Serial(COM_WRITE, baudrate=BAUDRATE, timeout=TIMEOUT_SECONDS)
portRead    = serial.Serial(COM_READ, baudrate=BAUDRATE, timeout=TIMEOUT_SECONDS)

while (True):

    userInput = input("Read (r), Write (w), or Quit (q): $").lower()

    if userInput == "q":
        # Exit the program.
        break

    elif userInput == "r":
        # Read data from the com port.
        try:
            numCharsToRead = int(input("Num of chars to read: $"))
        except:
            print("Could not convert provided num chars to int. Try again.")
            break
        rcv = portRead.read(numCharsToRead).decode('utf-8')
        print("Received: '{}'".format(rcv))
    
    elif userInput == "w":
        # Write data to the com port.
        msg = input("Message to write: $")
        portWrite.write(msg.encode('utf-8'))
        print("Message complete.")
    
    else:
        print("Invalid option provided. Try again.")
    print("---")

portWrite.close()
portRead.close()