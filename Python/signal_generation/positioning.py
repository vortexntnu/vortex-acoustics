from dataclasses import dataclass

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