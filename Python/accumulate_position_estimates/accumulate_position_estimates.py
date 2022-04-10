from dataclasses import dataclass
@dataclass
class Position:
    x: float = 0
    y: float = 0
    z: float = 0

pool_size = Position(25, 12.5, 5)

class PositionEstimate: 

    def __init__(self, m_last_elem, distance_threshold) -> None:
        self.mean = Position(0,0,0)
        self.variance = Position(0,0,0)
        self.measurements = [[],[],[]]

        self.m_last_elements = m_last_elem           
        self.distance_threshold = distance_threshold 

    def integrate_new_measurement(self, measurment: list):

        if measurment[0] < pool_size.x and measurment[1] < pool_size.y and measurment[2] < pool_size.z:  
            self.measurements[0].append(measurment[0])
            self.measurements[1].append(measurment[1])
            self.measurements[2].append(measurment[2])

            n = len(self.measurements[0])

            self.mean.x = sum(self.measurements[0])/n
            self.mean.y = sum(self.measurements[1])/n
            self.mean.z = sum(self.measurements[2])/n

            self.variance.x = sum((x-self.mean.x)**2 for x in self.measurements[0]) / n
            self.variance.y = sum((y-self.mean.y)**2 for y in self.measurements[1]) / n
            self.variance.z = sum((z-self.mean.z)**2 for z in self.measurements[2]) / n


    def detect_change_in_pinger_pos(self, searching_for_second_pinger : bool):

        n = len(self.measurements[0])
        m = self.m_last_elements

        if searching_for_second_pinger and n>m*2:

            prev_mean_x = sum(self.measurements[0][:n-m])/(n-m)
            prev_mean_y = sum(self.measurements[1][:n-m])/(n-m)
            prev_mean_z = sum(self.measurements[2][:n-m])/(n-m)

            current_mean_x = sum(self.measurements[0][n-m:])/m
            current_mean_y = sum(self.measurements[1][n-m:])/m
            current_mean_z = sum(self.measurements[2][n-m:])/m

            if self.distance_threshold.x < abs(prev_mean_x-current_mean_x):
                if self.distance_threshold.y < abs(prev_mean_y-current_mean_y):
                    if self.distance_threshold.z < abs(prev_mean_z - current_mean_z):
                        self.measurements[0] = self.measurements[0][n-m:]
                        self.measurements[1] = self.measurements[1][n-m:]
                        self.measurements[2] = self.measurements[2][n-m:]


                        
                        
    



        
  
            

        



