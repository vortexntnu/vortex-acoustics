import numpy as np

import multilateration.analyze_data as andat



def test_given_equal_input_when_calculate_lags_then_all_zero():
    data_port = np.zeros(4096)
    data_port[0] = 1
    data_starboard = np.zeros(4096)
    data_starboard[0] = 1
    data_stern = np.zeros(4096)
    data_stern[0] = 1
    data_array = np.array([data_port, data_starboard, data_stern])

    ref_array = np.zeros(3)

    results_array = andat.calculate_xcorr_lag_array(data_array)
    
    assert np.array_equal(results_array, ref_array)


def test_analyze_data():
    data_port = np.zeros(4096)
    data_port[0] = 1
    data_starboard = np.zeros(4096)
    data_starboard[1] = 1
    data_stern = np.zeros(4096)
    data_stern[2] = 1
    data_array = np.array([data_port, data_starboard, data_stern])

    ref_array = np.array([-1,-2,-1])

    results_array = andat.calculate_xcorr_lag_array(data_array)
    
    assert np.array_equal(results_array, ref_array)