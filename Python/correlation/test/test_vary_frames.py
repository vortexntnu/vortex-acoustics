"""
Provides the possibility to change frames.
Plot the signals together with the correlated signals, and find the accuracy of the result.
"""
import matplotlib.pyplot as plt
import numpy as np
import pytest

import correlation.correlation as correl
import correlation.tdoa as tdoa

import signal_generation.positioning as sg_pos
import signal_generation.receiver as sg_rec
import signal_generation.source as sg_src


@pytest.mark.plot
def test_plot_of_signals_and_correlation_signal():
    # Set variables:
    sample_frequency = 100.0  # [kHz]
    pulse_length = 1.0  # [ms]
    signal_length = 400  # [samples]
    frame = [0, 400]  # [start of signal, end of signal]

    positions = np.array(
        [
            sg_pos.Position(x=0.0, y=0.0, z=0.0),
            sg_pos.Position(x=1.83, y=0.0, z=0.0),
            sg_pos.Position(x=0.0, y=0.91, z=0.91),
        ]
    )

    hydro_array = sg_rec.HydrophoneArray(
        positions=positions,
    )

    pinger_position = sg_pos.Position(
        x=4.0,
        y=0.0,
        z=0.0,
    )

    pinger = sg_src.Pinger(
        frequency=25.0,  # [kHz]
        sampling_frequency=sample_frequency,  # [kHz]
        period=30.0,
        pulse_length=pulse_length,
        use_window=True,
        position=pinger_position,
    )

    result = sg_rec.generate_signals(
        sources=np.array([pinger]),
        receivers=hydro_array,
        output_length=signal_length,
        sound_speed=1500.0,
    )
    cut_result = correl.cut_signals(frame, result)

    correlation_matrix, lag_matrix = correl.calculate_correlation_matrix(cut_result)

    #  Not necessary for test, but nice for debugging.
    tdoa_array = tdoa.calculate_tdoa_array(correlation_matrix, lag_matrix)
    differance_tdoa_array = tdoa.find_diff_between_tdoa_array(
        hydro_array=hydro_array,
        tdoa_array=tdoa_array,
        pinger_position=pinger.position,
        sampling_frequency=pinger.sampling_frequency,
    )

    fig, axs = plt.subplots(len(hydro_array), 1)
    fig.tight_layout()

    for index, result in enumerate(cut_result):
        axs[0].plot(result, label=f"Hydrophone {index}")
        axs[0].legend(loc='upper left')

    for index in range(len(correlation_matrix)):
        axs[index + 1].plot(lag_matrix[index], correlation_matrix[index], label=f"Hydrophone 0 and {index + 1}")
        axs[index + 1].legend(loc='upper left')

    plt.show()
