#!/usr/bin/python3

import rclpy
from rclpy.node import Node
import rclpy.logging
from std_msgs.msg import Int32MultiArray, Float32MultiArray
from acoustics_interface.acoustics_interface_lib import TeensyCommunicationUDP

class AcousticsInterfaceNode(Node):
    def __init__(self) -> None:
        super().__init__('acoustics_interface')
        rclpy.logging.initialize()

        self._hydrophone_1_publisher = self.create_publisher(Int32MultiArray, '/acoustics/hydrophone1', 10)
        self._hydrophone_2_publisher = self.create_publisher(Int32MultiArray, '/acoustics/hydrophone2', 10)
        self._hydrophone_3_publisher = self.create_publisher(Int32MultiArray, '/acoustics/hydrophone3', 10)
        self._hydrophone_4_publisher = self.create_publisher(Int32MultiArray, '/acoustics/hydrophone4', 10)
        self._hydrophone_5_publisher = self.create_publisher(Int32MultiArray, '/acoustics/hydrophone5', 10)

        self._filter_response_publisher = self.create_publisher(Int32MultiArray, '/acoustics/filter_response', 10)
        self._fft_publisher = self.create_publisher(Int32MultiArray, '/acoustics/fft', 10)
        self._peak_publisher = self.create_publisher(Int32MultiArray, '/acoustics/peaks', 10)
        self._tdoa_publisher = self.create_publisher(Float32MultiArray, '/acoustics/time_difference_of_arrival', 10)
        self._position_publisher = self.create_publisher(Float32MultiArray, '/acoustics/position', 10)

        self._timer1 = self.create_timer(1, self.publish_bms_data)
        self._timer2 = self.create_timer(0.001, self.update_data)

        # This list has to be exactly 10 entries long
        # format [(FREQUENCY, FREQUENCY_VARIANCE), ...] 
        frequenciesOfInterest = [
            (0, 0), 
            (0, 0), 
            (0, 0), 
            (0, 0), 
            (0, 0), 
            (10_000, 3_000), 
            (0, 0), 
            (0, 0), 
            (0, 0), 
            (0, 0)
        ]

        TeensyCommunicationUDP.init_communication(frequenciesOfInterest)

    def update_data(self):
        TeensyCommunicationUDP.fetch_data()

    def publish_bms_data(self):
        self._hydrophone_1_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["HYDROPHONE_1"]))
        self._hydrophone_2_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["HYDROPHONE_2"]))
        self._hydrophone_3_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["HYDROPHONE_3"]))
        self._hydrophone_4_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["HYDROPHONE_4"]))
        self._hydrophone_5_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["HYDROPHONE_5"]))

        self._filter_response_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["SAMPLES_FILTERED"]))
        self._fft_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["FFT"]))
        self._peak_publisher.publish(Int32MultiArray(data=TeensyCommunicationUDP.acoustics_data["PEAK"]))
        self._tdoa_publisher.publish(Float32MultiArray(data=TeensyCommunicationUDP.acoustics_data["TDOA"]))
        self._position_publisher.publish(Float32MultiArray(data=TeensyCommunicationUDP.acoustics_data["LOCATION"]))

def main(args=None):
    rclpy.init(args=args)

    node = AcousticsInterfaceNode()

    rclpy.spin(node)

    node.destroy_node()    
    rclpy.shutdown()
    pass

if __name__ == "__main__":
    main()
    