# Seting upp libraries
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import source
import matplotlib.pyplot as pyplot


# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
"""
This is iteration of the most basic test where only 1 single frequency is generated
Here we add complexity by varying amplitude
"""

freq = 50.0  # [kHz]
amplitude = 2.5  # [dB]
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
    amplitude=amplitude,
    use_window=False,
    # position: Position = Position(),
)

# Generate a sampling signal
signal = pingerObj.generate_signal(
    offset=0.0,
    length=pulseLenght,
)


# Write out data to a .txt file for later use
with open(f"{SCRIPT_DIR}/Test2.txt", "w+") as file:
    file.write(str(signal))

# Plot signal to see
t = list(range(0, len(signal)))
pyplot.plot(t, signal)
pyplot.title("Simulations")
pyplot.show()
