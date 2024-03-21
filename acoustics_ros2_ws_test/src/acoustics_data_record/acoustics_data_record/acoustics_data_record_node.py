#!/usr/bin/env python3

# ROS2 libraries
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32MultiArray, Int32MultiArray

# Python libraries
import array

class AcousticsDataRecordNode(Node):
    def __init__(self):
        # Variables for seting upp loging correctly
        hydrophoneDataSize = (2**10) * 3 # 1 hydrophone buffer is 2^10 long, Each hydrophone data has 3 buffers full of this data
        DSPDataSize = 2**10 # DSP (Digital Signal Processing) has 2^10 long data
        TDOADataSize = 5 # TDOA (Time Difference Of Arrival) has 5 hydrophones it has times for
        positionDataSize = 3 # position only has X, Y, Z basicaly 3 elements

        # Start the node
        super().__init__('acoustics_data_record_node')

        # Start listening to Hydrophone data
        self.subscriberHydrophone1 = self.create_subscription(
            Int32MultiArray,
            '/acoustics/hydrophone1',
            self.hydrophone1_callback,
            5)
        self.hydropone1Data = array.array("i", [0] * hydrophoneDataSize)

        self.subscriberHydrophone2 = self.create_subscription(
            Int32MultiArray,
            '/acoustics/hydrophone2',
            self.hydrophone2_callback,
            5)
        self.hydropone2Data = array.array("i", [0] * hydrophoneDataSize)

        self.subscriberHydrophone3 = self.create_subscription(
            Int32MultiArray,
            '/acoustics/hydrophone3',
            self.hydrophone3_callback,
            5)
        self.hydropone3Data = array.array("i", [0] * hydrophoneDataSize)

        self.subscriberHydrophone4 = self.create_subscription(
            Int32MultiArray,
            '/acoustics/hydrophone4',
            self.hydrophone4_callback,
            5)
        self.hydropone4Data = array.array("i", [0] * hydrophoneDataSize)

        self.subscriberHydrophone5 = self.create_subscription(
            Int32MultiArray,
            '/acoustics/hydrophone5',
            self.hydrophone5_callback,
            5)
        self.hydropone5Data = array.array("i", [0] * hydrophoneDataSize)

        # Start listening to DSP (Digital Signal Processing) data
        self.subscriberFilterResponse = self.create_subscription(
            Int32MultiArray,
            '/acoustics/filter_response',
            self.filter_response_callback,
            5)
        self.filterResponseData = array.array("i", [0] * DSPDataSize)

        self.subscriberFFT = self.create_subscription(
            Int32MultiArray,
            '/acoustics/fft',
            self.fft_callback,
            5)
        self.FFTData = array.array("i", [0] * DSPDataSize)

        self.subscriberPeaks = self.create_subscription(
            Int32MultiArray,
            '/acoustics/peaks',
            self.peaks_callback,
            5)
        self.peaksData = array.array("i", [0] * DSPDataSize)

        # Start listening to Multilateration data
        self.subscriberTDOAResponse = self.create_subscription(
            Float32MultiArray,
            '/acoustics/time_difference_of_arrival',
            self.tdoa_callback,
            5)
        self.TDOAData = array.array("f", [0.0] * TDOADataSize)

        self.subscriberPositionResponse = self.create_subscription(
            Float32MultiArray,
            '/acoustics/position',
            self.position_callback,
            5)
        self.positionData = array.array("f", [0.0] * positionDataSize)

        # Logs all the newest data 1 time(s) per second
        DATA_LOGING_RATE = 1.0
        timer_period = 1.0/DATA_LOGING_RATE
        self.logger_timer = self.create_timer(timer_period, self.logger)

    # Callback methods for diffrenet topics
    def hydrophone1_callback(self, msg):
        self.hydropone1Data = msg.data
    
    def hydrophone2_callback(self, msg):
        self.hydropone2Data = msg.data

    def hydrophone3_callback(self, msg):
        self.hydropone3Data = msg.data

    def hydrophone4_callback(self, msg):
        self.hydropone4Data = msg.data

    def hydrophone5_callback(self, msg):
        self.hydropone5Data = msg.data

    def filter_response_callback(self, msg):
        self.filterResponseData = msg.data

    def fft_callback(self, msg):
        self.FFTData = msg.data

    def peaks_callback(self, msg):
        self.peaksData = msg.data

    def tdoa_callback(self, msg):
        self.TDOAData = msg.data

    def position_callback(self, msg):
        self.positionData = msg.data
    
    # The logger that logs all the data
    def logger(self):
        # Debugging
        #self.get_logger().info(f"Hydrophone1: {self.hydropone1Data}, {self.hydropone2Data}, {self.hydropone3Data}, {self.hydropone4Data} {self.hydropone5Data}")
        #self.get_logger().info(f"DSP: {self.filterResponseData}, {self.FFTData}, {self.peaksData}")
        self.get_logger().info(f"DSP: {self.TDOAData}, {self.positionData}")
        



def main(args=None):
    # Initialize ROS2
    rclpy.init(args=args)

    # Start the ROS2 node and continue forever until exit
    acoustics_data_record_node = AcousticsDataRecordNode()
    rclpy.spin(acoustics_data_record_node)

    # Destroy the node explicitly once ROS2 stops runing
    acoustics_data_record_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
