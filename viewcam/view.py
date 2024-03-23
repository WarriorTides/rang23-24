import cv2
import threading
from http.server import BaseHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn
import io
import time

cams = [0, 0]
# cams=["http://bob.local:5000","http://bobalina.local:5000"]


# Capture the camera feed
def capture_feed(camera_index):
    cap = cv2.VideoCapture(camera_index)
    while True:
        ret, frame = cap.read()
        # if frame is None:
        #     print("Failed to read frame")
        #     cap.release()
        #     cap = cv2.VideoCapture(camera_index)
        #     continue
        if not ret:
            break
        yield cv2.imencode(".jpg", frame)[1].tobytes()


# HTTP request handler for a specific camera feed
class StreamingHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        camera_index = cams[int(self.path.split("/")[-1].split("?")[0])]
        # if

        self.send_response(200)
        self.send_header("Content-type", "multipart/x-mixed-replace; boundary=--frame")
        self.end_headers()
        for frame in capture_feed((camera_index)):
            self.wfile.write(b"--frame\r\n")
            self.send_header("Content-type", "image/jpeg")
            self.send_header("Content-length", len(frame))
            self.end_headers()
            self.wfile.write(frame)
            self.wfile.write(b"\r\n")


# Threaded HTTP server
class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    pass


if __name__ == "__main__":
    server_address = ("", 8000)
    httpd = ThreadedHTTPServer(server_address, StreamingHandler)
    print("Starting server...")
    httpd.serve_forever()
