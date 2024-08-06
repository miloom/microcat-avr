#include "serial.h"

#include <Arduino.h>

#include "src/nanopb/pb.h"
#include "src/nanopb/pb_encode.h"
#include "encoder.pb.h"


void readSerial() {
  
}

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
  Serial.write(buffer, msg_len);
}
