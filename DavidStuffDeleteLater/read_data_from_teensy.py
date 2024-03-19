# Setting up libraries
import os
import sys

PARENT_DIR = os.path.dirname(
    os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
)
MY_FILE_DIR = os.path.join(PARENT_DIR, "utilities", "plot_live_teensy_data")
sys.path.append(PARENT_DIR)

import csv
import time
from datetime import datetime

from Python.acoustics_interface.ethernetProtocolTeensy import TeensyCommunicationUDP

# Variables ==================================================
# This list has to be exactly ten entries long
# format [(FREQUENCY, FREQUENCY_VARIANCE), ...]
frequenciesOfInterest = [
    (40_000, 3000), 
    (20_000, 3000), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (0, 0), 
    (100, 200)
] 


# Timeout variables
# DON'T have timeout less than < 10 seconds, this WILL BRICK TEENSY!!!
timeoutMax = 60

# Setup ethernet protocol
teensy = TeensyCommunicationUDP(
    TEENSY_IP="10.0.0.111",
    TEENSY_PORT=8888,
    MY_PORT=9999,
    MAX_PACKAGE_SIZE_RECEIVED=65536,
    TIMEOUT=0.5,
)


# Create files we will write data to
dateAndTime = datetime.now()
formattedDateAndTime = dateAndTime.strftime("%Y_%m_%d___%H_%M_%S")
hydrophoneDataHeader = [
    "hydrophone1",
    "hydrophone2",
    "hydrophone3",
    "hydrophone4",
    "hydrophone5",
]
DSPHeader = ["raw_samples", "filtered_samples", "FFT", "peaks"]

with open(
    os.path.join(MY_FILE_DIR, "hydrophone_data", f"hydrophone_{formattedDateAndTime}.csv"),
    "w+",
    encoding="UTF8",
    newline="",
) as f:
    writer = csv.writer(f)
    writer.writerow(hydrophoneDataHeader)

with open(
    os.path.join(MY_FILE_DIR, "DSP_data", f"DSP_{formattedDateAndTime}.csv"),
    "w",
    encoding="UTF8",
    newline="",
) as f:
    writer = csv.writer(f)
    writer.writerow(DSPHeader)

def setup_teensy_communication():
    timeStart = time.time()

    # Wait for READY signal
    while not teensy.check_if_available():
        """
            IMPORTANT!
            DO NOT have "time.sleep(x)" value SMALLER than 1 second!!!
            This will interrupt sampling by asking teensy if its available to many times
            If less than 1 second you risc crashing teensy to PC communication O_O
        """

        print("Did not receive READY signal. Will wait.")
        time.sleep(1)
        
        if time.time() - timeStart > timeoutMax:
            print("Gave up on receiving READY. Sending acknowledge signal again")
            # Start over
            setup_teensy_communication()

    # teensy.send_acknowledge_signal();
    # teensy.send_acknowledge_signal();
    # teensy.send_acknowledge_signal();
    # teensy.send_acknowledge_signal();

    # time.sleep(1);
    print("READY signal received, sending frequencies...")
    teensy.send_frequencies_of_interest(frequenciesOfInterest)
    # teensy.send_frequency_of_interest(frequencyOfInterest, frequencyVariance)
    # teensy.send_SKIP()  # Once we are done we NEED to send teensy a confirmation code so that it can continue to calculate with the new given information

def get_data_from_teensy():
    # hydrophoneData = teensy.get_raw_hydrophone_data()
    # rawSampleData, filteredSampleData, FFTData, peakData, tdoaData, soundLocationData = teensy.get_DSP_data()
    # teensy.send_SKIP()

    teensy.get_message()

    print(teensy.fft_data)
    # print(teensy.location_data)
            
    # soundLocationData = teensy.get_data()

    # print(tdoaData);
    # print(soundLocationData);

    # try:
    #     with open(
    #         os.path.join(MY_FILE_DIR, "hydrophone_data", f"hydrophone_{formattedDateAndTime}.csv"),
    #         "a",
    #         encoding="UTF8",
    #         newline="",
    #     ) as f:
    #         writer = csv.writer(f)
    #         writer.writerow(hydrophoneData)
# TODO: Finish data transfer
    #     with open(
    #         os.path.join(MY_FILE_DIR, "DSP_data", f"DSP_{formattedDateAndTime}.csv"),
    #         "a",
    #         encoding="UTF8",
    #         newline="",
    #     ) as f:
    #         writer = csv.writer(f)
    #         writer.writerow([rawSampleData, filteredSampleData, FFTData, peakData])
    #     print("Data Saved")
    # except:
    #     print("ERROR saving data")

# initialize stuff
setup_teensy_communication()

# time.sleep(1);

while True:
    try:
        # print("Receiving data from teensy...")
        get_data_from_teensy()
    except Exception as e:
        print("ERROR: Receiving data did not work")
        print(e)


    # A little pause to not overwhelm the processor
    # This is needed to sync up with teensy's data transfer
    # time.sleep(0.1)