import serial
from time import sleep

def TestHc05():
    print("Running TestHc05..")
    uart = Uart()

    # Configuration read/writes
    configs = [
        b"AT+VERSION?\r\n",
        b"AT+ROLE=0\r\n",    # sets device to SLAVE
    ]

    uart.recv(uart.available()) # clear the buffer

    # Configure uart for command mode
    for c in configs:
        print("-----"*5)
        print("Sending AT command: {}".format(c))
        uart.send(c)
        sleep(0.3)
        
        print("Reading response..")
        try:
            numBytesInBuffer = uart.available()
            if numBytesInBuffer:
                print(uart.recv(numBytesInBuffer).decode('utf-8'))
            else:
                print("WARNING: No response from device..")
        except:
            print("Could not decode response from device")

class Uart:

    # Default baud rate:    38400

    def __init__(self):
        self.dev = serial.Serial("/dev/serial0", 38400)
        self.dev.parity = serial.PARITY_NONE
        self.dev.stopbits = serial.STOPBITS_ONE
        self.dev.bytesize = serial.EIGHTBITS

    def send(self, byte_array):
        self.dev.write(byte_array)

    def available(self):
        return self.dev.inWaiting()

    def recv(self, num_bytes):
        return self.dev.read(num_bytes)

if __name__ == "__main__":
    TestHc05()
    