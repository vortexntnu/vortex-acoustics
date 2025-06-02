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
#include "teensy_udp.h"
#include "adc.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h> // for debugging prints, if needed
#include <stdlib.h>
#include <string.h> // for memset()

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus



typedef struct {
    size_t index;    // FFT bin index (optional, for debugging)
    q31_t amplitude; // Peak amplitude (converted to Q31)
    q31_t frequency; // Frequency (in Hz, represented in Q31 if needed)
    q15_t phase;     // Phase shift in Q15 format
} Peak;



extern q15_t samplesFiltered[SAMPLE_LENGTH];
extern q15_t FFTResultsMagnified[SAMPLE_LENGTH];
extern Peak peaks_buffer[SAMPLE_LENGTH];
extern size_t num_peaks;

/**
* @brief checks if signal with set frequency is found in current sample
* @param buffer_to_check Which adc buffer to check
* @return 1 if signal found
*         0 if signal is not found
*/
int dsp_found_signal(uint8_t buffer_to_check); 

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !DSP_H
