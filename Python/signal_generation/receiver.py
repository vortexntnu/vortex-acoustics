import numpy as np
from dataclasses import dataclass

import signal_generation.conversion as sg_conv
import signal_generation.positioning as sg_pos
import signal_generation.source as sg_src


@dataclass
class HydrophoneArray:
    """Simple container class to hold receiver position."""

    positions: np.array

    def __len__(self):
        return len(self.positions)


def calculate_transmission_loss_factors(
    distances: np.array,
    geometric_spreading: bool = False,
):
    """Generates an array of transmission loss factors due to geometrical spreading.

    Args:
        distances: An (n,m) np.array that contains the distance from nth source to mth
            receiver.
        geometric_spreading: If True this will apply loss from spherical spreading corresponding
            to 1/r. Because this is only valid for far field for r<1 it will just give a factor
            of 1. If False all factors will be one.

    Returns:
        A (n,m) np.array that contains the transmission loss factors for the propagation of
        the signal from the nth source to the mth receiver.
    """
    distances_shape = np.shape(distances)
    transmission_loss_factors = np.ones(distances_shape)

    if geometric_spreading:
        for src_index in np.arange(distances_shape[0]):
            for rec_index in np.arange(distances_shape[1]):
                distance = distances[src_index, rec_index]
                if distance > 1:
                    transmission_loss_factors[src_index, rec_index] = 1 / distance

    return transmission_loss_factors


def generate_signals(
    receivers: HydrophoneArray,
    sources: np.array,
    output_length: int,
    sound_speed: float = 1500.0,
    geometric_spreading: bool = False,
) -> np.array:
    """Generate signals received by the hydrophones placed in the array.

    Args:
        receivers: HydrophoneArray containing the positions thereof
        output_length: The length of the expected output signal in number of samples.
        sources: Provide an iterable of initialized Pinger signal source.
        sound_speed: The expected sound speed in the medium in meter per seconds.
        geometric_spreading: If true uses 1/r loss from spherical spreading on the
            amplitudes.

    Returns:
        A numpy array containing the received signal at the hydrophones placed in the
        receiver array.
    """
    distances = sg_pos.calculate_distances(
        source_positions=np.array([source.position for source in sources]),
        receiver_positions=receivers.positions,
    )

    transmission_loss_factors = calculate_transmission_loss_factors(
        distances=distances,
        geometric_spreading=geometric_spreading,
    )

    signals = np.zeros((len(receivers), output_length))
    for rec_index in np.arange(len(receivers)):
        for src_index in np.arange(len(sources)):
            distance = distances[src_index, rec_index]
            factor = transmission_loss_factors[src_index, rec_index]
            signal = sources[src_index].generate_signal(
                offset=(distance / sound_speed) * 1000,  # [ms]
                length=output_length,
            )

            signals[rec_index] += signal * factor

    return signals
