import pulse_detection.pulse_determination as pd_pdeter

import matplotlib.pyplot as plt
import numpy as np

from signal_generation import positioning as sg_pos
from signal_generation import receiver as sg_rec
from signal_generation import source as sg_src
from signal_generation import noise as sg_noise

CARRIER_FREQUENCY = 25  # [kHz]
SAMPLING_FREQUENCY = 512  # [kHz]

def generate_noisy_pulses(
    source_position: sg_pos.Position = sg_pos.Position(10, 10, 10),
    length: int = 25000,
    carrier_frequency: int = CARRIER_FREQUENCY,  # [kHz]
    sampling_frequency: int = SAMPLING_FREQUENCY,  # [kHz]
    noise_variance: float = None,
):
    positions = np.array(
        [
            source_position,
        ]
    )

    hydro_array = sg_rec.HydrophoneArray(
        positions=positions,
    )

    pinger = [
        sg_src.Pinger(
            frequency=carrier_frequency,  # [kHz]
            period=500.0,
            pulse_length=4.0,
            use_window=True,
            position=sg_pos.Position(),
            sampling_frequency=sampling_frequency,  # [kHz]
        ),
    ]

    result = sg_rec.generate_signals(
        sources=pinger,
        receivers=hydro_array,
        output_length=length,
        sound_speed=1500.0,
        geometric_spreading=False,
    )[0]

    if noise_variance is not None:
        result += sg_noise.generate_gaussian_noise(
            length=length,
            variance=noise_variance,
            maximum_allowed_value=None,
        )

    return result

def generate_cosine_wave(
    pulse_length: int,
    carrier_frequency: float, 
    sampling_frequency: float
):

    dt = 1 / sampling_frequency
    secondary_frequency = 62
    time = np.arange(0, pulse_length, dt)
    signal = np.cos(time * np.pi * 2 * carrier_frequency ) + 0.1*np.cos(time * np.pi * 2 * secondary_frequency )

    return signal
 


def test_determine_signal_frequncy():
    sample_frequency = SAMPLING_FREQUENCY
    signal_frequency_incremetns = 5
    pulse_length = 4

    signal = generate_cosine_wave(
        pulse_length, 
        CARRIER_FREQUENCY, 
        SAMPLING_FREQUENCY
    )

    fft_size = pd_pdeter.compute_fft_size(
        sample_frequency, 
        signal_frequency_incremetns, 
        pulse_length
    )

    carrier_frequnecy, frequency_bins= pd_pdeter.determine_signal_frequency( #, frequency_bins
        signal, 
        sample_frequency,
        fft_size,
    )

    #add assert

    print("\nDETERMINE SIGNAL FREQUENCY\n")
    print("\nThe fft size is: ", fft_size)
    print("\nThe computed carrier frequnecy is: ", carrier_frequnecy)
    print("\nThe frequency bins are: \n ", frequency_bins)

def test_find_optimal_sampling_frequency():

    pulse_length = 4
    fft_size = 128
    sample_frequency =  pd_pdeter.find_optimal_sampling_frequency(fft_size)

    signal = generate_cosine_wave(
    pulse_length, 
    CARRIER_FREQUENCY, 
    SAMPLING_FREQUENCY
    )

    carrier_frequnecy, frequency_bins= pd_pdeter.determine_signal_frequency( #, frequency_bins
        signal, 
        sample_frequency,
        fft_size,
    )

    #add assert

    print("\nFIND OPTIMAL SAMPLING FREQUENCY\n")
    print("\nThe sampling frequency is: ", sample_frequency)
    print("\nThe computed carrier frequnecy is: ", carrier_frequnecy)
    print("\nThe frequency bins are: \n ", frequency_bins)
