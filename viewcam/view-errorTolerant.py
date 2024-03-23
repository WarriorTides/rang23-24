# Desc: View the camera feed thorugh OpenCV
import time
import cv2
import sys

print("imported cv2")

camera = "http://bob.local:5000/"
# for arg in sys.argv:

#     if arg == "-b":
#         path = "http://bobalina.local:5000/"


def open_camera(decice_name):
    cap = cv2.VideoCapture(decice_name)
    if not cap.isOpened():
        print(f"Failed to open camera {decice_name}")
        return None
    print(f"Opened camera {decice_name}")
    getCameraFeed(cap)


def getCameraFeed(cap):

    while True:
        if cap is None:
            print("Attempting to reconnect camera...")

            open_camera(camera)

            return
            # time.sleep(2)  # Wait for 5 seconds before attempting to reconnect
            # continue

        ret, frame = cap.read()
        if frame is None:
            # cap = open_camera(camera)
            cap.release()
            open_camera(camera)

            print("Failed to read frame")
            return
            # cap = open_camera(camera)  # Attempt to reopen the camera
            # ret, frame = cap.read()

            # time.sleep(1)  # Wait for 5 seconds before attempting to reopen
            # continue

        cv2.imshow("Camera Feed", frame)
        cv2.waitKey(1)


open_camera(camera)
