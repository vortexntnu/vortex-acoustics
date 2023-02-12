from dataclasses import dataclass
from typing import Iterable

import numpy as np


@dataclass
class Position:
    x: float = 0
    y: float = 0
    z: float = 0

    tol: float = 1 * 10 ** (-9)

    def __add__(
        self,
        other,
    ):
        return Position(
            x=self.x + other.x,
            y=self.y + other.y,
            z=self.z + other.z,
        )

    def __sub__(
        self,
        other,
    ):
        return Position(
            x=self.x - other.x,
            y=self.y - other.y,
            z=self.z - other.z,
        )

    def __abs__(
        self,
    ):
        return np.sqrt(self.x ** 2 + self.y ** 2 + self.z ** 2)

    def __eq__(
        self,
        other,
    ):
        difference = self - other
        return (
            (abs(difference.x) < self.tol)
            and (abs(difference.y) < self.tol)
            and (abs(difference.z) < self.tol)
        )


def find_maximum_distance(
    positions: np.array,
):
    """Finds maximum distance between all positions given in an array.

    Args:
        positions: Array of positions to determine maximum distance from.

    Returns:
        The maximum distance between all the given positions.
    """
    distances = abs(
        np.array(len(positions) * [positions])
        - np.array(len(positions) * [positions]).T
    )

    return np.max(distances)


def calculate_distances(
    source_positions: np.array,
    receiver_positions: np.array,
    sound_speed: float = 1500,  # [m/s]
) -> Iterable[float]:
    """Calculate the distances from the sources to the hydrophones.

    Args:
        source_positions: Provide the positions for the source.
        receiver_positions: Provide the positions of the receivers

    Returns:
        A 2D numpy.array containing the distances in meters [m].
        Where the first index is used for sources and the second for receivers,
        so distances[0][2] would give the distance from the zeroth source
        to the 2nd receiver.
    """
    distances = np.empty((len(source_positions), len(receiver_positions)))

    for index, source_position in enumerate(source_positions):
        distances[index, :] = abs(receiver_positions - source_position)

    return distances
