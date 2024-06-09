import msvcrt
import serial
import time
import matplotlib.pyplot as plt


def splitString(string, pref, end):
    return string.split(pref)[1].split(end)[0]


preassureData = []
timeData = []
if __name__ == "__main__":
    plt.ion()  # Turn on interactive mode
    fig, ax = plt.subplots()
    ser = serial.Serial("COM24", 115200, timeout=1)
    ser.reset_input_buffer()
    ser.write(b"s\n")

    dataString = ""

    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode("utf-8").rstrip()
            print(line)
            if line.startswith("DATA"):
                depth = float(splitString(line, "Depth:", "m"))
                time_ = float(splitString(line, "me:", "s"))
                ax.plot(time_, depth, "ro")  # Plot a red dot for each depth value
                # ax.set_xlim(
                #     time.time() - 10, time.time()
                # )  # Set the x-axis to the last 10 seconds
                ax.set_ylim(-1, 5)  # Adjust the y-axis limits as needed
                plt.draw()  # Redraw the plot
                plt.pause(0.01)  # Pause for a short period to allow the plot to update

        if msvcrt.kbhit():
            # print ("you pressed",msvcrt.getch(),"so now i will quit")
            reciv = msvcrt.getch()
            if (reciv) == b"\r":
                ser.write(((dataString) + "\n").encode("utf-8"))
                dataString = ""
                # ser.write(((dataString)+"\n"))
            dataString += (reciv).decode("utf-8").rstrip()
            print(dataString)
