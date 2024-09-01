#include "serial.h"

#include <Arduino.h>

#include <assert.h>

#include "encoder.pb.h"
#include "src/nanopb/pb.h"
#include "src/nanopb/pb_encode.h"
#include "src/cobs/cobs.h"

void readSerial() {}

void sendEncoderData(uint8_t location, int16_t position) {
  uint8_t buffer[ENCODER_DATA_SIZE];
  encoder_data_t msg = ENCODER_DATA_INIT_DEFAULT;
  pb_ostream_t stream = pb_ostream_from_buffer(buffer, sizeof(buffer));
  msg.location = (location_t)location;
  msg.position = position;
  bool status = pb_encode(&stream, ENCODER_DATA_FIELDS, &msg);

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