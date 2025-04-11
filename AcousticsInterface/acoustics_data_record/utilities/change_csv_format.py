from display_acoustics_data_live import * 
import pandas as pd
import os
import glob
import time

acoustics_data = get_acoustics_data()

# # Your 5 hydrophone data lists inside acoustics_data
hydrophone_data = [
    acoustics_data[0],
    acoustics_data[1],
    acoustics_data[2],
    acoustics_data[3],
    acoustics_data[4],
]

df = pd.DataFrame({
    'Hydrophone 1': hydrophone_data[0],
    'Hydrophone 2': hydrophone_data[1],
    'Hydrophone 3': hydrophone_data[2],
    'Hydrophone 4': hydrophone_data[3],
    'Hydrophone 5': hydrophone_data[4]
})

timestamp = time.strftime('%Y-%m-%d_%H:%M:%S')
data_file_name = 'readable_acoustics_data_' + timestamp + '.csv'

# Folder: go up from 'utilities/' and into 'readable_acoustics_data/'
output_dir = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'readable_acoustics_data'))
os.makedirs(output_dir, exist_ok=True)
print(output_dir)

# Full file path
output_path = os.path.join(output_dir, data_file_name)

df.to_csv(output_path, index=False)
