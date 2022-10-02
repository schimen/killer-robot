#include <RH_ASK.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#define RADIO_CHOICE 9 // command for choosing radio transmitter


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

uint8_t c;
bool start_command = false;
uint8_t msg_pos = 0;
uint8_t rx_buffer[MSG_SIZE] = { 0 };

SoftwareSerial bluetooth(2,3); // serial object for bluetooth module
RH_ASK backupRadio; // object for transmitting radio signals

bool useBluetooth = true;

void setup() {
    Serial.begin(9600);    // computer serial
    bluetooth.begin(9600); // bluetooth serial
    backupRadio.init();    // radio communication
}

void loop() {
  // new communcation:
  while (Serial.available() > 0) {
    c = Serial.read();
    if (c == ':') {
      start_command = true;
    }
    if (start_command) {
      rx_buffer[msg_pos] = c;
      msg_pos++;
      if (msg_pos >= MSG_SIZE || c == ';') {
        if (validRadioMessage()) {
          // Length of message is fixed at 4 bytes, therefore use number values for command and 
          // value, instead of string. This is fine as long as the values are 8-bit.
          char message[] = {rx_buffer[0], ',', limitSpeed(rx_buffer[1]), '\n'};
          backupRadio.send(message, strlen(message));
          backupRadio.waitPacketSent(); 
        }
        // reset
        msg_pos = 0;
        start_command = false;
        memset(rx_buffer, 0, sizeof(rx_buffer));
      }   
    }
  }
}

int8_t limitSpeed(int value)  {
  // limit the speed value to 8 bit signed integer, for the backup transmitter
  if (value >= 127) {
    return 127;
  }
  else if (value <= -127)  {
    return -127;
  }
  return value;
}

bool validRadioMessage() {
  // check if the message is valid message ("<command>,<value>\n")
  return ((rx_buffer[0] == ':') and (rx_buffer[3] == ';'));
  // I should add message as argument here, and not use the global buffer...
}
