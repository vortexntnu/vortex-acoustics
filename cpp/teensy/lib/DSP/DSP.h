#include <Arduino.h>
#include "arm_math.h"
#include "arm_const_structs.h"

#define SAMPLE_RATE 510000
#define SAMPLE_LENGTH 1024
#define BITSHiFT 9
#define SCALE_FACTOR 1000.0
#define FREQUENCY_LIMIT 60000 //The upper frequency limit of the frequency band we actually want to check

q15_t *filterButterWorth9thOrder50kHz(int16_t *samplesRaw);

q15_t *magFFT(q15_t *samples);

q31_t **peakDetection(q15_t *results, int16_t countTest);

