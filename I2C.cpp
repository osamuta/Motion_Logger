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

#include "I2C.h"

I2C::I2C()
{
  _clock = 0;
  _initialized = false;
}

I2C::I2C(uint32_t clockHz)
{
  initialize(clockHz);
}

void I2C::initialize(uint32_t clockHz)
{
  _clock = clockHz;
  Wire.begin();
  Wire.setClock(_clock); _initialized = true;
}

size_t I2C::writeArray(uint8_t slaveAdress,  uint8_t registerAdress, uint8_t *data, size_t size)
{
  size_t result = 0;
  Wire.beginTransmission(slaveAdress);
  Wire.write(registerAdress);
  result = Wire.write(data, size);
  Wire.endTransmission();
  return result;
}

void I2C::readArray(uint8_t slaveAdress,  uint8_t registerAdress, size_t size, uint8_t *output)
{
  Wire.beginTransmission(slaveAdress);
  Wire.write(registerAdress);
  Wire.endTransmission(false);
  Wire.requestFrom(slaveAdress, size);
  while ((size_t)Wire.available() < size) ;
  for (size_t i = 0; i < size; i++)
  {
    output[i] = Wire.read();
  }
  Wire.endTransmission(true);
}

void I2C::registerDump(uint8_t slaveAdress, size_t size)
{
  uint8_t *data;
  data = new uint8_t[size];
  readArray(slaveAdress, 0, size, data);
  Serial.print("register dump\n");
  for (size_t i = 0; i < size; i++)
  {
    Serial.print(i, HEX);
    Serial.print(" ");
    Serial.print(data[i], HEX);
    Serial.print("\n");
  }
  delete data;
}
