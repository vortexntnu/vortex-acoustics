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
import glob
import os

import random

# Important variables for later
SAMPLE_RATE = 430_000 # 430 kHz
MAX_FREQUENCY_TO_SHOW = 60_000 # 60 kHz

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

# Get the latest csv file
listOfHydrophoneFiles = glob.glob(f"{MY_FILE_DIR}hydrophone_data/*.csv")
latestHydrophoneFile = max(listOfHydrophoneFiles, key=os.path.getctime)
listOfDSPFiles = glob.glob(f"{MY_FILE_DIR}hydrophone_data/*.csv")
latestDSPFile = max(listOfDSPFiles, key=os.path.getctime)
hydrophoneDataFrame = pd.read_csv(latestHydrophoneFile)
DSPDataFrame = pd.read_csv(latestDSPFile)

def display_live_data(frame):
    # Variables
    xHydrophone = []
    hydrophoneData = [[], [], [], [], []]
    xFilter = []
    sampleData = []
    filterData = []
    xFFT = []
    FFTData = []
    xPeaks = []
    peaksData = []

    # Get hydrophone data
    for i in range(100):
        xHydrophone.append(i)
        hydrophoneData[0].append(random.randint(-100, 100))
        hydrophoneData[1].append(random.randint(-50, 50))
        hydrophoneData[2].append(random.randint(-100, 100))
        hydrophoneData[3].append(random.randint(-100, 100))
        hydrophoneData[4].append(random.randint(-100, 100))

    # Get DSP data
    for i in range(50):
        xFilter.append(i)
        sampleData.append(random.randint(-100, 100))
        filterData.append(random.randint(-200, 200))
    for i in range(30):
        xFFT.append(i)
        FFTData.append(random.randint(0, 6000))
    for i in range(10):
        xPeaks.append(random.randint(0, 30))
        peaksData.append(random.randint(0, 6000))

    # Plot hydrophone data
    for i in range(5):
        hydrophoneAxis[i].clear()
        hydrophoneAxis[i].plot(xHydrophone, hydrophoneData[i], label=f"Hydrophone {i + 1}", color="blue", alpha=0.3)
        hydrophoneAxis[i].legend(loc="upper right", fontsize="xx-small")
    
    # Plot Filter response
    filterAxis.clear()
    filterAxis.set_title("Filter response")
    filterAxis.plot(xFilter, sampleData, label="Raw", color="blue", alpha=0.3)
    filterAxis.plot(xFilter, filterData, label="Filter", color="green", alpha=0.5)
    filterAxis.legend(loc="upper right", fontsize="xx-small")

    # Plot FFT data
    FFTAxis.clear()
    FFTAxis.set_title("FFT")
    FFTAxis.set_xlabel("Frequency [kHz]")
    FFTAxis.set_ylabel("Amplitude")
    FFTAxis.bar(xFFT, FFTData, label="FFT", color="blue", alpha=0.7, width=0.8)
    FFTAxis.scatter(xPeaks, peaksData, label="Peaks", color="red", alpha=0.7, s=30, linewidths=1.4, marker="x")
    FFTAxis.legend(loc="upper right", fontsize="xx-small")

# Plotting live data
ani = animation.FuncAnimation(fig, display_live_data, interval=1000)
plt.show()