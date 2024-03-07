# Desc: View the camera feed thorugh OpenCV
import cv2

print("imported cv2")

cap = cv2.VideoCapture("http://bob.local:5000/")

while cap.isOpened():
    ret, frame = cap.read()
    if frame is None:
        continue
    cv2.imshow("Camera Feed", frame)
    cv2.waitKey(1)
