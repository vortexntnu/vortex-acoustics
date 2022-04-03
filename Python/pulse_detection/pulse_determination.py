from signal import signal
import numpy as np
import scipy.signal

"""
TODO:
- Find a sampling frequency that works for 25, 30, 35 and 45 kHz pinger freq.
- Apply windowing before find the fft
- Repeate the prses for multiple segmetns of the pulse
    how to deal with this giving different results? 
    perhaps it accually merges to the same result -> google

"""

def compute_fft_size(
    sample_frequncy: float, # [kHz]
    signal_frequncy_increments: float, # [kHz]
    pulse_length: float # [ms]
)-> int:
    n = sample_frequncy // signal_frequncy_increments
    exponent = 3
    while n > 2**exponent:
        exponent += 1
    n = 2**exponent

    assert n < pulse_length*sample_frequncy
         
    return n

def adjust_signal_length(
    signal: np.array,
    fft_size: int,
)-> np.array:
    if signal.size == fft_size:
        return signal
    elif signal.size > fft_size:
        return signal[:fft_size]
    else:
        diff = fft_size -signal.size
        for i in range (diff):
            np.append(signal, 0.0)
        return signal


def determine_signal_frequency(
    signal: np.array, 
    sampling_frequency: float,
    fft_size: int,
): # -> float, np.array
    signal = adjust_signal_length(signal, fft_size)
    fourier = np.fft.rfft(signal)
    corresponding_freq = np.fft.rfftfreq(len(signal), 1 / sampling_frequency)
    freq_index = np.argmax(fourier)
    dominating_freq = corresponding_freq[freq_index]

    return dominating_freq , corresponding_freq
