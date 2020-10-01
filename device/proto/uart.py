import serial
from time import sleep

def TestHc05():
    baudRateCmdMode = 38400
    uart = Uart()
    uart.set_baud_rate(baudRateCmdMode)

    # Configuration read/writes
    configs = [
        "AT+VERSION?\r\n",
        "AT+ROLE=0\r\n",    # sets device to SLAVE
    ]

    # Configure uart for command mode
    for c in configs:
        print("-----"*5)
        print("Sending AT command: {}".format(c))
        uart.send(c)
        sleep(0.3)
        
        print("Reading response..")
        numBytesInBuffer = uart.available()
        if numBytesInBuffer:
            print(uart.recv(numBytesInBuffer))
        else:
            print("WARNING: No response from device..")

class Uart:

    # Baud rate for AT command mode:    38400
    # Baud rate for sending data:       9600

    def __init__(self):
        self.dev = serial.Serial("/dev/ttyS0", 9600)
        self.dev.parity = serial.PARITY_NONE
        self.dev.stopbits = serial.STOPBITS_ONE
        self.dev.bytesize = serial.EIGHTBITS

    def set_baud_rate(self, baud_rate):
        self.dev.baudrate = baud_rate

    def send(self, byte_array):
        self.dev.write(byte_array.encode('utf-8'))

    def available(self):
        return self.dev.inWaiting()

    def recv(self, num_bytes):
        return self.dev.read(num_bytes)

if __name__ == "__main__":
    TestHc05()
    