# Desc: View the camera feed through OpenCV
import datetime
import time
import cv2
import sys

print("imported cv2")
savePath = "./"
camera = "http://bob.local:5000"
for arg in sys.argv:
    if arg == "-b":
        camera = "http://bobalina:5000/"

# Global variables
recording = False
video_writer = None
frame_counter = 0


def getCameraFeed(cap):
    global recording, video_writer, frame_counter

    while True:
        if cap is None:
            print("Attempting to reconnect camera...")
            open_camera(camera)
            return

        ret, frame = cap.read()
        if frame is None:
            cap.release()
            open_camera(camera)
            print("Failed to read frame")
            return
        frame = cv2.resize(frame, (1280, 720))

        # Display text on the screen
        if recording:
            video_writer.write(frame)
            cv2.putText(
                frame,
                "Recording...",
                (10, 30),
                cv2.FONT_HERSHEY_SIMPLEX,
                1,
                (0, 0, 255),
                2,
            )

        cv2.imshow("Camera Feed", frame)
        key = cv2.waitKey(1) & 0xFF

        # Start/stop recording on 'r' key press
        if key == ord("r"):
            if not recording:
                # Start recording
                fourcc = cv2.VideoWriter_fourcc(*"mp4v")  # or use 'XVID'
                datestring = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                # "/Volumes/file-holder/"
                video_writer = cv2.VideoWriter(
                    savePath + datestring + ".mp4",
                    fourcc,
                    20.0,
                    (1280, 720),
                )
                recording = True
                frame_counter = 0
            else:
                # Stop recording
                video_writer.release()
                video_writer = None
                recording = False

        # Take a picture on 'p' key press
        if key == ord("p"):
            datestring = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            image_name = savePath + datestring + ".jpg"
            cv2.imwrite(image_name, frame)
            print(f"Image saved: {image_name}")

        # Break the loop on 'q' key press
        if key == ord("q"):
            break

    cap.release()
    cv2.destroyAllWindows()


def open_camera(device_name):
    cap = cv2.VideoCapture(device_name)

    while not cap.isOpened():
        if not cap.isOpened():
            print(f"Failed to open camera {device_name}")
            time.sleep(0.5)
            cap = cv2.VideoCapture(device_name)

    print(f"Opened camera {device_name}")
    getCameraFeed(cap)


# def getCameraFeed(cap):
#     global recording, video_writer, frame_counter

#     while True:
#         if cap is None:
#             print("Attempting to reconnect camera...")
#             open_camera(camera)
#             return

#         ret, frame = cap.read()
#         if frame is None:
#             cap.release()
#             open_camera(camera)
#             print("Failed to read frame")
#             return
#         frame = cv2.resize(frame, (1280, 720))

#         # Display text on the screen
#         if recording:
#             video_writer.write(frame)
#             cv2.putText(
#                 frame,
#                 "Recording...",
#                 (10, 30),
#                 cv2.FONT_HERSHEY_SIMPLEX,
#                 1,
#                 (0, 0, 255),
#                 2,
#             )

#         cv2.imshow("Camera Feed", frame)
#         key = cv2.waitKey(1) & 0xFF

#         # Start/stop recording on 'r' key press
#         if key == ord("r"):
#             if not recording:
#                 # Start recording
#                 fourcc = cv2.VideoWriter_fourcc(*"mp4v")  # or use 'XVID'
#                 datestring = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
#                 # "/Volumes/file-holder/"
#                 video_writer = cv2.VideoWriter(
#                     "/Volumes/file-holder/" + datestring + ".mp4",
#                     fourcc,
#                     20.0,
#                     (1280, 720),
#                 )
#                 recording = True
#                 frame_counter = 0

#             else:
#                 # Stop recording
#                 video_writer.release()
#                 video_writer = None
#                 recording = False

#         # Write frame to video file if recording

#         # Break the loop on 'q' key press
#         if key == ord("q"):
#             break

#     cap.release()
#     cv2.destroyAllWindows()


open_camera(camera)
