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

// The MagneticSensor class was developed for QMC5883

#ifndef MAGNETIC_SENSOR_H
#define MAGNETIC_SENSOR_H

#include "ToolBox.h"
#include "I2C.h"

typedef enum
{
  DATARATE_10HZ        = 0b00,
  DATARATE_50HZ        = 0b01,
  DATARATE_100HZ       = 0b10,
  DATARATE_200HZ       = 0b11 //best
} MagneticSensorDataRate;

typedef enum
{
  SAMPLES_64     = 0b11,
  SAMPLES_128    = 0b10,
  SAMPLES_256    = 0b01,
  SAMPLES_512    = 0b00 // best
} MagneticSensorSample;

typedef enum
{
  RANGE_2GA     = 0b00,
  RANGE_8GA     = 0b01 // best
} MagneticSensorRange;

typedef enum
{
  STANDBY        = 0b00,
  CONTINOUS     = 0b01 //best
} MagneticSensorMode;

class MagneticSensor
{
  private:
    // constants
    static const uint8_t QMC5883_ADDRESS      = 0x0D;
    static const uint8_t REG_OUT_X_H          = 0x01;
    static const uint8_t REG_OUT_X_L          = 0x00;
    static const uint8_t REG_OUT_Z_H          = 0x05;
    static const uint8_t REG_OUT_Z_L          = 0x04;
    static const uint8_t REG_OUT_Y_H          = 0x03;
    static const uint8_t REG_OUT_Y_L          = 0x02;
    static const uint8_t REG_STATUS           = 0x06;
    static const uint8_t REG_TEMPERATURE_H    = 0x08;
    static const uint8_t REG_TEMPERATURE_L    = 0x07;
    static const uint8_t REG_CONFIG_1         = 0x09;
    static const uint8_t REG_CONFIG_2         = 0x0A;
    static const uint8_t REG_SETRESET         = 0x0B;
    static const uint8_t REG_IDENT_C          = 0x20;
    static const uint8_t REG_IDENT_D          = 0x21;
    
    static const float SENSITIVITY_2G_LSB_PER_GAUSE = 12;
    static const float SENSITIVITY_8G_LSB_PER_GAUSE = 3;
    
    static const float MAGNETIC_DECLINATION = 0.013587116;
    static const float GEOMAGNETISM_nT = 46511;
    

    // value
    I2C *_i2c;
    unsigned int _sensitivity;
    int _offset_x; // +-120 +-30
    int _offset_y;
    int _offset_z;

  public:
    MagneticSensor() { _i2c = NULL; _sensitivity = 0; _offset_x = 0; _offset_y = 0; _offset_z = 0; }
    // it call initialize() inside
    MagneticSensor(I2C *i2c,
                    MagneticSensorDataRate rate,
                    MagneticSensorSample sample,
                    MagneticSensorRange range) { initialize(i2c, rate, sample, range); }
    //
    // must call it first
    Error initialize(I2C *i2c,
                    MagneticSensorDataRate rate,
                    MagneticSensorSample sample,
                    MagneticSensorRange range);

    //
    // configure magnetic sensor
    void configure(MagneticSensorDataRate rate,
                    MagneticSensorSample sample,
                    MagneticSensorRange range);

    // check status
    bool isReadyToRead();
    
    //
    // data getter
    void getXYZRaw(int *x, int *y, int *z);
    XYZ getXYZnT();

    Angle getMagneticAzimuthXY();
    Angle getMagneticAzimuthXZ();
    Angle getMagneticAzimuthYZ();
    Angle getTrueAzimuthXY();
    Angle getTrueAzimuthXZ();
    Angle getTrueAzimuthYZ();
    void getTrueAzimuth(Angle *xy, Angle *xz, Angle *yz);
    int getTemperatureRaw();
    float getTemperature(); // unperfact
};

#endif
