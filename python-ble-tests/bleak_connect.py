import asyncio
from bleak import BleakClient, BleakScanner
from time import time

MODEL_NBR_UUID =      "00002a24-0000-1000-8000-00805f9b34fb"
CUSTOM_SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID =    "12345678-1234-5678-1234-56789abcdef1"

def callback(_, data):
    if len(data) == 2:
        head, value = data
        key = (0xE0 & head) >> 5
        address = 0x07 & head
        print(f'Received command {key} (id: {address}) value: {value}')
    else:
        print(f'Received data {data}')


async def write_command(client, key, value):
    head = 0xFF & ((key << 5) | write_command.id)
    data = bytearray([head, value])
    print(f'writing {write_command.id} value {value}')
    await client.write_gatt_char(COMMAND_WRITE_UUID, data)
    write_command.id += 1

write_command.id = 0    

async def connect(address):
    async with BleakClient(address) as client:
        # Start notification
        print('Start notification')
        await client.start_notify(COMMAND_WRITE_UUID, callback)

        # Get model number
        start = time()
        model_number = await client.read_gatt_char(MODEL_NBR_UUID)
        print(f'Got model number: {"".join(map(chr, model_number))} in {(time()-start)*1000} ms')

        n_tests = 7
        for i in range(n_tests):
            address = i
            value = 255 - i
            key = i+1
            head = 0xFF & ((key << 5) | address)

            print(f'Test {i+1} of {n_tests}: {key=}, {address=}, {value=}')
            # Write data
            data = bytearray([head, value])
            print(f'Write value: {[*data]}')
            start = time()
            await client.write_gatt_char(COMMAND_WRITE_UUID, data)
            print(f'Wrote value in {(time()-start)*1000} ms')

async def main():
    correct_device = lambda d, _: 'gatt-test' in d.name.lower()
    device = await BleakScanner.find_device_by_filter(
        correct_device,
        timeout=5
    )
    if device:
        print(f'Found device: {device.name} ({device.address})')
        await connect(device.address)
    else:
        print('Found no relevant devices')

asyncio.run(main())
