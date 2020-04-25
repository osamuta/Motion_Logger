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

#ifndef _I2C_H
#define _I2C_H

#include <Arduino.h>
#include <Wire.h>

class I2C
{
  private:
    uint32_t _clock;
    boolean _initialized;

  public:
    I2C();
    // it calls initialized() inside
    I2C(uint32_t clockHz);

    //
    // must call it first
    void initialize(uint32_t clockHz);

    uint32_t getClockHz() { return _clock; }
    boolean isInitialized() { return _initialized; }

    //
    // writer
    size_t writeArray(uint8_t slaveAdress,  uint8_t registerAdress, uint8_t *data, size_t size);
    size_t write1Byte(uint8_t slaveAdress, uint8_t registerAdress, uint8_t data)
    {
      return writeArray(slaveAdress, registerAdress, &data, 1);
    }

    //
    // reader
    void readArray(uint8_t slaveAdress,  uint8_t registerAdress, size_t size, uint8_t *output);
    uint8_t read1Byte(uint8_t slaveAdress,  uint8_t registerAdress)
    {
      uint8_t result;
      readArray(slaveAdress, registerAdress, 1, &result);
      return result;
    }

    //
    // for debugging
    void registerDump(uint8_t slaveAdress, size_t size);
};

#endif
