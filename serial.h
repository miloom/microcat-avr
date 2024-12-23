#pragma once

#include <stdint.h>
#include "motorController.h"

void readSerial(MotorController motors[static_cast<uint64_t>(MotorNum::COUNT)]);

void sendEncoderData(uint8_t location, int16_t position);

void sendData(uint8_t data[], size_t len);

size_t cobsEncode(const void* data, size_t length, uint8_t* buffer);

size_t cobsDecode(const uint8_t* buffer, size_t length, void* data);


