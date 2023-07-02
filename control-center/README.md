# Robot control center

This is the control-panel for rupert. It is a python gui that is used to sending messages to rupert.

The mesages can be sent from:
- Separate transmitter over serial communication
- Bluetooth

## Installation

To [install](https://docs.python.org/3/installing/index.html) the dependencies for this project, install all python packages in this list:
- [pyserial](https://pypi.org/project/pyserial/)
- [bleak](https://pypi.org/project/bleak/)

## Usage
To use this gui, run the file [control_panel.py](./control_panel.py).
This can for example be run via the command 
```bash
python3 control_panel.py
```