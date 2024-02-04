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

from ethernet_protocol import ethernetProtocolTeensy

# Variables ==================================================
frequencyOfInterest = 40_000 
frequencyVariance = 3_000 

# Timeout variables
# DON'T have timeout less than < 10 seconds, this WILL BRICK TEENSY!!!
timeoutMax = 60

# Setup ethernet protocol
teensy = ethernetProtocolTeensy.TeensyCommunicationUDP(
    TEENSY_IP="10.0.0.111",
    TEENSY_PORT=8888,
    MY_PORT=9999,
    MAX_PACKAGE_SIZE_RECEIVED=65536,
    TIMEOUT=1,
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
    teensy.send_acknowledge_signal()

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

    print("READY signal received, sending frequencies...")
    teensy.send_frequency_of_interest(frequencyOfInterest, frequencyVariance)
    teensy.send_SKIP()  # Once we are done we NEED to send teensy a confirmation code so that it can continue to calculate with the new given information
        
def get_data_from_teensy():
    hydrophoneData = teensy.get_raw_hydrophone_data()
    rawSampleData, filteredSampleData, FFTData, peakData = teensy.get_DSP_data()
    teensy.send_SKIP()

    try:
        with open(
            os.path.join(MY_FILE_DIR, "hydrophone_data", f"hydrophone_{formattedDateAndTime}.csv"),
            "a",
            encoding="UTF8",
            newline="",
        ) as f:
            writer = csv.writer(f)
            writer.writerow(hydrophoneData)

        with open(
            os.path.join(MY_FILE_DIR, "DSP_data", f"DSP_{formattedDateAndTime}.csv"),
            "a",
            encoding="UTF8",
            newline="",
        ) as f:
            writer = csv.writer(f)
            writer.writerow([rawSampleData, filteredSampleData, FFTData, peakData])
        print("Data Saved")
    except:
        print("ERROR saving data")

# initialize stuff
setup_teensy_communication()

while True:
    try:
        print("Receiving data from teensy...")
        get_data_from_teensy()
    except:
        print("ERROR: Receiving data did not work")

    print();

    # A little pause to not overwhelm the processor
    time.sleep(1)


# Also needs to be split into setup and update
# Infinite loop for reading data
count = 0
while False:
    try:
        timeStart = time.time()
        teensy.send_acknowledge_signal()

        while not teensy.check_if_available():
            """
            IMPORTANT!
            DO NOT have "time.sleep(x)" value SMALLER than 1 second!!!
            This will interrupt sampling by asking teensy if its available to many times
            If less than 1 second you risc crashing teensy to PC communication O_O
            """
            print("Pause time: " + str(time.time() - timeStart))
            time.sleep(1)
            if time.time() - timeStart > timeoutMax:
                break

        teensy.send_frequency_of_interest(frequencyOfInterest, frequencyVariance)
        hydrophoneData = teensy.get_raw_hydrophone_data()
        rawSampleData, filteredSampleData, FFTData, peakData = teensy.get_DSP_data()
        teensy.send_SKIP()  # Once we are done we NEED to send teensy a confirmation code so that it can continue to calculate with the new given information

        # Save data to csv files
        try:
            with open(
                os.path.join(MY_FILE_DIR, "hydrophone_data", f"hydrophone_{formattedDateAndTime}.csv"),
                "a",
                encoding="UTF8",
                newline="",
            ) as f:
                writer = csv.writer(f)
                writer.writerow(hydrophoneData)

            with open(
                os.path.join(MY_FILE_DIR, "DSP_data", f"DSP_{formattedDateAndTime}.csv"),
                "a",
                encoding="UTF8",
                newline="",
            ) as f:
                writer = csv.writer(f)
                writer.writerow([rawSampleData, filteredSampleData, FFTData, peakData])
            print("Data Saved")
        except:
            print("ERROR saving")
    except:
        print("ERROR")

    # For users to see that the loop is updating
    count += 1
    print(f"Try count: {count}")
    print()

    # A little pause to not overwhelm the processor
    time.sleep(1)
