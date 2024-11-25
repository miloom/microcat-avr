/****************************************************************
  Example code demonstrating the use of the Arduino Library for
  the SparkFun MiniMoto board, which uses the TI DRV8830 IC for I2C
  low-voltage DC motor control.

  This code is beerware; if you use it, please buy me (or any other
  SparkFun employee) a cold beverage next time you run into one of
  us at the local.

  17 Sep 2013- Mike Hord, SparkFun Electronics

  Code developed in Arduino 1.0.5, on a Fio classic board.

**Updated for Arduino 1.6.4 5/2015**
****************************************************************/

// Reworked 10.11.2024 by Mikk Loomets
#pragma once

#include <Arduino.h>

// I2C support constants
constexpr uint8_t I2C_READ =    0x01; // I2C read bit set
// Some values we'll load into TWCR a lot
constexpr uint8_t NEXT_BYTE =   0xC4; // (1<<TWINT) | (1<<TWEA) | (1<<TWEN)
constexpr uint8_t START_COND = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
constexpr uint8_t CLEAR_TWINT = (1 << TWINT);
constexpr uint8_t STOP_COND = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);

// Fault constants
constexpr uint8_t FAULT = 0x01;
constexpr uint8_t ILIMIT = 0x10;
constexpr uint8_t OTS = 0x08;
constexpr uint8_t UVLO = 0x04;
constexpr uint8_t OCP = 0x02;

class MiniMoto
{
  public:
    void initialize(byte addr);
    void drive(int speed);
    void stop();
    void brake();
    byte getFault();
  private:
    void I2CWriteBytes(byte addr, const byte *buffer, byte len) const;
    void I2CReadBytes(byte addr, byte *buffer, byte len);
    byte _addr{};
};