import asyncio
from bleak import BleakClient, BleakScanner
from time import time
from struct import unpack, calcsize
from aioconsole import ainput
import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser

MODEL_NBR_UUID =        "00002a24-0000-1000-8000-00805f9b34fb"
CUSTOM_SERVICE_UUID =   "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID =    "12345678-1234-5678-1234-56789abcdef1"
DEBUG_SERVICE_UUID =    "deb12345-1234-5678-1234-56789abcdefa"
DEBUG_READ_UUID =       "deb12345-1234-5678-1234-56789abcdefb"
PING_COMMAND = 2
LEFT_MOTOR_COMMAND = 3
RIGHT_MOTOR_COMMAND = 4

MAX_VAL = 2**15
ACCEL_RANGE = MAX_VAL/16
GYRO_RANGE = MAX_VAL/2000
MAGN_RANGE = MAX_VAL/4900

measurements = []

def debug_callback(_, data):
    debug_info = unpack('< q hhh hhh hhh h xxxx', data)
    measurements.append(debug_info)

async def live_plot_task():
    # Create plot
    plt.ion()
    _, axs = plt.subplots(3, 1)
    ax_acc, ax_gyr, ax_mag = axs
    # Plot settings
    ax_acc.set_ylabel('Acceleration [g]')
    ax_gyr.set_ylabel('Rotation [dps]')
    ax_mag.set_ylabel(r'Magnetix flux density [$\mu T$]')
    for ax in axs:
        ax.set_xlabel('Time [s]')
        ax.grid()

    plt.show()

    # Live plot
    while True:
        if len(measurements) <= 0:
            await asyncio.sleep(1)
            continue
        
        # Extract data
        data = np.array(measurements)
        time = (data[:, 0] - data[0, 0])/1000
        # Get last 10 seconds of data
        start_time = time[-1] - 10
        area = time > start_time

        data = data[area, :]
        time = time[area]
        accel = data[:, 1:4]/ACCEL_RANGE
        gyro = data[:, 4:7]/GYRO_RANGE
        mag = data[:, 7:10]/MAGN_RANGE

        # Plot accel, gyro and magnetometer data
        senses = (accel, gyro, mag)
        for i, sensor in enumerate(('accel', 'gyro', 'mag')):
            ax = axs[i]
            ax.clear()
            for j, dir in enumerate('XYZ'):
                ax.plot(time, senses[i][:, j], label=f'{dir} {sensor}')

            ax.legend()

        plt.pause(0.1)
        await asyncio.sleep(0.1)

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
        description='Do a test with rupert and save sensor data',
    )
    parser.add_argument('--type',
        type=str, default='test',
        help='Test type. choose from test, silent, stages and spinning'
    )
    parser.add_argument('--live-plot',
        action='store_true',
        help='Set this flag to enable live plotting of data'
    )
    args = parser.parse_args()

    correct_device = lambda d, _: 'rupert' in d.name.lower()
    device = await BleakScanner.find_device_by_filter(
        correct_device,
        timeout=5
    )
    if device is None:
        print('Found no relevant devices')
        return
    
    print(f'Found device: {device.name} ({device.address})')

    if args.live_plot:
        plot_task = asyncio.create_task(live_plot_task())

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
        await do_test(client, args.type)
        ping_task.cancel() 

    plot_task.cancel()
    print('Program finished')
    

asyncio.run(main())