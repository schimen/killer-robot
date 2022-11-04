import asyncio
from bleak import BleakClient, BleakScanner
from time import time

MODEL_NBR_UUID =        "00002a24-0000-1000-8000-00805f9b34fb"
CUSTOM_SERVICE_UUID =   "12345678-1234-5678-1234-56789abcdef0"
COMMAND_WRITE_UUID =    "12345678-1234-5678-1234-56789abcdef1"

def callback(_, data):
    head, value = data
    key = (0xE0 & head) >> 5
    address = 0x07 & head
    print(f'Received command {key} (id: {address}) value: {value}')

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
            if not command.isdigit() or not value.isdigit():
                if any(
                    (
                        'q' in command.lower(), 
                        'q' in value.lower(), 
                        'exit' in command.lower(), 
                        'exit' in value.lower()
                    )
                ):
                    print('Exit program')
                    break
                else:
                    print(f'Command "{command}" or value "{value}" is not numerical')
            else:
                start = time()
                await write_command(client, int(command), int(value))
                print(f'Wrote command in {(time()-start)*1000} ms')

async def main():
    print("Script for sending commands to rupert. Type `exit` or `q` to quit")
    correct_device = lambda d, _: 'rupert' in d.name.lower()
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
