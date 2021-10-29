#include "SimpleTimer.h"
#include "Motor.h"

// motor commands
#define MOTOR_1_COMMAND 10
#define MOTOR_2_COMMAND 11

// left motor
const uint8_t motor1Enable = 5;
const uint8_t motor1Pin1   = 6;
const uint8_t motor1Pin2   = 7;

// right motor
const uint8_t motor2Enable = 10;
const uint8_t motor2Pin1   = 8;
const uint8_t motor2Pin2   = 9;

// acceleration settings
const uint16_t stepDelay = 50;
const uint8_t  stepSize  = 10;

SimpleTimer timer1;
SimpleTimer timer2;

Motor motor1(1, motor1Pin1, motor1Pin2, motor1Enable, stepDelay, stepSize, &timer1);
Motor motor2(2, motor2Pin1, motor2Pin2, motor2Enable, stepDelay, stepSize, &timer2);

void setup() {
  Serial.begin(9600);
}

void loop() {
  timer1.run();
  timer2.run();
  while (Serial.available() > 0) {
    int command = Serial.parseInt();
    int value   = Serial.parseInt();
    if (command == MOTOR_1_COMMAND) {
      motor1.set(value);
    }
    else if (command == MOTOR_2_COMMAND) {
      motor2.set(value);
    }
  }
}
