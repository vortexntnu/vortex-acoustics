from socket import *
import time

HOST = '10.0.0.111'
PORT = 8888  # (non-privileged ports are > 1023)

# Socket setup
address = (HOST, PORT)
clientSocket = socket(AF_INET, SOCK_DGRAM)
clientSocket.settimeout(5) # Wait period before giving up on signal
 
while(True):
    data = bytes(69) #Set data request to Temperature
    clientSocket.sendto(data, address) #Send the data request
    print("Data sent")
    """
    try:
        #Read response from arduino
        rec_data, addr = clientSocket.recvfrom(2048) 
        messageReceived = float(rec_data)
        print ("Message: ", messageReceived)
    except:
        print ("Failed to send message, trying again...")
    """

    #delay before sending next command
    time.sleep(0.5)