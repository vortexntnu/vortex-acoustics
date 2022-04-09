import matplotlib.pyplot as plt
import numpy as np
import pulse_detection.pulse_determination as pd_pdeter
import pytest
from signal_generation import noise as sg_noise
from signal_generation import positioning as sg_pos
from signal_generation import receiver as sg_rec
from signal_generation import source as sg_src

CARRIER_FREQUENCY = 25  # [kHz]
SAMPLING_FREQUENCY = 427  # [kHz]
FFT_SIZE = 256


def test_short_time_fourier_transform():
    pulse_length = 4
    noise_amplitude = 0.0
    fft_size = FFT_SIZE

    signal = generate_cosine_wave(
        pulse_length, CARRIER_FREQUENCY, SAMPLING_FREQUENCY, noise_amplitude
    )

    fft = pd_pdeter.short_time_fourier_transform(
        signal, fft_size, SAMPLING_FREQUENCY, pulse_length
    )
    tone = pd_pdeter.find_tone(fft, SAMPLING_FREQUENCY, fft_size)

    print("\nTEST STFT WITH WINDOWING\n")
    print("\nThe carrier freq is: ", tone)
    print("\nArgmax: ", np.argmax(fft))
    print("The fft is: \n", fft)
    print("\nThe fft size is: ", np.size(fft))

    tolerance = SAMPLING_FREQUENCY / fft_size

    assert abs(tone - CARRIER_FREQUENCY) < tolerance


def test_bratlett_window(plt):
    fft_size = FFT_SIZE
    pulse_length = 4
    noise_amplitude = 0.2

    original_signal = generate_cosine_wave(
        pulse_length, CARRIER_FREQUENCY, SAMPLING_FREQUENCY, noise_amplitude
    )

    original_signal = pd_pdeter.adjust_signal_length(original_signal, fft_size)

    signal = pd_pdeter.apply_hamming_window(original_signal)

    print("\nTEST bratlett WINDOWING\n")
    # print("\nOriginal signal: ", original_signal)
    # print("\nSignal: ", signal)

    assert np.size(signal) == np.size(original_signal)


def test_determine_signal_frequncy():

    pulse_length = 4
    noise_amplitude = 0.1
    fft_size = FFT_SIZE

    signal = generate_cosine_wave(
        pulse_length, CARRIER_FREQUENCY, SAMPLING_FREQUENCY, noise_amplitude
    )

    (
        computed_carrier_frequnecy,
        frequency_bins,
        fft,
    ) = pd_pdeter.determine_signal_frequency(
        signal,
        SAMPLING_FREQUENCY,
        fft_size,
    )
    computed_carrier_frequnecy_v2 = pd_pdeter.find_tone(
        fft, SAMPLING_FREQUENCY, fft_size
    )

    tolerance = 2 * SAMPLING_FREQUENCY / fft_size

    assert abs(computed_carrier_frequnecy - CARRIER_FREQUENCY) < tolerance

    assert abs(computed_carrier_frequnecy_v2 - CARRIER_FREQUENCY) < tolerance

    print("\nTEST FFT WITHOUT WINDOWING\n")
    print("\nThe fft size is: ", fft_size)
    print("\nThe computed carrier frequnecy is: ", computed_carrier_frequnecy)
    print("\nThe frequency bins are: \n ", frequency_bins)


def test_find_optimal_sampling_frequency():

    fft_size = FFT_SIZE
    sample_frequency, frequency_bins = pd_pdeter.find_optimal_sampling_frequency(
        fft_size
    )

    print("\nFIND OPTIMAL SAMPLING FREQUENCY\n")
    print("\nThe optimal sampling frequency is: ", sample_frequency)
    # print("\nThe frequency bins are: \n ", frequency_bins)


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
    sampling_frequency: float,
    noise_amplitude: float,
) -> np.array:

    dt = 1 / sampling_frequency
    secondary_frequency = 62
    time = np.arange(0, pulse_length, dt)
    signal = np.cos(time * np.pi * 2 * carrier_frequency) + noise_amplitude * np.cos(
        time * np.pi * 2 * secondary_frequency
    )

    return signal
