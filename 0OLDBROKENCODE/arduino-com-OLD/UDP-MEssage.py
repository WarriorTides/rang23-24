import socket
import time

# Define the IP address and port of the Arduino
arduino_ip = "192.168.1.151"
arduino_port = 8888

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to a specific network interface and port number
sock.bind(("192.168.1.204", arduino_port))

# The message to send

try:
    # Send data continously- used to test rate of messages
    while True:
        # message = input("Enter message: ")

        # message = "c,1500,1500,1500,1500,1500,1500,1500,1500,90,90,90"
        message = input("Enter message: ")
        print(f"{message}")

        sent = sock.sendto(message.encode(), (arduino_ip, arduino_port))
        time.sleep(0.01)
        print("Waiting for response...")
        data, server = sock.recvfrom(8888)
        print(f"Received: {data.decode()}")
        # time.sleep(0.1)


except KeyboardInterrupt:
    print("Closing socket")
    sock.close()

finally:
    print("Closing socket")
    sock.close()
    # c, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 90, 90, 90
