"""
TODO: 
    - make functions for prettier look
    - detect change in pinger position
    - test
"""

from itertools import count
from numpy import NaN
from dataclasses import dataclass

#Redefining Position in order not be reliant on other modules
@dataclass #could just be a tuple
class Position:
    x: float = 0
    y: float = 0
    z: float = 0


class PositionEstimate: 
    mean = Position(0,0,0)
    variance = Position(0,0,0)
    sample_variance = Position(0,0,0)
    M2 = Position(0,0,0)
    count = 0

    def __welford_update(self, new_measurement: Position):
        self.count += 1

        #can perhaps make a function that takes in a coordinate as arg
        diff_prev_mean = new_measurement.x - self.mean.x 
        self.mean.x += diff_prev_mean / self.count
        diff_mean = new_measurement.x - self.mean.x
        self.M2.x += diff_prev_mean*diff_mean

        diff_prev_mean = new_measurement.y - self.mean.y
        self.mean.y += diff_prev_mean / self.count
        diff_mean = new_measurement.y - self.mean.y
        self.M2.y += diff_prev_mean*diff_mean

        diff_prev_mean = new_measurement.z - self.mean.z
        self.mean.z += diff_prev_mean / self.count
        diff_mean = new_measurement.z - self.mean.z
        self.M2.z += diff_prev_mean*diff_mean


    def __welford_finalize(self):
        if self.count >= 2:
            self.variance.x = self.M2.x / self.count
            self.sample_variance.x = self.M2.x / (self.count-1)

            self.variance.y = self.M2.y / self.count
            self.sample_variance.y = self.M2.y / (self.count-1)

            self.variance.z = self.M2.z / self.count
            self.sample_variance.x = self.M2.x / (self.count-1)

    def integrate_new_position(self, position : Position):
        self.__welford_update(position)
        self.__welford_finalize()


