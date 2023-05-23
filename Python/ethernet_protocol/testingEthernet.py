from socket import *
import time

# Teensy networking
HOST = "10.0.0.111"
PORT = 8888  # (non-privileged ports are > 1023)
MAX_PACKAGE_SIZE_RECEIVED = 65536
TIMEOUT = 100 # Wait period before giving up on communications [seconds], Remember teensy takes time to calculate everything
address = (HOST, PORT)

# Code words
SEND_FREQUENCY = "sf" # Send frequency to look for and variance
SEND_SKIP = "ss" # Send SKIP command, teensy will stop waiting for data transfer and will continue with its calculations
GET_HYDROPHONE_DATA = "gh" # Get all 5 hydrophone raw sample data
GET_DSP_DATA = "gd" # Get Digital Signal Processing data -> raw data, filtered data, FFT data and peak data

# Socket setup
clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(TIMEOUT)

def send_frequency_of_interest(frequencyOfInterest, frequencyVariance):
    # Send a request to send
    clientSocket.sendto(SEND_FREQUENCY.encode(), address)

    # Send data
    data = str(frequencyOfInterest).encode()
    clientSocket.sendto(data, address)
    data = str(frequencyVariance).encode()
    clientSocket.sendto(data, address)

def send_SKIP():
    # Send a request to send
    clientSocket.sendto(SEND_SKIP.encode(), address)

def get_raw_hydrophone_data():
    # Send request
    clientSocket.sendto(GET_HYDROPHONE_DATA.encode(), address)

    try:
        #Read response from Teensy 5 times because of 5 hydrophones
        allHydrophoneData = [[], [], [], [], []]
        hydrophoneNr = 0
        while (hydrophoneNr < 5):
            done = False
            tempString = ""
            
            while (not done):
                # Read data
                rec_data, addr = clientSocket.recvfrom(MAX_PACKAGE_SIZE_RECEIVED) 
                messageReceived = rec_data.decode()

                # Check if data is done sending, else save
                if messageReceived == "DONE":
                    done = True
                else:
                    tempString += messageReceived

            # Save string into a integer array
            tempString = tempString[0:-1]
            tempIntList = list(map(int, tempString.split(',')))
            allHydrophoneData[hydrophoneNr] = tempIntList

            hydrophoneNr += 1

        return allHydrophoneData
    except:
        return "ERROR"

#send_frequency_of_interest(30000, 1000)
BigBoy =  get_raw_hydrophone_data()
for longList in BigBoy:
    print(longList)
    print("="*200)
    print("="*200)
send_SKIP()
