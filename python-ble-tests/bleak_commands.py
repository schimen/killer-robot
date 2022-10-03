import asyncio
from bleak import BleakClient, BleakScanner
from time import time

MODEL_NBR_UUID =        "00002a24-0000-1000-8000-00805f9b34fb"
CUSTOM_SERVICE_UUID =   "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID =    "12345678-1234-5678-1234-56789abcdef1"

def callback(_, data):
    print(f'Notification: {[*data]}')

async def write_command(client, key, value):
    if write_command.id >= 32:
        write_command.id = 0
    head = 0xFF & ((key << 5) | write_command.id)
    data = bytearray([head, value])
    await client.write_gatt_char(COMMAND_WRITE_UUID, data)
    print(f'Wrote command {key} (id: {write_command.id}), value {value}')
    write_command.id += 1

write_command.id = 0    

async def connect(address):
    async with BleakClient(address) as client:
        await client.start_notify(COMMAND_WRITE_UUID, callback)
        while True:
            command = input('Command: ')
            value = input('Value: ')
            if not command.isdigit or not value.isdigit:
                print(f'Command "{command}" or value "{value}" is not numerical')
                continue

            await write_command(client, int(command), int(value))

async def main():
    devices = await BleakScanner.discover()
    for d in devices:
        if 'gatt-test' in d.name.lower():
            print(f'Found device: {d.name} ({d.address})')
            await connect(d.address)
            return

    print('Found no relevant devices')

asyncio.run(main())
