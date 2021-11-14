import numpy as np

import multilateration.multilateration as mult
import multilateration.parameters as param


def valid_number_of_hydrophones():
    return param.HydrophoneDetailes.NUM_HYDROPHONES < 3


def calculate_toa_array():
    """Generates TOA of the sound-signals for the different hydrophones
    and calculates the lag between them.
    """

    dist_src_port = mult.calculate_distance(
        param.TestParameters.SOURCE_POS_X,
        param.TestParameters.SOURCE_POS_Y,
        param.TestParameters.SOURCE_POS_Z,
        param.HydrophoneDetails.PORT_HYD_X,
        param.HydrophoneDetails.PORT_HYD_Y,
        param.HydrophoneDetails.PORT_HYD_Z,
    )

    dist_src_starboard = mult.calculate_distance(
        param.TestParameters.SOURCE_POS_X,
        param.TestParameters.SOURCE_POS_Y,
        param.TestParameters.SOURCE_POS_Z,
        param.HydrophoneDetails.STARBOARD_HYD_X,
        param.HydrophoneDetails.STARBOARD_HYD_Y,
        param.HydrophoneDetails.STARBOARD_HYD_Z,
    )

    dist_src_stern = mult.calculate_distance(
        param.TestParameters.SOURCE_POS_X,
        param.TestParameters.SOURCE_POS_Y,
        param.TestParameters.SOURCE_POS_Z,
        param.HydrophoneDetails.STERN_HYD_X,
        param.HydrophoneDetails.STERN_HYD_Y,
        param.HydrophoneDetails.STERN_HYD_Z,
    )

    """
    Calculating the number of samples it takes for the sound to arrive at x hyd
    Since the TOA uses lag (direct measuremend), these values are uint32_t
    converting to int32 works as floor. Should be rounded.
    """
    samp_port = np.int32(
        param.DSPConstants.SAMPLE_FREQUENCY
        * (dist_src_port / param.PhysicalConstants.SOUND_SPEED)
    )
    samp_starboard = np.int32(
        param.DSPConstants.SAMPLE_FREQUENCY
        * (dist_src_starboard / param.PhysicalConstants.SOUND_SPEED)
    )
    samp_stern = np.int32(
        param.DSPConstants.SAMPLE_FREQUENCY
        * (dist_src_stern / param.PhysicalConstants.SOUND_SPEED)
    )

    lag_port_starboard = samp_port - samp_starboard
    lag_port_stern = samp_port - samp_stern
    lag_starboard_stern = samp_starboard - samp_stern

    return [lag_port_starboard, lag_port_stern, lag_starboard_stern]


def test_trilateration_algorithm():
    """Testing if algorithm gives wanted results.

    Calculates the distance between the estimated position and the
    actual position to check if within given tolerance value.
    Only considering x and y, since we only have three hydrophones.
    """

    lag_array = calculate_toa_array()

    x_pos_es, y_pos_es = mult.trilaterate_pinger_position(lag_array)

    distance_diff = np.sqrt(
        (x_pos_es - param.TestParameters.SOURCE_POS_X) ** 2
        + (y_pos_es - param.TestParameters.SOURCE_POS_Y) ** 2
    )

    tolerance = 5

    assert distance_diff < tolerance
