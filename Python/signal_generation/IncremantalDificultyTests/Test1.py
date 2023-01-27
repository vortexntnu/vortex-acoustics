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
This is the most basic test where only 1 single frequency is generated
"""

freq = 18.0  # [kHz]
waveNum = 5  # Number of waves you want to generate in a signal
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


# Convert signal into binary data and save it in a .txt file
with open(f"{SCRIPT_DIR}/Test1.txt", "w+") as file:
    signalDigital = conversion.convert_to_integer_type(
        resulting_type=numpy.int8,
        input_signal=signal,
        pre_offset=0.0,
    )
    file.write(str(signalDigital.tolist()))

# Plot signal to see
fig, axs = pyplot.subplots(2)
fig.suptitle("Simulation - Simple signal")
t = list(range(0, len(signal)))
axs[0].plot(t, signal, "tab:purple")
axs[0].set_title("Analog")
t = list(range(0, len(signalDigital)))
axs[1].plot(t, signalDigital, "tab:red")
axs[1].set_title("Digital")
pyplot.show()
