import asyncio
from bleak import BleakClient, BleakScanner
from time import time

MODEL_NBR_UUID =      "00002a24-0000-1000-8000-00805f9b34fb"
CUSTOM_SERVICE_UUID = "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID =    "12345678-1234-5678-1234-56789abcdef1"

def callback(_, data):
    print(f'Notification: {[*data]}')

async def write_command(client, key, value):
    head = 0xFF & ((key << 5) | write_command.id)
    data = bytearray([head, value])
    print(f'writing {write_command.id} value {value}')
    await client.write_gatt_char(COMMAND_WRITE_UUID, data)
    print(f'wrote {write_command.id}: value {value}')
    write_command.id += 1

write_command.id = 0    

async def connect(address):
    async with BleakClient(address) as client:
        # Start notification
        print('Start notification')
        #await client.start_notify(COMMAND_WRITE_UUID, callback)

        # Get model number
        model_number = await client.read_gatt_char(MODEL_NBR_UUID)
        print(f'Model Number: {"".join(map(chr, model_number))}')

        n_tests = 3
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
            
        # Wait a second before disconnecting
        await asyncio.sleep(1)

async def main():
    devices = await BleakScanner.discover()
    for d in devices:
        if 'gatt-test' in d.name.lower():
            print(f'Found device: {d.name} ({d.address})')
            await connect(d.address)
            return

    print('Found no relevant devices')

asyncio.run(main())
