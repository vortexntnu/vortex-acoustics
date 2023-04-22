#include "arm_const_structs.h"
#include "arm_math.h"
#include <Arduino.h>

// How fast the ADC samples, important to know for FFT, the max is 510 kHz, HOWEVER for some reason ADC can not go max, real value is lower at:
#define SAMPLE_RATE 92000 // 92.0 kHz
// How many samples we want from ADC
#define SAMPLE_LENGTH 1024

// For FFT to shift bits
#define BITSHIFT 9
// How more pronounced the peaks of a frequencies will be in contrast with the lower
// dont have it to high as this will make the noise have high peaks as well
#define SCALE_FACTOR 1000.0 
// The upper frequency limit of the frequency band we actually want to check
#define FREQUENCY_LIMIT 60000 

// A manual variable to filter out small peaks that don't manage to get over the threshold, so called "fake peaks"
#define PEAK_THRESHOLD 200


q15_t* filter_butterwort_9th_order_50kHz(int16_t* samplesRaw);

q15_t* FFT_raw(q15_t* samples);
q15_t* FFT_mag(q15_t* resultsRaw);

q31_t** peak_detection(q15_t* resultsRaw, q15_t* results);

float32_t phaseQ31_to_radianFloat32(q31_t phaseQ15);
