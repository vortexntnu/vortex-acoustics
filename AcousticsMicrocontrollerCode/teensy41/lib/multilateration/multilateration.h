#ifndef MULTILATERATION_H
#define MULTILATERATION_H






#include <stddef.h>
#include "arm_math.h"
// #include <Arduino.h>


#define NUM_HYDROPHONES 5
#define NUM_DIMENSIONS 3

#define SOUND_SPEED  1480.0 //[m/s]
#define SAMPLING_FREQ  300000     //[Hz]


// OBS: use same x, y, z system as autonomous
const float32_t hydrophonePositions[NUM_HYDROPHONES][NUM_DIMENSIONS] = {
    {0.00f, 0.00f, 0.00f}, // Hydrophone 0: {x, y, z}
    {0.11f, 0.31f, 0.10f},  // Hydrophone 1: {x, y, z}
    {0.0f, -0.24f, 0.0f},   // Hydrophone 2: {x, y, z}
    {0.5f, -0.1f, 0.4f},    // Hydrophone 3: {x, y, z}
    {0.4f, 0.0f, -0.4f}     // Hydrophone 4: {x, y, z}
};


size_t find_peak_index(q15_t* signal, int size);


arm_status tdoa_multilateration(const float32_t hydrophone_array[4][3], const float32_t TDOA[4], float32_t* result); 



#endif // !MULTILATERATION_H
