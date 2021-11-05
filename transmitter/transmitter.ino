#include <RH_ASK.h>
#include <SPI.h>
#include <SoftwareSerial.h>

#define RADIO_CHOICE 9 // command for choosing radio transmitter

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
    // read command and value to transmit further
    uint8_t command = Serial.parseInt();
    int     value   = Serial.parseInt();
    // if RADIO_CHOICE command, choose radio for transmission and do not transmit
    if (command == RADIO_CHOICE) {
        useBluetooth = (value > 0); // use bluetooth if value is more than 0
    }
    else { // transmit command and value further
      if (Serial.read() == '\n') {
        if (useBluetooth) {
          // format: "<command>,<value>\n"
          bluetooth.print(String(command));
          bluetooth.write(',');
          bluetooth.print(String(value));
          bluetooth.write('\n');
        }
        else {
          // Length of message is fixed at 4 bytes, therefore use number values for command and 
          // value, instead of string. This is fine as long as the values are 8-bit.
          char message[] = {command, ',', limitSpeed(value), '\n'};
          backupRadio.send(message, strlen(message));
          backupRadio.waitPacketSent();
        }
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
