import asyncio
import serial

# Define the serial port and baud rate for your Arduino
SERIAL_PORT = "/dev/ttyUSB0"
BAUD_RATE = 9600


# Function to send data to Arduino
async def send_data(arduino, data):
    arduino.write(data.encode())


# Function to receive data from Arduino
async def receive_data(arduino):
    while True:
        data = arduino.readline().decode().strip()
        if data:
            # Do something with the received data
            print(f"Received data: {data}")


# Main function
async def main():
    # Open the serial connection with Arduino
    arduino = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)

    # Start the receive_data task
    receive_task = asyncio.create_task(receive_data(arduino))

    while True:
        # Ask the user for input
        command1 = input("cmd:")
        time1 = input("time:")
        command2 = input("cmd:")
        time2 = input("time:")
        data = command1 + "," + time1 + "/" + command2 + "!" + time2
        # Send the user input to Arduino
        await send_data(arduino, data)

        # Add any additional logic or processing here


# Run the main function
if __name__ == "__main__":
    asyncio.run(main())
