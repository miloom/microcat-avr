#include "serial.h"

#include <Arduino.h>

#include <string.h>
#include <assert.h>

#include "log.h"

#include "src/autogenerated/message.pb.h"
#include "src/cobs/cobs.h"
#include "src/nanopb/pb.h"
#include "src/nanopb/pb_encode.h"


bool receivePacket(uint8_t* data, size_t dataSize);

void sendEncoderData(uint8_t location, int16_t position) {
  uint8_t buffer[MESSAGE_MESSAGE_SIZE];
  message_message_t msg = MESSAGE_MESSAGE_INIT_DEFAULT;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  msg.which_data = MESSAGE_MESSAGE_ENCODER_TAG;
  msg.data.encoder.location = (encoder_location_t)location;
  msg.data.encoder.position = position;
  bool status = pb_encode(&stream, MESSAGE_MESSAGE_FIELDS, &msg);

  size_t msg_len = stream.bytes_written;
  if (!status) {
    return;
  }
  sendData(buffer, msg_len);
}

void sendData(uint8_t data[], const size_t len) {
  uint8_t encoded[static_cast<int>(len / 256) + 2 + len] = {0};
  const auto result = cobs_encode(encoded, sizeof(encoded), data, len);

  if (result.status == COBS_ENCODE_OK) {
    Serial.write(encoded, result.out_len + 1);
  } else {
    constexpr uint8_t error_data[] = {0};
    Serial.write(error_data, sizeof(error_data));
  }
}

static bool initializedSerial = false;
static uint8_t previousData[MESSAGE_MESSAGE_SIZE + 2] = {0}; // The +2 is needed because cobs will add aditional bytes
static size_t decodedLength = 0;

// data: Output buffer for the decoded packet. The buffer needs to be at least MESSAGE_MESSAGE_SIZE length.
// Will only decode input data into a single packet at a time. When returning false, no packet has been decoded.
bool receivePacket(uint8_t* data, size_t dataSize) {
    if (dataSize < MESSAGE_MESSAGE_SIZE + 2) {
        return false; // Ensure the output buffer is large enough
    }

    uint8_t readData[MESSAGE_MESSAGE_SIZE + 2] = {0};
    size_t amountRead = Serial.readBytes(readData, MESSAGE_MESSAGE_SIZE );
    if (amountRead != 0) {
        DEBUG_LOG("Read %d bytes", static_cast<uint8_t>(amountRead));

      }
    if (amountRead == 0) {return false;} // No data read

    if (!initializedSerial) {
        int lastZeroIndex = -1;
        for (size_t i = 0; i < amountRead; ++i) {
            if (readData[i] == 0) {
                lastZeroIndex = i;
            }
        }

        if (lastZeroIndex != -1) { // Found a 0 byte
            DEBUG_LOG("Found 0");
            size_t startIndex = lastZeroIndex + 1;
            if (startIndex < amountRead) {
                size_t bytesToCopy = amountRead - startIndex;
                memcpy(previousData, readData + startIndex, bytesToCopy);
                decodedLength = bytesToCopy;
            }
            initializedSerial = true;
            return false;
        }
    } else {
        int firstZeroIndex = -1;
        for (size_t i = 0; i < amountRead; ++i) {
            if (readData[i] == 0) {
                firstZeroIndex = i;
                break;
            }
        }

        if (firstZeroIndex != -1) { // Found a 0 byte
            size_t copyLength = firstZeroIndex;
            if (decodedLength + copyLength <= MESSAGE_MESSAGE_SIZE + 2) {
                // Append to previousData using memcpy
                memcpy(previousData + decodedLength, readData, copyLength);

                // Copy the complete packet to the output data buffer
                memcpy(data, previousData, decodedLength + copyLength);

                decodedLength = 0;
                return true;
            } else {
                // Handle overflow error
                decodedLength = 0;
                return false;
            }
        } else {
            // Append the entire readData to previousData
            if (decodedLength + amountRead <= MESSAGE_MESSAGE_SIZE + 2) {
                memcpy(previousData + decodedLength, readData, amountRead);
                decodedLength += amountRead;
            } else {
                // Handle overflow error
                decodedLength = 0;
            }
            return false;
        }
    }

    return false; // Default case
}


void readSerial() {

  uint8_t data[MESSAGE_MESSAGE_SIZE + 2];
  if (receivePacket(data, sizeof(data))) {
    uint8_t cobsDecoded[MESSAGE_MESSAGE_SIZE + 2];
    auto status = cobs_decode(cobsDecoded, sizeof(cobsDecoded), data, sizeof(data));
    if (status.status == COBS_DECODE_OK) {
      DEBUG_LOG("Success decode");

    } else {
      DEBUG_LOG("Failed to decode cobs data %d", static_cast<uint8_t>(status.status));
    }


  } else {
    // DEBUG_LOG("No packet decoded");
  }
}
