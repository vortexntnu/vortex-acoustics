#ifndef MULTILATERATION_H
#define MULTILATERATION_H

#include "adc.h"
#include "arm_math.h"
#include "dsp.h"
#include <stddef.h>
#include <stdint.h>

#define MAX_LAG (2 * RAW_HYDROPHONE_SIZE - 1)

#define TDOA_DATA_LENGTH 5
#define POSITION_DATA_LENGTH 3 + 1

#define NUM_HYDROPHONES 5
#define NUM_DIMENSIONS 3

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus


extern float32_t timeDifferenceOfArrival[TDOA_DATA_LENGTH]; // time difference for hydrophone 1, 2, 3, 4, 5 [s]
extern float32_t soundLocation[POSITION_DATA_LENGTH];       // X, Y, Z [m]

int find_pinger_position(void);

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // !MULTILATERATION_H
