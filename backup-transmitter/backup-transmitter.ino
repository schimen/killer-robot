#include <RH_ASK.h>
#include <SPI.h>

RH_ASK backupRadio;

void setup()
{
    Serial.begin(9600);    // Debugging only
    if (!backupRadio.init()) {
         Serial.println("ERROR: backup radio failed");
    }
}

void loop()
{
  while (Serial.available() > 0) {
    char transmitBuffer[4];
    int numChars = Serial.readBytesUntil('\n', transmitBuffer, 10);
    if (numChars == 4) {
      backupRadio.send((char *)transmitBuffer, numChars);
      backupRadio.waitPacketSent();
    }
  }
}
