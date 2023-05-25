import time
from socket import *

# Teensy networking
HOST = "10.0.0.111"
PORT = 8888  # (non-privileged ports are > 1023)
MAX_PACKAGE_SIZE_RECEIVED = 65536
TIMEOUT = 60  # Wait period before giving up on communications [seconds], Remember teensy takes time to calculate everything
address = (HOST, PORT)

# Code words for communication
SEND_SKIP = "ss"  # Send SKIP command, teensy will stop waiting for data transfer and will continue with its calculations
SEND_FREQUENCY = "sf"  # Send frequency to look for and variance
GET_HYDROPHONE_DATA = "gh"  # Get all 5 hydrophone raw sample data
GET_DSP_DATA = "gd"  # Get Digital Signal Processing data -> raw data, filtered data, FFT data and peak data

# Socket setup
clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(TIMEOUT)


def send_SKIP():
    # Send a request to send
    clientSocket.sendto(SEND_SKIP.encode(), address)


def send_frequency_of_interest(frequencyOfInterest, frequencyVariance):
    # Send a request to send
    clientSocket.sendto(SEND_FREQUENCY.encode(), address)

    # Send data
    data = str(frequencyOfInterest).encode()
    clientSocket.sendto(data, address)
    data = str(frequencyVariance).encode()
    clientSocket.sendto(data, address)


def get_data():
    data = []
    done = False
    tempString = ""

    while not done:
        # Read data
        rec_data, addr = clientSocket.recvfrom(MAX_PACKAGE_SIZE_RECEIVED)
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


def get_raw_hydrophone_data():
    # Send request
    clientSocket.sendto(GET_HYDROPHONE_DATA.encode(), address)

    try:
        # Read response from Teensy 5 times because of 5 hydrophones
        allHydrophoneData = [[], [], [], [], []]
        hydrophoneNr = 0
        while hydrophoneNr < 5:
            allHydrophoneData[hydrophoneNr] = get_data()
            hydrophoneNr += 1

        return allHydrophoneData
    except:
        return "ERROR"


def get_DSP_data():
    # Send request
    clientSocket.sendto(GET_DSP_DATA.encode(), address)

    try:
        """
        DSP data is sent in a specific way since all the data types are different
        That is why it is important to keep the order as it is now
        - Raw samples
        - Filtered samples
        - FFT
        - Peaks
        """
        rawSampleData = get_data()
        filteredSampleData = get_data()
        FFTData = get_data()

        # For peek data we need to process it a bit more since its actually a array of [[amplitude, frequency, phase], [amplitude, frequency, phase],...]
        peakDataRaw = get_data()
        peakData = [peakDataRaw[i : i + 3] for i in range(0, len(peakDataRaw), 3)]

        return rawSampleData, filteredSampleData, FFTData, peakData
    except:
        return "ERROR"


# Get data
startTime = time.time()

send_frequency_of_interest(20000, 1000)
BigBoy = get_raw_hydrophone_data()
rawSampleData, filteredSampleData, FFTData, peakData = get_DSP_data()
send_SKIP()

stoptTime = time.time()

print("=" * 200)

# Print data
for index, longList in enumerate(BigBoy):
    print("=" * 100)
    print("Hydrophone " + str(index + 1))
    print(len(longList))
    print("=" * 100)
    print(longList)

print("=" * 100)
print("rawSampleData")
print(rawSampleData)
print("=" * 200)
print(filteredSampleData)
print("=" * 200)
print(FFTData)
print("=" * 200)
print(peakData)
print("=" * 200)

print(stoptTime - startTime)
