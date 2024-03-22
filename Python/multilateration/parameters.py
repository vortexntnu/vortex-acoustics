"""Contains parameters and constants used for the multilateration algorithm.
"""

from dataclasses import dataclass


@dataclass
class PhysicalConstants:
    SOUND_SPEED: float = 1480.0
