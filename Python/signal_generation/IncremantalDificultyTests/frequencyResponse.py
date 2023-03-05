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
    181,
    404,
    0,
    511,
    181,
    478,
    511,
    313,
    313,
    362,
    404,
    313,
    0,
    404,
    181,
    0,
    652,
    652,
    3243,
    923,
    2907,
    652,
    886,
    0,
    0,
    443,
    181,
    0,
    443,
    0,
    181,
    313,
    362,
    313,
    0,
    600,
    511,
    181,
    627,
    1023,
    627,
    404,
    2804,
    1070,
    5893,
    3415,
    886,
    255,
    627,
    701,
    0,
    255,
    362,
    181,
    0,
    0,
    0,
    478,
    443,
    0,
    255,
    404,
    255,
    181,
    0,
    181,
    313,
    0,
    255,
    478,
    789,
    511,
    0,
    0,
    181,
    0,
    0,
    0,
    181,
    181,
    313,
    181,
    404,
    0,
    0,
    443,
    181,
    362,
    255,
    0,
    0,
    0,
    181,
    0,
    181,
    255,
    362,
    443,
    1609,
    957,
    767,
    0,
    181,
    255,
    181,
    0,
    0,
    0,
    313,
    181,
    652,
    600,
    404,
    543,
    0,
    181,
    0,
    0,
    0,
    0,
    181,
    0,
    0,
    255,
    404,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    313,
    478,
    767,
    868,
    627,
    0,
    600,
    478,
    443,
    255,
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
    181,
    0,
    0,
    0,
    0,
    0,
    313,
    255,
    181,
    181,
    313,
    313,
    0,
    181,
    0,
    0,
    181,
    0,
    0,
    0,
    181,
    181,
    0,
    362,
    0,
    313,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    313,
    181,
    443,
    0,
    313,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    0,
    181,
    0,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    0,
    181,
    181,
    255,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    255,
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
    255,
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
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    181,
    0,
    255,
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
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
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
    255,
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
    0,
    0,
    0,
    0,
    0,
    0,
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
    181,
    0,
    255,
    0,
    181,
    0,
    0,
    0,
    0,
    0,
    181,
    0,
    255,
    181,
    181,
    0,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    255,
    0,
    181,
    0,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    313,
    0,
    443,
    181,
    313,
    255,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    313,
    0,
    362,
    0,
    181,
    181,
    0,
    0,
    0,
    181,
    0,
    0,
    181,
    0,
    313,
    313,
    181,
    181,
    255,
    313,
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
    181,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    255,
    443,
    478,
    600,
    0,
    627,
    868,
    767,
    478,
    313,
    181,
    0,
    0,
    0,
    0,
    0,
    0,
    181,
    404,
    255,
    0,
    0,
    181,
    0,
    0,
    0,
    0,
    181,
    0,
    543,
    404,
    600,
    652,
    181,
    313,
    0,
    0,
    0,
    181,
    255,
    181,
    0,
    767,
    957,
    1609,
    443,
    362,
    255,
    181,
    0,
    181,
    0,
    0,
    0,
    255,
    362,
    181,
    443,
    0,
    0,
    404,
    181,
    313,
    181,
    181,
    0,
    0,
    0,
    181,
    0,
    0,
    511,
    789,
    478,
    255,
    0,
    313,
    181,
    0,
    181,
    255,
    404,
    255,
    0,
    443,
    478,
    0,
    0,
    0,
    181,
    362,
    255,
    0,
    701,
    627,
    255,
    886,
    3415,
    5893,
    1070,
    2804,
    404,
    627,
    1023,
    627,
    181,
    511,
    600,
    0,
    313,
    362,
    313,
    181,
    0,
    443,
    0,
    181,
    443,
    0,
    0,
    886,
    652,
    2907,
    923,
    3243,
    652,
    652,
    0,
    181,
    404,
    0,
    313,
    404,
    362,
    313,
    313,
    511,
    478,
    181,
    511,
    0,
    404,
    181,
    181,
]

fftPeaks = np.array(
    [
        [8964, 652],
        [9960, 3243],
        [10957, 2907],
        [11953, 886],
        [18427, 600],
        [20419, 1023],
        [21914, 2804],
        [22910, 5893],
        [25400, 701],
        [35859, 789],
        [49804, 1609],
        [55781, 652],
        [57275, 543],
        [68730, 868],
        [70224, 600],
        [439775, 600],
        [441269, 868],
        [452724, 543],
        [454218, 652],
        [460195, 1609],
        [474140, 789],
        [484599, 701],
        [487089, 5893],
        [488085, 2804],
        [489580, 1023],
        [491572, 600],
        [498046, 886],
        [499042, 2907],
        [500039, 3243],
        [501035, 652],
    ]
)


# Calculate frequencies of specific amplitudes
sampleLenght = len(fftSampleAmplitude)
frequency = []

for i in range(sampleLenght):
    frequency += [i * (samplingFreq / sampleLenght)]

# Take the peaks from the 2D array
xPeaks = fftPeaks[:, 0] / 1000
yPeaks = fftPeaks[:, 1]

# Plot the frequency response of fft in frequency domain
fig, ax = pyplot.subplots()
ax.bar(frequency, fftSampleAmplitude, color="blue", width=2)
ax.scatter(xPeaks, yPeaks, color="red", s=10, linewidths=0.5, marker="x")

ax.set_title("FFT response in Frequency domain")
ax.set_xlabel("Frequency [kHz]")
ax.set_ylabel("Amplitude")
pyplot.show()
