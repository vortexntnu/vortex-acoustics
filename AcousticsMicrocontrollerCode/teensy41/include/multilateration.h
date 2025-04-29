#ifndef MULTILATERATION_H
#define MULTILATERATION_H






#include <stddef.h>
#include "arm_math.h"
#include "adc.h"
#include "dsp.h"
// #include <Arduino.h>

#define MAX_LAG (2 * RAW_HYDROPHONE_SIZE - 1)

#define TDOA_DATA_LENGTH 5                           // TODO: Should be moved into multilateration library once that is operational
#define POSITION_DATA_LENGTH 3 + 1                   // TODO: Should be moved into multilateration library once that is operational


#ifdef __cplusplus

extern "C"{


#endif // __cplusplus

const int32_t NUM_HYDROPHONES = 5;
const int32_t NUM_DIMENSIONS = 3;

const float32_t SOUND_SPEED = 1480.0; //[m/s]
const int SAMPLING_FREQ = 300000;     //[Hz]


// OBS: use same x, y, z system as autonomous
const float32_t hydrophonePositions[NUM_HYDROPHONES][NUM_DIMENSIONS] = {
    {-0.11f, 0.31f, 0.10f}, // Hydrophone 0: {x, y, z}
    {0.11f, 0.31f, 0.10f},  // Hydrophone 1: {x, y, z}
    {0.0f, -0.24f, 0.0f},   // Hydrophone 2: {x, y, z}
    {0.5f, -0.1f, 0.4f},    // Hydrophone 3: {x, y, z}
    {0.4f, 0.0f, -0.4f}     // Hydrophone 4: {x, y, z}
};


extern float32_t timeDifferenceOfArrival[TDOA_DATA_LENGTH]; // time difference for hydrophone 1, 2, 3, 4, 5 [s]
extern float32_t soundLocation[POSITION_DATA_LENGTH];       // X, Y, Z [m]



int find_pinger_position(void);

#ifdef __cplusplus

}

#endif // __cplusplus


#endif // !MULTILATERATION_H
