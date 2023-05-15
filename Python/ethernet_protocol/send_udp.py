import socket

TEENSY_IP = "10.0.0.1"
UDP_PORT = 8888
MESSAGE = b"Testing UDP package"

print("UDP target IP: %s" % TEENSY_IP)
print("UDP target port: %s" % UDP_PORT)
print("message: %s" % MESSAGE)

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (TEENSY_IP, UDP_PORT))