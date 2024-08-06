#pragma once 
#include <Arduino.h>
  
#include <stdint.h>

constexpr uint8_t LED = A0;
#define PLACEHOLDER               1
#define SERIAL_SPEED              115200      // baud

// motor settings
enum class MotorNum : uint8_t {
  FRONT_RIGHT = 0,
  BACK_RIGHT,
  BACK_LEFT,
  FRONT_LEFT,
  COUNT
};
constexpr uint8_t MOTOR_POWER_EN = A3;
      

constexpr int reversed_sensing[(uint64_t)MotorNum::COUNT] = { -1, -1, -1, -1};
constexpr byte feedback_chipselect_pins[(uint64_t)MotorNum::COUNT] = {7, 8, 9, 10};

// battery measurement settings
constexpr uint8_t BATTERY = A2;
#define LOW_BATTERY_LEVEL         620         // e-02 Volts
#define BATTERY_ALARM_DELAY       20          // seconds of low voltage before alarm
/*
#if ROBOT_NUM == 2
  #define DESIRED_DEPTH           105         // cm
  #define DESIRED_DEPTH_DEADZONE  5           // cm, +-
  #define PRESSURE_SLOPE          0.106145
  #define PRESSURE_OFFSET         12492.1
  const int calibrated_zeros[(uint64_t)MotorNum::COUNT] = {188, 161, 726, 356};
#else
  #error "Robot number out of range. Check config.h"
#endif // ROBOT_NUM == ?
*/
// loop & sampling timings
constexpr float MAIN_LOOP_FREQ = 100.f;         // Hz
constexpr float MOTOR_LOOP_FREQ = 10.f;          // Hz
constexpr float ROS_PUBLISH_FREQ = 10.f;          // Hz
constexpr float HEARTBEAT_FREQ = 10.f;           // Hz
constexpr float PRESSURE_READ_FREQ = 10.f;          // Hz
constexpr float BATTERY_READ_FREQ = 10.f;          // Hz
constexpr float BEACON_READ_FREQ = 10.f;          // Hz

