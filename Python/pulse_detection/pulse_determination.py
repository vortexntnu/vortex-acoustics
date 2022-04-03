from signal import signal
from socket import SOCK_DGRAM
import numpy as np
from scipy.fft import fft
from scipy.fftpack import fft2
import scipy.signal

"""
TODO:
- Test
    - windowing - it runs, but dont know how to see if it is correct
    - short time fourier transform
    - with signal generation 


"""

#try without hanning window. 
def short_time_fourier_transform(
    signal: np.array, 
    fft_size: int, 
    sampling_frequency: float, # [kHz]
    pulse_length: int, # [ms]
)-> np.array:

    MN_point_fft = np.zeros(fft_size, dtype=np.complex) 
    M = (pulse_length * sampling_frequency) // fft_size
    for i in range(M):
        windowed_signal_segment = \
            apply_hanning_window(signal[i*fft_size:(i+1)*fft_size])
        print("\n window size is: ", np.size(windowed_signal_segment)) #lik 128
        N_point_fft = np.fft.rfft(windowed_signal_segment)
        print("\n fft size of segment is; ", np.size(N_point_fft)) #lik 65! but why? 

        for index in range(np.size(N_point_fft)): # not equal to fft size !!
            MN_point_fft[index] += N_point_fft[index]

    return MN_point_fft


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
    adj_signal = adjust_signal_length(signal, fft_size)
    print("Size of adj singal is: ", np.size(adj_signal)) #128
    fourier = np.fft.rfft(adj_signal)
    print("Size of fft is: ", np.size(fourier)) #65
    corresponding_freq = np.fft.rfftfreq(fft_size, 1 / sampling_frequency)
    print("Size of fft freq is: ", np.size(corresponding_freq)) #65
    freq_index = np.argmax(fourier)
    dominating_freq = corresponding_freq[freq_index]

    return dominating_freq , corresponding_freq, fourier



    

