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



real = read_csv_numbers("AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/real_signal.csv")


cross = read_csv_numbers("AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/corr_values.csv")

time_cross = np.linspace(-len(cross)/2, len(cross)/2, len(cross))

plt.plot(real)
plt.figure()
plt.plot(time_cross, cross)


plt.show()