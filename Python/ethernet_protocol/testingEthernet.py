from socket import *
import time

# Teensy networking
HOST = "10.0.0.111"
PORT = 8888  # (non-privileged ports are > 1023)
MAX_PACKAGE_SIZE_RECEIVED = 65536
TIMEOUT = 10 # Wait period before giving up on communications [seconds]
address = (HOST, PORT)

# Code words
SEND_FREQUENCY = "sf" # Send frequency to look for and variance
GET_HYDROPHONE_DATA = "gh" # Get all 5 hydrophone raw sample data
GET_DSP_DATA = "gd" # Get Digital Signal Processing data -> raw data, filtered data, FFT data and peak data

# Socket setup
clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(TIMEOUT)

def send_frequency_of_interest(frequencyOfInterest):
    data = str(frequencyOfInterest).encode() # Converts the string to bytes
    clientSocket.sendto(SEND_FREQUENCY.encode(), address)
    clientSocket.sendto(data, address) # Send data

def get_raw_hydrophone_data():
    clientSocket.sendto(GET_HYDROPHONE_DATA.encode(), address)

    try:
        #Read response from Teensy
        rec_data, addr = clientSocket.recvfrom(MAX_PACKAGE_SIZE_RECEIVED) 
        messageReceived = rec_data.decode()

        return messageReceived
    except:
        return "ERROR"


while(True):
    hydrophoneData = get_raw_hydrophone_data()
    print(hydrophoneData)

    #delay before sending next command
    time.sleep(0.1)