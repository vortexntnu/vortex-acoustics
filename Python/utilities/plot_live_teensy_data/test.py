# Setting up libraries
import os
import sys
PARENT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
MY_FILE_DIR = f"{PARENT_DIR}/utilities/plot_live_teensy_data/"
sys.path.append(PARENT_DIR)

from ethernet_protocol import ethernetProtocolTeensy
from datetime import datetime
import csv
import time

# Variables ==================================================
frequencyOfInterest = 20_000 # 20 kHz
frequencyVariance = 2_000 # 2 kHz

# Setup ethernet protocol
teensy = ethernetProtocolTeensy.TeensyCommunicationUDP(
    TEENSY_IP = "10.0.0.111",
    TEENSY_PORT = 8888,
    MY_PORT = 9999,
    MAX_PACKAGE_SIZE_RECEIVED = 65536,
    TIMEOUT = 10,
)

teensy.send_frequency_of_interest(frequencyOfInterest, frequencyVariance)
print(teensy.address)