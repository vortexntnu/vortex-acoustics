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
This is iteration of the most basic test where only 1 single frequency is generated
Here we add complexity by adding noise into the mix
"""

freq = 18.0  # [kHz]
waveNum = 5  # Number of waves you want to generate in a signal
noiseVariance = 0.001  # Variaty in noise, if variance >0.1, set noiseCliping
noiseCliping = None  # If noiseVariance > 0.1 => Set cliping to equal noiseVariance or les, this is so that program doesent crash because of to much variance
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------


# Set upp variables
samplingFreq = 500.0  # [kHz]
pulseLenght = (
    int(samplingFreq / freq) * waveNum
)  # Multiply by 10 to get a really good resolution of the sample

# Make pinger object
pingerObj = source.Pinger(
    frequency=freq,  # [kHz]
    sampling_frequency=samplingFreq,  # [kHz]
    pulse_length=pulseLenght,  # [ms]
    period=100000,  # [ms]
    amplitude=1.0,
    use_window=False,
    # position: Position = Position(),
)

# Generate a sampling signal
signal = pingerObj.generate_signal(
    offset=0.0,
    length=pulseLenght,
)

# Generate noise
signalNoise = noise.generate_gaussian_noise(
    length=pulseLenght,
    variance=noiseVariance,
    maximum_allowed_value=noiseCliping,
)


# Combine signal with noise
signalCombo = numpy.multiply(signal, signalNoise)

# Write out data to a .txt file for later use
with open(f"{SCRIPT_DIR}/Test4.txt", "w+") as file:
    file.write(str(signalCombo))

# Plot signal to see
t = list(range(0, len(signalNoise)))
pyplot.plot(t, signalNoise)
t = list(range(0, len(signal)))
pyplot.plot(t, signal)
pyplot.title("Simulating Seperate pieces")
pyplot.show()

t = list(range(0, len(signalCombo)))
pyplot.plot(t, signalCombo)
pyplot.title("Simulations")
pyplot.show()
