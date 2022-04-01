from signal import signal
import numpy as np
import scipy.signal

"""
Find required frequency bins. -> find order of fft. 
Decide what frquency. 

Same problem as with pdoa?!
Fft order 


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
)-> np.np.array:
    if len(signal) == fft_size:
        return signal
    elif len(signal) > fft_size:
        return signal[:fft_size]
    else:
        diff = fft_size -len(signal)
        zero_padding = np.empty(diff)
        signal = np.concatenate(signal, zero_padding)
        return signal


def determine_signal_frequency(
    signal: np.array, 
    sampling_frequency: float,
    fft_size: int,
) -> float:
    signal = adjust_signal_length(signal)
    fourier = np.fft.rfft(signal)
    corresponding_freq = np.fft.rfftfreq(len(signal), 1 / sampling_frequency)
    freq_index = np.argmax(fourier)
    dominating_freq = corresponding_freq[freq_index]

    return dominating_freq
