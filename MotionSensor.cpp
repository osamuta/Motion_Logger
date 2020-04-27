// DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
//                     Version 2, December 2004 
// 
// Copyright (C) 2020 Kawakami Shuta <rivertop.osamuta@gmail.com> 
// 
// Everyone is permitted to copy and distribute verbatim or modified 
// copies of this license document, and changing it is allowed as long 
// as the name is changed. 
// 
//            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE 
// TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION 
// 
// 0. You just DO WHAT THE FUCK YOU WANT TO.

#include "MotionSensor.h"

Error MotionSensor::initialize(I2C *i2c, GyroFullScaleRange gyroFullScaleRange, AccelFullScaleRange accelFullScaleRange)
{
  if (!i2c)
  { 
    return Error(FATAL_ERROR, F("FATAL_ERROR : I2C pointer is NULL!\n"));
  }
  if (!i2c->isInitialized())
  {
    return Error(FATAL_ERROR, F("FATAL_ERROR : I2C was not initialized!\n"));
  }
  _i2c = i2c;

  // begin wiring to sensor
  if (!i2c->isSlaveConnected(MPU6050_ADRESS))
  {
    return Error(FATAL_ERROR, F("FATAL_ERROR : MPU6050 is not available!\n"));;
  }
  if (_i2c->read1Byte(MPU6050_ADRESS, REG_WHO_AM_I) != 0x68)
  {
    return Error(FATAL_ERROR, F("FATAL_ERROR : MPU6050's register is not available!\n "));
  }
  
  // configure sensor
  configure(gyroFullScaleRange, accelFullScaleRange);
  
  return Error(SUCCEEDED, F("SUCCEEDED : MPU6050  was initialized correctly.\n"));
}

void MotionSensor::configure(GyroFullScaleRange gyroFullScaleRange, AccelFullScaleRange accelFullScaleRange)
{
  _i2c->write1Byte(MPU6050_ADRESS, REG_EXT_SYNC_SET_DLPF_CFG, 0);
  _i2c->write1Byte(MPU6050_ADRESS, REG_XYZG_ST_GFS_SEL, gyroFullScaleRange << 3);
  _i2c->write1Byte(MPU6050_ADRESS, REG_XYZA_ST_AFS_SEL, accelFullScaleRange << 3);
  _i2c->write1Byte(MPU6050_ADRESS, REG_I2C_MASTER_CLOCK_SPEED, 13);
  _i2c->write1Byte(MPU6050_ADRESS, REG_PWR_MGMT_1, 0);

  switch (gyroFullScaleRange)
  {
    case GYRO_FULL_SCALE_RANGE_250_DEGEREE_PER_SEC:
      _gyroSensitivity = GYRO_SENSITIVITY_250_DEGEREE_PER_SEC;
      break;
      
    case GYRO_FULL_SCALE_RANGE_500_DEGEREE_PER_SEC:
      _gyroSensitivity = GYRO_SENSITIVITY_500_DEGEREE_PER_SEC;
      break;
      
    case GYRO_FULL_SCALE_RANGE_1000_DEGEREE_PER_SEC:
      _gyroSensitivity = GYRO_SENSITIVITY_1000_DEGEREE_PER_SEC;
      break;
      
    case GYRO_FULL_SCALE_RANGE_2000_DEGEREE_PER_SEC:
      _gyroSensitivity = GYRO_SENSITIVITY_2000_DEGEREE_PER_SEC;
      break;
      
    default:
      _gyroSensitivity = GYRO_SENSITIVITY_250_DEGEREE_PER_SEC;
      break;
      
  }

  switch (accelFullScaleRange)
  {
    case ACCEL_FULL_SCALE_RANGE_2G:
      _accelSensitivity = ACCEL_SENSITIVITY_2G;
      break;
      
    case ACCEL_FULL_SCALE_RANGE_4G:
      _accelSensitivity = ACCEL_SENSITIVITY_4G;
      break;
      
    case ACCEL_FULL_SCALE_RANGE_8G:
      _accelSensitivity = ACCEL_SENSITIVITY_8G;
      break;
      
    case ACCEL_FULL_SCALE_RANGE_16G:
      _accelSensitivity = ACCEL_SENSITIVITY_16G;
      break;
      
    default:
      _accelSensitivity = ACCEL_SENSITIVITY_2G;
      break;
  }
}

void MotionSensor::getGyroXYZRaw(int *x, int *y, int *z)
{
  uint8_t data[6];
  _i2c->readArray(MPU6050_ADRESS, REG_GYRO_XOUT_H, 6, data);
  *x = data[0] << 8 | data[1];
  *y = data[2] << 8 | data[3];
  *z = data[4] << 8 | data[5];
}

void MotionSensor::getAccelerationXYZRaw(int *x, int *y, int *z)
{
  uint8_t data[6];
  _i2c->readArray(MPU6050_ADRESS, REG_ACCEL_XOUT_H, 6, data);
  *x = data[0] << 8 | data[1];
  *y = data[2] << 8 | data[3];
  *z = data[4] << 8 | data[5];
}

XYZ MotionSensor::getGyroXYZDegreePerSec()
{
  XYZ xyz;
  int x, y, z;  
  getGyroXYZRaw(&x, &y, &z);
  xyz.x = x;
  xyz.y = y;
  xyz.z = z;
  xyz.x = xyz.x / _accelSensitivity;
  xyz.y = xyz.y / _accelSensitivity;
  xyz.z = xyz.z / _accelSensitivity;
  return  xyz;
}

XYZ MotionSensor::getAccelerationXYZMeterPerSecSquared()
{
  XYZ xyz;
  int x, y, z;  
  getAccelerationXYZRaw(&x, &y, &z);

  xyz.x = x;
  xyz.y = y;
  xyz.z = z;
  xyz.x = xyz.x / _accelSensitivity * GRAVITATIONAL_ACCELERATION;
  xyz.y = xyz.y / _accelSensitivity * GRAVITATIONAL_ACCELERATION;
  xyz.z = xyz.z / _accelSensitivity * GRAVITATIONAL_ACCELERATION;
  return  xyz;
}

int MotionSensor::getTemperature()
{
  uint8_t data[2]; int result;
  _i2c->readArray(MPU6050_ADRESS, REG_TEMPERATURE_H, 2, data);
  result =  data[0] << 8 | data[1];
  return result;
}
