"""
Module for communicating with robot, used by control panel.
Made by Simen
"""
from string import printable
from serial                  import Serial
from serial.serialutil       import SerialException
from serial.tools.list_ports import comports
from bleak import BleakClient, BleakScanner
import atexit
from threading import Thread
from queue import Queue
import asyncio
from time import time

CUSTOM_SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
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

# queue for incoming commands
incoming_commands = Queue()

# Dictionary for interfaces
interfaces = dict()

class Communication:
    def __init__(self):
        self.interfaces = dict()
        self.id = 0
        self.incoming_commands = Queue()
        self.outgoing_commands = Queue(maxsize=8)
        self.ble_client = None
        self.event_loop = asyncio.get_event_loop()

        # BT disconnect callback 
        # (for when bluetooth disconnects unwanted)
        self.bt_disconnect_cb = None
        
        # Start thread for sending commands
        Thread(
            target=self.send_commands,
            daemon=True
        ).start()
        
        # Variables for `send_message`, 
        # for keeping track of earlier messages
        self.old_message = dict()

    def get_interface(self):
        if len(self.interfaces) < 1:
            return None

        return tuple(self.interfaces.items())[0][0]

    def add_interface(self, interface, priority):
        # Save interface object and a corresponding priority and busy flag
        self.interfaces[interface] = priority
        # Sort interfaces by priority
        sorted_iface_items = sorted(
            self.interfaces.items(), 
            key=lambda item: item[1]
        )
        self.interfaces = dict(sorted_iface_items)
        
    def remove_interface(self, interface):
        if interface in self.interfaces:
            del self.interfaces[interface]

    def get_message_id(self):
        self.id += 1
        if self.id > 31:
            self.id = 0
        return self.id

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
        if duplicate_message_a and duplicate_message_b:
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

        self.print_new_commands()

    def add_command(self, command, value):
        # Remove first item if queue is full
        if self.outgoing_commands.full():
            print('remove command from queue')
            _, _ = self.outgoing_commands.get_nowait()
        
        self.outgoing_commands.put_nowait((command, value))

    def send_commands(self):
        while True:
            head = lambda command: command << 5 | self.get_message_id()
            interface = self.get_interface()
            
            command, value = self.outgoing_commands.get()
            printable_message = f'command: {command}, value: {value}'

            # Send over serial
            if type(interface) == Serial:
                if interface.is_open:
                    interface.write(bytearray([ord(':'), head(command), value, ord(';')]))
                else: 
                    print(f'{printable_message} (serial not open)')
            
            elif type(interface) == BleakClient:
                if interface.is_connected:
                    data = [head(command), value]
                    while (not self.outgoing_commands.empty()) and (len(data) < 8):
                        command, value = self.outgoing_commands.get_nowait()
                        data.extend([head(command), value])
                        try:
                            self.event_loop.run_until_complete(self.gatt_send(data))
                        except EOFError:
                            print('Client is disconnected')    
                            if self.bt_disconnect_cb:
                                self.bt_disconnect_cb()

                            break
                else:
                    print(f'{printable_message} (bluetooth not connected)')
            else:
                print(f'{printable_message} (no interface)')

    def print_new_commands(self):
        if not self.incoming_commands.empty():
            for i in range(self.incoming_commands.qsize()):
                print(f'new command: {self.incoming_commands.get()}')

    def bt_notification_cb(self, _, data):
        if len(data) != 2:
            return
        
        head, value = data
        key = (0xE0 & head) >> 5
        address = 0x07 & head
        print(f'Received command {key} (id: {address}) value: {value}')
        incoming_commands.put((key, address, value))

    async def ble_async_connect(self, name):
        atexit.register(self.ble_disconnect, run_callback=False)
        print('Search for device')
        device = await find_device(name)
        if device is None:
            print("Found no relevant device")
            return
        
        client = BleakClient(device.address)
        print(f'Connecting to {device.name} at address {device.address}')
        await client.connect()

        if client:
            await client.start_notify(COMMAND_WRITE_UUID, self.bt_notification_cb)
            return client

        print('could not connect')
        return None

    def ble_connect(self, name, connect_cb, disconnect_cb):
        self.bt_connect_cb = connect_cb
        self.bt_disconnect_cb = disconnect_cb
        self.ble_client = self.event_loop.run_until_complete(self.ble_async_connect(name))
        if type(self.ble_client) == BleakClient:
            if self.ble_client.is_connected and connect_cb:
                connect_cb()

    def ble_disconnect(self, run_callback=True):
        if type(self.ble_client) == BleakClient:
            if self.ble_client.is_connected:
                print(f'Disconnecting from {self.ble_client.address}')
                self.event_loop.run_until_complete(self.ble_client.disconnect())
            else:
                print('Client is already disconnected')

        if self.bt_disconnect_cb and run_callback:
            self.bt_disconnect_cb()
        
        self.ble_client = None

    async def gatt_send(self, data):
        data_bytes = bytearray(data)
        start = time()
        await self.ble_client.write_gatt_char(COMMAND_WRITE_UUID, data_bytes)
        print(f'Time used to send: {(time()-start)*1000} ms')

async def find_device(name):
    correct_device = lambda d, _: name in d.name.lower()
    device = await BleakScanner.find_device_by_filter(
        correct_device,
        timeout=5
    )
    return device

def list_comports():
    return list(port.device for port in comports())

def init_serial():
    ser = Serial()
    atexit.register(ser.close)
    return ser

def open_serial(ser, port, baudrate):
    ser.port = port
    ser.baudrate = baudrate

    try: # Try to open the port
        print(f'Opening serial at:\nport: {ser.port}, baud: {ser.baudrate}')
        ser.open()
        return True

    except SerialException:
        print(f'Could not open serial port at {ser.port} with {ser.baudrate}.')
        print(f'Maybe try one of these: {list_comports()}')
        return False

def read_serial_thread(ser):
    """
    Function that continually reads serial information and prints to terminal
    """
    serial_buffer = b''
    while ser.is_open:
        try:
            # add new characters to buffer
            serial_buffer += ser.read()

        except TypeError:
            # serial was apparently closed
            print('Stopped listening to serial')
            return None

        if b':' not in serial_buffer:
            # reset serial_buffer if there is no command start
            serial_buffer = b''

        # command beginning and end is in buffer
        if b':' in serial_buffer and b';' in serial_buffer:
            while len(serial_buffer) > 0:
                start = serial_buffer.find(b':')
                end = serial_buffer.find(b';') + 1
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
                    incoming_commands.put((command, message_id, value))

                    # command finished, reset
                    serial_buffer = serial_buffer[end:]
