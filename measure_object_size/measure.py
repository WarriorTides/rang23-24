import cv2
import math
import numpy as np

print("imported")
image = None
pointCoordinates1 = []
pointCoordinates2 = []
scale_length = None
scale_pixels = None


def click_points(event, x, y, flags, param):
    global pointCoordinates1, pointCoordinates2

    if event == cv2.EVENT_LBUTTONDOWN:
        coordinate = x, y
        if not scale_length:
            if len(pointCoordinates1) < 2:
                pointCoordinates1.append(coordinate)
            else:
                pointCoordinates1 = []
                pointCoordinates1.append(coordinate)
        else:
            if len(pointCoordinates2) < 2:
                pointCoordinates2.append(coordinate)
            else:
                pointCoordinates2 = []
                pointCoordinates2.append(coordinate)

        font = cv2.FONT_HERSHEY_SIMPLEX

        cv2.circle(image, (x, y), 1, (255, 0, 0), 1)

        # cv2.putText(image, str(x) + "," + str(y), (x, y), font, 1, (255, 0, 0), 2)
        cv2.imshow("Image", image)


def calculate_distance(x1, y1, x2, y2):
    return math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)


def set_scale_length():
    global scale_length, scale_pixels

    if len(pointCoordinates1) == 2:
        x1, y1 = pointCoordinates1[0]
        x2, y2 = pointCoordinates1[1]
        scale_pixels = calculate_distance(x1, y1, x2, y2)
        scale_length = float(
            input("Enter the actual length (in cm) of the selected scale: ")
        )


def calculate_length():
    global scale_length, scale_pixels

    if len(pointCoordinates2) == 2:
        x1, y1 = pointCoordinates2[0]
        x2, y2 = pointCoordinates2[1]
        pixels = calculate_distance(x1, y1, x2, y2)
        length = (pixels * scale_length) / scale_pixels
        print(f"Length: {length:.2f} cm")


path = "./image.png"
image = cv2.imread(path)
cv2.imshow("Image", image)

cv2.setMouseCallback("Image", click_points)

while True:
    key = cv2.waitKey(1) & 0xFF

    if key == ord("s"):
        set_scale_length()
    elif key == ord("m"):
        calculate_length()
    elif key == ord("q"):
        break

cv2.destroyAllWindows()
