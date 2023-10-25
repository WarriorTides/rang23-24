from flask import Flask, render_template, Response
import cv2

app = Flask(__name__)

maincap = cv2.VideoCapture("http://maincam.local:5000/")
# maincap = cv2.VideoCapture(1)
cap2 = cv2.VideoCapture(1)


def gen_frames(cap):
    while True:
        success, frame = cap.read()  # read the camera frame
        if not success:
            break
        else:
            ret, buffer = cv2.imencode(".jpg", frame)
            frame = buffer.tobytes()
            yield (b"--frame\r\n" b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n")


@app.route("/maincam")
def maincam():
    return Response(
        gen_frames(maincap), mimetype="multipart/x-mixed-replace; boundary=frame"
    )


@app.route("/bottomcam")
def video_feed2():
    return Response(
        gen_frames(cap2), mimetype="multipart/x-mixed-replace; boundary=frame"
    )


@app.route("/")
def index():
    return "Hello World!"


if __name__ == "__main__":
    app.run()
