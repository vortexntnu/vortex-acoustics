# import time
from acoustics_interface_asv_lib import TeensyCommunicationUDP

TeensyCommunicationUDP.init_communication()

while True:
    TeensyCommunicationUDP.get_data_from_teensy()

    # print(TeensyCommunicationUDP.data.tdoa)
    # time.sleep(1)