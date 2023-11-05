from flask import Flask, jsonify, send_file, Response
from flask_socketio import SocketIO, send
import socket
import time

# Define the IP address and port of the Arduino
arduino_ip = "192.168.1.123"
arduino_port = 8888

# Create a UDP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(("192.168.1.185", arduino_port))

# Setup Flask and SocketIO
app = Flask(__name__)
app.config["SECRET_KEY"] = "secret!"
socketio = SocketIO(app, cors_allowed_origins="*")


# The message to send UDP
def sendUDP(message):
    print(f"Sending {message}")
    sent = sock.sendto(message.encode(), (arduino_ip, arduino_port))
    time.sleep(0.1)


# handle socketio messages
@socketio.on("message")
def handle_message(data):
    print("received message: " + data)
    sendUDP(str(data))


# run app
if __name__ == "__main__":
    socketio.run(app, port=5001)
