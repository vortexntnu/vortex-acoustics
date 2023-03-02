# Seting upp libraries
import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import matplotlib.pyplot as pyplot

# Filtered signal
signal = [0, 0, 0]


# Get unfiltered signal
signalNoise = []
try:
    with open(f"{SCRIPT_DIR}/Test4.txt", "r+") as file:
        temp = file.readline()[1:-1]
        tempProcessed = temp.split(",")

        for value in tempProcessed:
            signalNoise += [int(value)]
except:
    print("Run Test4 first")


# Plot signal to see
t = list(range(0, len(signal)))
pyplot.plot(t, signal, "b", label="Filtered Signal")
t = list(range(0, len(signalNoise)))
pyplot.plot(t, signalNoise, "c", label="Noisy Signal")
pyplot.title("Before and after filtering")
pyplot.legend()
pyplot.show()
