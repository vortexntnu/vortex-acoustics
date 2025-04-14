import numpy as np
import matplotlib.pyplot as plt

def write_csv_columns(filename, headers, arrays):
    # Ensure all arrays have the same length
    if not all(len(arr) == len(arrays[0]) for arr in arrays):
        raise ValueError("All arrays must have the same length.")
    
    # Stack arrays column-wise
    data = np.column_stack(arrays)
    
    # Create a header string (comma separated)
    header_str = ','.join(headers)
    
    # Save the data with np.savetxt.
    # Setting comments='' ensures that the header is not prefixed with a comment character.
    np.savetxt(filename, data, delimiter=',', header=header_str, comments='')


def read_csv_columns(filename):
    # Read the header line manually
    with open(filename, 'r') as f:
        header_line = f.readline().strip()
    headers = header_line.split(',')
    
    # Load the remaining data using np.loadtxt (skip the header row)
    data = np.loadtxt(filename, delimiter=',', skiprows=1)
    
    # Extract each column as a separate NumPy array
    arrays = [data[:, i] for i in range(data.shape[1])]
    return headers, arrays



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


def add_lag(liste, n):
    if n >= 0:
        # For positive lag: prepend zeros and remove the last n items
        return [0] * n + liste[:len(liste) - n]
    else:
        # For negative lag: remove the first -n items and append zeros at the end
        m = -n
        return liste[m:] + [0] * m

def add_time_lag(liste, time, sampling_frequenzy=60000):
    samples = int(sampling_frequenzy*time)
    print(samples)
    return add_lag(liste, samples)




time_difference = [0.0010929141752747853, 0.0011729570319257523, 0.0011812076933235053, 0.0012324018128422195]

if __name__ == "__main__":

    main = read_csv_numbers("AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/signal_values/hydrophone_0.csv")

    hydrophone_data = [main.copy()]
    for tdoa in time_difference:
        hydrophone_data.append(add_time_lag(main, tdoa))

        
    write_csv_columns("AcousticsMicrocontrollerCode/teensy41/lib/Thomas_impl/Thomas_impl_2/signal_values/hydrophone.csv", ["Main hydrophone", "Hydrophone_1", "Hydrophone_2", "Hydrophone_3", "Hydrophone_4"], hydrophone_data)