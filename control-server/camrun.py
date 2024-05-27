import socket
import time
import subprocess
import matplotlib.pyplot as plt

# Define the IP address and port of the Arduino
arduino_ip = "192.168.1.151"
arduino_port = 8888
lastmessage = ""
# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Bind the socket to a specific network interface and port number
command = "ifconfig | grep 192 |awk '/inet/ {print $2; exit}' "
result = subprocess.run(
    command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
)
device_ip = ""

if result.returncode == 0:
    device_ip = result.stdout.strip()
else:
    print(f"Command failed with error: {result.stderr}")
print(device_ip)
sock.bind((device_ip, arduino_port))

# Initialize the plot
plt.ion()  # Turn on interactive mode
fig, ax = plt.subplots()

try:
    # Send data7 contin44ously- used to test rate of messages
    while True:
        newmessage = ""
        with open("./send.txt", "r") as f:
            newmessage = f.read().strip()
        print(f"last message: {lastmessage}")
        if newmessage != lastmessage:
            # print("Sending new message")
            print(newmessage)

            sent = sock.sendto(newmessage.encode(), (arduino_ip, arduino_port))
            lastmessage = newmessage
        # time.sleep(1)
        sock.settimeout(0.1)  # Set the timeout to  1 second
        try:
            sent = sock.sendto("xy".encode(), (arduino_ip, arduino_port))

            data, server = sock.recvfrom(8888)
            print(f"Received: {data.decode()}")
            depth = float(data.decode().split("dd")[1].split("tt")[0])

            # Update the plot
            ax.plot(time.time(), depth, "ro")  # Plot a red dot for each depth value
            ax.set_xlim(
                time.time() - 10, time.time()
            )  # Set the x-axis to the last 10 seconds
            ax.set_ylim(-0.3, 2.5)  # Adjust the y-axis limits as needed
            plt.draw()  # Redraw the plot
            plt.pause(0.01)  # Pause for a short period to allow the plot to update
            time.sleep(0.1)
        except socket.timeout:
            print(".", end="")

        # sock.settimeout(0.1)  # Set the timeout to  1 second
        # try:
        #     data, server = sock.recvfrom(6666)
        #     print(f"Received: {data.decode()}")
        # except socket.timeout:
        #     print("No response received within  1 second.")


except KeyboardInterrupt:
    print("Closing socket")
    sock.close()

finally:
    print("Closing socket")
    sock.close()
    # plt.ioff()  # Turn off interactive mode
    # plt.show()  # Show the plot
