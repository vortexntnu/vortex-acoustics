import numpy as np
from multilateration import parameters
from signal_generation.positioning import Position


def generate_lag_array(
    hydrophone_positions: np.array,
    source_position: Position,
    sample_frequency: float = 100000,
):
    """Generates a complete lag array of all possible combinations.

    The indexes of lag_array[m][n] correspond to the time difference tnm.
    So to get the time (sample) difference t01 the lag array has to be evaluated
    with lag_array[1][0].

    Args:
        hydrophone_positions: The position of the receiving hydrophones which is an array of Position objects.
        source_position: The position of the source.
        sample_frequency: The sampling frequency to be used, which corresponds to the available resolution.

    Returns:
        An array containing all the time difference of arrivals in samples (lag) given by the hydrophone
        positions.
    """
    time_of_flight = (
        abs(hydrophone_positions - source_position)
        / parameters.PhysicalConstants.SOUND_SPEED
    )
    time_difference_of_arrival = (
        np.array(len(time_of_flight) * [time_of_flight])
        - np.array(len(time_of_flight) * [time_of_flight]).T
    )
    lag_array = np.int32(time_difference_of_arrival * sample_frequency)

    return lag_array
