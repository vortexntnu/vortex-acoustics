# Seting upp libraries
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import matplotlib.pyplot as pyplot
import numpy as np


# Set upp variables
samplingFreq = 510.0  # [kHz]

fftSampleAmplitude = [
    0, 0, 1, 5, 1, 1
]

fftPeaks = np.array([
    [1, 1],
    [20, 3],
    [3, 5]
])


# Calculate frequencies of specific amplitudes
sampleLenght = len(fftSampleAmplitude)
frequency = []

for i in range(sampleLenght):
    frequency += [i * (samplingFreq / sampleLenght)]

# Take the peaks from the 2D array
xPeaks = fftPeaks[:, 0]
yPeaks = fftPeaks[:, 1]

# Plot the frequency response of fft in frequency domain
fig, ax = pyplot.subplots()
ax.bar(frequency, fftSampleAmplitude, color="blue", width=2)
ax.scatter(xPeaks, yPeaks, color="red", s=10, linewidths=0.5, marker="x")

ax.set_title("FFT response in Frequency domain")
ax.set_xlabel("Frequency [kHz]")
ax.set_ylabel("Amplitude")
pyplot.show()
