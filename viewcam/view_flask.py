import os
import subprocess
import time
from flask import Flask, jsonify, send_file, Response
import sys
import cv2

app = Flask(__name__)


# maincap = cv2.VideoCapture("http://maincam.local:5000/")
def gen_frames(cap, camindex):
    while True:
        try:
            success, frame = cap.read()  # read the camera frame
            if not success:
                raise ValueError("Could not read frame from camera")
            else:
                ret, buffer = cv2.imencode(".jpg", frame)
                frame = buffer.tobytes()
                yield (
                    b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"
                )
        except ValueError as e:
            print("error")
            # break


possible_cams = ["http://bob.local:5000/"]
cams = ["maincam"]
caps = [cv2.VideoCapture(cam) for cam in possible_cams]


@app.route("/<string:cam>")
def streamcam(cam):
    if cam not in cams:
        return "Invalid cam"
    camindex = cams.index(cam)
    try:
        return Response(
            gen_frames(caps[camindex], camindex),
            mimetype="multipart/x-mixed-replace; boundary=frame",
        )
    except ValueError:
        return "Invalid cam"


@app.route("/restart")
def restarter():
    os.kill(os.getpid(), 9)
    # os.execv("poetry", ["poetry", "run", "python3"] + sys.argv)


@app.route("/")
def index():
    # return a json of all the cams
    # available_cams = [cam for cam in caps if cam.isOpened()]
    return jsonify(cams)


if __name__ == "__main__":
    app.run(port=1234)
