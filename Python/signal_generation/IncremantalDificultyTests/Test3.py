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
"""

freq = [10.0, 18.0, 40.0]  # [kHz]
waveNum = 5  # Number of waves you want to generate with the SMALEST frequency
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------


# Set upp variables
samplingFreq = 500.0  # [kHz]
pulseLenght = (
    int(samplingFreq / min(freq)) * waveNum
)  # Multiply by 10 to get a really good resolution of the sample

# Make a list of multiple pinger objects of the same size
pingerObj = source.Pinger(
    frequency=freq[0],  # [kHz]
    sampling_frequency=samplingFreq,  # [kHz]
    pulse_length=pulseLenght,  # [ms]
    period=100000,  # [ms]
    amplitude=1.0,
    use_window=False,
    # position: Position = Position(),
)

# Generate a list of sampling signal with identical lenght
signalList = []
for i in range(len(freq)):
    pingerObj.frequency = freq[i]

    signalList += [
        pingerObj.generate_signal(
            offset=0.0,
            length=pulseLenght,
        )
    ]

# Combine all individual signals into one signal
signalCombo = [1] * len(signalList[0])
for i in range(len(signalList)):
    signalCombo = numpy.multiply(signalCombo, signalList[i])


# Convert signal into binary data and save it in a .txt file
with open(f"{SCRIPT_DIR}/Test3.txt", "w+") as file:
    signalComboDigital = conversion.convert_to_integer_type(
        resulting_type=numpy.int8,
        input_signal=signalCombo,
        pre_offset=0.0,
    )
    file.write(str(signalComboDigital))

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
