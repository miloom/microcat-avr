#pragma once

#include <stdint.h>

void readSerial();

void sendEncoderData(uint8_t location, int16_t position);
