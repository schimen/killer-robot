"""
Module for communicating with robot, used by control panel.
Made by Simen
"""
from serial                  import Serial
from serial.serialutil       import SerialException
from serial.tools.list_ports import comports
from bleak import BleakClient, BleakScanner
import atexit
from threading import Thread
from queue import Queue
import asyncio
from time import time, sleep

COMMAND_SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID  = "12345678-1234-5678-1234-56789abcdef1"

ERROR_COMMAND             = 0
ACK_COMMAND               = 1
PING_COMMAND              = 2
LEFT_MOTOR_COMMAND        = 3
RIGHT_MOTOR_COMMAND       = 4
WEAPON_MOTOR_COMMAND      = 5
DEFAULT_INTERFACE_COMMAND = 6

ERROR_UNRECOGNIZED = 0
ERROR_TIMEOUT      = 1
ERROR_PARSE        = 2
ERROR_VALUE        = 3

COMMAND_START = b'%'
COMMAND_END = b'&'

# Dictionary for interfaces
interfaces = dict()

class Communication:
    def __init__(self):
        self.interfaces = dict()
        self.msg_id = 0
        self.incoming_commands = Queue()
        self.outgoing_commands = Queue(maxsize=10)
        self.bluetooth = BluetoothCommunication(self.incoming_commands)
        self.serial = SerialCommunication(self.incoming_commands)
        
        # Start thread for sending commands
        Thread(
            target=self.send_commands,
            daemon=True
        ).start()

        # Start thread for printing received commands
        Thread(
            target=self.print_new_commands,
            daemon=True
        ).start()

        # Start thread for pinging all interfaces regularly
        Thread(
            target=self.ping_all,
            daemon=True
        ).start()
        
        # Variables for `send_message`, 
        # for keeping track of earlier messages
        self.old_message = dict()

    def ping_all(self):
        while True:
            for interface in self.interfaces:
                start = time()
                if interface.send([(self.create_head(PING_COMMAND), 1)]):
                    self.run_send_cb(interface, time()-start)

            sleep(2)

    def get_interface(self):
        if len(self.interfaces) < 1:
            return None

        return tuple(self.interfaces.items())[0][0]

    def run_send_cb(self, interface, time):
        if interface in self.interfaces:
            cb_func =  self.interfaces[interface][1]
            if cb_func is not None:
                cb_func(time)

    def add_interface(self, interface, priority, send_cb):
        # Save interface object and a corresponding priority and busy flag
        self.interfaces[interface] = (priority, send_cb)
        # Sort interfaces by priority
        sorted_iface_items = sorted(
            self.interfaces.items(), 
            key=lambda item: item[1][0]
        )
        self.interfaces = dict(sorted_iface_items)
        
    def remove_interface(self, interface):
        if interface in self.interfaces:
            del self.interfaces[interface]

    def get_message_id(self):
        self.msg_id += 1
        if self.msg_id > 31:
            self.msg_id = 0
        return self.msg_id

    def send_message(self, motor_a, motor_b, motor_w):
        """ send message to transmitter """

        # motor a
        command_a = LEFT_MOTOR_COMMAND
        # motor b
        command_b = RIGHT_MOTOR_COMMAND
        # weapon motor
        command_w = WEAPON_MOTOR_COMMAND

        # check if message_a or message_b  are duplicates from earlier
        duplicate_message_a = self.old_message.get(command_a) == motor_a
        duplicate_message_b = self.old_message.get(command_b) == motor_b
        duplicate_message_w = self.old_message.get(command_w) == motor_w
        if all((duplicate_message_a, duplicate_message_b, duplicate_message_w)):
            return
        # add message a
        if not duplicate_message_a:
            self.add_command(command_a, motor_a)
            self.old_message[command_a] = motor_a
        # add message b
        if not duplicate_message_b:
            self.add_command(command_b, motor_b)
            self.old_message[command_b] = motor_b

        if not duplicate_message_w:
            self.add_command(command_w, motor_w)
            self.old_message[command_w] = motor_w


    def add_command(self, command, value):
        # Remove first item if queue is full
        if self.outgoing_commands.full():
            print('Queue full, remove command oldest command')
            _, _ = self.outgoing_commands.get_nowait()
        
        self.outgoing_commands.put_nowait((command, value))

    def create_head(self, command):
        return command << 5 | self.get_message_id()

    def send_commands(self):
        while True:
            # Get all new commands
            command_list = []
            command, value = self.outgoing_commands.get()
            command_list.append((self.create_head(command), value))
            while not self.outgoing_commands.empty():
                command, value = self.outgoing_commands.get_nowait()
                command_list.append((self.create_head(command), value))

            # Get an interface to send command through
            interface = self.get_interface()
            if interface is None:
                print('No interfaces available')
            else:
                # Send!
                start = time()
                if interface.send(command_list):
                    self.run_send_cb(interface, time()-start)
                else: 
                    print(f'Failed to send commands')
            

    def print_new_commands(self):
        command_errors = {
            ERROR_UNRECOGNIZED: 'Unrecognized command',
            ERROR_TIMEOUT: 'Command timed out',
            ERROR_PARSE: 'Failed to parse command',
            ERROR_VALUE: 'Invalid value'
        }
        while True:
            command, message_id, value = self.incoming_commands.get()
            if command == ERROR_COMMAND:

                print(f'Received error: {command_errors[value]}')
            elif command == ACK_COMMAND:
                pass
            else:
                print(f'Received command {command} with value {value}')

