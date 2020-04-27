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

// Debug symbol
#define DEBUG

#include "ToolBox.h"
#include "MagneticSensor.h"
#include "MotionSensor.h"

#define LICENSE F(" DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE \n                     Version 2, December 2004 \n Copyright (C) 2020 Kawakami Shuta <rivertop.osamuta@gmail.com> \n Everyone is permitted to copy and distribute verbatim or modified \n copies of this license document, and changing it is allowed as long \n as the name is changed. \n            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE \n TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION \n 0. You just DO WHAT THE FUCK YOU WANT TO.\n\n")
#define DATA_FORMAT F("Time[milliSec],Temperature sensored by magnetic sensor[C],True azimuth XY[°],True azimuth XZ[°],True azimuth YZ[°],Temperature sensored by motion sensor[C],Gyro sensor X[°],Gyro sensor Y[°],Gyro sensor Z[°],Acceleration X[Meter Per Second Sqaure],Acceleration Y[Meter Per Second Sqaure],Acceleration Z[Meter Per Second Sqaure],\n")


class Interface
{
  private:
    Stopwatch _stopwatch;
    DigitalPin _button;
    DigitalPin _led;
    boolean _ledState;

  public:
    Interface() {
      _ledState = false;
    }

    void initialize()
    {
      _button.initialize(9, INPUT_PULLUP);
      _led.initialize(8, OUTPUT);
      _stopwatch.startMillis();
    }

    boolean isButtonPressed()
    {
      if (!_button.read())
      {
        if (_stopwatch.lapMillis() > 1000)
        {
          _stopwatch.stopMillis();
          _stopwatch.startMillis();
          return true;
        } else
          return false;
      } else
        return false;
    }

    void LED(boolean ledState) {
      _ledState = ledState;
      _led.write(ledState);
    }

    void blinkLED()
    {
      if (_stopwatch.lapMillis() > 583)
      {
        _ledState = !_ledState;
        _led.write(_ledState);
        _stopwatch.stopMillis();
        _stopwatch.startMillis();
      }
    }
};

class CanSatellite
{
  private:
    Stopwatch _timStamp;
    I2C _i2c;
    Storage _storage;
    MagneticSensor _magneticSensor;
    MotionSensor _motionSensor;
    String _fileName;
    const String TOKEN;

  public:
    CanSatellite() : TOKEN(",") {}
    Error initialize()
    {
      _i2c.initialize(400000);

      if (!_magneticSensor.initialize(&_i2c, DATARATE_200HZ, SAMPLES_512, RANGE_2GA).isSucceeded())
        return Error(FATAL_ERROR, F("FATAL_ERROR : Error occured in initializing magnetic sensor!\n"));

      if (!_motionSensor.initialize(&_i2c, GYRO_FULL_SCALE_RANGE_2000_DEGEREE_PER_SEC, ACCEL_FULL_SCALE_RANGE_16G).isSucceeded())
        return Error(FATAL_ERROR, F("FATAL_ERROR : Error occured in initializing motion sensor!\n"));

      if (!_storage.initialize(&SD, 10).isSucceeded())
        return Error(FATAL_ERROR, F("FATAL_ERROR : Error occured in initializing micro SD card!\n"));

      return Error(SUCCEEDED, F("SUCCEEDED : Can satellite was initialized correctly.\n"));
    }

    Error prepare()
    {
      size_t size = String(DATA_FORMAT).length();
      for (int i = 0; SD.exists(_fileName = String(F("DATA_")) + i + String(F(".CSV"))); i++) {}

      if (_storage.writeBOM(_fileName) != 3)
        return Error(FATAL_ERROR, F("FATAL_ERROR : Error occured in writing Byte Order Mark!\n"));
      if (_storage.printString(_fileName, DATA_FORMAT) != size)
        return Error(FATAL_ERROR, F("FATAL_ERROR : Error occured in writing Data format!\n"));

      _timStamp.startMillis();
      return Error(SUCCEEDED, F("SUCCEEDED : Preparing for recording was finished correctly.\n"));
    }

    Error record()
    {
      String data;
      unsigned long timestamp;
      float temperature1, temperature2;
      Angle xy, xz, yz;
      XYZ gyro, accel;

      timestamp = _timStamp.lapMillis();
      temperature1 = _magneticSensor.getTemperature();
      _magneticSensor.getTrueAzimuth(&xy, &xz, &yz);
      temperature2 = _motionSensor.getTemperature();
      gyro = _motionSensor.getGyroXYZDegreePerSec();
      accel = _motionSensor.getAccelerationXYZMeterPerSecSquared();

      data.reserve(240);

      data += timestamp; data += TOKEN;
      data += temperature1; data += TOKEN;
      data += xy.getAngleDegree(); data += TOKEN;
      data += xz.getAngleDegree(); data += TOKEN;
      data += yz.getAngleDegree(); data +=  TOKEN;
      data += temperature2; data += TOKEN;
      data += gyro.x; data +=  TOKEN;
      data += gyro.y; data +=  TOKEN;
      data += gyro.z; data +=  TOKEN;
      data += accel.x; data +=  TOKEN;
      data += accel.y; data +=  TOKEN;
      data += accel.z; data +=  "\n";

      if (_storage.printString(_fileName, data) == data.length())
        return Error(SUCCEEDED, F("SUCCEEDED : wrote data correctly.\n"));
      else
        return Error(FATAL_ERROR, F("FATAL_ERROR : Error occured in writing data!\n"));
    }
};

Interface interface;
CanSatellite canSat;
boolean errored = false;
boolean logging = false;

void setup()
{
#ifdef DEBUG
  Serial.begin(9600);
  Serial.print(LICENSE);
#endif
  interface.initialize();
  if (!canSat.initialize().isSucceeded())
    errored = true;
}

void loop()
{
  if (!errored)
  {
    if (interface.isButtonPressed())
    {
      if (!logging)
      {
        if (!canSat.prepare().isSucceeded())
          errored = true;
      }

      logging = !logging;
    }
    if (logging)
    {
      if (!canSat.record().isSucceeded())
        errored = true;
    }
    interface.LED(logging);
  }
  else
  {
    interface.blinkLED();
  }
}
