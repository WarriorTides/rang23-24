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


# possible_cams = ["http://maincam.local:5000/", "http://bottomcam.local:5000/"]
possible_cams = ["http://127.0.0.1:1100/maincam", "http://127.0.0.1:1100/bottomcam"]
cams = ["maincam", "bottomcam"]
caps = [cv2.VideoCapture(cam) for cam in possible_cams]
# caps = [cv2.VideoCapture(0), cv2.VideoCapture(1)]
testmode = False
if not testmode:

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

else:

    @app.route("/")
    def index():
        return send_file(
            "/Users/aayanmaheshwari/Desktop/MATE/rang23-24/pi.jpg", mimetype="image/jpg"
        )


if __name__ == "__main__":
    app.run()
