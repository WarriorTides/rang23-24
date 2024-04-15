import cv2

# Define the codec and create a VideoWriter object
fourcc = cv2.VideoWriter_fourcc(*"mp4v")  # or use 'XVID'
out = cv2.VideoWriter("output.mp4", fourcc, 20.0, (1280, 720))

# Open the webcam
cap = cv2.VideoCapture("http://robert.local:5000/")


while True:
    ret, frame = cap.read()
    if not ret:
        break

    # Resize the frame to 720p
    frame = cv2.resize(frame, (1280, 720))

    # Write the frame to the output file
    out.write(frame)

    # Display the resulting frame
    cv2.imshow("Recording", frame)

    # Break the loop on 'q' key press
    if cv2.waitKey(1) & 0xFF == ord("q"):
        break

# Release the VideoWriter and VideoCapture objects
out.release()
cap.release()

# Close all OpenCV windows
cv2.destroyAllWindows()
