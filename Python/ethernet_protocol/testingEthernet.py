from socket import *
import time

HOST = '10.0.0.142'  # Standard loopback interface address (localhost)
PORT = 8888  # Port to listen on (non-privileged ports are > 1023)

address= ('10.0.0.142', 8888) #define server IP and port
clientSocket = socket(AF_INET, SOCK_DGRAM) #Set up the Socket
clientSocket.settimeout(1) # Only wait 1 second for a response
 
while(True):
    data = bytes(69) #Set data request to Temperature
    clientSocket.sendto(data, address) #Send the data request
    try:
        #Read response from arduino
        rec_data, addr = clientSocket.recvfrom(2048) 
        messageReceived = float(rec_data)
        print ("Message: ", messageReceived)
    except:
        print ("ERROR!!!")

    #delay before sending next command
    time.sleep(2)