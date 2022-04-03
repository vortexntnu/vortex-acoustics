from signal import signal
from socket import SOCK_DGRAM
import numpy as np
from scipy.fftpack import fft2
import scipy.signal

"""
TODO:
- Test
    - windowing
    - short time fourier transform
    - with signal generation 

"""

def short_time_fourier_transform(
    signal: np.array, 
    fft_size: int, 
    sampling_frequency: float, # [kHz]
    pulse_length: int, # [ms]
):
    """
    segment the signal into M N-point segments
    do windowing and ftt on each segment
    sum up al the compoents
    """
    MN_point_fft = np.empty(fft_size)
    n_segments = (pulse_length * sampling_frequency) // fft_size
    for i in range(n_segments):
        windowed_signal_segment = \
            apply_hanning_window(signal[i*fft_size:(i+1)*fft_size-1])
        fft = np.fft.rfft(windowed_signal_segment)
        for fft_component in fft:
            MN_point_fft[i] += fft_component


def find_tone( 
    fft: np.array, 
    sampling_frequency: float,
    fft_size: int,
)-> float:
    frequency_bins = np.fft.rfftfreq(fft_size, 1 / sampling_frequency)
    tone = frequency_bins[np.argmax(fft)]

    return tone


def apply_hanning_window( 
    signal: np.array
):
    signal_length = np.size(signal)
    n = np.linspace(0,signal_length-1, signal_length) 

    hanning_window =  0.5-0.5*np.cos(2*np.pi*n/(signal_length-1))
    windowed_signal = np.convolve(signal, hanning_window, 'same')

    assert np.size(windowed_signal) == np.size(signal)

    return windowed_signal


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

# ------- only used for debugging/testing ------------

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



    

