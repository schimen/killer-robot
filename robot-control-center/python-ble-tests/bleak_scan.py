import asyncio
from bleak import BleakScanner

async def main():
    devices = await BleakScanner.discover()
    for d in devices:
        print()
        print(f'Name: {d.name}, address: {d.address}, RSSI: {d.rssi}')
        print(f'Details: {d.details}')
        print(f'Metadata: {d.metadata}')
        print()

asyncio.run(main())
