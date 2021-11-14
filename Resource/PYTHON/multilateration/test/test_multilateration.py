import numpy as np
#from Resource.PYTHON.multilateration.multilateration import check_valid_signals

import multilateration.multilateration as mult
import multilateration.parameters as param



def calculate_tdoa_array():
    """Generates TOA of the sound-signals for the different hydrophones
    and calculates the lag between them.
    """
    distance_array = []
    N = param.HydrophoneDetails.NUM_HYDROPHONES
    for j in range(N):
        distance_array.append(
            mult.calculate_distance(
                x1 = param.TestParameters.SOURCE_POS_X,
                y1 = param.TestParameters.SOURCE_POS_Y,
                z1 = param.TestParameters.SOURCE_POS_Z,
                x2=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[j][0],
                y2=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[j][1],
                z2=param.HydrophoneDetails.HYDROPHONE_POSITIONING_MATRIX[j][2],
            )
        )
    distance_array = np.array(distance_array)
    toa_sample_array = np.int32(distance_array * param.DSPConstants.SAMPLE_FREQUENCY / param.PhysicalConstants.SOUND_SPEED)



    """
    Calculating the number of samples it takes for the sound to arrive at x hyd
    Since the TOA uses lag (direct measuremend), these values are uint32_t
    converting to int32 works as floor. Should be rounded.
    """
    tdoa_sample_array = []
    for i in range (N-1):
        tdoa_sample_array.append(toa_sample_array[0] - toa_sample_array[i+1])
 
    return tdoa_sample_array


def test_trilateration_algorithm():
    """Testing if algorithm gives wanted results.

    Calculates the distance between the estimated position and the
    actual position to check if within given tolerance value.
    Only considering x and y, since we only have three hydrophones.
    """

    tdoa_sample_array = calculate_tdoa_array()
    mult.initialize_trilateration_globals()
    #assert check_valid_signals(tdoa_sample_array) == True

    x_pos_es, y_pos_es, z_pos_es = mult.trilaterate_pinger_position(tdoa_sample_array)

    distance_diff = np.sqrt(
        (x_pos_es - param.TestParameters.SOURCE_POS_X) ** 2
        + (y_pos_es - param.TestParameters.SOURCE_POS_Y) ** 2
        + (z_pos_es - param.TestParameters.SOURCE_POS_Z) ** 2
    )

    tolerance = 5

    assert distance_diff < tolerance