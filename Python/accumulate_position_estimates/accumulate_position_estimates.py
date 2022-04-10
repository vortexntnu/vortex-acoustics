"""
TODO: 
    - store measurments
        - np array with fixed size equal worst case
        - use python list 
    - Detect change in pinger position
    - Test
        - mean corresponds to initial position
        - var decreases with many itterations
        - sampling frequency equals 320, 427
        - decrease tolerance

"""

from dataclasses import dataclass
import numpy as np


@dataclass 
class Position:
    x: float = 0
    y: float = 0
    z: float = 0


class PositionEstimate: 
    mean = Position(0,0,0)
    variance = Position(0,0,0)
    measurements = [[],[],[]]


    def integrate_new_measurement(self, measurment: list):
        for i in range (3):
            self.measurements[i].append(measurment[i])
 

        n = len(self.measurements[0])

        self.mean.x = sum(self.measurements[0])/n
        self.mean.y = sum(self.measurements[1])/n
        self.mean.z = sum(self.measurements[2])/n

        self.variance.x = sum((x-self.mean.x)**2 for x in self.measurements[0]) / n
        self.variance.y = sum((y-self.mean.y)**2 for y in self.measurements[1]) / n
        self.variance.z = sum((z-self.mean.z)**2 for z in self.measurements[2]) / n

        



