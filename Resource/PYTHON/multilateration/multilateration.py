"""Estimates position of sound source in xy-coordinates based on time difference of arrival between hydrophones.
"""


import numpy as np

import multilateration.parameters as param
from multilateration.parameters import HydrophoneDetails


# Initializing the variables maximum_time_diff and max_hydrophone_distance.
# These values are updated in the function initialize_trilateration_globals.
max_hydrophone_distance = None
max_time_diff = None


def calculate_distance(x1, y1, z1, x2, y2, z2):
    return np.sqrt((x2 - x1) ** 2 + (y2 - y1) ** 2 + (z2 - z1) ** 2)


def initialize_trilateration_globals():
    N = param.HydrophoneDetails.NUM_HYDROPHONES
    hydrophone_distances = []
    for i in range(N - 1):
        for j in range(N - 1 - i):
            hydrophone_distances.append(
                calculate_distance(
                    x1=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i][0],
                    y1=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i][1],
                    z1=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i][2],
                    x2=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[j][0],
                    y2=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[j][1],
                    z2=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[j][2],
                )
            )

    max_hydrophone_distance = max(hydrophone_distances)

    # Calculating max time allowed over that distance
    time_diff_tolerance = 0.1
    max_time_diff = (1 + time_diff_tolerance) * (
        max_hydrophone_distance / param.PhysicalConstants.SOUND_SPEED
    )

    return check_initialized_globals()


def check_initialized_globals():
    return (max_hydrophone_distance is not None) and (max_time_diff is not None)


def check_invalid_time(sample_diff: np.uint32):
    """Checking if the time difference exceeds the maximum allowed time for a valid signal."""
    return (abs(sample_diff) * param.DSPConstants.SAMPLE_TIME) > max_time_diff


def check_valid_signals(tdoa_sample_array: np.array) -> bool:
    N = param.HydrophoneDetails.NUM_HYDROPHONES
    for i in range(N - 1):
        if check_invalid_time(tdoa_sample_array[i]):
            return False
    return True


def trilaterate_pinger_position(tdoa_sample_array):
    """
    Args:
        tdoa_array: Array of the lag between the hydrophones:
                     [lag_port_starboard, lag_port_stern, lag_starboard_stern]

    Returns:
        x_estimate: Estimated x-position of source
        y_estimate: Estimated y-postition of source'
    """

    tdoa_sample_array = np.array(tdoa_sample_array)
    tdoa_array = param.DSPConstants.SAMPLE_TIME * tdoa_sample_array

    """ gammelt

    tdoa_port_starboard = param.DSPConstants.SAMPLE_TIME * tdoa_array[0]
    tdoa_port_stern = param.DSPConstants.SAMPLE_TIME * tdoa_array[1]
    tdoa_starboard_stern = param.DSPConstants.SAMPLE_TIME * tdoa_array[2]

    tdoa_array = [tdoa_port_starboard, tdoa_port_stern, tdoa_starboard_stern]

    """

    A, B = calculate_tdoa_matrices(tdoa_array)

    A_T = np.transpose(A)

    A_dot_A_T = np.matmul(A_T, A)

    A_dot_A_T_inv = np.linalg.inv(A_dot_A_T)

    solution_vec = np.matmul(
        A_dot_A_T_inv, np.matmul(A_T, B)
    )  # (A_T * A).invers() * A_T * B

    x_estimate = solution_vec[0]
    y_estimate = solution_vec[1]
    z_estimate = solution_vec[2]

    return x_estimate, y_estimate, z_estimate


def calculate_tdoa_matrices(tdoa_array):
    """Calculates time difference of arrival matrices for multilateration.

        Hydrophones are here labeled as a number
        Port hydrophone         : 0
        Starboard hydrophone    : 1
        Stern hydrophone        : 2

        The positions and distances are therefore calculated using the
        port hydrophone as a reference. Example:
             x_01 = x_0 - x_1        Difference in x-position between hyd 0 and 1
             x_02 = x_0 - x_2        Difference in x-position between hyd 0 and 2
             etc.

        Only x and y is required as we are using 3 hydrophones and calculating
        z will in most cases result in linear dependent equations.

    Args:
        tdoa_array: Array containing time difference of arrival between the hydrophones:
                    [tdoa_port_starboard, tdoa_port_stern, tdoa_starboard_stern]
    Returns:
        A, B: Matrices for use in multilateration algorithm.
    """
    N = param.HydrophoneDetails.NUM_HYDROPHONES
    A = []
    for i in range(N - 1):
        line = []
        line.append(
            param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[0][0]
            - param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i + 1][0]
        )
        line.append(
            param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[0][1]
            - param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i + 1][1]
        )
        line.append(
            param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[0][2]
            - param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i + 1][2]
        )
        line.append(tdoa_array[i] * param.PhysicalConstants.SOUND_SPEED)
        A.append(line)

    """
    x_01 = param.HydrophoneDetails.PORT_HYD_X - param.HydrophoneDetails.STARBOARD_HYD_X
    x_02 = param.HydrophoneDetails.PORT_HYD_X - param.HydrophoneDetails.STERN_HYD_X
    y_01 = param.HydrophoneDetails.PORT_HYD_Y - param.HydrophoneDetails.STARBOARD_HYD_Y
    y_02 = param.HydrophoneDetails.PORT_HYD_Y - param.HydrophoneDetails.STERN_HYD_Y

    tdoa_port_starboard = tdoa_array[0]
    tdoa_port_stern = tdoa_array[1]
    # tdoa_starboard_stern = tdoa_array[2]  - not used

    # Using TDOA to calculate the distances
    d_01 = param.PhysicalConstants.SOUND_SPEED * tdoa_port_starboard
    d_02 = param.PhysicalConstants.SOUND_SPEED * tdoa_port_stern

    A = [[x_01, y_01, d_01], [x_02, y_02, d_02]]

    """
    B = []
    for i in range(N - 1):
        bi = (
            1
            / 2
            * (
                param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[0][0] ** 2
                - param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i + 1][0] ** 2
                + param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[0][1] ** 2
                - param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i + 1][1] ** 2
                + param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[0][2] ** 2
                - param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[i + 1][2] ** 2
                + (tdoa_array[i] * param.PhysicalConstants.SOUND_SPEED) ** 2
            )
        )
        B.append(bi)

    """

    b1 = (
        1
        / 2
        * (
            d_01 ** 2
            + param.HydrophoneDetails.PORT_HYD_X ** 2
            - param.HydrophoneDetails.STARBOARD_HYD_X ** 2
            + param.HydrophoneDetails.PORT_HYD_Y ** 2
            - param.HydrophoneDetails.STARBOARD_HYD_Y ** 2
        )
    )

    b2 = (
        1
        / 2
        * (
            d_02 ** 2
            + param.HydrophoneDetails.PORT_HYD_X ** 2
            - param.HydrophoneDetails.STERN_HYD_X ** 2
            + param.HydrophoneDetails.PORT_HYD_Y ** 2
            - param.HydrophoneDetails.STERN_HYD_Y ** 2
        )
    )
    
    """

    return A, B
