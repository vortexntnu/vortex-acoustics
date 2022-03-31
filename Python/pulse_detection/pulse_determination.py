import numpy as np
import scipy.signal

"""
Find required frequency bins. -> find order of fft. 
Decide what frquency. 

Same problem as with pdoa?!
Fft order 


"""

def determine_signal_frequency(
    signal: np.array,
    sampling_frequency: float,
) -> float:
    fourier = np.fft.rfft(signal)
    corresponding_freq = np.fft.rfftfreq(len(signal), 1 / sampling_frequency)
    freq_index = np.argmax(fourier)
    dominating_freq = corresponding_freq[freq_index]

    return dominating_freq
