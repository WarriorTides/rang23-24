import time
import threading

import pygame
import pygame_controller
import getPID

# Setup Flask and SocketIO
from flask import Flask

from flask_socketio import SocketIO, send

app = Flask(__name__)
app.config["SECRET_KEY"] = "secret!"
socketio = SocketIO(app, cors_allowed_origins="*")

# pygame.init()


def runSocket():
    socketio.run(app, port=5001, host="0.0.0.0")


def sendMsg(msg):
    socketio.emit("message", msg)
    print("Sent: " + str(msg))


@socketio.on("message")
def handle_message(data):
    # print("received message: " + str(data))
    #
    # sendMsg(str(data))
    pygame.event.post(pygame.event.Event(pygame_controller.SOCKETEVENT, message=data))


@socketio.on("joystick")
def handle_message(data):
    print("received message: " + str(data))
    send(str(data), broadcast=True)


@socketio.on("connect")
def handle_message():
    print("connected")
    # send("connected", broadcast=True)


@socketio.on("disconnect")
def handle_message():
    print("disconnected")
    # send("disconnected", broadcast=True)


if __name__ == "__main__":
    socketio_thread = threading.Thread(target=runSocket)
    socketio_thread.start()
    pid_thread = threading.Thread(target=getPID.runStuff)
    pid_thread.start()
    # time.sleep(1)
    pygame_controller.runJoyStick()
