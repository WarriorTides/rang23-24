from flask import Flask
from flask_socketio import SocketIO, send
import socket
import time

import pygame

# Setup Flask and SocketIO
SENDUPD = False
global sock
app = Flask(__name__)
app.config["SECRET_KEY"] = "secret!"
socketio = SocketIO(app, cors_allowed_origins="*")
EVENTTYPE = pygame.event.custom_type()

arduino_ip = "192.168.1.151"
arduino_port = 8888


@socketio.on("message")
def handle_message(data):
    print("received message: " + str(data))
    send(str(data), broadcast=True)
    if SENDUPD:
        sock.sendto(data.encode(), (arduino_ip, arduino_port))

    # pygame.fastevent.post(pygame.event.Event(EVENTTYPE, message=data))


# run app
async def main():
    socketio.run(app, port=5001)


def stop():
    socketio.stop()


if __name__ == "__main__":
    # main()\
    socketio.run(app, port=5001)
    if SENDUPD:
        # Create a UDP socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        # Bind the socket to a specific network interface and port number
        sock.bind(("192.168.1.131", arduino_port))
