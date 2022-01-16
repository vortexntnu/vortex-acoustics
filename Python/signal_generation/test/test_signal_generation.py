""" Provides tests to demonstrate usage of modules inside package
"""
import matplotlib.pyplot as plt
import numpy as np
import pytest

import signal_generation.conversion as sg_conv
import signal_generation.noise as sg_noise
import signal_generation.positioning as sg_pos
import signal_generation.receiver as sg_rec
import signal_generation.source as sg_src


@pytest.mark.plot
def test_given_multiple_sources_then_multiple_frequencies():
    positions = np.array(
        [
            sg_pos.Position(),
        ]
    )

    hydro_array = sg_rec.HydrophoneArray(
        positions=positions,
    )

    pinger_position_one = sg_pos.Position(
        x=3.0,
        y=-2.2,
        z=1.2,
    )
    pinger_position_two = sg_pos.Position(
        x=3.0,
        y=-2.2,
        z=4.2,
    )
    pinger = [
        sg_src.Pinger(
            frequency=10.0,  # [kHz]
            period=30.0,
            pulse_length=15.0,
            use_window=True,
            position=pinger_position_one,
        ),
        sg_src.Pinger(
            frequency=20.0,  # [kHz]
            period=30.0,
            pulse_length=15.0,
            use_window=True,
            position=pinger_position_two,
        ),
    ]

    result = sg_rec.generate_signals(
        sources=pinger,
        receivers=hydro_array,
        output_length=2000,
        sound_speed=1500.0,
    )

    fig, axs = plt.subplots(2, 1)
    fig.tight_layout()
    axs[0].plot(result[0])
    axs[0].set_xlabel("n")
    axs[0].set_xlim([250, 500])
    axs[1].plot(abs(np.fft.fft(result[0])))
    axs[1].set_xlabel("frequency bins")

    plt.show()


@pytest.mark.plot
def test_given_single_source_when_geometrical_spreading_then_amplitude_decay():
    positions = np.array(
        [
            sg_pos.Position(),
            sg_pos.Position(x=3.0, y=0.0, z=0.0),
            sg_pos.Position(x=6.0, y=0.0, z=0.0),
            sg_pos.Position(x=9.0, y=0.0, z=0.0),
            sg_pos.Position(x=12.0, y=0.0, z=0.0),
        ]
    )

    hydro_array = sg_rec.HydrophoneArray(
        positions=positions,
    )

    pinger = [
        sg_src.Pinger(
            frequency=10.0,  # [kHz]
            period=30.0,
            pulse_length=15.0,
            use_window=True,
            position=sg_pos.Position(),
        ),
    ]

    result = sg_rec.generate_signals(
        sources=pinger,
        receivers=hydro_array,
        output_length=2000,
        sound_speed=1500.0,
        geometric_spreading=True,
    )

    plt.plot(result[0], label=f"{hydro_array.positions[0]}")
    plt.plot(result[1], label=f"{hydro_array.positions[1]}")
    plt.plot(result[2], label=f"{hydro_array.positions[2]}")
    plt.plot(result[3], label=f"{hydro_array.positions[3]}")
    plt.plot(result[4], label=f"{hydro_array.positions[4]}")
    plt.xlabel("n")
    plt.legend()

    plt.show()


@pytest.mark.plot
def test_given_multiple_receivers_and_noise_when_conversion_then_difference_in_arrivals():
    positions = np.array(
        [
            sg_pos.Position(),
            sg_pos.Position(x=2.0, y=0.0, z=0.0),
            sg_pos.Position(x=1.5, y=0.0, z=0.5),
            sg_pos.Position(x=0.0, y=0.6, z=0.0),
        ]
    )

    hydro_array = sg_rec.HydrophoneArray(
        positions=positions,
    )

    pinger = [
        sg_src.Pinger(
            frequency=10.0,  # [kHz]
            period=30.0,
            pulse_length=15.0,
            use_window=True,
            position=sg_pos.Position(x=10.0, y=5.0, z=8.0),
        ),
    ]

    output_length = 2000

    result = 0.7 * sg_rec.generate_signals(
        sources=pinger,
        receivers=hydro_array,
        output_length=output_length,
        sound_speed=1500.0,
    )

    variance = 0.01
    result[0] += sg_noise.generate_gaussian_noise(
        length=output_length,
        variance=variance,
    )
    result[1] += sg_noise.generate_gaussian_noise(
        length=output_length,
        variance=variance,
    )
    result[2] += sg_noise.generate_gaussian_noise(
        length=output_length,
        variance=variance,
    )
    result[3] += sg_noise.generate_gaussian_noise(
        length=output_length,
        variance=variance,
    )

    result = sg_conv.convert_to_integer_type(
        resulting_type=np.uint16,
        input_signal=result,
    )

    fig, axs = plt.subplots(5, 1)
    fig.tight_layout()
    for index, res in enumerate(result):
        axs[index].plot(res)
        axs[index].set_xlabel("n")
        axs[index].set_xlim([600, 1200])
        axs[4].plot(res, "--", label=f"{hydro_array.positions[index]}")
    axs[4].set_xlim([600, 1200])
    axs[4].legend()

    plt.show()
