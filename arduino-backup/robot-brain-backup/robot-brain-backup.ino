#include <RH_ASK.h>
#include <SPI.h>
#include "SimpleTimer.h"
#include "Motor.h"


/** @brief Enumerated command constants */
enum commands {
    /* Indicates that an error occurred */
    error_command,
    /* Acknowledge a received command */
    ack_command,
    /* Send ping command to specified interface */
    ping_command,
    /* Set speed of left motor */
    left_motor_command,
    /* Set speed of right motor */
    right_motor_command,
    /* Set speed of weapon motor */
    weapon_motor_command,
    /* Set default interface */
    default_interface_command
};

/** @brief Enumerated error codes for commands */
enum errors {
    /* Unrecognized command */
    error_unrecognized,
    /* Message was not acked before timeout */
    error_timeout,
    /* Message was parsed incorrectly */
    error_parse,
    /* Value in command not valid */
    error_value
};

#define MSG_SIZE 4
const uint8_t bufLen = MSG_SIZE;

RH_ASK backupRadio;
uint8_t receiveBuffer[MSG_SIZE];

uint8_t c;
bool start_command = false;
uint8_t msg_pos = 0;
uint8_t rx_buffer[MSG_SIZE] = { 0 };


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


// timers for acceleration control
SimpleTimer timer1;
SimpleTimer timer2;
SimpleTimer timer3;

// objects for motor control
Motor motor1(1, motor1Pin1, motor1Pin2, motor1Enable, stepDelay, stepSize, &timer1);
Motor motor2(2, motor2Pin1, motor2Pin2, motor2Enable, stepDelay, stepSize, &timer2);
//Motor servo( 3,          0,          0,     servoPin, stepDelay, stepSize, &timer3);

void setup() {
  //Serial.begin(9600); // communication with bluetooth module
  backupRadio.init();
}

void loop() {
  // start both timers to keep track of the motors
  timer1.run();
  timer2.run();
  //timer3.run();
  // check if there are new messages at the backup radio (buffer is full)
  // new message from the bluetooth module
  if (backupRadio.recv(receiveBuffer, &bufLen)) {
    if (receiveBuffer[0] == ':' && receiveBuffer[3] == ';') {
      handleNewMessage(receiveBuffer[1], receiveBuffer[2]);
    }
  }
  /*
  if (Serial.available() > 0) {
    c = Serial.read();
    if (c == ':') {
      start_command = true;
    }
    if (start_command) {
      rx_buffer[msg_pos] = c;
      msg_pos++;
      if (msg_pos >= MSG_SIZE || c == ';') {
        if (validRadioMessage()) {
          handleNewMessage(rx_buffer[1], rx_buffer[2]); 
        }
        // reset
        msg_pos = 0;
        start_command = false;
        memset(rx_buffer, 0, sizeof(rx_buffer));
      }   
    }
  }
  */
}

int8_t limitSpeed(int value)  {
  // limit the speed if it is outside our range
  int limitedValue = value - 127;
  if (limitedValue >= 127) {
    return 127;
  }
  else if (limitedValue <= -127)  {
    return -127;
  }
  return limitedValue;
}

void ack(uint8_t id) {
  uint8_t head = (ack_command << 5) | id;
  Serial.write(':');
  Serial.write(head);
  Serial.write(0);
  Serial.write(';');
}

void unrecognized(uint8_t id) {
  uint8_t head = (error_command << 5) | id;
  Serial.write(':');
  Serial.write(head);
  Serial.write(error_unrecognized);
  Serial.write(';');
}

void handleNewMessage(uint8_t head, uint8_t value) {
  uint8_t key = ((head & 0xE0) >> 5); // read the 3 MSB from head
  uint8_t id = head & 0x1F;           // read the 5 LSB from head
  switch (key) {
    case ack_command:
      // we don't care about acks
      break;
    case left_motor_command:
      //ack(id);
      motor1.set(limitSpeed(value));
      break;
    case right_motor_command:
      //ack(id);
      motor2.set(limitSpeed(value));
      break;
    case weapon_motor_command:
      //ack(id);
      analogWrite(servoPin, value);
      break;
    default:
      //unrecognized(id);
      break;
  }
}

bool validRadioMessage() {
  // check if the message is valid message ("<command>,<value>\n")
  return ((rx_buffer[0] == ':') and (rx_buffer[3] == ';'));
  // I should add message as argument here, and not use the global buffer...
}
