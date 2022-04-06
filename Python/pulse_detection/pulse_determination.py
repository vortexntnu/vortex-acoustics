from signal import signal
from socket import SOCK_DGRAM
import numpy as np
from scipy.fft import fft
#from scipy.fftpack import fft2


"""
TODO:
- Test
    - windowing - not sure how to see if it is correct
    - short time fourier transform
        - works without windowing
    - with signal generation 

- not able to distinguish 35 and 40 with fft size 128
    with fft size 256 its good -> bin width = 1 kHz

- port to cpp

"""
 
def short_time_fourier_transform(
    signal: np.array, 
    fft_size: int, 
    sampling_frequency: float, # [kHz]
    pulse_length: int, # [ms]
)-> np.array:

    MN_point_fft = np.zeros(fft_size//2 +1, dtype=np.complex) 
    M = (pulse_length * sampling_frequency) // fft_size
    for i in range(M):
        #windowed_signal_segment = \
        #    apply_hanning_window(signal[i*fft_size:(i+1)*fft_size])
        N_point_fft = np.fft.rfft(signal[i*fft_size:(i+1)*fft_size]) # np.fft.rfft(windowed_signal_segment) 
        for index in range(np.size(N_point_fft)): 
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

    hanning_window = 0.5-0.5*np.cos(2*np.pi*n/(signal_length-1)) #np.hanning(signal_length) 
    windowed_signal = np.convolve(signal, hanning_window, 'same')

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

#obs - not using this result 
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

def determine_signal_frequency( 
    signal: np.array, 
    sampling_frequency: float,
    fft_size: int,
): # -> float, np.array
    adj_signal = adjust_signal_length(signal, fft_size)
    fourier = np.fft.rfft(adj_signal)
    corresponding_freq = np.fft.rfftfreq(fft_size, 1 / sampling_frequency)
    freq_index = np.argmax(fourier)
    dominating_freq = corresponding_freq[freq_index]

    return dominating_freq , corresponding_freq, fourier

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


    

