'''
CLASS DESCRIPTION:
This class reads from the accelerometer, gyro, magnetomer information.
It also includes functions to read json strings with rounded floats of the
data returned from the sensors.
'''

import json, math
import board
import busio
import adafruit_fxos8700 as fxoslib
import adafruit_fxas21002c as fxaslib
import time

class sensor:
    
    def __init__(self, precision=3):
        self.fxos = None
        self.fxas = None
        self.precision = precision

        # call the setup functions
        (self.fxos, self.fxas) = self.__SetupSensors()


    # +----------+----------+----------+----------+
    # |             PRIVATE METHODS               |
    # +----------+----------+----------+----------+
    
    '''
    SUMMARY:
    Initialize the I2C bus and the sensor backpack.

    RETURN:
    fxos - (obj) Data from the accelerometer, magnetometer
    fxas - (obj) Data from the gyroscope
    '''
    def __SetupSensors(self):
        i2c = busio.I2C(board.SCL, board.SDA)
        fxos = fxoslib.FXOS8700(i2c, accel_range=fxoslib.ACCEL_RANGE_2G) # accelerometer, magnetometer
        fxas = fxaslib.FXAS21002C(i2c) # gyro
        return fxos, fxas
    
    # +----------+----------+----------+----------+
    # |              PUBLIC METHODS               |
    # +----------+----------+----------+----------+
    
    '''
    SUMMARY:
    Read raw x,y,z information from the accelerometer sensor.

    RETURN:
    x - (float) X-axis data from whichever sensor is chosen
    y - (float) Y-axis data from whichever sensor is chosen
    z - (float) Z-axis data from whichever sensor is chosen
    '''
    def accel(self):
        x,y,z = self.fxos.accelerometer
        #print("Python Accel: {}, {}, {}".format(x, y, z))
        return x,y,z

    def mag(self):
        x,y,z = self.fxos.magnetometer
        #print("Python Mag: {}, {}, {}".format(x, y, z))
        return x,y,z

    def gyro(self):
        x,y,z = self.fxas.gyroscope
        #print("Python Gyro: {}, {}, {}".format(x, y, z))
        return x,y,z    

# +----------+----------+----------+----------+
# |                 TEST UNIT                 |
# +----------+----------+----------+----------+
def main():
    print("Running nxp.main()")
    s = sensor()

    while True:
        accel_x, accel_y, accel_z = s.accel()
        mag_x, mag_y, mag_z = s.mag()
        gyro_x, gyro_y, gyro_z = s.gyro()
        
        print('Gyroscope (radians/s): ({0:0.3f},  {1:0.3f},  {2:0.3f})'.format(gyro_x, gyro_y, gyro_z))
        print('Acceleration (m/s^2): ({0:0.3f}, {1:0.3f}, {2:0.3f})'.format(accel_x, accel_y, accel_z))
        print('Magnetometer (uTesla): ({0:0.3f}, {1:0.3f}, {2:0.3f})'.format(mag_x, mag_y, mag_z))

        time.sleep(.1)

if __name__ == '__main__':
    main()
