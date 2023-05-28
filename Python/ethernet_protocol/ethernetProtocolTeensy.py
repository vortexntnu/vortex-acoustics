from socket import *

class TeensyCommunicationUDP:
    # Setup the communications with Teensy on initialization
    def __init__(
            self,
            TEENSY_IP = "10.0.0.111",
            TEENSY_PORT = 8888,  # (non-privileged ports are > 1023)
            MY_PORT = 9999,
            MAX_PACKAGE_SIZE_RECEIVED = 65536,
            TIMEOUT = 10,  # Wait period before giving up on communications [seconds], Remember teensy takes time to calculate everything)
    ):
        # Teensy networking Setup
        self.TEENSY_IP = TEENSY_IP
        self.TEENSY_PORT = TEENSY_PORT
        self.MY_PORT = MY_PORT
        self.MAX_PACKAGE_SIZE_RECEIVED = MAX_PACKAGE_SIZE_RECEIVED
        self.TIMEOUT = TIMEOUT
        self.address = (TEENSY_IP, TEENSY_PORT)

        # Code words for communication
        self.INITIALIZATION_MESSAGE = "HELLO :D" # This is a message only sent once to establish 2 way communication between Teensy and client
        self.SEND_SKIP = "ss"  # Send SKIP command, teensy will stop waiting for data transfer and will continue with its calculations
        self.SEND_FREQUENCY = "sf"  # Send frequency to look for and variance
        self.GET_HYDROPHONE_DATA = "gh"  # Get all 5 hydrophone raw sample data
        self.GET_DSP_DATA = "gd"  # Get Digital Signal Processing data -> raw data, filtered data, FFT data and peak data

        # Get PC IP
        pcHostname = gethostname()
        self.MY_IP = gethostbyname(pcHostname)

        # Socket setup
        self.clientSocket = socket(AF_INET, SOCK_DGRAM)
        self.clientSocket.settimeout(TIMEOUT)
        self.clientSocket.bind((self.MY_IP, self.MY_PORT))

        # Send initialization message to Teensy
        #self.clientSocket.sendto(self.INITIALIZATION_MESSAGE.encode(), self.address)
        #print("DONE INIT!")

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

            # Check if data is done sending, else save
            if messageReceived == "DONE":
                done = True
            else:
                tempString += messageReceived

        # Try saving string into a integer array, if error -> string empty
        try:
            tempString = tempString[0:-1]
            data = list(map(int, tempString.split(",")))
        except:
            data = [0]

        return data

    def get_raw_hydrophone_data(self):
        # Send request
        self.clientSocket.sendto(self.GET_HYDROPHONE_DATA.encode(), self.address)

        try:
            # Read response from Teensy 5 times because of 5 hydrophones
            allHydrophoneData = [[], [], [], [], []]
            hydrophoneNr = 0
            while hydrophoneNr < 5:
                allHydrophoneData[hydrophoneNr] = self.get_data()
                hydrophoneNr += 1

            return allHydrophoneData
        except:
            return "ERROR"

    def get_DSP_data(self):
        # Send request
        self.clientSocket.sendto(self.GET_DSP_DATA.encode(), self.address)

        try:
            """
            DSP data is sent in a specific way since all the data types are different
            That is why it is important to keep the order as it is now
            - Raw samples
            - Filtered samples
            - FFT
            - Peaks
            """
            rawSampleData = self.get_data()
            filteredSampleData = self.get_data()
            FFTData = self.get_data()

            # For peek data we need to process it a bit more since its actually a array of [[amplitude, frequency, phase], [amplitude, frequency, phase],...]
            peakDataRaw = self.get_data()
            peakData = [peakDataRaw[i : i + 3] for i in range(0, len(peakDataRaw), 3)]

            return rawSampleData, filteredSampleData, FFTData, peakData
        except:
            return "ERROR"
