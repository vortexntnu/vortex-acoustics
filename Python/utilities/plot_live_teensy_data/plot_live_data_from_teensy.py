# Import libraries
import os
import sys
PARENT_DIR = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
MY_FILE_DIR = f"{PARENT_DIR}/utilities/plot_live_teensy_data/"
sys.path.append(PARENT_DIR)

import matplotlib.pyplot as plt
import matplotlib.gridspec as gridspec
import matplotlib.animation as animation
import pandas as pd
import ast
import glob
import os

# Important variables for later
SAMPLE_RATE = 430_000 # 430 kHz
MAX_FREQUENCY_TO_SHOW = 60_000 # 60 kHz
FPS = 1

# Make a good plot layout ==================================================
fig = plt.figure()
# Create an outer GridSpec for the two columns
outer_gs = gridspec.GridSpec(1, 2, figure=fig, width_ratios=[1, 1])
# Create an inner GridSpec for the first column
gs_hydrophone = gridspec.GridSpecFromSubplotSpec(5, 1, subplot_spec=outer_gs[0], hspace=0.1)
# Create an inner GridSpec for the second column, with height ratios for the 70%/30% split
gs_dsp = gridspec.GridSpecFromSubplotSpec(2, 1, subplot_spec=outer_gs[1], height_ratios=[7, 3], hspace=0.3)

hydrophoneAxis = [None]*5

# Add subplots in the first column for hydrophone data
for i in range(5):
    hydrophoneAxis[i] = fig.add_subplot(gs_hydrophone[i, 0], sharex=hydrophoneAxis[0] if i else None)
    hydrophoneAxis[i].label_outer()
fig.text(0.25, 0.965, 'Hydrophone Data', ha='center')

# Add subplots in the second column
FFTAxis = fig.add_subplot(gs_dsp[0])
filterAxis = fig.add_subplot(gs_dsp[1])

# Plot type so that the size is dynamic
plt.tight_layout()

# Select nice color pallet for graphs
colorSoftPurple = (168/255, 140/255, 220/255)
colorSoftBlue = (135/255, 206/255, 250/255)
colorSoftGreen = (122/255, 200/255, 122/255)

# Get the latest csv file names
listOfHydrophoneFiles = glob.glob(f"{MY_FILE_DIR}hydrophone_data/*.csv")
latestHydrophoneFile = max(listOfHydrophoneFiles, key=os.path.getctime)
listOfDSPFiles = glob.glob(f"{MY_FILE_DIR}DSP_data/*.csv")
latestDSPFile = max(listOfDSPFiles, key=os.path.getctime)

def display_live_data(frame):
    # Read latest data
    hydrophoneDataFrame = pd.read_csv(latestHydrophoneFile)
    DSPDataFrame = pd.read_csv(latestDSPFile)

    # Get latest hydrophone data
    hydrophoneData = [[],[],[],[],[]]
    hydrophoneData[0] = ast.literal_eval(hydrophoneDataFrame["hydrophone1"].tail(1).values[0])
    hydrophoneData[1] = ast.literal_eval(hydrophoneDataFrame["hydrophone2"].tail(1).values[0])
    hydrophoneData[2] = ast.literal_eval(hydrophoneDataFrame["hydrophone3"].tail(1).values[0])
    hydrophoneData[3] = ast.literal_eval(hydrophoneDataFrame["hydrophone4"].tail(1).values[0])
    hydrophoneData[4] = ast.literal_eval(hydrophoneDataFrame["hydrophone5"].tail(1).values[0])

    # Get DSP data
    rawData = ast.literal_eval(DSPDataFrame["raw_samples"].tail(1).values[0])
    filterData = ast.literal_eval(DSPDataFrame["filtered_samples"].tail(1).values[0])
    FFTData = ast.literal_eval(DSPDataFrame["FFT"].tail(1).values[0])
    peaksData = ast.literal_eval(DSPDataFrame["peaks"].tail(1).values[0])

    """
    Post process DSP data to desired scale and amount

    1. Convert FFTData to its corresponding frequency amount
    2. Cut out big FFT frequencies out as they are not relevant
    3. Cut out big peak frequencies as they are not relevant 
    """
    sampleLength = len(FFTData)
    FFTDataFrequency = [(i * (SAMPLE_RATE / sampleLength)) for i in range(sampleLength)]

    maxFrequencyIndex = int(MAX_FREQUENCY_TO_SHOW * sampleLength / SAMPLE_RATE)
    FFTDataFrequency = FFTDataFrequency[0:maxFrequencyIndex]
    FFTDataAmplitude = FFTData[0:maxFrequencyIndex]

    peaksData = [peak for peak in peaksData if peak[1] < MAX_FREQUENCY_TO_SHOW]
    peaksDataAmplitude = [subList[0] for subList in peaksData]
    peaksDataFrequency = [subList[1] for subList in peaksData]

    # Plot hydrophone data
    for i in range(5):
        xHydrophone = list(range(len(hydrophoneData[i])))
        hydrophoneAxis[i].clear()
        hydrophoneAxis[i].plot(xHydrophone, hydrophoneData[i], label=f"Hydrophone {i + 1}", color=colorSoftBlue, alpha=1)
        hydrophoneAxis[i].legend(loc="upper right", fontsize="xx-small")
    
    # Plot Filter response
    xRaw = list(range(len(rawData)))
    xFilter = list(range(len(filterData)))
    filterAxis.clear()
    filterAxis.set_title("Filter response")
    filterAxis.plot(xRaw, rawData, label="Raw", color=colorSoftBlue, alpha=0.5)
    filterAxis.plot(xFilter, filterData, label="Filter", color=colorSoftGreen, alpha=0.7)
    filterAxis.legend(loc="upper right", fontsize="xx-small")

    # Plot FFT data
    FFTAxis.clear()
    FFTAxis.set_title("FFT")
    FFTAxis.set_xlabel("Frequency [kHz]")
    FFTAxis.set_ylabel("Amplitude")
    FFTAxis.bar(FFTDataFrequency, FFTDataAmplitude, label="FFT", color=colorSoftPurple, alpha=1, width=500)
    FFTAxis.scatter(peaksDataFrequency, peaksDataAmplitude, label="Peaks", color="red", alpha=0.7, s=30, linewidths=1.4, marker="x")
    FFTAxis.legend(loc="upper right", fontsize="xx-small")

# Plotting live data
ani = animation.FuncAnimation(fig, display_live_data, interval=1000/FPS)
plt.show()