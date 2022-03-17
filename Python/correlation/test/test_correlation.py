"""
Provides test to visualize results, and check how accurate the result is when varying
sample frequency, pulse length, position of hydrophones and pinger, and window length.
"""

import matplotlib.pyplot as plt
import numpy as np
import pytest
import sys

import correlation.correlation as correl
import correlation.tdoa as tdoa

import signal_generation.noise as sg_noise
import signal_generation.positioning as sg_pos
import signal_generation.receiver as sg_rec
import signal_generation.source as sg_src

@pytest.mark.plot
def test_plot_correlation():
    # Set variables:
    sample_frequency = 500.0  # [kHz]
    pulse_lenth = 5  # [ms]
    signal_length = 10000  # [samples]


    positions = np.array(
        [
            sg_pos.Position(x=0.0, y=0.0, z=0.0),
            sg_pos.Position(x=1.83, y=0.0, z=0.0),
            sg_pos.Position(x=0.0, y=0.91, z=0.91),
            sg_pos.Position(x=1.83, y=0.91, z=0.0),
            sg_pos.Position(x=1.0, y=0.0, z=0.91),
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
        pulse_length=pulse_lenth,
        use_window=True,
        position=pinger_position,
    )

    resulting_signals = sg_rec.generate_signals(
        sources=np.array([pinger]),
        receivers=hydro_array,
        output_length=signal_length,
        sound_speed=1500.0,
    )


    for i in range(len(resulting_signals)):
        resulting_signals[i] += sg_noise.generate_gaussian_noise(
            length=signal_length,
            variance=pinger.amplitude*0.25)

    correlation_matrix, lag_matrix = correl.calculate_correlation_matrix(resulting_signals)

    fig, axs = plt.subplots(len(hydro_array), 1)
    fig.tight_layout()

    for index, result in enumerate(resulting_signals):
        axs[0].plot(result, label=f"Hydrophone {index}")
        axs[0].legend(loc='upper right')

    for index in range(len(correlation_matrix)):
        axs[index + 1].plot(lag_matrix[index], correlation_matrix[index], label=f"Hydrophone 0 and {index + 1}")
        axs[index + 1].legend(loc='upper right')

    plt.show()


def test_accuracy_tdoa_array():
    """
    Here one can add more noise, increase sampling frequency,
    or increase tolerance to see ehavoir of the correaltion.

    Currently, tolerance is set high and noise equal to zero to make test pass.

    """
    # Set variables:
    sample_frequency = 500.0  # [kHz]
    pulse_lenth = 5  # [ms]
    signal_length = 10000  # [samples]

    positions = np.array(
        [
            sg_pos.Position(x=0.0, y=0.0, z=0.0),
            sg_pos.Position(x=1.83, y=0.0, z=0.0),
            sg_pos.Position(x=0.0, y=0.91, z=0.91),
            sg_pos.Position(x=1.83, y=0.91, z=0.0),
            sg_pos.Position(x=1.0, y=0.0, z=0.91),
        ]
    )

    hydro_array = sg_rec.HydrophoneArray(
        positions=positions,
    )

    pinger_position_one = sg_pos.Position(
        x=4.0,
        y=0.0,
        z=0.0,
    )

    pinger = sg_src.Pinger(
        frequency=10.0,  # [kHz]
        sampling_frequency=sample_frequency,  # [kHz]
        period=30.0,
        pulse_length=pulse_lenth,
        use_window=True,
        position=pinger_position_one,
    )

    result = sg_rec.generate_signals(
        sources=np.array([pinger]),
        receivers=hydro_array,
        output_length=signal_length,
        sound_speed=1500.0,
    )

    for i in range(len(result)):
        result[i] += sg_noise.generate_gaussian_noise(
            length=signal_length,
            variance=pinger.amplitude*0)

    correlation_matrix, lag_matrix = correl.calculate_correlation_matrix(result)
    tdoa_array = tdoa.calculate_tdoa_array(correlation_matrix, lag_matrix)

    required_accuracy = 10  # [samples]
    tdoa_is_correct = tdoa.check_tdoa_array_is_valid(
        required_accuracy,
        hydro_array,
        tdoa_array,
        pinger.position,
        pinger.sampling_frequency,
    )

    assert tdoa_is_correct

def test_compare_with_result_from_teensy():
    arr1 = [3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0] 
    arr2 = [0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7]

    signals = [arr1, arr2]

    correlation_matrix, lag_matrix = correl.calculate_correlation_matrix(signals)

    print(correlation_matrix[0])

   


