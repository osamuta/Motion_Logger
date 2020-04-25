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

#ifndef _TOOLBOX_H
#define _TOOLBOX_H

#include <stdarg.h>
#include <stdio.h>
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>

struct XYZ
{
  float x;
  float y;
  float z;
};

class Communication
{  
  public:
    Communication() {}
    Communication(unsigned long baud, unsigned long timeout) { initialize(baud, timeout); }
    virtual ~Communication() {}

    void initialize(unsigned long baud, unsigned long timeout)
    {
      Serial.begin(baud);
      Serial.setTimeout(timeout);
    }


    //
    // method
    void sendMessage(String message) { Serial.print(message); }
    // maximum length of message is 256
    void sendMessagef(String  format, ...)
    {
      va_list args;
      char result[256];
      va_start(args, format);
      vsnprintf(result, sizeof(result), format.c_str(), args);
      sendMessage(result);
      va_end(args);
    }

    String recieveMessage()
    {
      if (isReadable())
      {
        return Serial.readString();
      }
      else
      {
        return String("");
      }
    }

    boolean isReadable() { return Serial.available() > 0; }
    
};

class DigitalPin
{
  private:
    uint8_t _pin;
    uint8_t _mode;

  public:
    DigitalPin() { _pin = 0; _mode = 0; }
    DigitalPin(uint8_t pin, uint8_t mode) : DigitalPin() { initialize(pin, mode); }


    //
    // method
    void initialize(uint8_t pin, uint8_t mode)
    {
      _pin = pin; _mode = mode;
      pinMode(_pin, _mode);
    }

    int read() { return digitalRead(_pin); }
    void write(uint8_t value) { digitalWrite(_pin, value); }
};

class Time
{
  public:
    Time() {}

    //
    // method
    unsigned long timeMillis() { return millis(); }
    unsigned long timeMicros() { return micros(); }
    void delayMillis(unsigned long millis) { delay(millis); }
    void delayMicros(unsigned int us) { delayMicroseconds(us); } // Maximum value is  16383.
};

class Stopwatch : public Time
{
  private:
    unsigned long _startMillis;
    unsigned long _startMicros;
    void reset() { _startMillis = 0; _startMicros = 0; }

  public:
    Stopwatch() { _startMillis = 0; _startMicros = 0; }

    //
    // method
    void startMillis() { reset(); _startMillis = Time::timeMillis(); }
    void startMicros() { reset(); _startMicros = Time::timeMicros(); }

    unsigned long lapMillis() { return Time::timeMillis() - _startMillis; }
    unsigned long lapMicros() { return Time::timeMicros() - _startMicros; }

    unsigned long stopMillis() { unsigned long temp = lapMillis(); reset(); return temp; }
    unsigned long stopMicros() { unsigned long temp = lapMicros(); reset(); return temp; }
};

enum ErrorCode
{
  SUCCEEDED,
  FATAL_ERROR,
  ERROR,
  WARNING,
};

class Error
{
  private:
    ErrorCode _errorCode;
    String _message;

  public:
    Error() { _errorCode = FATAL_ERROR; _message = "no message\n"; }
    Error(const Error &error) { _errorCode = error.getErrorCode(); _message = error.getMessage(); }
    #ifdef DEBUG
    Error(ErrorCode errorCode, const String message) { _errorCode = errorCode; _message = message; Serial.print(_message); }
    #else
    Error(ErrorCode errorCode, const String message) : _errorCode(errorCode), _message(message) {}
    #endif
    boolean isSucceeded() const { return _errorCode == SUCCEEDED; }
    ErrorCode getErrorCode() const { return _errorCode; }
    String getMessage() const { return _message; }
    Error sendStatus(){ Serial.print(_message); return *this; }
};

class Angle
{
  private:
    float _radian;

  public:
    Angle() { _radian = 0; }
    Angle(float radian) { _radian = radian; }
    Angle(const Angle &angle) { if (&angle != this) { _radian = angle.getAngleRadian(); } }
    Angle& operator=(const Angle &angle) { if (&angle != this) { _radian = angle.getAngleRadian(); }  return *this; }    

    float getAngleRadian() const { return _radian; }
    float getAngleDegree() const { return _radian * 180 / PI; }

    void setAngleRadian(float radian) { _radian = radian; }
    void setAngleDegree(float degree) { _radian = degree * PI / 180; }
};

class Storage
{
  private:
    SDClass *_sd;

  public:
    Storage() { _sd = NULL; }
    
    Error initialize(SDClass *sd, uint8_t pin)
    {
      if (!sd)
      {
        return Error(FATAL_ERROR, F("FATAL_ERROR : The micro SD card was not initialized!\n"));
      }
      _sd = sd;
      if (!_sd->begin(pin))
      {
        return Error(FATAL_ERROR, F("FATAL_ERROR : The micro SD card is not available!\n"));
      }
      return Error(SUCCEEDED, F("SUCCEEDED : The micro SD card  was initialized correctly.\n"));
    }

    size_t writeBOM(String filePath)
    {
      static constexpr uint8_t BYTE_ORDER_MARK[4] = { 0xEF, 0xBB, 0xBF };
      File dataFile = _sd->open(filePath, FILE_WRITE);
      size_t result = dataFile.write(BYTE_ORDER_MARK, 3); 
      dataFile.close();
      return result;
    }
    size_t printString(String filePath, String data)
    {
      File dataFile = _sd->open(filePath, FILE_WRITE);
      size_t result = dataFile.print(data); 
      dataFile.close();
      return result;
    }
    
};

#endif
