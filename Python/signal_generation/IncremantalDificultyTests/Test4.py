# Seting upp libraries
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
sys.path.append(os.path.dirname(SCRIPT_DIR))

import source
import noise
import conversion
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
noiseVariance = 0.1  # Variaty in noise
noiseCliping = None  # Set a float value you want noise amplitude to be cliped. Set variable to "None" for no cliping of noise
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
signalCombo = numpy.add(signal, signalNoise)


# Convert signal into binary data and save it in a .txt file
with open(f"{SCRIPT_DIR}/Test4.txt", "w+") as file:
    signalComboDigital = conversion.convert_to_integer_type(
        resulting_type=numpy.int8,
        input_signal=signalCombo,
        pre_offset=0.0,
    )
    file.write(str(signalComboDigital.tolist()))

# Plot signal to see
t = list(range(0, len(signal)))
pyplot.plot(t, signal, "b", label="Signal")
t = list(range(0, len(signalNoise)))
pyplot.plot(t, signalNoise, "c", label="Noise")
pyplot.title("Simulating combined signals with noise added")
pyplot.legend()
pyplot.show()


fig, axs = pyplot.subplots(2)
fig.suptitle("Simulation - Signal with noise")
t = list(range(0, len(signalCombo)))
axs[0].plot(t, signalCombo, "tab:purple")
axs[0].set_title("Analog")
t = list(range(0, len(signalComboDigital)))
axs[1].plot(t, signalComboDigital, "tab:red")
axs[1].set_title("Digital")
pyplot.show()
