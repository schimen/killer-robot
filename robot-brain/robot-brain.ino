#include <RH_ASK.h>
#include <SPI.h>
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

// backup radio (433MHz module)
RH_ASK backupRadio;
const uint8_t bufLen = 4;
char receiveBuffer[bufLen];

// timers for acceleration control
SimpleTimer timer1;
SimpleTimer timer2;
// objects for motor control
Motor motor1(1, motor1Pin1, motor1Pin2, motor1Enable, stepDelay, stepSize, &timer1);
Motor motor2(2, motor2Pin1, motor2Pin2, motor2Enable, stepDelay, stepSize, &timer2);

void setup() {
  Serial.begin(9600);
  backupRadio.init();
  resetBuffer();
}

void loop() {
  
  timer1.run();
  timer2.run();
  backupRadio.recv(receiveBuffer, &bufLen);
  if (newRadioMessage()) {
    // save new message in seperate buffer
    char validMessage[bufLen];
    memcpy(validMessage, receiveBuffer, bufLen);
    
    uint8_t command = validMessage[0];
    int8_t  value   = validMessage[2];
    handleNewMessage(command, value);
  }
  
  while (Serial.available() > 0) {
    uint8_t command = Serial.parseInt();
    int8_t  value   = Serial.parseInt();
    if (Serial.read() == '\n') {
      handleNewMessage(command, value); 
    }
  }
}

void handleNewMessage(uint8_t command, int8_t value) {
  if (command == MOTOR_1_COMMAND) {
    motor1.set(value);
  }
  else if (command == MOTOR_2_COMMAND) {
    motor2.set(value);
  } 
}

bool newRadioMessage() {
  // check if there is a new valid message
  if ((receiveBuffer[1] == ',') and (receiveBuffer[3] == '\n')) {
    return true;
  }
  else {
    // reset buffer if message is not what we want
    for (uint8_t i; i < bufLen; i++) {
      if ((receiveBuffer[i] == ',') or receiveBuffer[i] == '\n') {
        resetBuffer();
        return false;
      }
    }
    return false;
  }
}

void resetBuffer() {
  // reset backupRadio buffer
  memset(receiveBuffer, 0, bufLen);
}
