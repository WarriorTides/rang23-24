import socket
import sys
from flask import Flask
import pygame
from pygame.locals import *


import os
import socketio


sio = socketio.Client()


@sio.event
def connect():
    print("I'm connected!")


@sio.event
def disconnect():
    print("I'm disconnected!")


SEND_UDP = False
MAX_TROTTLE = 0.7
arduino_ip = "192.168.1.112"
arduino_port = 1337
ARDUINO_DEVICE = (arduino_ip, arduino_port)
SOCKETEVENT = pygame.event.custom_type()


device_ip = "192.168.1.164"

os.environ["SDL_VIDEO_ALLOW_SCREENSAVER"] = "1"
os.environ["SDL_JOYSTICK_ALLOW_BACKGROUND_EVENTS"] = "1"
os.environ["SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS"] = "1"
os.environ["SDL_VIDEO_X11_NET_WM_BYPASS_COMPOSITOR"] = "0"


CTRL_DEADZONES = [0.07] * 6  # Adjust these to your liking.


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
    for i in range(len(message)):
        output += "," + str(message[i])
    return output + ",180,90,90"


class mainProgram(object):
    def init(self):
        pygame.init()
        self.runJoy = False

        self.curMessage = ""
        pygame.joystick.init()
        self.lastaxes = []
        self.lastbuttons = []
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

        # Find out the best window size

    def run(self):
        print("Running")
        pygameRunning = True
        while pygameRunning:
            for event in [
                pygame.event.wait(),
            ] + pygame.event.get():
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
                    if "c" in event.message:
                        self.runJoy = False
                        self.curMessage = event.message
                        self.sendUDP()
                    if event.message == "RUN":
                        self.runJoy = True

                    # socketconfig.sendMsg("RECIVED MESSAGE FROM SOCKET", broadcast=True)
                # Get rid of deadzones

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
    1: IFR purpole
    2: OFL grey
    3: IFL blue
    4: OBL orange
    5: OFR light blue
    6: IBR red
    7: IBL yellow
    8: OBR pink
    
    
    """

    def control(self):
        # print("Control")
        surge = self.axes[1]
        sway = -self.axes[0]
        heave = self.axes[3]
        yaw = -self.axes[2]
        controlData = {
            "surge": surge,
            "sway": sway,
            "heave": heave,
            "yaw": yaw,
            "axes": self.axes,
            "buttons": self.buttons,
        }
        sio.emit("joystick", str(controlData))
        # print(con)

        # forward,right,up are positive
        combined = [
            -(heave),  # IFR purpole
            surge + yaw + sway,  # OFL grey
            heave,  # IFL blue
            -(surge + yaw - sway),  # OBL orange
            surge - yaw - sway,  # OFR light blue
            heave,  # IBR red
            heave,  # IBL yellow
            surge - yaw + sway,  # OBR pink
        ]

        max_motor = max(abs(x) for x in combined)
        max_input = max(abs(surge), abs(sway), abs(heave), abs(yaw))
        if max_motor == 0:
            max_motor = 1
        for i, t in enumerate(combined):
            combined[i] = mapnum(
                (t / max_motor * max_input),
                1500 - (MAX_TROTTLE * 400),
                1500 + (MAX_TROTTLE * 400),
            )
        # print("Combined: " + str(formatMessage(combined)))
        self.curMessage = formatMessage(combined)
        self.sendUDP()

    # angularx = self.axes[4]

    def sendUDP(self):
        if SEND_UDP:
            sock.sendto(self.curMessage.encode(), ARDUINO_DEVICE)
            print(f"Sent: {self.curMessage}")
            print("Waiting for response...")
            data, server = sock.recvfrom(1337)
            print(f"Received: {data.decode()}")

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
