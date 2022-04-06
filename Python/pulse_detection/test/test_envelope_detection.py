import matplotlib.pyplot as plt
import numpy as np
import pytest
import scipy.signal
from pulse_detection.envelope_detection import (
    asynchronous_full_wave,
    asynchronous_hilbert_complex,
)
from signal_generation import noise as sg_noise
from signal_generation import positioning as sg_pos
from signal_generation import receiver as sg_rec
from signal_generation import source as sg_src

CARRIER_FREQUENCY = 25  # [kHz]
SAMPLING_FREQUENCY = 500  # [kHz]


def generate_noisy_pulses(
    source_position: sg_pos.Position = sg_pos.Position(10, 10, 10),
    length: int = 25000,
    carrier_frequency: int = 25,  # [kHz]
    sampling_frequency: int = 100,  # [kHz]
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


@pytest.mark.plot
def test_given_noisy_signal_when_asynchronous_full_wave():
    signal = generate_noisy_pulses(
        length=100000,
        noise_variance=0.1,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    result = asynchronous_full_wave(
        signal=signal,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    fig, ax = plt.subplots(2, 1)
    fig.suptitle("asynchronous full wave")
    ax[0].plot(signal, label="signal")
    ax[1].plot(result, label="envelop")

    plt.show()


@pytest.mark.plot
@pytest.mark.parametrize(
    "filter_length, filter_order",
    [
        (251, 2),
        (51, 5),
    ],
)
def test_given_noisy_signal_when_asynchronous_full_wave_and_savgol_filtered(
    filter_length,
    filter_order,
):
    """
    Result is additionally filtered with savitzky golay filter to remove
    some of the noise.
    Larger filter length with small order favors envelope filtering (filtering post envelope detection)
    """
    signal = generate_noisy_pulses(
        length=100000,
        noise_variance=0.1,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    filtered = scipy.signal.savgol_filter(
        x=signal,
        window_length=filter_length,
        polyorder=filter_order,
    )

    result = asynchronous_full_wave(
        signal=signal,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    result_pre_filtered = asynchronous_full_wave(
        signal=filtered,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    result_post_filtered = scipy.signal.savgol_filter(
        x=result,
        window_length=filter_length,
        polyorder=filter_order,
    )

    fig, ax = plt.subplots(2, 1)
    fig.suptitle(
        f"asynchronous full wave - savitzky golay len:{filter_length} order:{filter_order}"
    )
    ax[0].set_title("signal")
    ax[0].plot(signal, label="noisy")
    ax[0].plot(filtered, label="filtered")
    ax[0].legend()
    ax[0].set_title("envelope")
    ax[1].plot(result, label="noisy")
    ax[1].plot(result_pre_filtered, label="pre-filtered")
    ax[1].plot(result_post_filtered, label="post-filtered")
    ax[1].legend()

    plt.tight_layout()
    plt.show()


@pytest.mark.plot
@pytest.mark.parametrize(
    "filter_length",
    [
        (51),
        (251),
    ],
)
def test_given_noisy_signal_when_asynchronous_full_wave_and_wiener_filtered(
    filter_length,
):
    """
    Result is additionally filtered with wiener filter to remove
    some of the noise.
    """
    signal = generate_noisy_pulses(
        length=100000,
        noise_variance=0.1,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    filtered = scipy.signal.wiener(
        im=signal,
        mysize=filter_length,
    )

    result = asynchronous_full_wave(
        signal=signal,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    result_pre_filtered = asynchronous_full_wave(
        signal=filtered,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    result_post_filtered = scipy.signal.wiener(
        im=result,
        mysize=filter_length,
    )

    fig, ax = plt.subplots(2, 1)
    fig.suptitle(f"asynchronous full wave filtered with wiener len:{filter_length}")
    ax[0].set_title("signal")
    ax[0].plot(signal, label="noisy")
    ax[0].plot(filtered, label="filtered")
    ax[0].legend()
    ax[1].set_title("envelope")
    ax[1].plot(result, label="noisy")
    ax[1].plot(result_pre_filtered, label="pre-filtered")
    ax[1].plot(result_post_filtered, label="post-filtered")
    ax[1].legend()

    plt.tight_layout()
    plt.show()


@pytest.mark.plot
@pytest.mark.parametrize(
    "noise_variance",
    [
        (None),
        (0.01),
        (0.1),
    ],
)
def test_given_noisy_signal_when_asynchronous_hilbert_complex(
    noise_variance,
):
    signal = generate_noisy_pulses(
        length=100000,
        noise_variance=noise_variance,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    result = asynchronous_hilbert_complex(
        signal=signal,
    )

    fig, ax = plt.subplots(2, 1)
    fig.suptitle("asynchronous hilbert complex")
    ax[0].set_title(f"signal with noise variance = {noise_variance}")
    ax[0].plot(signal, label="signal")

    ax[1].set_title(f"envelope")
    ax[1].plot(result, label="envelope")

    plt.tight_layout()
    plt.show()


@pytest.mark.plot
@pytest.mark.parametrize(
    "filter_length",
    [
        (51),
        (251),
    ],
)
def test_given_noisy_signal_when_asynchronous_hilbert_complex_and_wiener_pre_filtered(
    filter_length,
):
    signal = generate_noisy_pulses(
        length=100000,
        noise_variance=0.1,
        carrier_frequency=CARRIER_FREQUENCY,
        sampling_frequency=SAMPLING_FREQUENCY,
    )

    filtered = scipy.signal.wiener(
        im=signal,
        mysize=filter_length,
    )

    result = asynchronous_hilbert_complex(
        signal=filtered,
    )

    fig, ax = plt.subplots(2, 1)
    fig.suptitle(
        f"asynchronous hilbert complex filtered with wiener len:{filter_length}"
    )
    ax[0].set_title("signal")
    ax[0].plot(signal, label="signal")
    ax[1].set_title("envelope")
    ax[1].plot(result, label="envelop")

    plt.tight_layout()
    plt.show()
