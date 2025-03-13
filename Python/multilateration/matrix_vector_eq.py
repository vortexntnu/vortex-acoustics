#This doesn't work yet
import numpy as np
 
hydrophone_0 = [0,0,0]
hydrophone_1 = [0.2,0.2,0]
hydrophone_2 = [0,0.2,0.2]
hydrophone_3 = [0.2,0,0.2]
hydrophone_4 = [0,0,0.2]
 
 
# d0i == c*TDOhydrophone_0[0]
 
time_differences = [
    -0.00011895594115311958,
    -0.00017927086518018065,
    -0.0001319085461932688,
    -9.454954236107109e-05
]
 
 
A = np.array([
    [hydrophone_0[0] - hydrophone_1[0], hydrophone_0[1] - hydrophone_1[1], hydrophone_0[2] - hydrophone_1[2], time_differences[0]],
    [hydrophone_0[0] - hydrophone_2[0], hydrophone_0[1] - hydrophone_2[1], hydrophone_0[2] - hydrophone_2[2], time_differences[1]],
    [hydrophone_0[0] - hydrophone_3[0], hydrophone_0[1] - hydrophone_3[1], hydrophone_0[2] - hydrophone_3[2], time_differences[2]],
    [hydrophone_0[0] - hydrophone_4[0], hydrophone_0[1] - hydrophone_4[1], hydrophone_0[2] - hydrophone_4[2], time_differences[3]],
])
 
 
b = np.array([
    0.5 * (hydrophone_0[0]**2 - hydrophone_1[0]**2 + hydrophone_0[1]**2 - hydrophone_1[1]**2 + hydrophone_0[2]**2 - hydrophone_1[2]**2 + time_differences[0]**2),
    0.5 * (hydrophone_0[0]**2 - hydrophone_2[0]**2 + hydrophone_0[1]**2 - hydrophone_2[1]**2 + hydrophone_0[2]**2 - hydrophone_2[2]**2 + time_differences[1]**2),
    0.5 * (hydrophone_0[0]**2 - hydrophone_3[0]**2 + hydrophone_0[1]**2 - hydrophone_3[1]**2 + hydrophone_0[2]**2 - hydrophone_3[2]**2 + time_differences[2]**2),
    0.5 * (hydrophone_0[0]**2 - hydrophone_4[0]**2 + hydrophone_0[1]**2 - hydrophone_4[1]**2 + hydrophone_0[2]**2 - hydrophone_4[2]**2 + time_differences[3]**2)
])
 
# solve
x = np.linalg.lstsq(A, b)[0]
 
print("Solution vector [x, y, z, d0]:", x)