import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
 
# Load the dataset
file_path = "RawHydrophoneData/acoustics_data_2025-03-09_18:55:05.csv"
df = pd.read_csv(file_path)
 
# Convert the 'Time' column to a datetime format
df['Time'] = pd.to_datetime(df['Time'], format='%H:%M:%S.%f')
 
# Function to extract numerical values from array strings
def parse_array_string(array_str):
    try:
        array_str = array_str.replace("array('i', [", "").replace("])", "")
        return list(map(int, array_str.split(',')))  # Convert to a list of integers
    except Exception:
        return []
 
# Extract and process hydrophone data
hydrophone_signals = {f'Hydrophone{i}': df[f'Hydrophone{i}'].apply(parse_array_string) for i in range(1, 6)}
 
# Compute average signal amplitude for each timestamp
average_signals = {key: [np.mean(sample) if sample else 0 for sample in signals] 
                   for key, signals in hydrophone_signals.items()}
 
# Plot all hydrophone signals in one figure
plt.figure(figsize=(12, 6))
for hydrophone, avg_signal in average_signals.items():
    plt.plot(df['Time'], avg_signal, label=hydrophone)
 
plt.xlabel('Time')
plt.ylabel('Average Signal Amplitude')
plt.title('Hydrophone Signal Amplitude Over Time')
plt.legend()
plt.grid(True)
plt.xticks(rotation=45)
plt.show()