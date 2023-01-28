# Seting upp libraries
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import source
import conversion
import numpy
import matplotlib.pyplot as pyplot


# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
"""
This is iteration of the most basic test where only 1 single frequency is generated
Here we add complexity by combining multiple frequencies
Aswell as phase shift
"""

# [frequency, phase shift]/[kHz, ms]
freqShift = numpy.array(
    [
        [10.0, 0.2],
        [18.0, 0.4],
        [40.0, 0.6],
    ]
)
waveNum = 7  # Number of waves you want to generate with the SMALEST frequency
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------


# Set upp variables
samplingFreq = 500.0  # [kHz]
minFreqIndex = numpy.argmin(freqShift[::, 0])
pulseLenght = int(samplingFreq / freqShift[minFreqIndex, 0]) * waveNum + int(
    samplingFreq * freqShift[minFreqIndex, 1]
)  # Multiply by 10 to get a really good resolution of the sample, compensate for phase shift by ading it in

# Make pinger object
pingerObj = source.Pinger(
    frequency=freqShift[0, 0],  # [kHz]
    sampling_frequency=samplingFreq,  # [kHz]
    pulse_length=pulseLenght,  # [ms]
    period=100000,  # [ms]
    amplitude=0.1,
    use_window=False,
    # position: Position = Position(),
)

# Generate a list of sampling signal with identical lenght
signalList = []
for i in range(len(freqShift)):
    pingerObj.frequency = freqShift[i, 0]

    signalList += [
        pingerObj.generate_signal(
            offset=freqShift[i, 1],
            length=pulseLenght,
        )
    ]

# Combine all individual signals into one signal
signalCombo = [0] * len(signalList[0])
for i in range(len(signalList)):
    signalCombo = numpy.add(signalCombo, signalList[i])


# Convert signal into binary data and save it in a .txt file
with open(f"{SCRIPT_DIR}/Test3.txt", "w+") as file:
    signalComboDigital = conversion.convert_to_integer_type(
        resulting_type=numpy.int8,
        input_signal=signalCombo,
        pre_offset=0.0,
    )
    file.write(str(signalComboDigital.tolist()))

# Plot signal to see
fig, axs = pyplot.subplots(len(signalList))
for i in range(len(signalList)):
    t = list(range(0, len(signalList[i])))
    axs[i].plot(t, signalList[i])
fig.suptitle("Simulating Seperate pieces")
pyplot.show()

fig, axs = pyplot.subplots(2)
fig.suptitle("Simulation - Signal with multiple frequencies")
t = list(range(0, len(signalCombo)))
axs[0].plot(t, signalCombo, "tab:purple")
axs[0].set_title("Analog")
t = list(range(0, len(signalComboDigital)))
axs[1].plot(t, signalComboDigital, "tab:red")
axs[1].set_title("Digital")
pyplot.show()
