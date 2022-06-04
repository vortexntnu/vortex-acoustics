from dataclasses import dataclass
from typing import List

@dataclass
class Position:
    x: float = 0
    y: float = 0
    z: float = 0


pool_size = Position(25, 12.5, 5)


class PositionEstimate:
    def __init__(self, m_last_elem, distance_between_pingers) -> None:
        self.mean = Position(0, 0, 0)
        self.variance = Position(0, 0, 0)
        self.measurements = [[], [], []]

        self.m_last_elements = m_last_elem
        self.distance_between_pingers = distance_between_pingers

    def integrate_new_measurment(self, measurement: list):

        if (self.within_pool_boundaries(measurement)):
            self.measurements[0].append(measurement[0])
            self.measurements[1].append(measurement[1])
            self.measurements[2].append(measurement[2])

            n = len(self.measurements[0])

            self.mean.x = sum(self.measurements[0]) / n
            self.mean.y = sum(self.measurements[1]) / n
            self.mean.z = sum(self.measurements[2]) / n

            self.variance.x = (
                sum((x - self.mean.x) ** 2 for x in self.measurements[0]) / n
            )
            self.variance.y = (
                sum((y - self.mean.y) ** 2 for y in self.measurements[1]) / n
            )
            self.variance.z = (
                sum((z - self.mean.z) ** 2 for z in self.measurements[2]) / n
            )

    def detect_change_in_pinger_pos(self, searching_for_second_pinger: bool):
        """
        Use bayesian detection to conclude if the last m measurments
        are generated from a new pinger position. 
        Assume both hyposthesis are equaly likely. 
        """

        n = len(self.measurements[0])
        m = self.m_last_elements

        if searching_for_second_pinger and n > m * 2:

            prev_mean_x = sum(self.measurements[0][: n - m]) / (n - m)
            prev_mean_y = sum(self.measurements[1][: n - m]) / (n - m)
            prev_mean_z = sum(self.measurements[2][: n - m]) / (n - m)

            current_mean_x = sum(self.measurements[0][n - m :]) / m
            current_mean_y = sum(self.measurements[1][n - m :]) / m
            current_mean_z = sum(self.measurements[2][n - m :]) / m

            if self.distance_between_pingers.x/2 < abs(prev_mean_x - current_mean_x): 
                if self.distance_between_pingers.y/2 < abs(prev_mean_y - current_mean_y):
                    if self.distance_between_pingers.z/2 < abs(prev_mean_z - current_mean_z):
                        self.remove_obsolete_measurements()
                        return True 
            
        return False

    def remove_obsolete_measurements(self): 

        n = len(self.measurements[0])
        m = self.m_last_elements

        self.measurements[0] = self.measurements[0][n - m :]
        self.measurements[1] = self.measurements[1][n - m :]
        self.measurements[2] = self.measurements[2][n - m :]


    def within_pool_boundaries(self, measurement: List) -> bool :
        valid_measurement = True
        if abs(measurement[0]) < pool_size.x:
            valid_measurement = False
        elif abs(measurement[1]) < pool_size.y:
            valid_measurement = False
        elif abs(measurement[2]) < pool_size.z:
            valid_measurement = False
        return valid_measurement

