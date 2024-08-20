#include "serial.h"

#include <Arduino.h>

#include <assert.h>

#include "encoder.pb.h"
#include "src/nanopb/pb.h"
#include "src/nanopb/pb_encode.h"

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
    Serial.println("Encoding failed");
  }
  uint8_t encoded[(int)(ENCODER_DATA_SIZE / 256) + 2 + ENCODER_DATA_SIZE] = {0};
  auto size = cobsEncode(buffer, sizeof(buffer), encoded);

  Serial.write(encoded, size);
}

size_t cobsEncode(const void* data, size_t length, uint8_t* buffer) {
  assert(data && buffer);

  uint8_t *encode = buffer; // Encoded byte pointer
	uint8_t *codep = encode++; // Output code pointer
	uint8_t code = 1; // Code value

	for (const uint8_t *byte = (const uint8_t *)data; length--; ++byte)
	{
		if (*byte) // Byte not zero, write it
			*encode++ = *byte, ++code;

		if (!*byte || code == 0xff) // Input is zero or block completed, restart
		{
			*codep = code, code = 1, codep = encode;
			if (!*byte || length)
				++encode;
		}
	}
	*codep++ = code; // Write final code value
  *encode++ = 0x00; // Add the 0 delimiter at the end
	return (size_t)(encode - buffer);

}

size_t cobsDecode(const uint8_t *buffer, size_t length, void *data)
{
	assert(buffer && data);

	const uint8_t *byte = buffer; // Encoded input byte pointer
	uint8_t *decode = (uint8_t *)data; // Decoded output byte pointer

	for (uint8_t code = 0xff, block = 0; byte < buffer + length; --block)
	{
		if (block) // Decode block byte
			*decode++ = *byte++;
		else
		{
			block = *byte++;             // Fetch the next block length
			if (block && (code != 0xff)) // Encoded zero, write it unless it's delimiter.
				*decode++ = 0;
			code = block;
			if (!code) // Delimiter code found
				break;
		}
	}

	return (size_t)(decode - (uint8_t *)data);
}
