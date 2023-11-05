# Killer robot
This repository contains all code and circuit for the robot **Rupert**, competing in [RoboWars](https://omegav.no/robotkrig) at [Omega Verksted](https://omegav.no/).

<img rupert_logo src=./images/rupert_logo.png width=200>

## Rupert
Rupert regularly undergoes changes, and so far there have been three versions of Rupert. The first Rupert competing in Robowars autumn 2021, Señor Rupert jr and Captain Rupert competing in robowars autumn 2022 and spring 2023 and our newest Rupert in autumn 2023.

Results can be seen at [confluence](https://confluence.omegav.no/display/OV/Tidligere+resultater).

### Rupert
*Autumn 2021* - ([Branch](https://github.com/schimen/killer-robot/tree/rupert-h2021))
The first Rupert to compete. Rupert did not have any efficient weapons, but he had a very durable shell which protected him from dangerous attacks. 
Rupert fought well but with a lack of attack he could not compete with the top. Rupert came in 3rd place.

<img rupert_logo src=./images/rupert.jpg width=600>

### Señor Rupert jr
*Autumn 2022* - ([Branch](https://github.com/schimen/killer-robot/tree/rupert-h2022))
The next Rupert to compete had a 600g steel cylinder with teeth which was spinning at an impressive speed. Now that Rupert was equipped with a weapon, he was able to bite back at the opponents.
Rupert fought more aggressively this time and he did very well, but the electronics could not handle the heat and ended up burning themselves up in the process. Rupert came in 2nd place.

<img rupert_logo src=./images/senor_rupert.jpg width=600>

### Captain Rupert
*Spring 2023* - ([Branch](https://github.com/schimen/killer-robot/tree/rupert-v2023))
Rupert was again equipped with the teeth, and armed with more experience. He fought like a pirate, with everything he got, but in the final fight his weapon was destroyed and the final battle was lost. He still came in a shared 1st place.

<img rupert_logo src=./images/captain_rupert.jpg width=600>

### Rupert
*Autumn 2023* - ([Branch](https://github.com/schimen/killer-robot/tree/rupert-h2023))
This time Rupert came out of his shell with a new strategy. He brought his padlock and threw it around at the opponents. Rupert survived surprisingly long in the arena but was sadly obliterated immediately when he met an opponent with a fast spinning weapon. Rupert came in 3rd place.

<img rupert_logo src=./images/rupert_h2023.jpg width=600>

## Projects
This repository consists of differenct projects that have different tasks in controlling the robot.
The different projects are:
- [Robot brain](./firmware/rupert-brain/): Firmware running on Rupert, receives commands and controls motors.
- [Robot pcb](./robot-pcb/): Printed circuit board supposed to control the robot. The robot brain runs on this hardware.
- [Robot control center](./control-center/): Python GUI program that controls the robot via commands.
- [Transmitter](./firmware/transmitter): Firmware for command transmitter, communicating with the control center via USB and the robot via radio.

## Zephyr guide
The firmware for Rupert is mainly developed in [Zephyr RTOS](https://www.zephyrproject.org/) and the SDK for Zephyr must be installed to build and upload firmware to the robot and the transmitter.
Zephyr is well documented and the latest documentation can be found [here](https://docs.zephyrproject.org/latest/index.html).

To install and get started with Zephyr, please follow [this guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html).
To install Zephyr in this project, make sure west and other dependencies from the guide is installed, and run `west update` from this repositories root folder. This will download Zephyr and relevant modules.
After installing and becoming familiar with the framework, you can install dependencies used by our hardware:
- Install ESP32 dependencies with `west espressif install`. Alternatively, follow [this guide](https://www.zephyrproject.org/zephyr-rtos-on-esp32/).

The Rupert firmware can now for example be built by the command `west build -b rupert_pcb firmware/rupert-brain` and then flashed with the command `west flash`.


