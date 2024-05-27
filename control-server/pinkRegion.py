import cv2
import numpy as np
import socketio
import time


def changeDepth(
    meterschange, sio
):  # sends the depth change to the ROV utilizing the PID controller in the Arduino
    sio.emit("joystick", "p," + str(meterschange))


def find_pink_region(image):
    # Converts the input image from BGR color space to HSV color space
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Defines the lower and upper bounds for the pink color in HSV
    lower_pink = np.array([100, 100, 150])  # Lower bound for pink color in HSV
    upper_pink = np.array([170, 170, 200])  # Upper bound for pink color in HSV

    # Creates a binary mask for the pink color
    mask = cv2.inRange(hsv_image, lower_pink, upper_pink)

    # Finds contours in the binary mask to ensure the region is square and coral head not detected
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # If contours are found
    if contours:
        # Initializes a list to store sharp contours
        sharp_contours = []

        # Iterates through each contour
        for cnt in contours:
            # Calculates the perimeter of the contour
            epsilon = 0.01 * cv2.arcLength(cnt, True)

            # Approximates the contour shape to reduce noise
            approx = cv2.approxPolyDP(cnt, epsilon, True)

            # Checks if the approximate contour has 6 or fewer sides (considered sharp)
            if len(approx) <= 6:
                sharp_contours.append(cnt)

        if sharp_contours:
            largest_sharp_contour = max(sharp_contours, key=cv2.contourArea)
            # creates a bounding box around the detected pink region
            x, y, w, h = cv2.boundingRect(largest_sharp_contour)

            pink_region_center = (x + w // 2, y + h // 2)

            cv2.rectangle(image, (x, y), (x + w, y + h), (0, 255, 0), 2)

            cv2.circle(image, pink_region_center, 5, (0, 0, 255), -1)
            # calcualte angles to rotate
            pink_region_angles = calculate_angles(pink_region_center, image.shape)

        else:
            # If no sharp contours were found, sets the center and angles to None
            pink_region_center = None
            pink_region_angles = None
    else:
        # If no contours were found in the mask, sets the center and angles to None
        pink_region_center = None
        pink_region_angles = None

    # Returns the center of the detected pink region, the angles, and the original image with bounding box
    return pink_region_center, pink_region_angles, image


def calculate_angles(
    center, image_shape
):  # calculates angle in degrees the rov needs to turn from center to face transpant location
    width, height = image_shape[1], image_shape[0]
    center_x, center_y = center

    # Calculates the horizontal angle
    horizontal_angle = (center_x / width) * 160 - 80

    # Calculates the vertical angle
    vertical_angle = (center_y / height) * 160 - 80

    return horizontal_angle, vertical_angle


def turn_thruster_for_degrees(horizontal_degrees, sio):
    print(f"Turning thruster for horizontal: {horizontal_degrees:.2f} degrees")
    if horizontal_degrees > 0:  # Turn right
        sio.emit("joystick", "c,1500,1500,1560,1560,1440,1440,1500,1500,90,90,90")
        time.sleep(abs(horizontal_degrees) * 0.03)  # delay based on angle
        print("Thruster turned off.")
        sio.emit("joystick", "c,1500,1500,1500,1500,1500,1500,1500,1500,90,90,90")
    else:  # Turn left
        sio.emit("joystick", "c,1500,1500,1440,1440,1560,1560,1500,1500,90,90,90")
        time.sleep(abs(horizontal_degrees) * 0.03)
        print("Thruster turned off.")
        sio.emit("joystick", "c,1500,1500,1500,1500,1500,1500,1500,1500,90,90,90")


def open_camera(device_name):  # opens the camera feed and returns capture device
    cap = cv2.VideoCapture(device_name)

    while not cap.isOpened():
        if not cap.isOpened():
            print(f"Failed to open camera {device_name}")
            time.sleep(0.5)
            cap = cv2.VideoCapture(device_name)

    print(f"Opened camera {device_name}")
    return cap


def runAutonomous(cap):
    global recording, video_writer, frame_counter
    changeDepth(0.6, sio)  # raise the ROV to hover above coral resotration area
    while True:

        ret, frame = cap.read()
        if frame is None:  # if the frame is lost, attempt to reconnect
            cap.release()
            cap = open_camera(camera)
            print("Failed to read frame")
            return

        pink_region_center, angles_to_turn, image_with_box = find_pink_region(
            frame
        )  # locate the transplant location

        if pink_region_center is not None:
            horizontal_angle, vertical_angle = angles_to_turn
            print(f"The average location of the pink square is {pink_region_center}")
            print(
                f"The ROV needs to turn horizontally: {horizontal_angle:.2f} degrees, vertically: {vertical_angle:.2f} degrees to face the pink square."
            )
            cv2.imshow("Image with Bounding Box", image_with_box)
            cv2.waitKey(1)

            # Turns the thruster for the angles  calculated by the pink region code thing
            turn_thruster_for_degrees(
                horizontal_angle, sio
            )  # turn the ROV to go head on with the transplant location

            sio.emit(
                "joystick", "c,1500,1500,1560,1560,1560,1560,1500,1500,90,90,90"
            )  # drive straight after rotating

        else:
            print("No pink square found in the image. We are done")
            changeDepth(
                -0.4, sio
            )  # lower and place the coral over the transplant location
            time.sleep(5)
            sio.emit(
                "joystick", "c,1500,1500,1500,1500,1500,1500,1500,1500,180,90,90"
            )  # open claw
            break

        key = cv2.waitKey(1) & 0xFF

        # Breaks the loop when the k key is pressed
        if key == ord("k"):
            break

        time.sleep(2)  # Wait for 2 seconds before repeating

    cap.release()
    cv2.destroyAllWindows()


camera = "http://192.168.1.101:5000/"

# Initializes the Websocket client
sio = socketio.Client()
sio.connect("http://192.168.1.2:5001", transports=["websocket"])
cap = open_camera(camera)
runAutonomous(cap)
sio.disconnect()
