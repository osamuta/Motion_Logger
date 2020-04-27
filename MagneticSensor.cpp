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

#include "MagneticSensor.h"

Error MagneticSensor::initialize(I2C *i2c,
                                   MagneticSensorDataRate rate,
                                   MagneticSensorSample sample,
                                   MagneticSensorRange range)
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
  
  // begin wiring to magnetic sensor
  if (!i2c->isSlaveConnected(QMC5883_ADDRESS))
  {
    return Error(FATAL_ERROR, F("FATAL_ERROR : QMC5883 is not available!\n"));;
  }
  if (_i2c->read1Byte(QMC5883_ADDRESS, QMC5883_ADDRESS) != 0b11111111)
  {
    return Error(FATAL_ERROR, F("FATAL_ERROR : QMC58837's register is not available!\n "));
  }
  _i2c->write1Byte(QMC5883_ADDRESS, REG_SETRESET, 1);
  
  // configure sensor
  configure(rate, sample, range);
  
  return Error(SUCCEEDED, F("SUCCEEDED : QMC5883 was initialized correctly.\n"));;
}

void MagneticSensor::configure(MagneticSensorDataRate rate,
                    MagneticSensorSample sample,
                    MagneticSensorRange range)
{
  _i2c->write1Byte(QMC5883_ADDRESS, REG_CONFIG_1, sample << 6 | range << 4 | rate << 2 | 0b01);
  if (range == RANGE_2GA)
    _sensitivity = SENSITIVITY_2G_LSB_PER_GAUSE;
   else
    _sensitivity = SENSITIVITY_8G_LSB_PER_GAUSE;
}

bool MagneticSensor::isReadyToRead()
{
  uint8_t status = _i2c->read1Byte(QMC5883_ADDRESS, REG_STATUS);
  return status & 1;
}

void MagneticSensor::getXYZRaw(int *x, int *y, int *z)
{
  uint8_t data[6];
  _i2c->readArray(QMC5883_ADDRESS, REG_OUT_X_L, 6, data);
  *x = (data[0] | data[1] << 8) + _offset_x;
  *y = (data[2] | data[3] << 8) + _offset_y;
  *z = (data[4] | data[5] << 8) + _offset_z;
}

XYZ MagneticSensor::getXYZnT()
{
  XYZ xyz;
  int x, y, z;  
  getXYZRaw(&x, &y, &z);
  xyz.x = x;
  xyz.y = y;
  xyz.z = z;
  xyz.x = xyz.x / _sensitivity;
  xyz.y = xyz.y / _sensitivity;
  xyz.z = xyz.z / _sensitivity;
  return  xyz;
}

Angle MagneticSensor::getMagneticAzimuthXY()
{
  int x, y, z;
  getXYZRaw(&x, &y, &z);
  float angle = atan2(y, x);
  if (angle < 0)
    angle = angle + 2* PI;
  return Angle(angle); 
}

Angle MagneticSensor::getMagneticAzimuthXZ()
{
  int x, y, z;
  getXYZRaw(&x, &y, &z);
  float angle = atan2(z, x);
  if (angle < 0)
    angle = angle + 2* PI;
  return Angle(angle); 
}

Angle MagneticSensor::getMagneticAzimuthYZ()
{
  int x, y, z;
  getXYZRaw(&x, &y, &z);
  float angle = atan2(z, y);
  if (angle < 0)
    angle = angle + 2* PI;
  return Angle(angle); 
}

Angle MagneticSensor::getTrueAzimuthXY()
{
  int x, y, z;
  getXYZRaw(&x, &y, &z);
  float angle = atan2(y, x) + MAGNETIC_DECLINATION;
  if (angle < 0)
    angle = angle + 2* PI;
  if (angle > 2 * PI)
    angle = angle - 2* PI;
  return Angle(angle); 
}

Angle MagneticSensor::getTrueAzimuthXZ()
{
  int x, y, z;
  getXYZRaw(&x, &y, &z);
  float angle = atan2(z, x) + MAGNETIC_DECLINATION;
  if (angle < 0)
    angle = angle + 2* PI;
  if (angle > 2 * PI)
    angle = angle - 2* PI;
  return Angle(angle); 
}

Angle MagneticSensor::getTrueAzimuthYZ()
{
  int x, y, z;
  getXYZRaw(&x, &y, &z);
  float angle = atan2(z, y) + MAGNETIC_DECLINATION;
  if (angle < 0)
    angle = angle + 2* PI;
  if (angle > 2 * PI)
    angle = angle - 2* PI;
  return Angle(angle); 
}

void MagneticSensor::getTrueAzimuth(Angle *xy, Angle *xz, Angle *yz)
{
  int x, y, z;
  float angle;
  getXYZRaw(&x, &y, &z);
  angle = atan2(y, x) + MAGNETIC_DECLINATION;
  if (angle < 0)
    angle = angle + 2* PI;
  if (angle > 2 * PI)
    angle = angle - 2* PI;
  xy->setAngleRadian(angle);
  angle = atan2(z, x) + MAGNETIC_DECLINATION;
  if (angle < 0)
    angle = angle + 2* PI;
  if (angle > 2 * PI)
    angle = angle - 2* PI;
  xz->setAngleRadian(angle);
  angle = atan2(z, y) + MAGNETIC_DECLINATION;
  if (angle < 0)
    angle = angle + 2* PI;
  if (angle > 2 * PI)
    angle = angle - 2* PI;
  yz->setAngleRadian(angle);
}

int MagneticSensor::getTemperatureRaw()
{
  uint8_t data[2];
  _i2c->readArray(QMC5883_ADDRESS, REG_TEMPERATURE_L, 2, data);
  return data[0] | data[1] << 8;
}

float MagneticSensor::getTemperature()
{
  return getTemperatureRaw();
}
