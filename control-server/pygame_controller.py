import socket
import sys
import time
from flask import Flask
import pygame
from pygame.locals import *
import subprocess
import power_comp

import os
import socketio


sio = socketio.Client()


@sio.event
def connect():
    print("I'm connected!")


@sio.event
def disconnect():
    print("I'm disconnected!")


SEND_UDP = True
ROV_MAX_AMPS = 25
MAX_TROTTLE = 0.5
RUN_THRUSTER = True
arduino_ip = "192.168.1.151"
arduino_port = 8888
ARDUINO_DEVICE = (arduino_ip, arduino_port)
SOCKETEVENT = pygame.event.custom_type()
mapping = [
    {"name": "OFL", "color": "gray", "index": 3, "posIndex": 0, "rightpad": 2},
    {"name": "OFR", "color": "cyan", "index": 1, "posIndex": 1, "rightpad": 1},
    {"name": "IFL", "color": "blue", "index": 2, "posIndex": 2, "rightpad": 0},
    {"name": "IFR", "color": "purple", "index": 5, "posIndex": 3, "rightpad": 2},
    {"name": "IBL", "color": "yellow", "index": 7, "posIndex": 4, "rightpad": 0},
    {"name": "IBR", "color": "red", "index": 0, "posIndex": 5, "rightpad": 1},
    {"name": "OBL", "color": "orange", "index": 6, "posIndex": 6, "rightpad": 2},
    {"name": "OBR", "color": "pink", "index": 4, "posIndex": 7, "rightpad": 0},
]
mapping_dict = {item["name"]: item["index"] for item in mapping}
print(mapping_dict)

# GETIP and set it to device_ip
command = "ifconfig | grep 192 |awk '/inet/ {print $2; exit}' "
result = subprocess.run(
    command, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
)
device_ip = ""
if result.returncode == 0:
    device_ip = result.stdout.strip()
else:
    print(f"Command failed with error: {result.stderr}")

# env vars to make joystik work in background
os.environ["SDL_VIDEO_ALLOW_SCREENSAVER"] = "1"
os.environ["SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"] = "1"
os.environ["SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS"] = "1"
os.environ["SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR"] = "0"


CTRL_DEADZONES = [0.2] * 6  # Adjust these to your liking.


def mapnum(
    num,
    outMin,
    outMax,
    inMin=-1,
    inMax=1,
):
    return round(
        outMin + (float(num - inMin) / float(inMax - inMin) * (outMax - outMin))
    )


def formatMessage(message):
    # convert message array to comma sepearted string

    output = "c"
    if RUN_THRUSTER:
        for i in range(len(message)):
            output += "," + str(message[i])
    else:
        for i in range(len(message)):
            output += ",1500"
    return output