class BluetoothCommunication:
    def __init__(self, incoming_queue):
        self.client = None
        self.characteristic = None
        self.command_mtu = 8 # Default to mtu of 8 commands (16 bytes)
        self.event_loop = asyncio.get_event_loop()
        self.incoming_queue = incoming_queue

    def bt_notification_cb(self, _, data):
        if len(data) != 2:
            print(f'Received strange notification {data}')
            return
        
        head, value = data
        key = (0xE0 & head) >> 5
        address = 0x07 & head
        self.incoming_queue.put((key, address, value))

    def send(self, data):
        if self.client.is_connected:
            if len(data) > self.command_mtu:
                print('More data than mtu, remove oldest commands')
                # If there are too many commands, remove the oldest ones
                data = data[(len(data) - self.command_mtu):]
            command_bytes = [byte for command_data in data for byte in command_data]
            try:
                self.event_loop.run_until_complete(self.gatt_send(command_bytes))
                return True
            except Exception as e:
                print(f'Client is already disconnected ({e})')

        return False

    async def gatt_send(self, data):
        data_bytes = bytearray(data)
        await self.client.write_gatt_char(self.characteristic, data_bytes)

    async def ble_async_connect(self, name, disconnect_cb):
        # Register disconnect function at exit
        atexit.register(self.disconnect)
        # Search for relevant devices
        print('Search for device')
        device = await find_device(name)
        if device is None:
            print("Found no relevant device")
            return None

        # Set up client object and connect to device
        client = BleakClient(device.address, disconnected_callback = disconnect_cb)
        print(f'Connecting to {device.name} at address {device.address}')
        await client.connect()

        if client is None:
            print('Could not connect to device')
            return None

        # Make sure client support custom command characteristic
        self.characteristic = client.services.get_characteristic(
            COMMAND_WRITE_UUID
        )
        if self.characteristic is None:
            print(f'Device {device.name} ({device.address}) ' + \
                'does not support rupert command charactiristic')
            await client.disconnect()
            return None
        # Get mtu of for command write
        self.command_mtu = self.characteristic.max_write_without_response_size//2
        
        await client.start_notify(
            self.characteristic, self.bt_notification_cb
        )
        return client

    def connect(self, name, connect_cb, disconnect_cb):
        self.bt_connect_cb = connect_cb
        self.client = self.event_loop.run_until_complete(self.ble_async_connect(name, disconnect_cb))
        if type(self.client) == BleakClient:
            if self.client.is_connected and connect_cb:
                connect_cb()

    def disconnect(self):
        if type(self.client) == BleakClient:
            if self.client.is_connected:
                print(f'Disconnecting from {self.client.address}')
                self.event_loop.run_until_complete(self.client.disconnect())
            else:
                print('Client is already disconnected')
        
        self.client = None

class SerialCommunication:
    def __init__(self, incoming_queue):
        self.ser = Serial()
        atexit.register(self.ser.close)
        self.incoming_queue = incoming_queue
        self.listen_id = []

    def send(self, data):
        if self.ser.is_open:
            # Write command to serial interface
            for command_data in data:
                self.ser.write(bytearray([
                    ord(COMMAND_START), 
                    *command_data,
                    ord(COMMAND_END)
                ]))
            # Get id of all messages sent
            self.listen_id = [(head & 0x1F) for head, _ in data]
            start_time = time()
            while len(self.listen_id) > 0:
                if time() - start_time > 0.15:
                    print(f'Waiting for messages {self.listen_id} timed out')
                    return False
                sleep(0.005)

            return True

        return False

    def read_serial_thread(self):
        """
        Function that continually reads serial information and prints to terminal
        """
        serial_buffer = b''
        while self.ser.is_open:
            try:
                # add new characters to buffer
                serial_buffer += self.ser.read()

            except TypeError:
                # serial was apparently closed
                print('Stopped listening to serial')
                return None

            if COMMAND_START not in serial_buffer:
                # reset serial_buffer if there is no command start
                serial_buffer = b''

            # command beginning and end is in buffer
            if COMMAND_START in serial_buffer and COMMAND_END in serial_buffer:
                while len(serial_buffer) > 0:
                    start = serial_buffer.find(COMMAND_START)
                    end = serial_buffer.find(COMMAND_END) + 1
                    if start >= end:
                        print('received partial command, throw it away')
                        serial_buffer = serial_buffer[start:]
                    elif end - start > 4:
                        serial_buffer = serial_buffer[start+1:]
                    else:
                        command_string = serial_buffer[start:end]
                        if len(command_string) == 4:
                            head = command_string[1]
                            value = command_string[2]

                        command = (head & 0xE0) >> 5
                        message_id = head & 0x1F
                        self.incoming_queue.put((command, message_id, value))
                        # Ack off message based on ID
                        if message_id in self.listen_id:
                            self.listen_id.remove(message_id)

                        # command finished, reset
                        serial_buffer = serial_buffer[end:]

    def open_serial(self, port, baudrate):
        self.ser.port = port
        self.ser.baudrate = baudrate

        try: # Try to open the port
            print(f'Opening serial at:\nport: {self.ser.port}, baud: {self.ser.baudrate}')
            self.ser.open()
            return True

        except SerialException:
            print(f'Could not open serial port at {self.ser.port} with {self.ser.baudrate}.')
            print(f'Maybe try one of these: {list_comports()}')
            return False


async def find_device(name):
    correct_device = lambda d, _: name in d.name.lower()
    device = await BleakScanner.find_device_by_filter(
        correct_device,
        timeout=5
    )
    return device

def list_comports():
    return list(port.device for port in comports())
