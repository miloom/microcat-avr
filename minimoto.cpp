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


#include <Arduino.h>
#include "minimoto.h"
#include "log.h"

// The address of the part is set by a jumper on the board.
//  See datasheet or schematic for details; default is 0xD0.


void MiniMoto::initialize(byte addr) {
    // 60,61,63,64 -> C0, C2, C6, C8

    if (addr == 0x60) {
        _addr = 0xC0;
    } else if (addr == 0x61) {
        _addr = 0xC2;
    } else if (addr == 0x63) {
        _addr = 0xC6;
    } else if (addr == 0x64) {
        _addr = 0xC8;
    } else {
        _addr = addr;
    }
    //    _addr = addr;


    // This sets the bit rate of the bus; I want 100kHz. See the
    //  datasheet for details on how I came up with this value.
    TWBR = 72;
}

// Return the fault status of the DRV8830 chip. Also clears any existing faults.
byte MiniMoto::getFault() {
    byte buffer = 0;
    byte clearFault = 0x80;
    I2CReadBytes(0x01, &buffer, 1);
    I2CWriteBytes(0x01, &clearFault, 1);
    return buffer;
}

// Send the drive command over I2C to the DRV8830 chip. Bits 7:2 are the speed
//  setting; range is 0-63. Bits 1:0 are the mode setting:
//  - 00 = HI-Z
//  - 01 = Reverse
//  - 10 = Forward
//  - 11 = H-H (brake)
void MiniMoto::drive(int speed) {
    byte regValue = 0x80; // Before we do anything, we'll want to
    //  clear the fault status. To do that
    //  write 0x80 to register 0x01 on the
    //  DRV8830.
    //I2CWriteBytes(0x01, &regValue, 1); // Clear the fault status.
    regValue = (byte) abs(speed); // Find the byte-ish abs value of the input
    if (regValue > 63) { regValue = 63; } // Cap the value at 63.
    regValue = regValue << 2; // Left shift to make room for bits 1:0
    if (speed < 0) { regValue |= 0b01; } // Set bits 1:0 based on sign of input.
    else { regValue |= 0b10; }

    I2CWriteBytes(0x00, &regValue, 1);
}

// Coast to a stop by hi-z'ing the drivers.
void MiniMoto::stop() {
    byte regValue = 0; // See above for bit 1:0 explanation.

    I2CWriteBytes(0x00, &regValue, 1);
}

// Stop the motor by providing a heavy load on it.
void MiniMoto::brake() {
    byte regValue = 0x03; // See above for bit 1:0 explanation.

    I2CWriteBytes(0x00, &regValue, 1);
}

// Private function that reads some number of bytes from the accelerometer.
void MiniMoto::I2CReadBytes(byte addr, byte *buffer, byte len) {
    byte temp = 0;
    // First, we need to write the address we want to read from, so issue a write
    //  with that address. That's two steps: first, the slave address:
    TWCR = START_COND; // Send a start condition
    while (!(TWCR & (1 << TWINT))) {
    }; // When TWINT is set, start is complete, and we
    //  can initiate data transfer.
    TWDR = _addr; // Load the slave address
    TWCR = CLEAR_TWINT; // Clear TWINT to begin transmission (I know,
    //  it LOOKS like I'm setting it, but this is
    //  how we clear that bit. Dumb.)
    while (!(TWCR & (1 << TWINT))) {
    }; // Wait for TWINT again.
    // Now, we need to send the address we want to read from:
    TWDR = addr; // Load the slave address
    TWCR = CLEAR_TWINT; // Clear TWINT to begin transmission (I know,
    //  it LOOKS like I'm setting it, but this is
    //  how we clear that bit. Dumb.)
    while (!(TWCR & (1 << TWINT))) {
    }; // Wait for TWINT again.
    TWCR = STOP_COND;

    // Now, we issue a repeated start (by doing what we just did again), and this
    //  time, we set the READ bit as well.
    TWCR = START_COND; // Send a start condition
    while (!(TWCR & (1 << TWINT))) {
    }; // When TWINT is set, start is complete, and we
    //  can initiate data transfer.
    TWDR = (_addr) | I2C_READ; // Load the slave address and set the read bit
    TWCR = CLEAR_TWINT; // Clear TWINT to begin transmission (I know,
    //  it LOOKS like I'm setting it, but this is
    //  how we clear that bit. Dumb.)
    while (!(TWCR & (1 << TWINT))) {
    }; // Wait for TWINT again.

    // Now, we can fetch data from the slave by clearing TWINT, waiting, and
    //  reading the data. Rinse, repeat, as often as needed.
    for (byte i = 0; i < len; i++) {
        if (i == len - 1) { TWCR = CLEAR_TWINT; } // Clear TWINT to begin transmission (I know,
        //  it LOOKS like I'm setting it, but this is
        //  how we clear that bit. Dumb.)
        else { TWCR = NEXT_BYTE; }
        while (!(TWCR & (1 << TWINT))) {
        }; // Wait for TWINT again.
        buffer[i] = TWDR; // Now our data can be fetched from TWDR.
    }
    // Now that we're done reading our data, we can transmit a stop condition.
    TWCR = STOP_COND;
}

void MiniMoto::I2CWriteBytes(const byte addr, const byte *buffer, byte len) const {
    if (buffer == NULL || len == 0) {
        DEBUG_LOG("Invalid buffer or length");
        return;
    }

    // Send start condition
    TWCR = START_COND;
    uint16_t timeout = 10000;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (timeout == 0 || (TWSR & 0xF8) != 0x08) {
        DEBUG_LOG("Failed to send START condition, TWSR: %x", TWSR & 0xF8);
        TWCR = STOP_COND; // Release bus
        return;
    }

    // Load slave address with write bit
    TWDR = _addr;
    TWCR = CLEAR_TWINT | (1 << TWEN);
    timeout = 10000;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (timeout == 0 || (TWSR & 0xF8) != 0x18) {
        DEBUG_LOG("Failed to send SLA+W, TWSR: %x", TWSR & 0xF8);
        TWCR = STOP_COND; // Release bus
        return;
    }

    // Load data address
    TWDR = addr;
    TWCR = CLEAR_TWINT | (1 << TWEN);
    timeout = 10000;
    while (!(TWCR & (1 << TWINT)) && --timeout);
    if (timeout == 0 || (TWSR & 0xF8) != 0x28) {
        DEBUG_LOG("Failed to send data address, TWSR: %x", TWSR & 0xF8);
        TWCR = STOP_COND; // Release bus
        return;
    }

    // Write the data bytes
    for (byte i = 0; i < len; i++) {
        TWDR = buffer[i];
        TWCR = CLEAR_TWINT | (1 << TWEN);
        timeout = 10000;
        while (!(TWCR & (1 << TWINT)) && --timeout);
        if (timeout == 0 || (TWSR & 0xF8) == 0x30) {
            DEBUG_LOG("NACK or timeout on byte %d, TWSR: %x", i, TWSR & 0xF8);
            TWCR = STOP_COND; // Release bus
            return;
        }
    }

    // Send stop condition
    TWCR = STOP_COND;
}
