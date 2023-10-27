import cv2


cap = cv2.VideoCapture("http://bottomcam.local:5000/")


while cap.isOpened():
    ret, frame = cap.read()
    if frame is None:
        continue
    cv2.imshow("Camera Feed", frame)
    cv2.waitKey(1)
