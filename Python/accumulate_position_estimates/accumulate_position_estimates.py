"""
TODO: 
    - store measurments
        - np array with fixed size equal worst case
        - use python list 

    - Detect change in pinger position
        - can check what task is being worked on (check state)
        - can detect significant change in mean
        - both? 

    - Test
        - mean corresponds to initial position
        - sampling frequency equals 320, 427
        - decrease tolerance
        - increasing distance to source positions
        - detect change in pinger pos
            - change fsm bool before and after
            - find good number for n_last_elem

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
    recent_measurements = [[],[],[]]
    
    n_last_elements = 5                    # make init func
    distance_threshold = Position(3,3,3)  #check competition spec - 22x12

    def __init__(self, n_last_elem, distance_threshold) -> None:
        self.n_last_elements = n_last_elem
        self.distance_threshold = distance_threshold


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

    def detect_change_in_pinger_pos(
        self, 
        searching_for_second_pinger : bool
        ):

        if searching_for_second_pinger:
            n = len(self.measurements[0])
            m = self.n_last_elements

            prev_mean_x = sum(self.measurements[0][:n-m])/(n-m)
            prev_mean_y = sum(self.measurements[1][:n-m])/(n-m)
            prev_mean_z = sum(self.measurements[2][:n-m])/(n-m)

            current_mean_x = sum(self.measurements[0][n-m:])/m
            current_mean_y = sum(self.measurements[1][n-m:])/m
            current_mean_z = sum(self.measurements[2][n-m:])/m

            if self.distance_threshold.x < abs(prev_mean_x-current_mean_x):
                if self.distance_threshold.y < abs(prev_mean_y-current_mean_y):
                    if self.distance_threshold.z < abs(prev_mean_z - current_mean_z):
                        print("CHANGE IN POS")
                        for i in range (3):
                            self.measurements[i] = self.measurements[i][n-m:]
    



        
  
            

        



