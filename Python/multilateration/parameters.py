"""Contains parameters and constants used for the multilateration algorithm.
"""

from dataclasses import dataclass

import numpy as np


@dataclass
class HydrophoneDetails:
    """Parameters that indicate the setup of the hydrophones.

    Datasheet for currently used hydrophones:
    https://www.benthowave.com/products/Specs/BII-7014FGPGDatasheet.pdf

    """

    HYDROPHONE_POSITIONING_MATRIX: np.ndarray = np.array(
        [
            [-0.11, 0.31, 0.1],
            [0.11, 0.31, 0.1],
            [0.0, -0.24, 0.0],
            [0.5, -0.1, 0.4],
            [0.4, 0.0, -0.4],
        ]
    )
    NUM_HYDROPHONES: int = len(HYDROPHONE_POSITIONING_MATRIX)


@dataclass
class TestParameters:
    SOURCE_POS_X: float = 10.0
    SOURCE_POS_Y: float = 2.0
    SOURCE_POS_Z: float = 1.0


@dataclass
class PhysicalConstants:
    SOUND_SPEED: float = 1480.0
