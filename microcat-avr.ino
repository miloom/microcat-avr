#include <avr/wdt.h>
#include <SPI.h>
#include <Wire.h>

#include "as5040.h"
#include "config.h"
#include "encoder.h"
#include "serial.h"

volatile bool wait = false;

Encoder motor[(uint64_t)MotorNum::COUNT];

uint64_t main_loop_counter = 0;

void setup() {
  pinMode(LED, OUTPUT);
  pinMode(MOTOR_POWER_EN, OUTPUT);
  digitalWrite(MOTOR_POWER_EN, HIGH);
  digitalWrite(LED, HIGH);

  Wire.begin();
  Serial.begin(SERIAL_SPEED);

  SPI.begin();
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  
  for (int motorNum = 0; motorNum < (uint8_t)MotorNum::COUNT; motorNum++) {
    motor[motorNum].initialize(motorNum);
  }

  
  // =========================================================================
  // setup 16-bit timer 1 to control the loop timing at 100Hz:
  cli();                // disable interrupts
  TCCR1A = 0;           // set entire TCCR1A register to 0
  TCCR1B = 0;           // same for TCCR1B
  TCNT1  = 0;           // initialize counter value to 0
  // set compare match register for 100 Hz increments
  OCR1A = 19999;        // = 16000000 / (8 * 100) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12, CS11 and CS10 bits for prescaler: 8
  TCCR1B |= (0 << CS12) | (1 << CS11) | (0 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  sei();                // enable interrupts
  // =========================================================================

  wdt_enable (WDTO_1S); // enable watchdog timer with 1 sec timeout
  
}

void loop() {
  wait = true;
  wdt_reset ();

  main_loop_counter++;

  for (uint8_t motorNum = 0; motorNum < static_cast<uint8_t>(MotorNum::COUNT); motorNum++) {
    motor[motorNum].update();
    sendEncoderData(motorNum, static_cast<int16_t>(motor[motorNum].servo_position));
  }


  if ((main_loop_counter % (uint32_t)(MAIN_LOOP_FREQ / HEARTBEAT_FREQ)) == 0) { // every 100 main loop iterations (once every second): red LED blinking
    digitalWrite(LED, !digitalRead(LED));
  }

  while(wait) {
    volatile int i = 0;
    i++;
  };
}

ISR(TIMER1_COMPA_vect) {    // timer compare interrupt service routine
  // this interrupt hits every 100ms. it is used for loop timing
  wait = 0;
}

void serialEvent() {
 readSerial(); 
}
