import socket
import time
import subprocess

import flask
import socketio

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

sio = socketio.Client()

ipsfound = []


@sio.event
def connect():
    print("PID connected!")


@sio.event
def disconnect():
    print("PIDdisconnected!")


app = flask.Flask(__name__)

# Function that runs when the app gets a connection


@app.route("/<path:cam>", methods=["GET"])
def page(cam):
    # If the IP that pinged flask is not already connected, and the length of request form is not 0
    curip = cam
    if curip not in ipsfound:
        ipsfound.append(curip)
        print(curip)
        # sio.emit("camera", curip)
    return ""


# Run the app


def runStuff():
    # sio.connect("http://localhost:5001", transports=["websocket"])
    app.run(host="0.0.0.0", port=12345)


runStuff()
