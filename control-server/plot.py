import matplotlib.pyplot as plt
import numpy as np


# Function to read CSV data
def read_csv_data(file_path):
    data = np.genfromtxt(file_path, delimiter=",", skip_header=0)
    return data[:, 0], data[:, 1]


# Main script
if __name__ == "__main__":
    # Read data from CSV
    x, y = read_csv_data("./fakedata.csv")
    for i in range(0, len(x)):
        x[i] /= 7
        x[i] += 21

    # Smooth the data

    # Plot original data
    plt.plot(x, y, "ro")
    plt.axhline(y=1, color="blue", linestyle="-", label="setpoint")

    # Plot smoothed data

    # Add labels and title
    plt.xlabel("time (s)")
    plt.ylabel("Depth (m)")

    # Add legend
    plt.legend()

    # Show the plot
    plt.show()
