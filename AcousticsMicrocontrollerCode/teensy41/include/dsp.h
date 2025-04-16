#ifndef DSP_H
#define DSP_H

// How fast the ADC samples, important to know for FFT, the max is 510 kHz, HOWEVER for some reason ADC can not go max, real value is lower at:
#include "arm_math.h"
#define SAMPLE_RATE 430000 // 430.0 kHz
// How many samples we want from ADC
#define SAMPLE_LENGTH 1024

// How much should the signal should be amplified before filtering it
#define FILTER_AMPLIFICATION 2

// For FFT to shift bits
#define BITSHIFT 9
// How more pronounced the peaks of a frequencies will be in contrast with the lower
// dont have it to high as this will make the noise have high peaks as well
#define SCALE_FACTOR 1000.0
// The upper frequency limit of the frequency band we actually want to check
#define FREQUENCY_LIMIT 60000

// A manual variable to filter out small peaks that don't manage to get over the threshold, so called "fake peaks"
#define PEAK_THRESHOLD 1000

#include "Include/arm_const_structs.h"
#include "Include/arm_math.h"
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>  // for memset()
#include <stdio.h>   // for debugging prints, if needed

#ifdef __cplusplus

extern "C"{

#endif // __cplusplus




typedef struct {
    size_t index;    // FFT bin index (optional, for debugging)
    q31_t amplitude; // Peak amplitude (converted to Q31)
    q31_t frequency; // Frequency (in Hz, represented in Q31 if needed)
    q15_t phase;     // Phase shift in Q15 format
} Peak;

q15_t* filter_butterwort_9th_order_50kHz(int16_t* samplesRaw);
q15_t* filter_butterwort_2th_order_50kHz(int16_t* samplesRaw);
void filter_butterwort_1th_order_50kHz(int16_t* samplesRaw, q15_t* samples);

void FFT_raw(q15_t* samples, q15_t* resultsRaw);
void FFT_mag(q15_t* resultsRaw, q15_t* results);

Peak* peak_detection(const q15_t *resultsRaw, const q15_t *results, size_t *out_num_peaks); 

float32_t phaseQ31_to_radianFloat32(q31_t phaseQ15);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !DSP_H
