import correlation.kronecker as kr
import signal_generation.noise as sg_noise
import signal_generation.positioning as sg_pos
import signal_generation.receiver as sg_rec
import signal_generation.source as sg_src
from multilateration import parameters

import numpy as np

def test_with_simple_array():
    arr1 = [3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7,0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 2.0, 3.0, 2.0, 3.0, 8.9, 2.0, 6.7, 6.7, 2.0, 3.0, 2.0, 3.0]
    arr2 = [0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7,0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 2.0, 3.0, 2.0, 3.0, 8.9, 2.0, 6.7, 6.7, 2.0, 3.0]

    sdoa_estimation = kr.Kronecker(arr1, arr2)
    sdoa_estimation.iteration()
    sdoa = sdoa_estimation.final_computation()

    assert abs(sdoa) == 2

def test_with_signal_generation():
    sample_frequency = 420
    signals = generate_signal(sample_frequency)

    sdoa_estimation = kr.Kronecker(signals[0], signals[1])
    sdoa_estimation.iteration()
    sdoa = sdoa_estimation.final_computation()

    offset1 = round(1.83* sample_frequency / parameters.PhysicalConstants.SOUND_SPEED)

    threshold = 2

    assert abs(sdoa - offset1) < threshold

def generate_signal(sample_frequency):
    # Set variables:
    pulse_lenth = 4  # [ms]
    pinger_x_pos = 2.0
    signal_length = round(pinger_x_pos * sample_frequency*10e3 / parameters.PhysicalConstants.SOUND_SPEED) + 100# [samples]

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
        x=pinger_x_pos,
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
            length=signal_length, variance=pinger.amplitude * 0.01
        )

    return resulting_signals


    




