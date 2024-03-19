import time
from socket import *
import netifaces as ni
from enum import Enum
import errno
 
class TeensyCommunicationUDP:
    # Setup the communications with Teensy on initialization
    def __init__(
        self,
        TEENSY_IP="10.0.0.111",
        TEENSY_PORT=8888,  # (non-privileged ports are > 1023)
        MY_PORT=9999,
        MAX_PACKAGE_SIZE_RECEIVED=65536,
        TIMEOUT=1,  # Wait period before giving up on communications [seconds], Remember teensy takes time to calculate everything)
    ):
        # Teensy networking Setup
        self.TEENSY_IP = TEENSY_IP
        self.TEENSY_PORT = TEENSY_PORT
        self.MY_PORT = MY_PORT
        self.MAX_PACKAGE_SIZE_RECEIVED = MAX_PACKAGE_SIZE_RECEIVED
        self.TIMEOUT = TIMEOUT
        self.address = (TEENSY_IP, TEENSY_PORT)

        # Code words for communication
        self.INITIALIZATION_MESSAGE = "HELLO :D"  # This is a message only sent once to establish 2 way communication between Teensy and client
        self.SEND_SKIP = "ss"  # Send SKIP command, teensy will stop waiting for data transfer and will continue with its calculations
        self.SEND_FREQUENCY = "sf"  # Send frequency to look for and variance
        self.GET_HYDROPHONE_DATA = "gh"  # Get all 5 hydrophone raw sample data
        self.GET_DSP_DATA = "gd"  # Get Digital Signal Processing data -> raw data, filtered data, FFT data and peak data

        # Get PC IP
        self.MY_IP = self.get_ip()

        print(self.MY_IP)

        # Socket setup
        self.clientSocket = socket(AF_INET, SOCK_DGRAM)
        self.clientSocket.settimeout(TIMEOUT)
        self.clientSocket.bind((self.MY_IP, self.MY_PORT))
        self.clientSocket.setblocking(False)

        # send initialization signal
        self.send_acknowledge_signal()

        self.data_target = DataTarget.NONE

        self.samples_raw = []
        self.samples_filtered = []
        self.fft_data = []
        self.peak_data = []
        self.tdoa_data = []
        self.location_data = []

    # stackoverflow <3
    # https://stackoverflow.com/questions/166506/finding-local-ip-addresses-using-pythons-stdlib
    def get_ip(self):
        s = socket(AF_INET, SOCK_DGRAM)
        s.settimeout(0)
        try:
            # doesn't even have to be reachable
            s.connect((self.TEENSY_IP, 1))
            IP = s.getsockname()[0]
        except Exception:
            IP = '127.0.0.1'
        finally:
            s.close()
        
        return IP

    def send_acknowledge_signal(self):
        try:
            self.clientSocket.sendto(self.INITIALIZATION_MESSAGE.encode(), self.address)
            print("DEBUGING: Sent scknowledge packsge")
        except:
            print("DEBUGING: FAILED to send acknowledge package")
            pass

    def check_if_available(self):
        try:
            # Read data
            rec_data, addr = self.clientSocket.recvfrom(self.MAX_PACKAGE_SIZE_RECEIVED)
            messageReceived = rec_data.decode()

            # Check if correct signal was sent
            if messageReceived == "READY":
                return True
            else:
                return False
        except:
            return False

    def send_SKIP(self):
        try:
            # Send a request to send frequency data
            self.clientSocket.sendto(self.SEND_SKIP.encode(), self.address)
        except:
            print("Couldn't send SKIP command...")

    def send_frequencies_of_interest(self, frequenciesOfInterest: list[tuple[float, float]]):
        # NOTE: frequency/variance lists must have a length of ten
        try:
            # Send a request to send frequency data
            # To ingore entries in the frequency list, just make the frequency entry -1, and make the variance 0

            # Format (CSV): xxx,x,xx,x...,x (frequency list comes first, then variances)
            assert len(frequenciesOfInterest) == 10, "List of frequencies has to be ten entries long!"

            # ten messages in total, one message for each entry to work around the max packet size
            # Change to for 0..10 for maximum safety??
            for (frequency, variance) in frequenciesOfInterest:
                frequency_variance_msg = f"{str(frequency)},{str(variance)},"

                # print(self.address);
                self.clientSocket.sendto(frequency_variance_msg.encode(), self.address)
        except:
            print("Couldn't send Frequency data")



    def send_frequency_of_interest(self, frequencyOfInterest, frequencyVariance):
        try:
            # Send a request to send frequency data
            self.clientSocket.sendto(self.SEND_FREQUENCY.encode(), self.address)

            # Send data
            data = str(frequencyOfInterest).encode()
            self.clientSocket.sendto(data, self.address)
            data = str(frequencyVariance).encode()
            self.clientSocket.sendto(data, self.address)
        except:
            print("Couldn't send Frequency data...")

    # Universal function for getting data from Teensy, it is standardized in Teensy to send data back in such format
    def get_data(self):
        data = []
        done = False
        tempString = ""

        while not done:
            # Read data
            rec_data, addr = self.clientSocket.recvfrom(self.MAX_PACKAGE_SIZE_RECEIVED)
            messageReceived = rec_data.decode()

            if messageReceived == "DONE":
                done = True
            else:
                tempString += messageReceived

        # Try saving string into a integer array, if error -> string empty
        try:
            tempString = tempString[0:-1]
            # data = list(map(float, tempString.split(","))) # This may not be safe )
            data = list(map(float, tempString.split(",")))
        except Exception as e:
            print("get_data raised exception: ")
            print(e)
            data = [0]

        return data

    def get_raw_hydrophone_data(self):
        # Send request
        # self.clientSocket.sendto(self.GET_HYDROPHONE_DATA.encode(), self.address)

        try:
            # Read response from Teensy 5 times because of 5 hydrophones
            allHydrophoneData = [[], [], [], [], []]
            for i in range(5):
                allHydrophoneData[i] = self.get_data()
            
            return allHydrophoneData
        except Exception as e:
            print("get_raw_hydrophone_data raised exception: ")
            print(e)
            return [[], [], [], [], []]

    def get_DSP_data(self):
        # Send request
        # self.clientSocket.sendto(self.GET_DSP_DATA.encode(), self.address)

        try:
            """
            DSP data is sent in a specific way since all the data types are different
            That is why it is important to keep the order as it is now
            - Raw samples
            - Filtered samples
            - FFT
            - Peaks
            - time difference of arrival data
            - sound location data
            """
            rawSampleData = self.get_data()
            filteredSampleData = self.get_data()
            FFTData = self.get_data()

            # For peek data we need to process it a bit more since its actually a array of [[amplitude, frequency, phase], [amplitude, frequency, phase],...]
            peakDataRaw = self.get_data()
            peakData = [peakDataRaw[i : i + 3] for i in range(0, len(peakDataRaw), 3)]

            tdoaData = self.get_data()
            soundLocationData = self.get_data()


            return rawSampleData, filteredSampleData, FFTData, peakData, tdoaData, soundLocationData
        except Exception as e:
            print("get_DSP_data raised exception: ")
            print(e)
            return [0], [0], [0], [0], [0], [0]

    def get_raw_data(self):
        try:
            rec_data, _ = self.clientSocket.recvfrom(self.MAX_PACKAGE_SIZE_RECEIVED)
            messageReceived = rec_data.decode()
            return messageReceived
        except error as e: # `error` is really `socket.error`
            if e.errno == errno.EWOULDBLOCK:
                # print("No data")
                pass
            else:
                print("Socket error: ", e)

    def parse_message(self, message):
        return list(map(float, message.split(",")))

    def get_message(self):
        data = self.get_raw_data()

        if data == None:
            return

        if data == "RAW":
            self.data_target = DataTarget.SAMPLES_RAW
            self.samples_raw = []
            return
        elif data == "FILTERED":
            self.data_target = DataTarget.SAMPLES_FILTERED
            self.samples_filtered = []
            return
        elif data == "FFT":
            self.data_target = DataTarget.FFT
            self.fft_data = []
            return
        elif data == "PEAK":
            self.data_target = DataTarget.PEAK
            self.peak_data = []
            return
        elif data == "TDOA":
            self.data_target = DataTarget.TDOA
            self.tdoa_data = []
            return
        elif data == "LOCATION":
            self.data_target = DataTarget.LOCATION
            self.location_data = []
            return

        data = self.parse_message(data)

        match self.data_target:
            case DataTarget.NONE:
                pass
            case DataTarget.SAMPLES_RAW:
                self.samples_raw.extend(data)
            case DataTarget.SAMPLES_FILTERED:
                self.samples_filtered.extend(data)
            case DataTarget.FFT:
                self.fft_data.extend(data)
            case DataTarget.PEAK:
                self.peak_data.extend(data)
            case DataTarget.TDOA:
                self.tdoa_data.extend(data)
            case DataTarget.LOCATION:
                self.location_data.extend(data)

        


        # if self.data_target == DataTarget.NONE:


        # else:
        #     return None, MessageType.NONE



        
        # data = []
        # done = False
        # tempString = ""

        # while not done:
        #     # Read data
        #     rec_data, addr = self.clientSocket.recvfrom(self.MAX_PACKAGE_SIZE_RECEIVED)
        #     messageReceived = rec_data.decode()

        #     if messageReceived == "DONE":
        #         done = True
        #     else:
        #         tempString += messageReceived

        # # Try saving string into a integer array, if error -> string empty
        # try:
        #     tempString = tempString[0:-1]
        #     # data = list(map(float, tempString.split(","))) # This may not be safe )
        #     data = list(map(float, tempString.split(",")))
        # except Exception as e:
        #     print("get_data raised exception: ")
        #     print(e)
        #     data = [0]

        # return data
    

class DataTarget(Enum):
    NONE = 0,
    SAMPLES_RAW = 1,
    SAMPLES_FILTERED = 2,
    FFT = 3,
    PEAK = 4,
    TDOA = 5,
    LOCATION = 6,

# class DataObject:
#     def __init__(self) -> None:
#         pass
