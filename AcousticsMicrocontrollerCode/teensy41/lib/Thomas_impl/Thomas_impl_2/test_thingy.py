import numpy as np
import matplotlib.pyplot as plt





def read_csv_numbers(file_path):
    """
    Reads a CSV file containing comma-separated floating point numbers
    and returns a list of floats.

    Parameters:
        file_path (str): The path to the CSV file.

    Returns:
        List[float]: A list of floating point numbers read from the file.
    """
    with open(file_path, 'r') as file:
        # Read the entire file content as a string.
        data = file.read()
    # Split the string by commas, remove extra whitespace, and convert each token to float.
    numbers = [float(token.strip()) for token in data.split(',') if token.strip()]
    return numbers



real = read_csv_numbers("/home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/signal_values/hydrophone_0.csv")
real_laged = read_csv_numbers("/home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/signal_values/hydrophone_1.csv")


cross = read_csv_numbers("/home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/signal_values/corr_values.csv")

time_cross = np.linspace(-len(cross)/2, len(cross)/2, len(cross))



# Create subplots: one for each signal.
fig, axs = plt.subplots(3, 1, figsize=(10, 8), constrained_layout=True)

# Plot the first hydrophone signal.
axs[0].plot(real, color='blue')
axs[0].set_title("Hydrophone 0 Signal")
axs[0].set_ylabel("Amplitude")
axs[0].grid(True)

# Plot the second (lagged) hydrophone signal.
axs[1].plot(real_laged, color='green')
axs[1].set_title("Hydrophone 1 Signal (Lagged)")
axs[1].set_ylabel("Amplitude")
axs[1].grid(True)

# Plot the cross-correlation.
axs[2].plot(time_cross, cross, color='red')
axs[2].set_title("Cross-Correlation")
axs[2].set_xlabel("Time Lag")
axs[2].set_ylabel("Correlation")
axs[2].grid(True)

plt.show()



plt.show()