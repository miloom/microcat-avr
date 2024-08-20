#pragma once

#include <stdint.h>
#include <stddef.h>

void readSerial();

void sendEncoderData(uint8_t location, int16_t position);

size_t cobsEncode(const void* data, size_t length, uint8_t* buffer);

size_t cobsDecode(const uint8_t* buffer, size_t length, void* data);
