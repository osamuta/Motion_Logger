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

// The MotionSensor class was developed for MPU6050

#ifndef _MOTION_SENSOR_H
#define _MOTION_SENSOR_H

#include "ToolBox.h"
#include "I2C.h"

enum GyroFullScaleRange
{
  GYRO_FULL_SCALE_RANGE_250_DEGEREE_PER_SEC = 0,
  GYRO_FULL_SCALE_RANGE_500_DEGEREE_PER_SEC = 1,
  GYRO_FULL_SCALE_RANGE_1000_DEGEREE_PER_SEC = 2,
  GYRO_FULL_SCALE_RANGE_2000_DEGEREE_PER_SEC = 3,
};

enum AccelFullScaleRange
{
  ACCEL_FULL_SCALE_RANGE_2G = 0,
  ACCEL_FULL_SCALE_RANGE_4G = 1,
  ACCEL_FULL_SCALE_RANGE_8G = 2,
  ACCEL_FULL_SCALE_RANGE_16G = 3,
};

class MotionSensor
{
  private:
    static const uint8_t MPU6050_ADRESS                   = 0x68;
    static const uint8_t REG_EXT_SYNC_SET_DLPF_CFG        = 0x1A;
    static const uint8_t REG_XYZG_ST_GFS_SEL              = 0x1B;
    static const uint8_t REG_XYZA_ST_AFS_SEL              = 0x1C;
    static const uint8_t REG_TEMP_XYZG_ACCEL_FIFO_ENABLE  = 0x23;
    static const uint8_t REG_I2C_MASTER_CLOCK_SPEED       = 0x24;
    static const uint8_t REG_ACCEL_XOUT_H                 = 0x3B;
    static const uint8_t REG_TEMPERATURE_H                = 0x41;
    static const uint8_t REG_GYRO_XOUT_H                  = 0x43;
    static const uint8_t REG_PWR_MGMT_1                   = 0x6B;
    static const uint8_t REG_WHO_AM_I                     = 0x75;
    
    static constexpr float GYRO_SENSITIVITY_250_DEGEREE_PER_SEC = 131;
    static constexpr float GYRO_SENSITIVITY_500_DEGEREE_PER_SEC = 65.5;
    static constexpr float GYRO_SENSITIVITY_1000_DEGEREE_PER_SEC = 32.8;
    static constexpr float GYRO_SENSITIVITY_2000_DEGEREE_PER_SEC = 16.4;

    static constexpr float ACCEL_SENSITIVITY_2G = 16384;
    static constexpr float ACCEL_SENSITIVITY_4G = 8192;
    static constexpr float ACCEL_SENSITIVITY_8G = 4096;
    static constexpr float ACCEL_SENSITIVITY_16G = 2048;

    static constexpr float GRAVITATIONAL_ACCELERATION = 9.80665;
    
    I2C *_i2c;
    float _gyroSensitivity;
    float _accelSensitivity;

  public:
    MotionSensor() { _i2c = NULL; _gyroSensitivity = 0; _accelSensitivity = 0; }
    // it call initialize() inside
    MotionSensor(I2C *i2c, GyroFullScaleRange gyroFullScaleRange, AccelFullScaleRange accelFullScaleRange) { initialize(i2c, gyroFullScaleRange, accelFullScaleRange); }

    //
    // must call it first
    Error initialize(I2C *i2c, GyroFullScaleRange gyroFullScaleRange, AccelFullScaleRange accelFullScaleRange);
    
    //
    // configure sensor
    void configure(GyroFullScaleRange gyroFullScaleRange, AccelFullScaleRange accelFullScaleRange);


    //
    // date getter
    void getGyroXYZRaw(int *x, int *y, int *z);
    void getAccelerationXYZRaw(int *x, int *y, int *z);
    XYZ getGyroXYZDegreePerSec();
    XYZ getAccelerationXYZMeterPerSecSquared();
    int getTemperature(); // unperfect
};

#endif
