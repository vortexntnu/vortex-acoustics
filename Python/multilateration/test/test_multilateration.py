import multilateration.multilateration as mult
import multilateration.parameters as param
import numpy as np
from signal_generation.positioning import Position
import pytest
import matplotlib.pyplot as plt

n = 1
x = np.arange(n)
# src_x_arr = np.exp(x/100) / n
# src_x_arr = np.log(x/100) * 10
src_x_arr = [16]
# plt.plot(x, src_x_arr)
# plt.show()

result_mtx = np.ndarray((n, 2))
# result_mtx = np.ndarray((n, 4))

source_position = Position(
    x=8.0,
    y=2.0,
    z=1.0,
)

hydrophone_positions = np.array(
    [
        Position(
            x=-0.11,
            y=0.31,
            z=0.1,
        ),
        Position(
            x=0.11,
            y=0.31,
            z=0.1,
        ),
        Position(
            x=0.0,
            y=-0.24,
            z=0.0,
        ),
        Position(
            x=0.5,
            y=-0.1,
            z=0.4,
        ),
        Position(
            x=0.4,
            y=0.0,
            z=-0.4,
        ),
    ]
)


def generate_tdoa_lag_array(
    hydrophone_positions: np.array,
    source_position: Position,
    sample_frequency: float,
):
    """Generates a complete lag array of all possible combinations. Where
    the indexes lag_array[m][n] correspond to the time difference tnm.

    So to get the time (sample) difference t01 the lag array has to be evaluated
    with lag_array[1][0].
    """
    time_of_flight = (
        abs(hydrophone_positions - source_position)
        / param.PhysicalConstants.SOUND_SPEED
    )
    time_difference_of_arrival = (
        np.array(len(time_of_flight) * [time_of_flight])
        - np.array(len(time_of_flight) * [time_of_flight]).T
    )
    lag_array = np.int32(time_difference_of_arrival * sample_frequency)

    return lag_array


def calculate_tdoa_array(
    hydrophone_positions: np.array,
    source_position: Position,
    sample_frequency: float,
):
    """Generates TOA of the sound-signals for the different hydrophones
    and calculates the lag between them.
    """
    distance_array = []
    N = param.HydrophoneDetails.NUM_HYDROPHONES
    for hydrophone_position in hydrophone_positions:
        distance_array.append(
            mult.calculate_distance(
                x1=source_position.x,
                y1=source_position.y,
                z1=source_position.z,
                x2=hydrophone_position.x,
                y2=hydrophone_position.y,
                z2=hydrophone_position.z,
            )
        )
    distance_array = np.array(distance_array)
    toa_sample_array = np.int32(
        distance_array * sample_frequency / param.PhysicalConstants.SOUND_SPEED
    )

    """
    Calculating the number of samples it takes for the sound to arrive at x hyd
    Since the TOA uses lag (direct measuremend), these values are uint32_t
    converting to int32 works as floor. Should be rounded.
    """
    tdoa_sample_array = np.empty((N - 1,))
    for i in range(N - 1):
        tdoa_sample_array[i] = toa_sample_array[0] - toa_sample_array[i + 1]

    return tdoa_sample_array


@pytest.mark.parametrize(
    "source_position_x",
    src_x_arr,
    ids=x.tolist(),
)
def test_trilateration_algorithm(source_position_x, request):
    """Testing if algorithm gives wanted results.

    Calculates the distance between the estimated position and the
    actual position to check if within given tolerance value.
    Only considering x and y, since we only have three hydrophones.
    """
    print("\n")
    tolerance = 40000
    sample_frequency = 300000
    source_position_new = Position(source_position_x, source_position.y, source_position.z)
    tdoa_sample_array = generate_tdoa_lag_array(
        hydrophone_positions=hydrophone_positions,
        source_position=source_position_new,
        sample_frequency=sample_frequency,
    )
    print(tdoa_sample_array)
    print("\n")
    tdoa_sample_array = generate_tdoa_lag_array(
        hydrophone_positions=hydrophone_positions,
        source_position=source_position_new,
        sample_frequency=sample_frequency,
    )[:, 0][1:]

    res_x, res_y, res_z = mult.calculate_pinger_position(
        tdoa_lag_array=tdoa_sample_array,
        hydrophone_positions=hydrophone_positions,
        sample_frequency=sample_frequency,
    )

    res_position = Position(
        x=res_x,
        y=res_y,
        z=res_z,
    )

    # print(source_position_x)
    # print(request.node.callspec.id)
    # print(abs(source_position_new - res_position))
    # print("\n")
    result_mtx[int(request.node.callspec.id)] = [int(request.node.callspec.id), abs(source_position_new - res_position)]
    if int(request.node.callspec.id) == 999:
        # plt.plot(x, result_mtx[:,1])

        # plt.show()

        print(max(result_mtx[:,1]))
        print(max(src_x_arr))
        
        ax1 = plt.subplot()
        l1, = ax1.plot(result_mtx[:,1], color='red')
        ax2 = ax1.twinx()
        l2, = ax2.plot(src_x_arr, color='orange')

        plt.legend([l1, l2], ["error", "x_placement"])
        plt.grid()
        
        plt.show()
    
    
    
    assert abs(source_position_new - res_position) < tolerance

def test_compare_with_teensy():
    """Provides a reference for the code on the Teensy to compare against.

    The estimates that are generated within this test should be the same if run on the Teensy.
    """

    tdoa_sample_array = np.empty(4, int)
    tdoa_sample_array[0] = 1
    tdoa_sample_array[1] = 0
    tdoa_sample_array[2] = 2
    tdoa_sample_array[3] = 0

    sample_frequency = 300000

    hydrophone_positions = np.array(
        [
            Position(
                x=-0.11,
                y=0.31,
                z=0.1,
            ),
            Position(
                x=0.11,
                y=0.31,
                z=0.1,
            ),
            Position(
                x=0.0,
                y=-0.24,
                z=0.0,
            ),
            Position(
                x=0.5,
                y=-0.1,
                z=0.4,
            ),
            Position(
                x=0.4,
                y=0.0,
                z=-0.4,
            ),
        ]
    )

    x_estimate, y_estimate, z_estimate = mult.calculate_pinger_position(
        tdoa_lag_array=tdoa_sample_array,
        hydrophone_positions=hydrophone_positions,
        sample_frequency=sample_frequency,
    )

    print(f"x: {x_estimate}, y: {y_estimate}, z: {z_estimate} ")
