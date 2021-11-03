# Radio transmitter

This is the radio transmitter we use for communicationg with the robot, the radio transmitter is designed to receive serial commands from our robot-control-center, and send them further to the robot.

The Transmitter has a [bluetooth module](https://www.techonicsltd.com/product/mlt-bt05-ble4-0/) configured as "master", and a [433MHz transmitter](https://www.componentsinfo.com/fs1000a-433mhz-rf-transmitter-xy-mk-5v-receiver-module-explanation-pinout/) as a backup solution.

This sketch uses [this library](https://www.airspayce.com/mikem/arduino/RadioHead/) for the 433MHz radio. It needs to be (globally) installed.

### How to send
The format of messages is `<command>,<value>\n`. Example of a message with command 10 and value 100: `10,100\n`


### List of commands:

| Command | Value | Explanation |
| --- | --- | --- |
| 9 | Between 0 and 255 | This command chooses which radio transmitter to use. If the value is 0, the transmitter will use the backup transmitter, otherwise it will use the bluetooth module. This command is not sent to the robot. |
| 10 | Between -127 and 127 | This controls the speed and direction of motor A |
| 11 | Between -127 and 127 | This controls the speed and direction of motor B |
| 12 | Between 0 and 127 | This controls the pwm signal to the servo |