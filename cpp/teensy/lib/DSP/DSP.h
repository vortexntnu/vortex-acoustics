#include "arm_const_structs.h"
#include "arm_math.h"
#include <Arduino.h>

#define SAMPLE_RATE 510000
#define SAMPLE_LENGTH 1024
#define BITSHiFT 9
#define SCALE_FACTOR 1000.0
// The upper frequency limit of the frequency band we actually want to check
#define FREQUENCY_LIMIT 60000

q15_t* filter_butterwort_9th_order_50kHz(int16_t* samplesRaw);

q15_t* FFT_raw(q15_t* samples);

q15_t* FFT_mag(q15_t* resultsRaw);

q31_t** peak_detection(q15_t* resultsRaw, q15_t* results);
