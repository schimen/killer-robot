# Radio transmitter
This is the radio transmitter we use for communicationg with the robot, the radio transmitter is designed to receive serial commands from our robot-control-center, and send them to the robot.

Please see [command lib](../command_lib/) for a list of available commands.

### How to send
The format of messages is `:<command><value>;`. Example of a message with command 10 and value 100: `:<0x0A><0x64;`
