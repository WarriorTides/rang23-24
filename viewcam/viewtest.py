from flask import Flask, jsonify, send_file, Response
from flask_cors import CORS, cross_origin  # Add this line

import cv2

app = Flask(__name__)
CORS(app)  # Add this line


def gen_frames(cap):
    while True:
        success, frame = cap.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode(".jpg", frame)
            frame = buffer.tobytes()
            yield (b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n")


cams = ["maincam", "bottomcam"]
caps = ["http://bob.local:5000/", cv2.VideoCapture(0)]


@app.route("/<string:cam>")
@cross_origin()  # Add this line
def streamcam(cam):
    if cam not in cams:
        return "Invalid cam"
    camindex = cams.index(cam)
    return Response(
        gen_frames(caps[camindex]),
        mimetype="multipart/x-mixed-replace; boundary=frame",
    )


@app.route("/")
@cross_origin()  # Add this line
def index():
    # return a json of all the cams
    return jsonify(cams)


if __name__ == "__main__":
    app.run(port=5000)
