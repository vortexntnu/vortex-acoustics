# Setting up libraries
import os
import sys
PARENT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
MY_FILE_DIR = f"{PARENT_DIR}/utilities/plot_live_teensy_data/"
sys.path.append(PARENT_DIR)

from datetime import datetime
import csv

import time
import random

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

# Functions ==================================================
def randomDataHydrophones(minVal, maxVal):
    hydrophoneNr = 5
    lengthOfList = 3072
    buffer = [[], [], [], [], []]

    for i in range(hydrophoneNr):
        for u in range(lengthOfList):
            randomValue = random.randint(minVal, maxVal)
            buffer[i].append(randomValue)
    
    return buffer

def randomDataDSP(minVal, maxVal):
    lengthOfList = 1024
    buffer = []

    for i in range(lengthOfList):
        randomValue = random.randint(minVal, maxVal)
        buffer.append(randomValue)
    
    return buffer

def randomPeaks(minValA, maxValA, minValF, maxValF):
    lengthOfList = random.randint(0, 100)
    buffer = []

    for i in range(lengthOfList):
        tempList = []
        tempList.append(random.randint(minValA, maxValA))
        tempList.append(random.randint(minValF, maxValF))
        tempList.append(random.randint(-1000, 1000))

        buffer.append(tempList)
    
    return buffer

# Infinite loop for reading data
while True:
    try:
        hydrophoneData = randomDataHydrophones(-500, 500)
        rawSampleData = randomDataDSP(-500, 500)
        filteredSampleData = randomDataDSP(-1_000, 1_000)
        FFTData = randomDataDSP(0, 10_000)
        peakData = randomPeaks(0, 10_000, 0, 400_000)

        # Save data to csv files
        with open(f"{MY_FILE_DIR}hydrophone_data/hydrophone_{formattedDateAndTime}.csv", "a", encoding="UTF8", newline="") as f:
            writer = csv.writer(f)
            writer.writerow(hydrophoneData)

        with open(f"{MY_FILE_DIR}DSP_data/DSP_{formattedDateAndTime}.csv", "a", encoding="UTF8", newline="") as f:
            writer = csv.writer(f)
            writer.writerow([rawSampleData, filteredSampleData, FFTData, peakData])
        print("Data Saved")
    except:
        print("ERROR")

    time.sleep(1)