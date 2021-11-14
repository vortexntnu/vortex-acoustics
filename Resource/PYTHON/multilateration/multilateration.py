"""Estimates position of sound source in xy-coordinates based on time difference of arrival between hydrophones.
"""


import numpy as np

import multilateration.parameters as param


# Initializing the variables maximum_time_diff and max_hydrophone_distance.
# These values are updated in the function initialize_trilateration_globals.
max_hydrophone_distance = None
max_time_diff = None


def calculate_distance(hyd_1_x, hyd_1_y, hyd_1_z, hyd_2_x, hyd_2_y, hyd_2_z):
    return np.sqrt(
        (hyd_2_x - hyd_1_x) ** 2 + (hyd_2_y - hyd_1_y) ** 2 + (hyd_2_z - hyd_1_z) ** 2
    )


def initialize_trilateration_globals():
    dist_port_starboard = calculate_distance(
        hyd_1_x=param.HydrophoneDetails.STARBOARD_HYD_X,
        hyd_1_y=param.HydrophoneDetails.STARBOARD_HYD_Y,
        hyd_1_z=param.HydrophoneDetails.STARBOARD_HYD_Z,
        hyd_2_x=param.HydrophoneDetails.PORT_HYD_X,
        hyd_2_y=param.HydrophoneDetails.PORT_HYD_Y,
        hyd_2_z=param.HydrophoneDetails.PORT_HYD_Z,
    )

    dist_port_stern = calculate_distance(
        hyd_1_x=param.HydrophoneDetails.STERN_HYD_X,
        hyd_1_y=param.HydrophoneDetails.STERN_HYD_Y,
        hyd_1_z=param.HydrophoneDetails.STERN_HYD_Z,
        hyd_2_x=param.HydrophoneDetails.PORT_HYD_X,
        hyd_2_y=param.HydrophoneDetails.PORT_HYD_Y,
        hyd_2_z=param.HydrophoneDetails.PORT_HYD_Z,
    )

    dist_starboard_stern = calculate_distance(
        hyd_1_x=param.HydrophoneDetails.STERN_HYD_X,
        hyd_1_y=param.HydrophoneDetails.STERN_HYD_Y,
        hyd_1_z=param.HydrophoneDetails.STERN_HYD_Z,
        hyd_2_x=param.HydrophoneDetails.STARBOARD_HYD_X,
        hyd_2_y=param.HydrophoneDetails.STARBOARD_HYD_Y,
        hyd_2_z=param.HydrophoneDetails.STARBOARD_HYD_Z,
    )

    max_hydrophone_distance = max(
        dist_port_starboard, dist_starboard_stern, dist_port_stern
    )

    # Calculating max time allowed over that distance
    time_diff_tolerance = 0.1
    max_time_diff = (1 + time_diff_tolerance) * (
        max_hydrophone_distance / param.PhysicalConstants.SOUND_SPEED
    )

    return check_initialized_globals()


def check_initialized_globals():
    return (max_hydrophone_distance is not None) and (max_time_diff is not None)


def check_invalid_time(time_diff: np.uint32):
    """Checking if the time difference exceeds the maximum allowed time for a valid signal."""
    return (abs(time_diff) * param.DSPConstants.SAMPLE_TIME) > max_time_diff


def check_valid_signals(p_lag_array: np.array) -> bool:
    if any(
        [
            check_invalid_time(p_lag_array[0]),
            check_invalid_time(p_lag_array[1]),
            check_invalid_time(p_lag_array[2]),
        ]
    ):
        return False
    else:
        return True


def trilaterate_pinger_position(p_lag_array):
    """
    Args:
        p_lag_array: Array of the lag between the hydrophones:
                     [lag_port_starboard, lag_port_stern, lag_startboard_stern]

    Returns:
        x_estimate: Estimated x-position of source
        y_estimate: Estimated y-postition of source
    """

    tdoa_port_starboard = param.DSPConstants.SAMPLE_TIME * p_lag_array[0]
    tdoa_port_stern = param.DSPConstants.SAMPLE_TIME * p_lag_array[1]
    tdoa_starboard_stern = param.DSPConstants.SAMPLE_TIME * p_lag_array[2]

    tdoa_array = [tdoa_port_starboard, tdoa_port_stern, tdoa_starboard_stern]

    A, B = calculate_tdoa_matrices(tdoa_array)

    A_T = np.transpose(A)

    A_T_dot_A = np.matmul(A_T, A)

    A_T_dot_A_inv = np.linalg.inv(A_T_dot_A)

    solution_vec = np.matmul(
        A_T_dot_A_inv, np.matmul(A_T, B)
    )  # (A_T * A).invers() * A_T * B

    x_estimate = solution_vec[0]
    y_estimate = solution_vec[1]

    return x_estimate, y_estimate


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
                    [tdoa_port_starboard, tdoa_port_stern, tdoa_startboard_stern]
    Returns:
        A, B: Matrices for use in multilateration algorithm.
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

    B = [b1, b2]

    return A, B
