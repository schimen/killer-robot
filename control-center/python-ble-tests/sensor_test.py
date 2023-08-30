import asyncio
from bleak import BleakClient, BleakScanner
from time import time
from struct import unpack, calcsize
from aioconsole import ainput
import numpy as np
from argparse import ArgumentParser

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
    debug_info = unpack('< q hhh hhh hhh h xxxx', data)
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

async def do_test(client, test_string):
    if 'test' in test_string.lower():
        print('Type "l" or "r" and a speed to enter speed.\n'+\
                'Enter without "l" or "r" to exit the program.\n'+\
                'Example: "l 100 r -100"\n')
        while True:
            # Wait for input, so the test can be stopped
            result = await ainput('Enter command (enter emtpy to quit): ')
            result = result.split()
            if 'l' in result:
                speed = int(result[result.index('l') + 1])
                await write_command(client, LEFT_MOTOR_COMMAND, speed+127)
            if 'r' in result:
                speed = int(result[result.index('r') + 1])
                await write_command(client, RIGHT_MOTOR_COMMAND, speed+127)
            
            if not 'l' in result and not 'r' in result:
                break

        await write_command(client, LEFT_MOTOR_COMMAND, 0+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, 0+127)

    elif 'silent' in test_string.lower():
        print('Silent test, wait 4 seconds')
        await write_command(client, LEFT_MOTOR_COMMAND, 0+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, 0+127)
        await asyncio.sleep(4)

    elif 'stages' in test_string.lower():
        print('Wait 4 seconds before spinning')
        await asyncio.sleep(4)
        # Start spinning
        print('Spin faster')
        await write_command(client, LEFT_MOTOR_COMMAND, 50+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -50+127)
        await asyncio.sleep(2)
        await write_command(client, LEFT_MOTOR_COMMAND, 75+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -75+127)
        await asyncio.sleep(2)
        await write_command(client, LEFT_MOTOR_COMMAND, 100+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -100+127)
        await asyncio.sleep(2)
        await write_command(client, LEFT_MOTOR_COMMAND, 75+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -75+127)
        await asyncio.sleep(2)
        await write_command(client, LEFT_MOTOR_COMMAND, 50+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -50+127)
        await asyncio.sleep(2)
        await write_command(client, LEFT_MOTOR_COMMAND, 0+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, 0+127)
        print('Stop spinning and wait 4 seconds')
        await asyncio.sleep(4)

    elif 'spinning' in test_string.lower():
        print('Wait 4 seconds before spinning')
        await asyncio.sleep(4)
        # Start spinning
        await write_command(client, LEFT_MOTOR_COMMAND, 100+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, -100+127)
        print('Spinning test, spin for 8 seconds')
        await asyncio.sleep(8)
        # Stop spinning
        await write_command(client, LEFT_MOTOR_COMMAND, 0+127)
        await write_command(client, RIGHT_MOTOR_COMMAND, 0+127)
        print('Stop spinning and wait 4 seconds')
        await asyncio.sleep(4)

    else:
        print(f'Test name `{test_string}` is not recognized')

    # Save measurements
    print(f'Got {len(measurements)} measurements. Save in {test_string.lower()}.npy')
    measurements_np = np.array(measurements)
    with open(test_string.lower() + '.npy', 'wb') as f:
        np.save(f, measurements_np)


async def main():
    parser = ArgumentParser(
        prog='Sensor test',
        description='Do a test woth rupert and save sensor data',
    )
    parser.add_argument('--type',
        type=str, default='test',
        help='Test type. choose from test, silent, stages and spinning'
    )
    test_name = parser.parse_args().type

    correct_device = lambda d, _: 'rupert' in d.name.lower()
    device = await BleakScanner.find_device_by_filter(
        correct_device,
        timeout=5
    )
    if device is None:
        print('Found no relevant devices')
        return
    
    print(f'Found device: {device.name} ({device.address})')
    async with BleakClient(device.address) as client:
        # Check if debug characteristic is enabled
        debug_characteristic = client.services.get_characteristic(
            DEBUG_READ_UUID
        )
        if debug_characteristic is not None:
            print(f'Device {device.address} ' + \
                'supports sensor debug characteristic')
            await client.start_notify(DEBUG_READ_UUID, debug_callback)

        ping_task = asyncio.create_task(ping_continously(client))
        await do_test(client, test_name)
        ping_task.cancel() 

    print('Program finished')
    

asyncio.run(main())