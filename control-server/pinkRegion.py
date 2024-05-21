import cv2
import numpy as np
import socketio
import time
import datetime
import sys

recording = False
video_writer = None
frame_counter = 0

camera = "http://192.168.1.101:5000/"
for arg in sys.argv:
    if arg == "-b":
        camera = "http://192.168.1.102:5000/"

# Initializes the SocketIO client
sio = socketio.Client()
sio.connect("http://localhost:5001", transports=["websocket"])

def changeDepth(meterschange,sio):
        sio.emit("joystick", "p,"+str(meterschange))


def find_pink_region(image):
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)
    lower_pink = np.array([100, 100, 150])
    upper_pink = np.array([170, 170, 200])
    mask = cv2.inRange(hsv_image, lower_pink, upper_pink)
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    if contours:
        sharp_contours = []
        for cnt in contours:
            epsilon = 0.01 * cv2.arcLength(cnt, True)
            approx = cv2.approxPolyDP(cnt, epsilon, True)
            if len(approx) <= 6:
                sharp_contours.append(cnt)
        if sharp_contours:
            largest_sharp_contour = max(sharp_contours, key=cv2.contourArea)
            x, y, w, h = cv2.boundingRect(largest_sharp_contour)
            pink_region_center = (x + w // 2, y + h // 2)
            pink_region_angles = calculate_angles(pink_region_center, image.shape)
            cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.circle(image, pink_region_center, 5, (0, 0, 255), -1)
        else:
            pink_region_center = None
            pink_region_angles = None
    else:
        pink_region_center = None
        pink_region_angles = None
    return pink_region_center, pink_region_angles, image

def calculate_angles(center, image_shape):
    width, height = image_shape[1], image_shape[0]
    center_x, center_y = center

    # Calculates the horizontal angle
    horizontal_angle = (center_x / width) * 160 - 80

    # Calculates the vertical angle
    vertical_angle = (center_y / height) * 160 - 80

    return horizontal_angle, vertical_angle

def turn_thruster_for_degrees(horizontal_degrees, vertical_degrees, sio):
    print(f"Turning thruster for horizontal: {horizontal_degrees:.2f} degrees, vertical: {vertical_degrees:.2f} degrees")
    sio.emit("joystick", "c,1500,1500,1560,1560,1440,1440,1500,1500,90,90,90")
    time.sleep(abs(horizontal_degrees) * 0.1)
    print("Thruster turned off.")
    sio.emit("joystick", "c,1500,1500,1500,1500,1500,1500,1500,1500,90,90,90")

def open_camera(device_name):
    cap = cv2.VideoCapture(device_name)

    while not cap.isOpened():
        if not cap.isOpened():
            print(f"Failed to open camera {device_name}")
            time.sleep(0.5)
            cap = cv2.VideoCapture(device_name)

    print(f"Opened camera {device_name}")
    return cap

def getCameraFeed(cap):
    global recording, video_writer, frame_counter
    changeDepth(0.6,sio)
    while True:
        if cap is None:
            print("Attempting to reconnect camera...")
            cap = open_camera(camera)
            return

        ret, frame = cap.read()
        if frame is None:
            cap.release()
            cap = open_camera(camera)
            print("Failed to read frame")
            return

        pink_region_center, angles_to_turn, image_with_box = find_pink_region(frame)

        if pink_region_center is not None:
            horizontal_angle, vertical_angle = angles_to_turn
            print(f"The average location of the pink square is {pink_region_center}")
            print(f"The ROV needs to turn horizontally: {horizontal_angle:.2f} degrees, vertically: {vertical_angle:.2f} degrees to face the pink square.")
            cv2.imshow("Image with Bounding Box", image_with_box)
            cv2.waitKey(1)

            # Turns the thruster for the angles  calculated by the pink region code thing
            turn_thruster_for_degrees(horizontal_angle, vertical_angle, sio)
        else:
            print("No pink square found in the image. We are done")
            changeDepth(-0.4,sio)
            break

 

        key = cv2.waitKey(1) & 0xFF

  

        # Breaks the loop when the k key is pressed
        if key == ord("k"):
            break

        time.sleep(2)  # Wait for 2 seconds before repeating

    cap.release()
    cv2.destroyAllWindows()

sio.connect("http://localhost:5001", transports=["websocket"])
cap = open_camera(camera)
getCameraFeed(cap)
sio.disconnect()
