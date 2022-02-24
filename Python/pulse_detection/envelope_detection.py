import numpy as np
import scipy.signal


def asynchronous_full_wave(
    signal,
    carrier_frequency,  # [kHz]
    sampling_frequency,  # [kHz]
):
    cutoff_frequency = carrier_frequency / 2
    b, a = scipy.signal.butter(
        N=4,
        Wn=cutoff_frequency * 1000,
        fs=sampling_frequency * 1000,
    )

    signal = scipy.signal.lfilter(b, a, np.abs(signal))
    return signal


def asynchronous_hilbert_complex(
    signal,
):
    hil_signal = scipy.signal.hilbert(signal)
    return np.abs(hil_signal)

