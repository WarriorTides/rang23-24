from flask import Flask, jsonify, send_file, Response

import cv2

app = Flask(__name__)


# maincap = cv2.VideoCapture("http://maincam.local:5000/")
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
# caps = [cv2.VideoCapture(cam) for cam in possible_cams]
caps = [cv2.VideoCapture(0), cv2.VideoCapture(0)]


@app.route("/<string:cam>")
def streamcam(cam):
    if cam not in cams:
        return "Invalid cam"
    camindex = cams.index(cam)
    return Response(
        gen_frames(caps[camindex]),
        mimetype="multipart/x-mixed-replace; boundary=frame",
    )


@app.route("/")
def index():
    # return a json of all the cams
    return jsonify(cams)


if __name__ == "__main__":
    app.run(port=1100)
