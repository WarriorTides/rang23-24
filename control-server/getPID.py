import socket
import time
import subprocess

import socketio

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sio = socketio.Client()


@sio.event
def connect():
    print("PID connected!")


@sio.event
def disconnect():
    print("PIDdisconnected!")


def runStuff():
    sio.connect("http://localhost:5001", transports=["websocket"])
    time.sleep(0.2)

    try:
        while True:
            sock.settimeout(0.1)  # Set the timeout to  1 second
            try:
                data, server = sock.recvfrom(6666)
                print(f"Received: {data.decode()}")
                sio.emit("joystick", str(data.decode()))

            except socket.timeout:
                print(".", end="")

    except KeyboardInterrupt:
        print("Closing socket")
        sock.close()

    finally:
        print("Closing socket")
        sock.close()
        # plt.ioff()  # Turn off interactive mode
        # plt.show()  # Show the plot
