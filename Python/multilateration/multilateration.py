"""Estimates position of sound source in xy-coordinates based on time difference of arrival between hydrophones.
"""


import multilateration.parameters as param
import numpy as np
from signal_generation.positioning import find_maximum_distance


def check_invalid_time(
    sample_diff: np.uint32,
    sample_frequency: float,
    max_time_difference: float,
):
    """Checking if the time difference exceeds the maximum allowed time for a valid signal."""
    return (abs(sample_diff) * 1 / sample_frequency) > max_time_difference

def gauss_newton_nllr(x, y, z, t, x_init, y_init, z_init, t_init):
    tol = 0.0001                                        # tolerance for convergence
    iter_max = 50                                          # max iterations
    
    c = param.PhysicalConstants.SOUND_SPEED
    #t = T + 1/c*np.sqrt((x-X)**2+(y-Y)**2+(z-Z)**2)

    n = len(x)                                          # num of data point = num of hydrophones

    f = np.ndarray((1, n)) # fnc arr
    j = np.ndarray((n, 4)) # jacobian mtx
    d = np.ndarray((1, n)) # delta arr
    for iter in range(iter_max):
        x_0, y_0, z_0, t_0 = x_init, y_init, z_init, t_init # init values from linear least squares
        for i in range(n):
            f[i] = t[i] + 1/c*np.sqrt((x_0-x[i])**2+(y_0-y[i])**2+(z_0-z[0])**2) - t_0      # non-linear model
            j[i, 0] = (x[i] - x_0)/(c*np.sqrt((x_0-x[i])**2+(y_0-y[i])**2+(z_0-z[0])**2))   # pde wrt X
            j[i, 1] = (y[i] - y_0)/(c*np.sqrt((x_0-x[i])**2+(y_0-y[i])**2+(z_0-z[0])**2))   # pde wrt Y
            j[i, 2] = (z[i] - z_0)/(c*np.sqrt((x_0-x[i])**2+(y_0-y[i])**2+(z_0-z[0])**2))   # pde wrt Z
            j[i, 3] = 1                                                                     # pde wrt T

            print(f)

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
    print(A)
    print("\n")
    print(B)
    print("\n")
    print(solution_vec)
    print("\n")
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
                reference_hydrophone_position.x**2
                - hydrophone_position.x**2
                + reference_hydrophone_position.y**2
                - hydrophone_position.y**2
                + reference_hydrophone_position.z**2
                - hydrophone_position.z**2
                + (tdoa_array[i] * param.PhysicalConstants.SOUND_SPEED) ** 2
            )
        )

    return A, B
