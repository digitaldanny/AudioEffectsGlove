#include "Wire.h"
#include "sensor_processing_lib.h"

const int MPU_addr = 0x68; // i2c address of the mpu6050
int16_t acX, acY, acZ, tmp, gyX, gyY, gyZ;
unsigned long Start = 0,loop_start,temp2;
float delta,wx,wy,wz;
euler_angles angles;
vector_ijk fused_vector;
Quaternion q_acc;

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

  // GYRO_CONFIG +- 250 deg/s
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B); // PWR_MGMT register
  Wire.write(0x08); // Wakes up MPU6050
  Wire.endTransmission(true);
  Serial.begin(9600);

  // Initialize sensor fusion library
  fused_vector = vector_3d_initialize(0.0,0.0,-1.0);
  q_acc = quaternion_initialize(1.0,0.0,0.0,0.0);
  Start = millis();
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

  wx = 0.0005323*gyX;
  wy = 0.0005323*gyY;
  wz = 0.0005323*gyZ;

  float delta = 0.001*(millis()-Start);
  fused_vector = update_fused_vector(fused_vector,acX,acY,acZ,wx,wy,wz,delta);
  q_acc = quaternion_from_accelerometer(fused_vector.a,fused_vector.b,fused_vector.c);
  angles = quaternion_to_euler_angles(q_acc);
  Start = millis();

  // Offset 

  //Serial.print("AcX:"); Serial.print(acX);
  //Serial.print(" AcY:"); Serial.print(acY);
  //Serial.print(" AcZ:"); Serial.print(acZ);
  //Serial.print(" Tmp:"); Serial.print(tmp);

  /*
  // Print Euler angles
  //Serial.print(" GyX:"); Serial.print(gyX);
  //Serial.print(" GyY:"); Serial.print(gyY);
  //Serial.print(" GyZ:"); Serial.print(gyZ);
  */

  /*
  // Print Quaternion coefficients
  // q_acc = a + bi + cj + dk
  Serial.print(" Qa:"); Serial.print(q_acc.a);
  Serial.print(" Qb:"); Serial.print(q_acc.b);
  Serial.print(" Qc:"); Serial.print(q_acc.c);
  Serial.print(" Qd:"); Serial.print(q_acc.d);
  */

  // Adjusts readings so the hand's resting position is 0 degrees.
  // Rotating right increases from 0->90 degrees.
  // Rotating left decreases from 0->-90 degrees.
  float corrRoll = angles.roll + 180.0f;
  if (corrRoll > 180.0f)
  {
    corrRoll -= 360.0f; 
  }
  
  Serial.print(" Pitch:"); Serial.print(int(angles.pitch));
  Serial.print(" Roll:"); Serial.print(int(angles.roll));
  Serial.print(" CorrectedRoll:"); Serial.print(int(corrRoll));
  Serial.print("\n");
  delay(1);
}
