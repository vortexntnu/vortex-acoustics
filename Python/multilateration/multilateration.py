"""Estimates position of sound source in xy-coordinates based on time difference of arrival between hydrophones.
"""


import numpy as np

import multilateration.parameters as param
from signal_generation.positioning import find_maximum_distance


def check_invalid_time(
    sample_diff: np.uint32,
    sample_frequency: float,
    max_time_difference: float,
):
    """Checking if the time difference exceeds the maximum allowed time for a valid signal."""
    return (abs(sample_diff) * 1 / sample_frequency) > max_time_difference


def check_valid_signals(
    hydrophone_positions: np.array,
    tdoa_sample_array: np.array,
    sample_frequency: float,
) -> bool:
    max_distance = find_maximum_distance(positions=hydrophone_positions)
    max_time_difference = max_distance / param.PhysicalConstants.SOUND_SPEED

    for sample_diff in tdoa_sample_array:
        if check_invalid_time(
            sample_diff=sample_diff,
            sample_frequency=sample_frequency,
            max_time_difference=max_time_difference,
        ):
            return False
    return True


def calculate_pinger_position(
    tdoa_lag_array,
    hydrophone_positions,
    sample_frequency,
):
    """
    Args:
        tdoa_lag_array: Array of the lag between the hydrophones

    Returns:
        x_estimate: Estimated x-position of source
        y_estimate: Estimated y-postition of source'
    """

    tdoa_array = tdoa_lag_array / sample_frequency

    A, B = calculate_tdoa_matrices(
        tdoa_array=tdoa_array,
        hydrophone_positions=hydrophone_positions,
    )

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


def calculate_tdoa_matrices(
    tdoa_array,
    hydrophone_positions,
):
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
    # As one hydrophone is used as the reference position we seperate the reference from the others
    reference_hydrophone_position = hydrophone_positions[0]
    rest_hydrophone_positions = hydrophone_positions[1:]

    A = np.empty((len(rest_hydrophone_positions), 4))

    for i, hydrophone_position in enumerate(rest_hydrophone_positions):
        A[i, 0] = reference_hydrophone_position.x - hydrophone_position.x
        A[i, 1] = reference_hydrophone_position.y - hydrophone_position.y
        A[i, 2] = reference_hydrophone_position.z - hydrophone_position.z

        A[i, 3] = tdoa_array[i] * param.PhysicalConstants.SOUND_SPEED

    B = np.empty((len(rest_hydrophone_positions),))
    for i, hydrophone_position in enumerate(rest_hydrophone_positions):
        B[i] = (
            1
            / 2
            * (
                reference_hydrophone_position.x ** 2
                - hydrophone_position.x ** 2
                + reference_hydrophone_position.y ** 2
                - hydrophone_position.y ** 2
                + reference_hydrophone_position.z ** 2
                - hydrophone_position.z ** 2
                + (tdoa_array[i] * param.PhysicalConstants.SOUND_SPEED) ** 2
            )
        )

    return A, B
