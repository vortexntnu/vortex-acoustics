import numpy as np
import scipy.signal


def asynchronous_full_wave(
    signal,
    carrier_frequency,  # [kHz]
    sampling_frequency,  # [kHz]
):
    cutoff_frequency = (
        carrier_frequency / 2
    )  # High pass filter. Removes all the frequencies under half the carrier frequencies
    (
        b,
        a,
    ) = scipy.signal.butter(  # Using Butterworthfilter, b corresponds to zeroes and a to poles in the z-plane
        # Butterworth is maximally flat in the passband so all frequencies kept are amplified by same factor        N=4, # The order of the filter
        Wn=cutoff_frequency * 1000,  # Convert to form kHz to Hz
        fs=sampling_frequency * 1000,
    )

    signal = scipy.signal.lfilter(
        b, a, np.abs(signal)
    )  # Filters the signal with the poles and zeroes from Butterworth

    return signal


def asynchronous_hilbert_complex(
    signal,
):
    hil_signal = scipy.signal.hilbert(
        signal
    )  # Hilbert filter, determines amplitude of envelope
    return np.abs(hil_signal)
