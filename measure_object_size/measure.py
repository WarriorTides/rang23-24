# Calculator for length of Parts
import cv2
import math

pointCoordinates1 = []
pointCoordinates2 = []


# function to get coordinates of points clicked on image
def click_points(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        coordinateValue = x, y
        if len(pointCoordinates1) < 2:
            pointCoordinates1.append(coordinateValue)
        elif len(pointCoordinates1) >= 2 and len(pointCoordinates2) < 2:
            pointCoordinates2.append(coordinateValue)

        # display coordinates on image window
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(image, str(x) + "," + str(y), (x, y), font, 1, (255, 0, 0), 2)
        cv2.imshow("image", image)


def calculateDistance(x1, y1, x2, y2):
    dist = math.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2)
    return dist


# import and view image using cv2
path = "./model.jpeg"
image = cv2.imread(path)
cv2.imshow("image", image)

# get length of items in picture
cv2.setMouseCallback("image", click_points)

cv2.waitKey(0)

x1A, y1A = pointCoordinates1[0]
x2A, y2A = pointCoordinates1[1]
x1B, y1B = pointCoordinates2[0]
x2B, y2B = pointCoordinates2[1]
bar1_length = calculateDistance(x1A, y1A, x2A, y2A)
bar2_length = calculateDistance(x1B, y1B, x2B, y2B)

rate = bar2_length / bar1_length

bar1_actual = 8.17  # actual length of known bar in centimeters

bar2_actual = round((rate * bar1_actual), 2)

print("Length of unknown part: " + str(bar2_actual) + " cm")
