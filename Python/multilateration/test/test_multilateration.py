import multilateration.multilateration as mult
import multilateration.parameters as param
import numpy as np
from signal_generation.positioning import Position

source_position = Position(
    x=12.0,
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


def test_trilateration_algorithm():
    """Testing if algorithm gives wanted results.

    Calculates the distance between the estimated position and the
    actual position to check if within given tolerance value.
    Only considering x and y, since we only have three hydrophones.
    """
    tolerance = 3
    sample_frequency = 300000

    tdoa_sample_array = generate_tdoa_lag_array(
        hydrophone_positions=hydrophone_positions,
        source_position=source_position,
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

    assert abs(source_position - res_position) < tolerance
