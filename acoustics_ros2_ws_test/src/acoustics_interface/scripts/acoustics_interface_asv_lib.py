# Setting up libraries
import os
import sys
from socket import *
import netifaces as ni
from enum import Enum
import errno
import time


# Variables ==================================================
# This list has to be exactly ten entries long
# format [(FREQUENCY, FREQUENCY_VARIANCE), ...]
frequenciesOfInterest = [
    (40_000, 3000), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0)
] 

MAX_TRIES = 200

class DataTarget(Enum):
    NONE = 0,
    SAMPLES_RAW = 1,
    SAMPLES_FILTERED = 2,
    FFT = 3,
    PEAK = 4,
    TDOA = 5,
    LOCATION = 6,

class AcousticsData:
    def __init__(self) -> None:
        self.samples_raw = []
        self.samples_filtered = []
        self.fft = []
        self.peak = []
        self.tdoa = []
        self.location = []

class TeensyCommunicationUDP:
    # Teensy networking Setup
    TEENSY_IP = "10.0.0.111"
    TEENSY_PORT = 8888
    MY_PORT = 9999
    MAX_PACKAGE_SIZE_RECEIVED = 65536
    TIMEOUT = 1
    address = (TEENSY_IP, TEENSY_PORT)

    # Code words for communication
    INITIALIZATION_MESSAGE = "HELLO :D"  # This is a message only sent once to establish 2 way communication between Teensy and client

    clientSocket = socket(AF_INET, SOCK_DGRAM)

    data_target = DataTarget.NONE
    data = AcousticsData()
    timeoutMax = 10
    data_string = "" # Internal
    msg_types = ["RAW", "FILTERED", "FFT", "PEAK", "TDOA", "LOCATION"]

    @classmethod
    def init_communication(cls):
        cls.MY_IP = cls.get_ip()

        # Socket setup
        cls.clientSocket.settimeout(cls.TIMEOUT)
        cls.clientSocket.bind((cls.MY_IP, cls.MY_PORT))
        cls.clientSocket.setblocking(False)

        cls.send_acknowledge_signal()
        timeStart = time.time()

        # Wait for READY signal
        while not cls.check_if_available():
            """
                IMPORTANT!
                DO NOT have "time.sleep(x)" value SMALLER than 1 second!!!
                This will interrupt sampling by asking teensy if its available to many times
                If less than 1 second you risc crashing teensy to PC communication O_O
            """

            print("Did not receive READY signal. Will wait.")
            time.sleep(1)
            
            if time.time() - timeStart > cls.timeoutMax:
                print("Gave up on receiving READY. Sending acknowledge signal again")
                # Start over
                timeStart = time.time()
                cls.send_acknowledge_signal()

        print("READY signal received, sending frequencies...")
        cls.send_frequencies_of_interest(frequenciesOfInterest)

    @classmethod
    def get_data_from_teensy(cls):
        cls.get_message()

        # print(cls.data.samples_raw)

    # stackoverflow <3
    # https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
    @classmethod
    def get_ip(cls):
        s = socket(AF_INET, SOCK_DGRAM)
        s.settimeout(0)
        try:
            # doesn't even have to be reachable
            s.connect((cls.TEENSY_IP, 1))
            IP = s.getsockname()[0]
        except Exception:
            IP = '127.0.0.1'
        finally:
            s.close()
        
        return IP

    @classmethod
    def send_acknowledge_signal(cls):        
        try:
            cls.clientSocket.sendto(cls.INITIALIZATION_MESSAGE.encode(), cls.address)
            print("DEBUGING: Sent acknowledge package")
        except Exception as e:
            print("Error from send_acknowledge_signal")
            print(e)
            pass

    @classmethod
    def check_if_available(cls):
        try:
            # i = 0
            while True:
                # Read data
                message = cls.get_raw_data()
                # Check if there is no more data left
                if message == None:
                    return False

                # Check if correct signal was sent
                if message == "READY":
                    return True
                
                # Don't think this is necessary, but maybe?
                # i += 1

                # if i > MAX_TRIES:
                #     i = 0
                #     print("Max tries exceeded")
                #     break
        except Exception as e:
            print(f"check_if_available rased exception: {e}")
            return False

    @classmethod
    def send_frequencies_of_interest(cls, frequenciesOfInterest: list[tuple[float, float]]):
        """
            To ignore entries in the frequency list, just make the frequency entry -1, and make the variance 0
        """ 
        try:

            # Format (CSV): xxx,x,xx,x...,x (frequency list comes first, then variances)
            assert len(frequenciesOfInterest) == 10, "List of frequencies has to be ten entries long!"

            # ten messages in total, one message for each entry to work around the max packet size
            for (frequency, variance) in frequenciesOfInterest:
                frequency_variance_msg = f"{str(frequency)},{str(variance)},"

                # print(self.address);
                cls.clientSocket.sendto(frequency_variance_msg.encode(), cls.address)
        except:
            print("Couldn't send Frequency data")

    @classmethod
    def get_raw_data(cls) -> str | None:
        try:
            rec_data, _ = cls.clientSocket.recvfrom(cls.MAX_PACKAGE_SIZE_RECEIVED)
            messageReceived = rec_data.decode()
            return messageReceived
        except error as e: # `error` is really `socket.error`
            if e.errno == errno.EWOULDBLOCK:
                # print("No data")
                pass
            else:
                print("Socket error: ", e)

    @classmethod
    def parse_data_string(cls, float: bool):
        if cls.data_string == '': 
            print("empty")
            return
        
        try:
            # Format data from CSV string to floats, ignore last value
            return list(map(float, cls.data_string.split(",")[:-1]))
            # if float:
            # else:
            #     return list(map(int, cls.data_string.split(",")[:-1]))
        except Exception as e:
            print(f"The string '{cls.data_string}' caused an error when parsing")
            print(f"The exception was: {e}")

    @classmethod
    def get_message(cls):
        data = cls.get_raw_data()
        
        if data == None:
            return

        if data not in cls.msg_types:
            cls.data_string += data
        else:
            cls.write_to_target()

            # print(data)

            if data == "RAW":
                cls.data_target = DataTarget.SAMPLES_RAW
            elif data == "FILTERED":
                cls.data_target = DataTarget.SAMPLES_FILTERED
            elif data == "FFT":
                cls.data_target = DataTarget.FFT
            elif data == "PEAK":
                cls.data_target = DataTarget.PEAK
            elif data == "TDOA":
                cls.data_target = DataTarget.TDOA
            elif data == "LOCATION":
                cls.data_target = DataTarget.LOCATION
        

    @classmethod
    def write_to_target(cls):
        cls.data_string = ""
        data = cls.parse_data_string(float=False)
        print(data)

        match cls.data_target:
            case DataTarget.NONE:
                pass
            case DataTarget.SAMPLES_RAW:
                cls.data.samples_raw = data
            case DataTarget.SAMPLES_FILTERED:
                # data = cls.parse_data_string(float=False)
                cls.data.samples_filtered = data
            case DataTarget.FFT:
                # data = cls.parse_data_string(float=False)
                cls.data.fft = data
            case DataTarget.PEAK:
                # data = cls.parse_data_string(float=False)
                cls.data.peak = data
            case DataTarget.TDOA:
                # data = cls.parse_data_string(float=True)
                cls.data.tdoa = data
            case DataTarget.LOCATION:
                # data = cls.parse_data_string(float=True)
                cls.data.location = data