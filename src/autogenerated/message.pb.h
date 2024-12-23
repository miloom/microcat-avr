/* Automatically generated nanopb header */
/* Generated by nanopb-0.4.9-dev */

#ifndef PB_MESSAGE_MESSAGE_PB_H_INCLUDED
#define PB_MESSAGE_MESSAGE_PB_H_INCLUDED
#include "../nanopb/pb.h"
#include "encoder.pb.h"
#include "imu.pb.h"
#include "motor.pb.h"

#if PB_PROTO_HEADER_VERSION != 40
#error Regenerate this file with the current version of nanopb generator.
#endif

/* Struct definitions */
typedef struct message_message {
    pb_size_t which_data;
    union {
        encoder_data_t encoder;
        imu_telemetry_t telemetry;
        motor_motor_target_t motor;
    } data;
} message_message_t;


#ifdef __cplusplus
extern "C" {
#endif

/* Initializer values for message structs */
#define MESSAGE_MESSAGE_INIT_DEFAULT             {0, {ENCODER_DATA_INIT_DEFAULT}}
#define MESSAGE_MESSAGE_INIT_ZERO                {0, {ENCODER_DATA_INIT_ZERO}}

/* Field tags (for use in manual encoding/decoding) */
#define MESSAGE_MESSAGE_ENCODER_TAG              1
#define MESSAGE_MESSAGE_TELEMETRY_TAG            2
#define MESSAGE_MESSAGE_MOTOR_TAG                3

/* Struct field encoding specification for nanopb */
#define MESSAGE_MESSAGE_FIELDLIST(X, a) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,encoder,data.encoder),   1) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,telemetry,data.telemetry),   2) \
X(a, STATIC,   ONEOF,    MESSAGE,  (data,motor,data.motor),   3)
#define MESSAGE_MESSAGE_CALLBACK NULL
#define MESSAGE_MESSAGE_DEFAULT NULL
#define message_message_t_data_encoder_MSGTYPE encoder_data_t
#define message_message_t_data_telemetry_MSGTYPE imu_telemetry_t
#define message_message_t_data_motor_MSGTYPE motor_motor_target_t

extern const pb_msgdesc_t message_message_t_msg;

/* Defines for backwards compatibility with code written before nanopb-0.4.0 */
#define MESSAGE_MESSAGE_FIELDS &message_message_t_msg

/* Maximum encoded size of messages (where known) */
#define MESSAGE_MESSAGE_PB_H_MAX_SIZE            MESSAGE_MESSAGE_SIZE
#define MESSAGE_MESSAGE_SIZE                     36

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
