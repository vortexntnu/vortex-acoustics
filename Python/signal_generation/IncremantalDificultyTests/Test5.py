# Seting upp libraries
import os
import sys

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import conversion
import matplotlib.pyplot as pyplot
import noise
import numpy
import source

# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
"""
This is combination of Test 1, 2, 3 and 4
We add multiple frequencies with their own seperate amplitudes and phase shifts
In adition we can turn on and off noise
"""

# [frequency, amplitude, phase shift]/[kHz, dB, ms]
freqAmpShift = numpy.array(
    [
        [1.0, 0.20, 0.0],
        [5.0, 0.20, 0.0],
        [10.0, 0.20, 0.1],
        [22.0, 0.30, 0.4],
        [50.0, 0.30, 0.1],
    ]
)
waveNum = 8  # Number of waves you want to generate with the SMALEST frequency
noisActive = True  # Noise has effect when True, No noise when False
noiseVariance = 0.1  # Variaty in noise
noiseCliping = 0.2  # Set a float value you want noise amplitude to be cliped. Set variable to "None" for no cliping of noise
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------


# Set upp variables
samplingFreq = 510.0  # [kHz]
minFreqIndex = numpy.argmin(freqAmpShift[::, 0])
pulseLenght = int(samplingFreq / freqAmpShift[minFreqIndex, 0]) * waveNum + int(
    samplingFreq * freqAmpShift[minFreqIndex, 1]
)  # Multiply by 10 to get a really good resolution of the sample, compensate for phase shift by ading it in

# Make pinger object
pingerObj = source.Pinger(
    frequency=freqAmpShift[0, 0],  # [kHz]
    sampling_frequency=samplingFreq,  # [kHz]
    pulse_length=pulseLenght,  # [ms]
    period=100000,  # [ms]
    amplitude=freqAmpShift[0, 1],
    use_window=False,
    # position: Position = Position(),
)

# Generate a list of sampling signal with identical lenght
signalList = []
for i in range(len(freqAmpShift)):
    pingerObj.frequency = freqAmpShift[i, 0]
    pingerObj.amplitude = freqAmpShift[i, 1]

    signalList += [
        pingerObj.generate_signal(
            offset=freqAmpShift[i, 2],
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
signalCombo = [0] * len(signalList[0])
for i in range(len(signalList)):
    signalCombo = numpy.add(signalCombo, signalList[i])

# Combine signals with noise if allowed
if noisActive:
    signalComboNoNoise = signalCombo
    signalCombo = numpy.add(signalCombo, signalNoise)


# Convert signal into binary 12-bit data and save it in a .txt file
with open(f"{SCRIPT_DIR}/Test5.txt", "w+") as file:
    """
    We sample data in real life with 12-bit acuracy
    Python only has 8 or 16 bit acuracy
    We save value to 16 bits and shift by 4
    We lose 4 least significant bits and 0.025% acuracy, but it is an anolog signal so we just live with it
    """
    signalComboDigital12bit = []
    signalComboDigital16bit = conversion.convert_to_integer_type(
        resulting_type=numpy.int16,
        input_signal=signalCombo,
        pre_offset=0.0,
    )
    for sd16bit in signalComboDigital16bit:
        signalComboDigital12bit += [sd16bit >> 4]

    """
    NOTE IMPORTANT!!!
    We take the 16-bit signal and save it in text file
    This is because we temporarily will be using in real life a ADC that is 16-bit resolution
    This ADC is just temporary, and the real 12-bit ADC will be coming later
    That is why we first test with the 16-bit ADC instead :) 
    """
    file.write(str(signalComboDigital16bit.tolist())[1:-1])

# Plot signal to see
fig, axs = pyplot.subplots(len(signalList))
for i in range(len(signalList)):
    t = list(range(0, len(signalList[i])))
    axs[i].plot(t, signalList[i])
fig.suptitle("Simulating Seperate pieces")
pyplot.show()

if noisActive:
    t = list(range(0, len(signalComboNoNoise)))
    pyplot.plot(t, signalComboNoNoise, "b", label="Signal")
    t = list(range(0, len(signalNoise)))
    pyplot.plot(t, signalNoise, "c", label="Noise")
    pyplot.title("Simulating combined signals with noise added")
    pyplot.legend()
    pyplot.show()

fig, axs = pyplot.subplots(2)
fig.suptitle(
    "Simulation - Signal with multiple frequencies and amplitudes, combined with noise"
)
t = list(range(0, len(signalCombo)))
axs[0].plot(t, signalCombo, "tab:purple")
axs[0].set_title("Analog")
t = list(range(0, len(signalComboDigital12bit)))
axs[1].plot(t, signalComboDigital12bit, "tab:red")
axs[1].set_title("Digital")
pyplot.show()
