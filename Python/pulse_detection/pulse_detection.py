import logging

import numpy as np
import scipy.signal

import pulse_detection.envelope_detection as pd_ed


def envelope_based_detection(
    signal,
    carrier_frequency,
    pulse_length: float = 30,  # [ms]
):
    """ Uses the signal envelope and threshold generated from mean and
    variance to detect pulses.
    """
    sampling_frequency = 500  # [kHz]

    envelope = pd_ed.asynchronous_full_wave(
        signal=signal,
        carrier_frequency=carrier_frequency,
        sampling_frequency=sampling_frequency,
    )

    # calculate threshold:
    # the idea is that given a pulse length of a few micro seconds and
    # period of 2s, calculating the historic mean and variance will align
    # with the noise mean and variance
    frame_length = 200
    logging.debug(f"Frame Length: {frame_length}")

    historic_mean = np.mean(envelope)
    historic_variance = np.var(envelope)
    means = np.zeros(len(envelope) - frame_length)
    variances = np.zeros(len(means))

    for index in range(0, len(means)):
        frame = envelope[index:(index+frame_length)]
        means[index] = np.mean(frame)
        variances[index] = np.var(frame)

    logging.debug(f"Min Index: {np.argmin(means)}")
    logging.debug(f"Max Index: {np.argmax(means)}")
    threshold = np.mean(means) + 3*np.var(envelope)

    logging.debug(f"Threshold set at: {threshold:.2f}")

    # find transition of threshold
    pulse_detected = means > threshold
    return pulse_detected