class mainProgram(object):
    def init(self):
        pygame.init()
        self.curcam = 1
        self.runpid = False
        self.camval = [60, 70, 95]
        self.runJoy = True
        self.maxThrottle = MAX_TROTTLE
        self.curMessage = ""
        self.wrist = 0  # 0 is flat 1 is vertical

        self.lastaxes = []
        self.lastbuttons = []

        pygame.joystick.init()

        self.joycount = pygame.joystick.get_count()
        if self.joycount == 0:
            print(
                "This program only works with at least one joystick plugged in. No joysticks were detected."
            )
            self.quit(1)
        # for i in range(self.joycount):
        self.joystick = pygame.joystick.Joystick(0)
        self.joystick.init()
        self.axiscount = self.joystick.get_numaxes()
        self.buttoncount = self.joystick.get_numbuttons()
        self.axes = [0.0] * self.axiscount
        self.buttons = [0] * self.buttoncount
        sio.emit("joystick", "Power:" + str(self.maxThrottle))
        sio.emit("joystick", "ControlMode:" + str(not self.runJoy))
        # get all trhustures that have a O in the name and get array of indexes
        upthrust = [item["index"] for item in mapping if "I" in item["name"]]
        # set curmessage to t, comma seprated indexes of upthrust
        self.curMessage = "t," + ",".join(str(x) for x in upthrust)
        print(self.curMessage)
        self.sendUDP()

        self.curMessage = "p,550,9,1"
        print(self.curMessage)
        self.sendUDP()

        # Find out the best window size

    def run(self):
        print("Running")
        pygameRunning = True
        while pygameRunning:
            for event in [pygame.event.wait()] + pygame.event.get():
                if event.type == QUIT:
                    pygameRunning = False
                elif event.type == JOYAXISMOTION:
                    # print("Axis: " + str(event.axis) + " Value: " + str(event.value))
                    self.axes[event.axis] = event.value
                elif event.type == JOYBUTTONUP:
                    self.buttons[event.button] = 0
                elif event.type == JOYBUTTONDOWN:
                    self.buttons[event.button] = 1
                elif event.type == SOCKETEVENT:
                    print("Socket event: " + str(event.message))
                    if event.message == "STATUS":
                        sio.emit("joystick", "ControlMode:" + str(not self.runJoy))
                        time.sleep(0.1)
                        sio.emit("joystick", "Power:" + str(self.maxThrottle))
                    if "PIDOFF" in event.message:
                        self.runpid = False
                        self.curMessage = "f"
                        self.sendUDP()
                        self.control()
                    if "PIDON" in event.message:
                        self.runpid = True
                        self.curMessage = "n"
                        self.sendUDP()
                        self.control()
                    if "Power:" in event.message:
                        self.maxThrottle = float(event.message.split(":")[1])
                        print("Power: " + str(self.maxThrottle))
                    if "c" in event.message:
                        self.runJoy = False
                        self.curMessage = event.message
                        self.sendUDP()
                    if "xy" in event.message:
                        self.curMessage = event.message
                        self.sendUDP()
                    if event.message == "RUN":
                        self.runJoy = True

            for i in range(len(self.axes)):
                if abs(self.axes[i]) < CTRL_DEADZONES[i]:
                    self.axes[i] = 0.0
                self.axes[i] = round(self.axes[i], 2)
            # Check for change in vals
            if str(self.axes) != str(self.lastaxes) or str(self.buttons) != str(
                self.lastbuttons
            ):
                self.lastaxes = list(self.axes)
                self.lastbuttons = list(self.buttons)
                # print("ME SEES A CHANGE")
                # print("Axes: " + str(self.axes) + " Buttons: " + str(self.buttons))
                # print()
                if self.runJoy:
                    self.control()
            # time.sleep(0.1)

    """
    Thruster Mapping:
    1: IFL (blue)
    2: OFL (gray)
    3: IBL (yellow)
    4: OBL (orange)
    5: OFR (cyan)
    6: IBR (red)
    7: IFR (purple)
    8: OBR (pink)

    """

    def control(self):
        # print("Control")
        if RUN_THRUSTER:
            if self.buttons[2] == 1:  # square button
                self.runpid = not self.runpid
                if self.runpid:
                    self.curMessage = "n"
                    print("Running PID")
                else:
                    self.curMessage = "f"
                    print("Stopping PID")
                self.sendUDP()
        if (
            self.axes[-2] == 1 and not self.maxThrottle == 0.3
        ):  # left trigger is index: 5

            self.maxThrottle = 0.3
            sio.emit("joystick", "Power:" + str(self.maxThrottle))
        elif self.axes[-2] == -1 and not self.maxThrottle == 0.50:
            self.maxThrottle = 0.50
            sio.emit("joystick", "Power:" + str(self.maxThrottle))

        sway = -self.axes[2]  # right stick left right

        heave = self.axes[3]  # right stick up down
        # x button for pich and roll

        if self.buttons[0] == 0:  # x button

            surge = self.axes[1]
            yaw = -self.axes[0]
            roll = 0
            pitch = 0
        else:
            surge = 0
            yaw = 0
            roll = -self.axes[0]
            pitch = self.axes[1]
        controlData = {
            "surge": surge,
            "sway": sway,
            "heave": heave,
            "yaw": yaw,
            "roll": roll,
            "pitch": pitch,
            "axes": self.axes,
            "buttons": self.buttons,
        }
        # print(con)
        combinedthrust = {
            "IFL": heave - roll + pitch,
            "OFR": surge - yaw - sway,
            "OFL": surge + yaw + sway,
            "IBL": heave - roll - pitch,
            "IBR": heave + roll - pitch,
            "IFR": heave + roll + pitch,
            "OBR": surge - yaw + sway,
            "OBL": surge + yaw - sway,
        }
        combined = [0] * 8
        for key, value in combinedthrust.items():
            combined[mapping_dict[key]] = value

        # Step 3: Extract the values from the sorted items to create the final array
        # final_array = list(sorted_combinedthrust)

        # print(final_array)

        # forward,right,up are positive
        # cr
        max_motor = max(abs(x) for x in combined)
        max_input = max(
            abs(surge), abs(sway), abs(heave), abs(yaw), abs(pitch), abs(roll)
        )
        if max_motor == 0:
            max_motor = 1
        for i, t in enumerate(combined):
            combined[i] = mapnum(
                (t / max_motor * max_input),
                1500
                - (
                    400 * self.maxThrottle
                ),  # multiply 400 by maxtrhottle if u want that
                1500 + (400 * self.maxThrottle),
            )

        combined = power_comp.calcnew(combined, ROV_MAX_AMPS)

        # wrist: button[1]  claw: axes[-1]
        if self.buttons[1] == 1:  # circle button
            self.wrist += 1
            if self.wrist > 1:
                self.wrist = 0

        if not SEND_UDP:
            sio.emit("joystick", str(controlData))
        # prin()
        self.curMessage = formatMessage(combined) + ","
        if self.buttons[3] == 1:  # triangle button
            self.curcam += 1
            if self.curcam > 2:
                self.curcam = 0
        self.curMessage += str(self.camval[self.curcam])

        if self.wrist == 1:  # wristh is cicle button
            self.curMessage += ",96"
        else:
            self.curMessage += ",15"
        if self.axes[-1] == 1:
            self.curMessage += ",16"
        else:
            self.curMessage += ",130"

        self.sendUDP()

    # angularx = self.axes[4]

    def sendUDP(self):
        if SEND_UDP:
            sock.sendto(self.curMessage.encode(), ARDUINO_DEVICE)
            print(f"Sent: {self.curMessage}")
            print("Waiting for response...")
            sock.settimeout(1.0)  # Set the timeout to  1 second
            try:
                data, server = sock.recvfrom(8888)
                sio.emit("joystick", str(data.decode()))
                print(f"Received: {data.decode()}")
            except socket.timeout:
                print("No response received within  1 second.")
        # else:
        #     sio.emit("joystick", self.curMessage)

    def quit(self, status=0):
        pygame.quit()
        sys.exit(status)


def runJoyStick():
    global sock
    # sio.connect("http://localhost:5001")
    sio.connect("http://localhost:5001", transports=["websocket"])

    # sio.wait()

    if SEND_UDP:
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        sock.bind((device_ip, arduino_port))

    try:
        program = mainProgram()
        program.init()

        program.run()

    except KeyboardInterrupt:
        pygame.quit()
        if SEND_UDP:
            sock.close()

    finally:
        pygame.quit()
        if SEND_UDP:
            sock.close()


if __name__ == "__main__":
    runJoyStick()
