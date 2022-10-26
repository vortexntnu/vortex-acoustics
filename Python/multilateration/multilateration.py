"""Estimates position of sound source in xy-coordinates based on time difference of arrival between hydrophones.
"""

from turtle import pos
import numpy as np
from itertools import combinations

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

def mldivide(A, B):
    num_vars = A.shape[1]
    rank = np.linalg.matrix_rank(A)
    if rank == num_vars:
        # print("MLDIVIDE - LSTSQ")
        sol = np.linalg.lstsq(A, B)[0]    # not under-determined
    else:
        # print("MLDIVIDE - UNDERDETERMINED")
        for nz in combinations(range(num_vars), rank):    # the variables not set to zero
            try: 
                sol = np.zeros((num_vars, 1))  
                sol[nz, :] = np.asarray(np.linalg.solve(A[:, nz], B))
            except np.linalg.LinAlgError:
                print("MLDIVIDE - CANT SOLVE")   
                # raise ValueError("MLDIVIDE - CANT SOLVE")
                return np.ones((1, 3)) * 1000000
                pass                    # picked bad variables, can't solve
    return sol


def gauss_newton_nllr(pos_ref, pos_hyd, tdoa_ref2hyd, solution_vec, a_init=1.0):
    # f[0, i] = a_scalar*tdoa_ref2hyd[i] + 1/c*np.sqrt((x_src-x)**2+(y_src-y)**2+(z_src-z)**2)                   # non-linear model
    # j[i, 0] = (x - x_src)/(c*np.sqrt((x_src-x)**2+(y_src-y)**2+(z_src-z)**2))   # pde wrt X
    # j[i, 1] = (y - y_src)/(c*np.sqrt((x_src-x)**2+(y_src-y)**2+(z_src-z)**2))   # pde wrt Y
    # j[i, 2] = (z - z_src)/(c*np.sqrt((x_src-x)**2+(y_src-y)**2+(z_src-z)**2))   # pde wrt Z
    # j[i, 3] = a_scalar                                                       # pde wrt T
    # d[0, i] = tdoa_ref2hyd[i] + 1/c*np.sqrt((x_src-x)**2+(y_src-y)**2+(z_src-z)**2) - f[0, i]


    # f[0, i] = np.sqrt((pos_hyd[i, 0]-x_src)**2+(pos_hyd[i, 1]-y_src)**2+(pos_hyd[i, 2]-z_src)**2)                   # non-linear model
    # j[i, 0] = (pos_hyd[i, 0]-x_src)/(np.sqrt((pos_hyd[i, 0]-x_src)**2+(pos_hyd[i, 1]-y_src)**2+(pos_hyd[i, 2]-z_src)**2))   # pde wrt X
    # j[i, 1] = (pos_hyd[i, 1]-y_src)/(np.sqrt((pos_hyd[i, 0]-x_src)**2+(pos_hyd[i, 1]-y_src)**2+(pos_hyd[i, 2]-z_src)**2))   # pde wrt Y
    # j[i, 2] = (pos_hyd[i, 2]-z_src)/(np.sqrt((pos_hyd[i, 0]-x_src)**2+(pos_hyd[i, 1]-y_src)**2+(pos_hyd[i, 2]-z_src)**2))   # pde wrt Z
            
    tol = 1e-5                                        # tolerance for convergence
    iter_max = 1000                                       # max iterations

    c = param.PhysicalConstants.SOUND_SPEED
    #t = T + 1/c*np.sqrt((x-X)**2+(y-Y)**2+(z-Z)**2)

    n = len(tdoa_ref2hyd)                               # num of data point = num of hydrophones
    a = np.array([solution_vec[0],
                  solution_vec[1],
                  solution_vec[2]])                     # fnc arr
    f = np.zeros((1, n))                                # fnc arr
    j = np.zeros((n, 3))                                # jacobian mtx
    d = np.zeros((1, n))                                # delta arr
    print("GAUSSIAN NEWTON NLLSR")
    print(pos_hyd)
    print(tdoa_ref2hyd)
    print(solution_vec)

    
    a_iter = np.zeros((iter_max, 3))
    da_iter = np.zeros((iter_max, 3))

    for iter in range(iter_max):
        x_src, y_src, z_src = a[0], a[1], a[2]
        for i in range(n):
            f[0, i] = np.sqrt((x_src-pos_hyd[i, 0])**2+(y_src-pos_hyd[i, 1])**2+(z_src-pos_hyd[i, 2])**2)                   # non-linear model
            j[i, 0] = (x_src - pos_hyd[i, 0])/(np.sqrt((x_src-pos_hyd[i, 0])**2+(y_src-pos_hyd[i, 1])**2+(z_src-pos_hyd[i, 2])**2))   # pde wrt X
            j[i, 1] = (y_src - pos_hyd[i, 1])/(np.sqrt((x_src-pos_hyd[i, 0])**2+(y_src-pos_hyd[i, 1])**2+(z_src-pos_hyd[i, 2])**2))   # pde wrt Y
            j[i, 2] = (z_src - pos_hyd[i, 2])/(np.sqrt((x_src-pos_hyd[i, 0])**2+(y_src-pos_hyd[i, 1])**2+(z_src-pos_hyd[i, 2])**2))   # pde wrt Z
            d[0, i] = (tdoa_ref2hyd[i] + solution_vec[3]) - f[0, i]

            # d[0, i] = (np.sqrt((solution_vec[0]-pos_hyd[i, 0])**2+(solution_vec[1]-pos_hyd[i, 1])**2+(solution_vec[2]-pos_hyd[i, 2])**2)) - f[0, i]

            # d[0, i] = (tdoa_ref2hyd[i] + np.sqrt((f_prev[0])**2+(f_prev[1])**2+(f_prev[2])**2)) - f[0, i]
        da0 = np.matmul(np.transpose(j), j)
        da1 = np.matmul(np.transpose(j), np.transpose(d))
        da = np.transpose(mldivide(da0, da1))

        a = np.add(a, da)[0]
        a_iter[iter, :] = a
        da_iter[iter, :] = da
        conv = 0
        if (abs(da[0, 0]) < tol) and (abs(da[0, 1]) < tol) and (abs(da[0, 2]) < tol):
            print("CONVERGED")
            conv = iter
            # print(a_iter[:conv+1, :])
            # print(da_iter[:conv+1, :])
            print(conv)
            # print(abs(da[0,0])<tol)
            return -a

    if conv == 0:
        print("NOT CONVERGED")
        # print(a_iter)
        # print(da_iter)
    return [-1, -1, -1]

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

    pos_ref = np.array([hydrophone_positions[0].x, hydrophone_positions[0].y, hydrophone_positions[0].z])
    pos_hyd = np.ndarray((len(hydrophone_positions) - 1, 3))
    for i, hyd_pos in enumerate(hydrophone_positions):
        if i == 0:
            continue
        else:
            pos_hyd[i-1] = np.array([hyd_pos.x, hyd_pos.y, hyd_pos.z])
    # x_ref = hydrophone_positions[0].x
    # y_ref = hydrophone_positions[0].y
    # z_ref = hydrophone_positions[0].z
    tdoa_ref2hyd = A[:,3]

    a = gauss_newton_nllr(pos_ref, pos_hyd, tdoa_ref2hyd, solution_vec, a_init=1.0)
    return x_estimate, y_estimate, z_estimate, a[0], a[1], a[2]


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
