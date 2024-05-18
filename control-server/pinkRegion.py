
import cv2
import numpy as np

image = cv2.imread("/Users/kashishkapoor/Downloads/rang23-24/control-server/picture.png") 

def find_pink_region(image):
    hsv_image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    lower_pink = np.array([100, 100, 150])
    upper_pink = np.array([170, 170, 200])

    mask = cv2.inRange(hsv_image, lower_pink, upper_pink)

    pink_pixels = cv2.findNonZero(mask)

    if pink_pixels is not None:
        pink_pixels_x = pink_pixels[:, 0, 0]
        pink_pixels_y = pink_pixels[:, 0, 1]
        avg_x = int(np.mean(pink_pixels_x))
        avg_y = int(np.mean(pink_pixels_y))
        pink_region_center = (avg_x, avg_y)
        pink_region_angle = calculate_angle(pink_region_center, image.shape)
    else:
        pink_region_center = None
        pink_region_angle = None

    return pink_region_center, pink_region_angle

def calculate_angle(center, image_shape):
    height, width = image_shape[:2]
    center_x, center_y = center

    delta_x = center_x - width // 2
    delta_y = center_y - height // 2
    angle = np.arctan2(delta_y, delta_x) * 180 / np.pi

    return angle

pink_region_center, angle_to_turn = find_pink_region(image)

if pink_region_center is not None:
    print(f"The average location of the pink squre is {pink_region_center}")
    print(f"The ROV needs to turn {angle_to_turn:.2f} degrees to face the pink square.")
else:
    print("No pink square found in the image.")