# Seting upp libraries
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import source
import noise
import numpy
import matplotlib.pyplot as pyplot


# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
"""
This is combination of Test 1, 2, 3 and 4
We add multiple frequencies with their own seperate amplitudes
In adition we can turn on and off noise
"""

# [frequency, amplitude]/[kHz, dB]
freqAmp = numpy.array(
    [
        [10.0, 1.0],
        [15.0, 0.8],
        [20.0, 0.6],
        [25.0, 0.4],
        [30.0, 100.0],
    ]
)
waveNum = 5  # Number of waves you want to generate with the SMALEST frequency
noisActive = True  # Noise has effect when True
noiseVariance = 1.5  # Variaty in noise
noiseCliping = None  # Set a float value you want noise amplitude to be cliped. Set variable to "None" for no cliping of noise
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------


# Set upp variables
samplingFreq = 500.0  # [kHz]
minFreqIndex = numpy.argmin(freqAmp[::, 0])
pulseLenght = (
    int(samplingFreq / freqAmp[minFreqIndex, 0]) * waveNum
)  # Multiply by 10 to get a really good resolution of the sample

# Make a list of multiple pinger objects of the same size
pingerObj = source.Pinger(
    frequency=freqAmp[0, 0],  # [kHz]
    sampling_frequency=samplingFreq,  # [kHz]
    pulse_length=pulseLenght,  # [ms]
    period=100000,  # [ms]
    amplitude=freqAmp[0, 1],
    use_window=False,
    # position: Position = Position(),
)

# Generate a list of sampling signal with identical lenght
signalList = []
for i in range(len(freqAmp)):
    pingerObj.frequency = freqAmp[i, 0]
    pingerObj.amplitude = freqAmp[i, 1]

    signalList += [
        pingerObj.generate_signal(
            offset=0.0,
            length=pulseLenght,
        )
    ]

# Generate noise if allowed
if noisActive:
    signalNoise = noise.generate_gaussian_noise(
        length=pulseLenght,
        variance=noiseVariance,
        maximum_allowed_value=noiseCliping,
    )
else:
    signalNoise = None

# Combine all individual signals into one signal
signalCombo = [1] * len(signalList[0])
for i in range(len(signalList)):
    signalCombo = numpy.multiply(signalCombo, signalList[i])

# Combine signals with noise if allowed
if noisActive:
    signalComboNoNoise = signalCombo
    signalCombo = numpy.add(signalCombo, signalNoise)


# Write out data to a .txt file for later use
with open(f"{SCRIPT_DIR}/Test5.txt", "w+") as file:
    file.write(str(signalCombo))

# Plot signal to see
fig, axs = pyplot.subplots(len(signalList))
for i in range(len(signalList)):
    t = list(range(0, len(signalList[i])))
    axs[i].plot(t, signalList[i])
fig.suptitle("Simulating Seperate pieces")
pyplot.show()

if noisActive:
    t = list(range(0, len(signalComboNoNoise)))
    pyplot.plot(t, signalComboNoNoise)
    t = list(range(0, len(signalNoise)))
    pyplot.plot(t, signalNoise)
    pyplot.title("Simulating combined signals with noise added")
    pyplot.show()

t = list(range(0, len(signalCombo)))
pyplot.plot(t, signalCombo)
pyplot.title("Simulation of everything combined")
pyplot.show()
