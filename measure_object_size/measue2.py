import cv2
import numpy as np

print("imported cv2")
# Initialize global variables
points = []


def click_event(event, x, y, flags, param):
    global points
    if event == cv2.EVENT_LBUTTONDOWN:
        points.append((x, y))
        cv2.circle(img, (x, y), 5, (0, 0, 255), -1)
        cv2.imshow("image", img)

        if len(points) == 4:
            # When 4 points are selected, apply perspective transform
            pts1 = np.float32(points)
            dx = points[0][0]
            dy = points[0][1]

            # print

            width = max(
                np.linalg.norm(pts1[0] - pts1[1]), np.linalg.norm(pts1[2] - pts1[3])
            )
            height = max(
                np.linalg.norm(pts1[0] - pts1[3]), np.linalg.norm(pts1[1] - pts1[2])
            )
            pts2 = np.float32(
                [
                    [0 + dx, 0 + dy],
                    [width + dx, 0 + dy],
                    [width + dx, height + dy],
                    [0 + dx, height + dy],
                ]
            )

            # Compute the perspective transform matrix
            matrix = cv2.getPerspectiveTransform(pts1, pts2)

            # Get dimensions of the original image
            h, w = img.shape[:2]

            # Warp the entire image
            result = cv2.warpPerspective(img, matrix, (w, h))

            # Create a black canvas larger than the original image
            canvas_height = int(max(h, height) * 1.5)
            canvas_width = int(max(w, width) * 1.5)
            canvas = np.zeros((canvas_height, canvas_width, 3), dtype=np.uint8)

            # Calculate the offset to center the warped image on the canvas
            y_offset = (canvas_height - h) // 2
            x_offset = (canvas_width - w) // 2

            # Place the warped image on the canvas
            canvas[y_offset : y_offset + h, x_offset : x_offset + w] = result

            # Display the corrected image
            cv2.imshow("corrected image", result)


# Load the image
img = cv2.imread("/Users/aayanmaheshwari/Downloads/silly.jpg")

# Display the image and set the mouse callback function
cv2.imshow("image", img)
cv2.setMouseCallback("image", click_event)

cv2.waitKey(0)
cv2.destroyAllWindows()
