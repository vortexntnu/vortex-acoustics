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
frequencyOfInterest = 30_000 # 20 kHz
frequencyVariance = 30_000 # 2 kHz

# Setup ethernet protocol
teensy = ethernetProtocolTeensy.TeensyCommunicationUDP(
    TEENSY_IP = "10.0.0.111",
    TEENSY_PORT = 8888,
    MY_PORT = 9999,
    MAX_PACKAGE_SIZE_RECEIVED = 65536,
    TIMEOUT = 10,
)

# Create files we will write data to
dateAndTime = datetime.now()
formattedDateAndTime = dateAndTime.strftime("%Y_%m_%d___%H_%M_%S")
hydrophoneDataHeader = ['hydrophone1', 'hydrophone2', 'hydrophone3', 'hydrophone4', "hydrophone5"]
DSPHeader = ["raw_samples", "filtered_samples", "FFT", "peaks"]

with open(f"{MY_FILE_DIR}hydrophone_data/hydrophone_{formattedDateAndTime}.csv", "w", encoding="UTF8", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(hydrophoneDataHeader)

with open(f"{MY_FILE_DIR}DSP_data/DSP_{formattedDateAndTime}.csv", "w", encoding="UTF8", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(DSPHeader)

# Infinite loop for reading data
while True:
    try:
        while not teensy.check_if_available():
            """
            IMPORTANT! 
            DO NOT have "time.sleep(x)" value SMALLER than 1 second!!!
            This will interrupt sampling by asking teensy if its available to many times
            If less than 1 second you risc crashing teensy to PC communication O_O
            """
            time.sleep(1)
        
        teensy.send_frequency_of_interest(frequencyOfInterest, frequencyVariance)
        hydrophoneData = teensy.get_raw_hydrophone_data()
        rawSampleData, filteredSampleData, FFTData, peakData = teensy.get_DSP_data()
        teensy.send_SKIP() # Once we are done we NEED to send teensy a confirmation code so that it can continue to calculate with the new given information
        print("Got all the data :D")

        # Save data to csv files
        try:
            with open(f"{MY_FILE_DIR}hydrophone_data/hydrophone_{formattedDateAndTime}.csv", "a", encoding="UTF8", newline="") as f:
                writer = csv.writer(f)
                writer.writerow(hydrophoneData)

            with open(f"{MY_FILE_DIR}DSP_data/DSP_{formattedDateAndTime}.csv", "a", encoding="UTF8", newline="") as f:
                writer = csv.writer(f)
                writer.writerow([rawSampleData, filteredSampleData, FFTData, peakData])
            print("Data Saved")
        except:
            print("ERROR saving")
    except:
        print("ERROR")
    
    # A little pause to not overwhelm the processor
    print()
    time.sleep(1)