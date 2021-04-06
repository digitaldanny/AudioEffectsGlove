#include "Wire.h"

const int MPU_addr = 0x68; // i2c address of the mpu6050
int16_t acX, acY, acZ, tmp, gyX, gyY, gyZ;

void setup() {
  Wire.begin();

  // PWR_MGMT - wake up
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // ACCEL_CONFIG +- 4g
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C); // PWR_MGMT register
  Wire.write(0x08); // Wakes up MPU6050
  Wire.endTransmission(true);

  // GYRO_CONFIG +- 500 deg/s
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B); // PWR_MGMT register
  Wire.write(0x08); // Wakes up MPU6050
  Wire.endTransmission(true);
  
  Serial.begin(9600);
}

void loop() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B); // Starting address of data transfer (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr, 14, true); // request burst read of 14 registers

  acX = Wire.read() << 8 | Wire.read();
  acY = Wire.read() << 8 | Wire.read();
  acZ = Wire.read() << 8 | Wire.read();
  tmp = Wire.read() << 8 | Wire.read();
  gyX = Wire.read() << 8 | Wire.read();
  gyY = Wire.read() << 8 | Wire.read();
  gyZ = Wire.read() << 8 | Wire.read();

  Serial.print("AcX = "); Serial.print(acX);
  Serial.print(" | AcY = "); Serial.print(acY);
  Serial.print(" | AcZ = "); Serial.print(acZ);
  Serial.print(" | Tmp = "); Serial.print(tmp);
  Serial.print(" | GyX = "); Serial.print(gyX);
  Serial.print(" | GyY = "); Serial.print(gyY);
  Serial.print(" | GyZ = "); Serial.println(gyZ);
  delay(100);
}
