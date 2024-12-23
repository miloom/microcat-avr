#include "motorController.h"

#include "config.h"
#include "log.h"

void MotorController::init(uint8_t motor_num) {
    frequency = 0.1;
    zero_direction = 0;
    calibrated_zero = calibrated_zeros[motor_num];
    reversed = reversed_rotation[motor_num];
    motor.initialize(motor_i2c_addresses[motor_num]);
    amplitude = 300.0;
    phase_offset = 0;
    encoder.initialize(motor_num);
}

int32_t MotorController::update()
{
    // calculate generalized angular position based on amplitude, frequency and phase_offset
    // you can use the function millis() to get current time in milliseconds
    const auto temp_time = static_cast<float>(fmod(static_cast<double>(millis())/1000.0 , 1.0/frequency )); // calculate time step for phase
    const auto gen_xt = static_cast<float>(amplitude/2.0 * sin(2.0*PI*frequency*temp_time + phase_offset*RADIANS_PER_TICK));
    // calculate the individual motor's desired_position given gen_xt, zero_direction and calibrated_zeros
    // use the reversed_rotation matrix to correctly orient all motors
    auto desired_position = static_cast<int32_t>(reversed*(static_cast<float>(zero_direction) + gen_xt) - static_cast<float>(calibrated_zero));
    desired_position %= 1024; // avoid extra turns of the actuator

    encoder.update();
    // calculate shortest_path as the shortest distance between desired_position and servo_position.
    auto shortest_path = static_cast<int16_t>((desired_position - encoder.servo_position) % 1024);

    // shortest path is always in the -512...511 range. ensures correct direction of rotation
    // avoid multiple turns of the motor
    while (shortest_path > 511) {
        shortest_path -= 1024;
    }
    while (shortest_path < -512) {
        shortest_path += 1024;
    }

    move(shortest_path);
    return static_cast<int32_t>(encoder.servo_position);
}

void MotorController::move(int16_t movement)
{
  // saturate the movement(shortest_path) command between -MAX_SERVO_SPEED and MAX_SERVO_SPEED
  movement = min(movement, MAX_SERVO_SPEED);
  movement = max(movement, -MAX_SERVO_SPEED);

  motor.drive(movement);
}

void MotorController::setTarget(const float frequency, const float amplitude, const float target_direction) {
    this->frequency = frequency;
    this->amplitude = amplitude;
    this->target_direction = target_direction;
}
