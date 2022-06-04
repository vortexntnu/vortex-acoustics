from dataclasses import dataclass
from typing import List


@dataclass
class Position:
    x: float = 0
    y: float = 0
    z: float = 0


pool_size = Position(25, 12.5, 5)


class PositionEstimate:
    def __init__(self, m_last_elem, distance_threshold) -> None:
        self.mean = Position(0, 0, 0)
        self.variance = Position(0, 0, 0)
        self.measurments = [[], [], []]

        self.m_last_elements = m_last_elem
        self.distance_threshold = distance_threshold

    def integrate_new_measurment(self, measurment: list):

        if (self.within_pool_boundaries(measurment)):
            self.measurments[0].append(measurment[0])
            self.measurments[1].append(measurment[1])
            self.measurments[2].append(measurment[2])

            n = len(self.measurments[0])

            self.mean.x = sum(self.measurments[0]) / n
            self.mean.y = sum(self.measurments[1]) / n
            self.mean.z = sum(self.measurments[2]) / n

            self.variance.x = (
                sum((x - self.mean.x) ** 2 for x in self.measurments[0]) / n
            )
            self.variance.y = (
                sum((y - self.mean.y) ** 2 for y in self.measurments[1]) / n
            )
            self.variance.z = (
                sum((z - self.mean.z) ** 2 for z in self.measurments[2]) / n
            )

    def detect_change_in_pinger_pos(self, searching_for_second_pinger: bool):

        n = len(self.measurments[0])
        m = self.m_last_elements

        if searching_for_second_pinger and n > m * 2:

            prev_mean_x = sum(self.measurments[0][: n - m]) / (n - m)
            prev_mean_y = sum(self.measurments[1][: n - m]) / (n - m)
            prev_mean_z = sum(self.measurments[2][: n - m]) / (n - m)

            current_mean_x = sum(self.measurments[0][n - m :]) / m
            current_mean_y = sum(self.measurments[1][n - m :]) / m
            current_mean_z = sum(self.measurments[2][n - m :]) / m

            if self.distance_threshold.x < abs(prev_mean_x - current_mean_x):
                if self.distance_threshold.y < abs(prev_mean_y - current_mean_y):
                    if self.distance_threshold.z < abs(prev_mean_z - current_mean_z):
                        self.remove_obsolete_measurements()
                        return True 
            
        return False

    def remove_obsolete_measurements(self): 

        n = len(self.measurments[0])
        m = self.m_last_elements

        self.measurments[0] = self.measurments[0][n - m :]
        self.measurments[1] = self.measurments[1][n - m :]
        self.measurments[2] = self.measurments[2][n - m :]


    def within_pool_boundaries(self, measurment: List) -> bool :
        valid_measurment = True
        if abs(measurment[0]) < pool_size.x:
            valid_measurment = False
        elif abs(measurment[1]) < pool_size.y:
            valid_measurment = False
        elif abs(measurment[2]) < pool_size.z:
            valid_measurment = False
        return valid_measurment

