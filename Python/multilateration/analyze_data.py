import numpy as np
from scipy import signal


def calculate_xcorr_lag_array(data_array):
    """Crosscorrelates the filtered data array.

    Crosscorrelates the filtered data arrays and returns an array which
    gives the number of samples between the measured signals at hyd1, hyd2, hyd3.
    Input data array is indexed as follows:

    data_port = data_array[0]
    data_starboard = data_array[1]
    data_stern = data_array[2]

    Implementation is based on the example in the following link:
    https://docs.scipy.org/doc/scipy/reference/reference/generated/scipy.signal.correlation_lags.html#scipy.signal.correlation_lags

    Args:
        data_array: Array of filtered data from the hydrophones.

    Returns:
        An array containing calculated cross-correlated lag between the hydrophones:
        port_starboard_lag, port_stern_lag, starboard_stern_lag.
    """

    data_port = data_array[0]
    data_starboard = data_array[1]
    data_stern = data_array[2]

    cross_corr_port_starboard = signal.correlate(data_port, data_starboard, mode="full")
    port_starboard_lags = signal.correlation_lags(
        data_port.size, data_starboard.size, mode="full"
    )
    port_starboard_lag = port_starboard_lags[np.argmax(cross_corr_port_starboard)]

    cross_corr_port_stern = signal.correlate(data_port, data_stern, mode="full")
    port_stern_lags = signal.correlation_lags(
        data_port.size, data_stern.size, mode="full"
    )
    port_stern_lag = port_stern_lags[np.argmax(cross_corr_port_stern)]

    cross_corr_starboard_stern = signal.correlate(
        data_starboard, data_stern, mode="full"
    )
    starboard_stern_lags = signal.correlation_lags(
        data_starboard.size, data_stern.size, mode="full"
    )
    starboard_stern_lag = starboard_stern_lags[np.argmax(cross_corr_starboard_stern)]

    return port_starboard_lag, port_stern_lag, starboard_stern_lag
