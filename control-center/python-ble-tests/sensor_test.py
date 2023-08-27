import asyncio
from bleak import BleakClient, BleakScanner
from time import time
from struct import unpack, calcsize
from aioconsole import ainput
import numpy as np

MODEL_NBR_UUID =        "00002a24-0000-1000-8000-00805f9b34fb"
CUSTOM_SERVICE_UUID =   "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID =    "12345678-1234-5678-1234-56789abcdef1"
DEBUG_SERVICE_UUID =    "deb12345-1234-5678-1234-56789abcdefa"
DEBUG_READ_UUID =       "deb12345-1234-5678-1234-56789abcdefb"
PING_COMMAND = 2
LEFT_MOTOR_COMMAND = 3
RIGHT_MOTOR_COMMAND = 4

measurements = []

def debug_callback(_, data):
    debug_info = unpack('< q HHH HHH HHH H xxxx', data)
    measurements.append(debug_info)

async def write_command(client, key, value):
    if write_command.id >= 32:
        write_command.id = 0
    head = 0xFF & ((key << 5) | write_command.id)
    data = bytearray([head, value])
    await client.write_gatt_char(COMMAND_WRITE_UUID, data)
    write_command.id += 1

write_command.id = 0    

async def ping_continously(client):
    while True:
        await write_command(client, PING_COMMAND, 1)
        await asyncio.sleep(2)

async def connect(address):
    async with BleakClient(address) as client:
        # Check if debug characteristic is enabled
        debug_characteristic = client.services.get_characteristic(
            DEBUG_READ_UUID
        )
        if debug_characteristic is not None:
            print(f'Device {address} ' + \
                'supports sensor debug characteristic')
            await client.start_notify(DEBUG_READ_UUID, debug_callback)

        # Start pinging
        ping_task = asyncio.create_task(ping_continously(client))
        
        # Wait 2 seconds
        await asyncio.sleep(2)

        # Start spinning!
        await write_command(client, LEFT_MOTOR_COMMAND, 100+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -100+127)

        # Wait for input, so the test can be stopped
        await ainput('Enter any key to quit')

        # Stop test
        await write_command(client, LEFT_MOTOR_COMMAND, 0+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, 0+127)
        ping_task.cancel()
        print('Test over, wait 4 seconds before ending program')
        await asyncio.sleep(4)

        # Save measurements
        print(f'Got {len(measurements)} measurements')
        measurements_np = np.array(measurements)
        with open('test.npy', 'wb') as f:
            np.save(f, measurements_np)

async def main():
    correct_device = lambda d, _: 'rupert' in d.name.lower()
    device = await BleakScanner.find_device_by_filter(
        correct_device,
        timeout=5
    )
    if device:
        print(f'Found device: {device.name} ({device.address})')
        await connect(device.address)
        print('Program finished')
    
    else:
        print('Found no relevant devices')

asyncio.run(main())