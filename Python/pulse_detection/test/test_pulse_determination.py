import numpy as np
import pulse_detection.pulse_determination as pd_pdeter

CARRIER_FREQUENCY = 25  # [kHz]
SAMPLING_FREQUENCY = 427  # [kHz]
FFT_SIZE = 256


def test_short_time_fourier_transform():
    pulse_length = 4
    noise_amplitude = 0.9
    noise_variance = 5
    fft_size = FFT_SIZE

    signal = generate_cosine_wave(
        pulse_length,
        CARRIER_FREQUENCY,
        SAMPLING_FREQUENCY,
        noise_amplitude,
        noise_variance,
    )

    fft = pd_pdeter.short_time_fourier_transform(
        signal, fft_size, SAMPLING_FREQUENCY, pulse_length
    )
    tone = pd_pdeter.find_tone(fft, SAMPLING_FREQUENCY, fft_size)

    """
    print("\nTEST STFT WITH WINDOWING\n")
    print("\nThe carrier freq is: ", tone)
    print("\nArgmax: ", np.argmax(fft))
    print("The fft is: \n", fft)
    print("\nThe fft size is: ", np.size(fft))
    """

    tolerance = SAMPLING_FREQUENCY / fft_size

    assert abs(tone - CARRIER_FREQUENCY) < tolerance
    return tone


def test_find_optimal_sampling_frequency():

    fft_size = FFT_SIZE
    sample_frequency, frequency_bins = pd_pdeter.find_optimal_sampling_frequency(
        fft_size
    )

    """
    print("\nFIND OPTIMAL SAMPLING FREQUENCY\n")
    print("\nThe optimal sampling frequency is: ", sample_frequency)
    print("\nThe frequency bins are: \n ", frequency_bins)
    """


def generate_cosine_wave(
    pulse_length: int,
    carrier_frequency: float,
    sampling_frequency: float,
    amplitude_of_secondary_freq: float,
    noise_variance: float,
) -> np.array:

    dt = 1 / sampling_frequency
    secondary_frequency = 61.7158
    time = np.arange(0, pulse_length, dt)

    signal = np.cos(time * np.pi * 2 * carrier_frequency)
    secondary_signal = amplitude_of_secondary_freq * np.cos(
        time * np.pi * 2 * secondary_frequency
    )
    noise = amplitude_of_secondary_freq * np.random.normal(
        0, np.sqrt(noise_variance), pulse_length * SAMPLING_FREQUENCY
    )

    return signal + secondary_signal + noise
