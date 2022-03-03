#include <RH_ASK.h> // <- this must be (globally) installed to compile this sketch
#include <SPI.h>
#include "SimpleTimer.h"
#include "Motor.h"

// motor and servo commands
#define MOTOR_1_COMMAND 10
#define MOTOR_2_COMMAND 11
#define SERVO_COMMAND   12

// left motor
const uint8_t motor1Enable = 5;
const uint8_t motor1Pin1   = 2;
const uint8_t motor1Pin2   = 7;

// right motor
const uint8_t motor2Enable = 6;
const uint8_t motor2Pin1   = 8;
const uint8_t motor2Pin2   = 4;

// servo
const uint8_t servoPin = 3;

// acceleration settings
const uint16_t stepDelay = 50;
const uint8_t  stepSize  = 50;

// backup radio (433MHz module)
RH_ASK backupRadio;
const uint8_t bufLen = 4;
uint8_t receiveBuffer[bufLen];

// timers for acceleration control
SimpleTimer timer1;
SimpleTimer timer2;

// objects for motor control
Motor motor1(1, motor1Pin1, motor1Pin2, motor1Enable, stepDelay, stepSize, &timer1);
Motor motor2(2, motor2Pin1, motor2Pin2, motor2Enable, stepDelay, stepSize, &timer2);

void setup() {
  Serial.begin(9600); // communication with bluetooth module
  Serial.setTimeout(100);
  backupRadio.init(); // backup radio receiver
}

void loop() {
  // start both timers to keep track of the motors
  timer1.run();
  timer2.run();
  // check if there are new messages at the backup radio (buffer is full)
  if (backupRadio.recv(receiveBuffer, &bufLen)) {
    if (validRadioMessage()) { // check if the new message is valid
      uint8_t command = receiveBuffer[0];
      int8_t  value   = receiveBuffer[2];
      // do something with the values!
      handleNewMessage(command, value);
    }
  }
  // new message from the bluetooth module
  if (Serial.available() > 0) {
    // parse message ("<command>,<value>\n")
    uint8_t command = Serial.parseInt();
    int     value   = Serial.parseInt();
    if (Serial.read() == '\n') {
      handleNewMessage(command, value); // do something with it!
    }
  }
}

int8_t limitSpeed(int value)  {
  // limit the speed if it is outside our range
  if (value >= 127) {
    return 127;
  }
  else if (value <= -127)  {
    return -127;
  }
  return value;
}

void handleNewMessage(uint8_t command, int value) {
  // do something with the new message!
  int8_t limitedValue = limitSpeed(value);
  switch (command) {
    case MOTOR_1_COMMAND:
      motor1.set(limitedValue);
      break;
    case MOTOR_2_COMMAND:
      motor2.set(limitedValue);
      break;
    case SERVO_COMMAND:
      analogWrite(servoPin, abs(limitedValue)*2);
      break;
  }
}

bool validRadioMessage() {
  // check if the message is valid message ("<command>,<value>\n")
  return ((receiveBuffer[1] == ',') and (receiveBuffer[3] == '\n'));
  // I should add message as argument here, and not use the global buffer...
}
