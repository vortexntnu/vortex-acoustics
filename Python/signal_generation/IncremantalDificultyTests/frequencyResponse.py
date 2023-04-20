# Seting upp libraries
import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import matplotlib.pyplot as pyplot
import numpy as np

# Set upp variables
samplingFreq = 510.0  # [kHz]

fftSampleAmplitude = [
    313,
    181,
    362,
    181,
    543,
    362,
    0,
    255,
    362,
    3665,
    2862,
    1330,
    0,
    0,
    627,
    677,
    313,
    0,
    511,
    789,
    181,
    443,
    362,
    0,
    313,
    255,
    724,
    0,
    829,
    181,
    600,
    0,
    404,
    543,
    255,
    255,
    181,
    1470,
    600,
    0,
    0,
    0,
    478,
    511,
    0,
    701,
    3007,
    2968,
    1007,
    404,
    572,
    0,
    0,
    313,
    767,
    627,
    1130,
    511,
    181,
    478,
    0,
    313,
    0,
    255,
    0,
    600,
    181,
    511,
    0,
    0,
    0,
    0,
    0,
    362,
    724,
    0,
    0,
    0,
    362,
    181,
    255,
    181,
    404,
    181,
    0,
    181,
    313,
    181,
    0,
    181,
    572,
    677,
    809,
    4079,
    1292,
    724,
    627,
    0,
    404,
    0,
    0,
    313,
    362,
    181,
    0,
    255,
    0,
    0,
    181,
    0,
    443,
    0,
    0,
    313,
    181,
    0,
    181,
    181,
    0,
    181,
    181,
    255,
    181,
    181,
    0,
    0,
    313,
    181,
    255,
    0,
    1070,
    701,
    511,
    181,
    313,
    0,
    362,
    255,
    313,
    443,
    255,
    0,
    181,
    443,
    0,
    0,
    313,
    181,
    478,
    1115,
    0,
    181,
    181,
    0,
    0,
    0,
    0,
    255,
    404,
    255,
    313,
    0,
    0,
    0,
    181,
    181,
    362,
    443,
    404,
    255,
    181,
    181,
    0,
    0,
    181,
    313,
    313,
    313,
    0,
    181,
    255,
    0,
    0,
    0,
    0,
    0,
    478,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    181,
    572,
    255,
    0,
    0,
    0,
    0,
    0,
    255,
    0,
    181,
    255,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    313,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    362,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    181,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    362,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    181,
    0,
    181,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    181,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    181,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    181,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    181,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    181,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    181,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    181,
    0,
    181,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    362,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    181,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    362,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    313,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    0,
    255,
    181,
    0,
    255,
    0,
    0,
    0,
    0,
    0,
    255,
    572,
    181,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    478,
    0,
    0,
    0,
    0,
    0,
    255,
    181,
    0,
    313,
    313,
    313,
    181,
    0,
    0,
    181,
    181,
    255,
    404,
    443,
    362,
    181,
    181,
    0,
    0,
    0,
    313,
    255,
    404,
    255,
    0,
    0,
    0,
    0,
    181,
    181,
    0,
    1115,
    478,
    181,
    313,
    0,
    0,
    443,
    181,
    0,
    255,
    443,
    313,
    255,
    362,
    0,
    313,
    181,
    511,
    701,
    1070,
    0,
    255,
    181,
    313,
    0,
    0,
    181,
    181,
    255,
    181,
    181,
    0,
    181,
    181,
    0,
    181,
    313,
    0,
    0,
    443,
    0,
    181,
    0,
    0,
    255,
    0,
    181,
    362,
    313,
    0,
    0,
    404,
    0,
    627,
    724,
    1292,
    4079,
    809,
    677,
    572,
    181,
    0,
    181,
    313,
    181,
    0,
    181,
    404,
    181,
    255,
    181,
    362,
    0,
    0,
    0,
    724,
    362,
    0,
    0,
    0,
    0,
    0,
    511,
    181,
    600,
    0,
    255,
    0,
    313,
    0,
    478,
    181,
    511,
    1130,
    627,
    767,
    313,
    0,
    0,
    572,
    404,
    1007,
    2968,
    3007,
    701,
    0,
    511,
    478,
    0,
    0,
    0,
    600,
    1470,
    181,
    255,
    255,
    543,
    404,
    0,
    600,
    181,
    829,
    0,
    724,
    255,
    313,
    0,
    362,
    443,
    181,
    789,
    511,
    0,
    313,
    677,
    627,
    0,
    0,
    1330,
    2862,
    3665,
    362,
    255,
    0,
    362,
    543,
    181,
    362,
    181,
]

fftPeaks = np.array(
    [
        [3665, 966, 2.21],
        [677, 1611, 2.00],
        [789, 2041, -0.21],
        [724, 2792, -0.27],
        [829, 3007, -2.63],
        [1470, 3974, 1.87],
        [3007, 4941, 1.07],
        [767, 5800, -2.43],
        [1130, 6015, 2.85],
        [724, 7949, 2.51],
        [4079, 9990, 2.93],
        [1070, 13964, 0.00],
        [1115, 16005, 0.80],
        [1115, 93994, -0.80],
        [1070, 96035, 0.00],
        [4079, 100009, -2.93],
        [724, 102050, -2.51],
        [1130, 103984, -2.85],
        [767, 104199, 2.43],
        [3007, 105058, -1.07],
        [1470, 106025, -1.87],
        [829, 106992, 2.63],
        [724, 107207, 0.27],
        [789, 107958, 0.21],
        [677, 108388, -2.00],
        [3665, 109033, -2.21],
    ]
)


# Calculate frequencies of specific amplitudes
sampleLenght = len(fftSampleAmplitude)
frequency = []

for i in range(sampleLenght):
    frequency += [i * (samplingFreq / sampleLenght)]

# Take the peaks from the 2D array
xPeaks = fftPeaks[:, 1] / 1000
yPeaks = fftPeaks[:, 0]

# Plot the frequency response of fft in frequency domain
fig, ax = pyplot.subplots()
ax.bar(frequency, fftSampleAmplitude, color="blue", width=0.8)
ax.scatter(xPeaks, yPeaks, color="red", s=10, linewidths=0.8, marker="x")

ax.set_title("FFT response in Frequency domain")
ax.set_xlabel("Frequency [kHz]")
ax.set_ylabel("Amplitude")
pyplot.show()
