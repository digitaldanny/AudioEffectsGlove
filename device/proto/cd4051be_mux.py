from RPi import GPIO as gpio
import inspect

class Cd4051be:
    '''
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    Description: Cd4051be
    This class controls the 2 analog multiplexers used for reading the 
    flex sensors.

    Chip documentation is available here:
    https://ww1.microchip.com/downloads/en/DeviceDoc/21295d.pdf
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    '''

    SELECT_LINE = [21, 20, 16] # RPi GPIO pins to use as select Lines 0:2

    def __enter__(self):
        gpio.setmode(gpio.BCM)
        
        # Set up all select lines as outputs
        for s in self.SELECT_LINE:
            gpio.setup(s, gpio.OUT)
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        gpio.cleanup()

    def errorPrint(self, msg):
        print("**ERROR** - {}.{}: {}".format(self.__class__.__name__, inspect.stack()[1][3], msg))

    def setMuxInput(self, mux_channel):
        '''
        +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
        Description: setMuxInput
        This function changes the select lines of both 8:1 Analog Multiplexers
        to change which input channel will appear on the output.

        INPUTS:
        @(int) mux_channel
            Determines which mux input to allow on the output. Must be a
            value between 0-7, corresponding to the 8 mux input channels.
        
        RETURN: None
        +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
        '''

        if mux_channel < 0 or mux_channel > 7:
            self.errorPrint("Invalid mux channel. Must be an integer value between 0-4.")
            return None

        # Convert mux channel integer into a binary string to iterate over.
        chBin = "{:03b}".format(mux_channel)

        # determine which pins need to be set high.
        for i in range(3):
            b = chBin[i]
            if b == '1':
                gpio.output(self.SELECT_LINE[i], gpio.HIGH)
            else:
                gpio.output(self.SELECT_LINE[i], gpio.LOW)

if __name__ == "__main__":
    '''
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    Description: __main__
    This script allows the user to adjust the mux select lines. This is useful
    for debugging the flex sensor voltage readings.
    +=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+=====+
    '''
    with Cd4051be() as mux:
        while True:
            channel = int(input("Select a channel: $"))
            mux.setMuxInput(channel)