import logging

import numpy as np
import pulse_detection.envelope_detection as pd_ed
import scipy.signal


def detect_pulse_by_envelope_and_mean_variance(
    envelope,  # Envelope of signal, with interval of about 2s
    carrier_frequency,
    frame_length: int = 10,
    sampling_frequency: float = 500,  # [kHz]
    threshold: float = None,
):
    """Uses the signal envelope and threshold generated from mean and
    variance to detect pulses.
    """
    # calculate threshold:
    # the idea is that given a pulse length of a few micro seconds and
    # period of 2s, calculating the historic mean and variance will align
    # with the noise mean and variance
    historic_mean = np.mean(
        envelope
    )  # Getting the average of the noise in the enviroment
    historic_variance = np.var(envelope)  # Variance
    logging.debug(
        f"Historic Mean: {historic_mean:.3f}"
    )  # Information for the user only about noise mean and variance
    logging.debug(f"Historic Variance: {historic_variance:.3f}")

    pulse_detected = np.zeros(
        len(envelope) - frame_length, dtype=np.bool_
    )  # Making new array full of zeroes in the envelope, minus frame length so that
    # the array used later will not be to long, see for loop under

    if threshold is None:
        threshold = (
            historic_mean + 3 * historic_variance
        )  # Making a threshold based on previous mean and variance
        logging.debug(f"Threshold set at: {threshold:.2f}")

    for index in range(0, len(envelope) - frame_length):
        frame = envelope[
            index : (index + frame_length)
        ]  # Makes frames for each element in envelope
        above_treshold = (
            frame > threshold
        )  # True statement will give 1 at the given index. This means that signal amplitude at this index is higher than the threshold
        pulse_detected[index] = np.all(
            above_treshold
        )  # Makes new array where the detected signal is specified

    return pulse_detected


def detect_pulse_by_envelope_and_differentiation(  #
    signal,
    carrier_frequency,
    downsampling_factor: int = 300,
    sampling_frequency: float = 500,  # [kHz]
):
    envelope = pd_ed.asynchronous_full_wave(
        signal=signal,
        carrier_frequency=carrier_frequency,
        sampling_frequency=sampling_frequency,
    )

    frame_length = downsampling_factor
    envelope_means = np.zeros(len(envelope) // frame_length)
    for index in range(0, len(envelope_means)):
        env_index = index * frame_length
        envelope_means[index] = np.mean(envelope[env_index : env_index + frame_length])

    differentiated_means = scipy.signal.lfilter(
        [1, -1],
        [1],
        envelope_means,
    )
    diff_mean = np.mean(differentiated_means)
    diff_var = np.var(differentiated_means)
    logging.debug(f"Mean: {diff_mean}")
    logging.debug(f"Variance: {diff_var}")

    threshold = diff_mean + 3 * diff_var

    transition_detected = differentiated_means > threshold

    return transition_detected, differentiated_means
