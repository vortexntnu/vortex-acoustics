"""
This program let's you check if your solution gives an accurate pinger location given
the time differences of arrival. You can choose your own pinger locations.
"""
import math

#Speed of sound underwater: 1500 m/s
v=1500

#Hydrophone positions. 20cm between the hydrophones is optimal. Can have 20-40cm.
hydrophone_0 = [0,0,0]
hydrophone_1 = [0.2,0.2,0]
hydrophone_2 = [0,0.2,0.2]
hydrophone_3 = [0.2,0,0.2]
hydrophone_4 = [0,0,0.2]

#Pinger position
pinger_pos = [1.23,2.64,3.03]

#Positive TDOA if the reference hydrophone is closest to the pinger
def calculate_tdoa(ref, pos):
    return (1/v)*(math.sqrt((pos[0]-pinger_pos[0])**2+(pos[1]-pinger_pos[1])**2+(pos[2]-pinger_pos[2])**2)-math.sqrt((ref[0]-pinger_pos[0])**2+(ref[1]-pinger_pos[1])**2+(ref[2]-pinger_pos[2])**2))

#Time difference between hydrophone 0 and hydrophone number i 
tdoa_01 = calculate_tdoa(hydrophone_0, hydrophone_1)
tdoa_02 = calculate_tdoa(hydrophone_0, hydrophone_2)
tdoa_03 = calculate_tdoa(hydrophone_0, hydrophone_3)
tdoa_04 = calculate_tdoa(hydrophone_0, hydrophone_4)

print(tdoa_01, tdoa_02, tdoa_03, tdoa_04)

def compare_answer(calculated_coordinates):
    print("Actual pinger position:", pinger_pos)
    print("Calculated pinger position:", calculated_coordinates)
    print("Your error was", [pinger_pos[0]-calculated_coordinates[0], pinger_pos[1]-calculated_coordinates[1], pinger_pos[2]-calculated_coordinates[2]])








