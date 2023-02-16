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
Here we add complexity by varying amplitude
"""

freq = 50.0  # [kHz]
amplitude = 0.9  # [dB]
waveNum = 180  # Number of waves you want to generate in a signal
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------
# ----------------------------------------------------------------------------------------------------


# Set upp variables
samplingFreq = 510.0  # [kHz]
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


# Convert signal into binary 12-bit data and save it in a .txt file
with open(f"{SCRIPT_DIR}/Test2.txt", "w+") as file:
    """
    We sample data in real life with 12-bit acuracy
    Python only has 8 or 16 bit acuracy
    We save value to 16 bits and shift by 4
    We lose 4 least significant bits and 0.025% acuracy, but it is an anolog signal so we just live with it
    """
    signalDigital12bit = []
    signalDigital16bit = conversion.convert_to_integer_type(
        resulting_type=numpy.int16,
        input_signal=signal,
        pre_offset=0.0,
    )
    for sd16bit in signalDigital16bit:
        signalDigital12bit += [sd16bit >> 4]

    """
    NOTE IMPORTANT!!!
    We take the 16-bit signal and save it in text file
    This is because we temporarily will be using in real life a ADC that is 16-bit resolution
    This ADC is just temporary, and the real 12-bit ADC will be coming later
    That is why we first test with the 16-bit ADC instead :) 
    """
    file.write(str(signalDigital16bit.tolist())[1:-1])

# Plot signal to see
fig, axs = pyplot.subplots(2)
fig.suptitle("Simulation - Signal with varying amplitude")
t = list(range(0, len(signal)))
axs[0].plot(t, signal, "tab:purple")
axs[0].set_title("Analog")
t = list(range(0, len(signalDigital12bit)))
axs[1].plot(t, signalDigital12bit, "tab:red")
axs[1].set_title("Digital")
pyplot.show()

print(len(signalDigital12bit))
