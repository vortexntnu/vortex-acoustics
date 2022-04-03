from signal import signal
import numpy as np
import scipy.signal

"""
TODO:
- Apply windowing before finding the fft
    - Test with signal generation 
- Repeate the proses for multiple segmetns of the pulse
    how to deal with this giving different results? 
    perhaps it accually merges to the same result 
- Find a sampling frequency that works for 25, 30, 35 and 45 kHz pinger freq.
    make a cost function
        mabye use weighted costs

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

def find_optimal_sampling_frequency( fft_size: int):
    pinger_frequencies = {25.0, 30.0, 35.0, 40.0}
    min_cost = 5000000
    optimal_sampling_freq = -1
    for sampling_freq in range (300, 500):
        cost = 0
        frequency_bins = np.fft.rfftfreq(fft_size, 1/sampling_freq)
        for pinger_freq in pinger_frequencies:
            diff = 100
            closest_center_freq = 0
            for center_freq in frequency_bins:
                if abs(pinger_freq - center_freq) < diff:
                    closest_center_freq = center_freq
            cost += abs(closest_center_freq - pinger_freq)**3 #gives same reuslt for **1, **2, **3
        if cost < min_cost:
            min_cost = cost
            optimal_sampling_freq = sampling_freq

    return optimal_sampling_freq, np.fft.rfftfreq(fft_size, 1/optimal_sampling_freq)


def apply_hanning_window(
    signal: np.array
):
    window = np.hanning(np.size(signal)) 
    windowed_signal = np.convolve(signal, window, 'same')

    assert np.size(windowed_signal) == np.size(signal)

    return windowed_signal

    

