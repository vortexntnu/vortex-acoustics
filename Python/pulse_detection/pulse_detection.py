import logging

import numpy as np
import pulse_detection.envelope_detection as pd_ed
import scipy.signal


def detect_pulse_by_envelope_and_mean_variance(
    envelope,
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
    historic_mean = np.mean(envelope)
    historic_variance = np.var(envelope)
    logging.debug(f"Historic Mean: {historic_mean:.3f}")
    logging.debug(f"Historic Variance: {historic_variance:.3f}")

    pulse_detected = np.zeros(len(envelope) - frame_length, dtype=np.bool_)

    if threshold is None:
        threshold = historic_mean + 3 * historic_variance
        logging.debug(f"Threshold set at: {threshold:.2f}")

    for index in range(0, len(envelope) - frame_length):
        frame = envelope[index : (index + frame_length)]
        above_treshold = frame > threshold
        pulse_detected[index] = np.all(above_treshold)

    return pulse_detected


def detect_pulse_by_envelope_and_differentiation(
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
