"""Provides functions to correlate signals.
"""

import numpy as np
from scipy import signal


def calculate_correlation_matrix(
    signals: np.array,
):
    """
    Args:
    A matrix containing the signals from the receivers.

    Return:
    A matrix containing the correlation indexes between one signal and al the others.  
    A matrix containing the lags.   
    """

    correlation_matrix = []
    lag_matrix = []
    for i in range(len(signals)-1):
        correlation_matrix.append(
            signal.correlate(signals[0], signals[i+1], mode='full', method='direct')
        )
        lag_matrix.append(
            signal.correlation_lags(len(signals[0]), len(signals[i+1]), mode='full')
        )
    return correlation_matrix, lag_matrix


def cut_signals(
    frame,
    signals,
):

    """

    Args:
        frame:
        signals:

    Returns:
        A matrix "signals" where for every signal,values outside the frame are zero
    """

    for i in range(len(signals)):
        for j in range(len(signals[i])):
            if j < frame[0] or j > frame[1]:
                signals[i][j] = 0
    return signals
