"""
connect

sendData("frequencyOfInterest", adress?)
getData("mode", adress?)
"Mode"
    RawhrophoneData
    DSP data (rawSample, filteredSample, FFT, Peaks)
"""

from socket import *
import time

HOST = "10.0.0.111"
PORT = 8888  # (non-privileged ports are > 1023)
MAX_PACKAGE_SIZE_RECEIVED = 65536

# Socket setup
address = (HOST, PORT)
clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(5) # Wait period before giving up on signal
 
while(True):
    data = "Hello world".encode()  # Converts the string to bytes
    clientSocket.sendto(data, address) # Send data request

    try:
        #Read response from arduino
        rec_data, addr = clientSocket.recvfrom(MAX_PACKAGE_SIZE_RECEIVED) 
        messageReceived = rec_data.decode()
        print (f"Message from {addr}: {messageReceived}")
    except:
        print ("Failed to read message")

    #delay before sending next command
    time.sleep(0.1)