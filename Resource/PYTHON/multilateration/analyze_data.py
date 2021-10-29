from numpy.lib.index_tricks import index_exp
import parameters as param
from scipy import signal
import numpy as np


# Functions for analyzing the data


# @brief Function to find the index and the maximum abs value in an
# array. The function returns the index and the maximum value 
# indirectly as references

# If @p array_length is invalid (negative), the function sets both
# @p max_val and @p idx to -1
# @param data_array The array to find the maximum of
# @param array_length The length of the array
# @param idx The index containing the maximum abs value
# @param max_val Max absolute value


def calculate_xcorr_lag_array(data_array):
    # Getting the values from the arrays
    data_port = data_array[0]
    data_starboard = data_array[1]
    data_stern = data_array[2]

    # Crosscorrelating the data
    # https://docs.scipy.org/doc/scipy/reference/reference/generated/scipy.signal.correlation_lags.html#scipy.signal.correlation_lags
    cross_corr_port_starboard = signal.correlate(data_port, data_starboard, mode="full")
    port_starboard_lags = signal.correlation_lags(data_port.size, data_starboard.size, mode="full")
    port_starboard_lag = port_starboard_lags[np.argmax(cross_corr_port_starboard)]

    cross_corr_port_stern = signal.correlate(data_port, data_stern, mode="full")
    port_stern_lags = signal.correlation_lags(data_port.size, data_stern.size, mode="full")
    port_stern_lag = port_stern_lags[np.argmax(cross_corr_port_stern)]
    
    cross_corr_starboard_stern = signal.correlate(data_starboard, data_stern, mode="full")
    starboard_stern_lags = signal.correlation_lags(data_starboard.size, data_stern.size, mode="full")
    starboard_stern_lag = starboard_stern_lags[np.argmax(cross_corr_starboard_stern)]


    # Linear transformation to the cross-correlated lags to find the
    # difference in number samples. 
    # When these values are shifted around 0, it is much easier to later
    # determine which hydrophone measured the signal first
    port_starboard_lag -= param.DSPConstants.IN_BUFFER_LENGTH
    port_stern_lag -= param.DSPConstants.IN_BUFFER_LENGTH
    starboard_stern_lag -= param.DSPConstants.IN_BUFFER_LENGTH
    
    return port_starboard_lag, port_stern_lag, starboard_stern_lag

