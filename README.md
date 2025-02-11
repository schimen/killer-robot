# Killer robot

This repository contains all code and circuit for the robot **Rupert**, competing in [RoboWars](https://omegav.no/robotkrig) at [Omega Verksted](https://omegav.no/).

<img rupert_logo src=./images/rupert_logo.png width=200>

More information about Rupert and his history can be found in [history.md](./history.md).


## Projects

This repository consists of differenct projects that have different tasks in controlling the robot.
The different projects are:
- [Robot brain](./firmware/rupert-brain/): Firmware running on Rupert, receives commands and controls motors.
- [Robot pcb](./robot-pcb/): Printed circuit board supposed to control the robot. The robot brain runs on this hardware.
- [Robot control center](./control-center/): Python GUI program that controls the robot via commands.
- [Transmitter](./firmware/transmitter): Firmware for command transmitter, communicating with the control center via USB and the robot via radio.
This transmitter is deprecated, all communication with Rupert uses BLE via the "Robot control center".


## Zephyr guide

The firmware for Rupert is mainly developed in [Zephyr RTOS](https://www.zephyrproject.org/) and the SDK for Zephyr must be installed to build and upload firmware to the robot and the transmitter.
Zephyr is well documented and the latest documentation can be found [here](https://docs.zephyrproject.org/latest/index.html).

To install and get started with Zephyr, please follow [this guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).
To install Zephyr in this project, make sure west and other dependencies from the guide is installed, and run `west update` from this repositories root folder. This will download Zephyr and relevant modules.
After installing and becoming familiar with the framework, you can install dependencies used by our hardware:
- Install ESP32 dependencies with `west espressif install`. Alternatively, follow [this guide](https://www.zephyrproject.org/zephyr-rtos-on-esp32/).

The Rupert firmware can now for example be built by the command `west build -b rupert_pcb firmware/rupert-brain` and then flashed with the command `west flash`.
