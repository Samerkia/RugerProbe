import asyncio
from bleak import BleakClient

# Define the BLE device address and characteristic UUID
DEVICE_ADDRESS = "B0:B2:1C:56:7F:D6"  # Replace with your BLE device address
CHARACTERISTIC_UUID = "2A57"  # The characteristic UUID your Arduino uses

async def read_data():
    # Create a BleakClient instance to connect to the BLE device
    async with BleakClient(DEVICE_ADDRESS, timeout=60) as client:
        print(f"Connected: {client.is_connected}")

        while True:
            # Read the data from the characteristic
            data = await client.read_gatt_char(CHARACTERISTIC_UUID)
            
            # Decode the data and print it as a string
            decoded_data = data.decode('utf-8')
            print(f"Received data: {decoded_data}")
            
            await asyncio.sleep(1)  # Optional: sleep for a short time to avoid high CPU usage

# Run the asyncio loop to connect and read the data continuously
loop = asyncio.get_event_loop()
loop.run_until_complete(read_data())