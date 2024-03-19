"""
Provide functions for finding the tdoa array from correlation.
Provide function for evaluating tdoa array.
"""
import numpy as np


def calculate_tdoa_array(
    correlation_matrix,
    lag_matrix,
):
    """

    Args:
        correlation_matrix:
        lag_matrix:

    Returns:
        An array containing the time difference of arrival between the hydrophones, in samples.
    """
    tdoa_array = []
    for i in range(len(correlation_matrix)):
        max_lag = lag_matrix[i][np.argmax(correlation_matrix[i])]
        tdoa_array.append(max_lag)
    return tdoa_array


def calculate_distance(
    pinger_position,
    hydro_array_positions,
):
    """
    Calculate distance between source and hydrophones.
    This function is then used to find the theoretically correct TDOA array.

    Args:
        pinger_position:
        hydro_array_positions:

    Returns:
        Distance between pinger and the hydrophones.
    """
    distance = np.empty(len(hydro_array_positions))
    for index, position in enumerate(hydro_array_positions):
        distance[index] = abs(pinger_position - position)

    return distance


def check_tdoa_array_is_valid(
    tol,
    hydro_array,
    tdoa_array,
    pinger_position,
    sampling_frequency,
):
    """
    Calculate the theoretical/correct tdoa array, and compare it the tdoa array found from correlation.

    Arg:
    Tol : how many samples do you allow between calculated result and theoretically correct result.
    Array with hydrophones
    The calculated tdoa array
    Sampling frequency

    Returns:
    Bool saying if tdoa is correct
    """

    distances = calculate_distance(pinger_position, hydro_array.positions)
    distance_diff_array = np.empty(len(hydro_array) - 1)
    for i in range(len(hydro_array) - 1):
        distance_diff_array[i] = distances[0] - distances[i + 1]

    correct_tdoa_array = (
        distance_diff_array / 1500 * sampling_frequency * 1000
    )  # [samples]

    for i in range(len(hydro_array) - 1):
        diff = abs(correct_tdoa_array[i] - tdoa_array[i])
        if diff > tol:
            return False

    return True


def find_diff_between_tdoa_array(
    hydro_array,
    tdoa_array,
    pinger_position,
    sampling_frequency,
):
    """
    Different way of comparing tdoa array to theoretically tdoa array.
    Args:
        hydro_array:
        tdoa_array:
        pinger_position:
        sampling_frequency:

    Returns:
        An array containing the difference (in samples) between calculated result and theoretically correct result.
    """
    distances = calculate_distance(pinger_position, hydro_array.positions)
    distance_diff_array = np.empty(len(hydro_array) - 1)
    for i in range(len(hydro_array) - 1):
        distance_diff_array[i] = distances[0] - distances[i + 1]

    correct_tdoa_array = (
        distance_diff_array / 1500 * sampling_frequency * 1000
    )  # [samples]

    diff_array = np.empty(len(tdoa_array))
    for index in range(len(tdoa_array)):
        diff_array[index] = abs(round(tdoa_array[index] - correct_tdoa_array[index]))

    return diff_array
