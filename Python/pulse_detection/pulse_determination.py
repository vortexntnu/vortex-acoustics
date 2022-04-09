import numpy as np
from scipy.fft import fft


def short_time_fourier_transform(
    signal: np.array,
    fft_size: int,
    sampling_frequency: float,  # [kHz]
    pulse_length: int,  # [ms]
) -> np.array:

    MN_point_fft = np.zeros(fft_size // 2 + 1, dtype=np.complex)
    M = (pulse_length * sampling_frequency) // fft_size
    for i in range(M):
        N_point_fft = np.fft.rfft(signal[i * fft_size : (i + 1) * fft_size])

        for index in range(np.size(N_point_fft)):
            MN_point_fft[index] += N_point_fft[index]

    return MN_point_fft


def find_tone(
    fft: np.array,
    sampling_frequency: float,
    fft_size: int,
) -> float:
    frequency_bins = np.fft.rfftfreq(fft_size, 1 / sampling_frequency)
    tone = frequency_bins[np.argmax(fft)]

    return tone


def find_optimal_sampling_frequency(fft_size: int):
    pinger_frequencies = {25.0, 30.0, 35.0, 40.0}
    min_cost = 5000000
    optimal_sampling_freq = -1
    for sampling_freq in range(350, 500):
        cost = 0
        frequency_bins = np.fft.rfftfreq(fft_size, 1 / sampling_freq)
        for pinger_freq in pinger_frequencies:
            cost += np.min(np.abs(frequency_bins - pinger_freq)) ** 2
        if cost < min_cost:
            min_cost = cost
            optimal_sampling_freq = sampling_freq

    return optimal_sampling_freq, np.fft.rfftfreq(fft_size, 1 / optimal_sampling_freq)
