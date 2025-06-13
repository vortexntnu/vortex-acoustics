#ifndef DSP_H
#define DSP_H

// How fast the ADC samples, important to know for FFT, the max is 510 kHz, HOWEVER for some reason ADC can not go max, real value is lower at:
#include "arm_math.h"
#include "adc.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h> // for debugging prints, if needed
#include <stdlib.h>
#include <string.h> // for memset()

#define SAMPLE_RATE 430000 // 430.0 kHz
#define SAMPLE_LENGTH 1024

#define FILTER_AMPLIFICATION 2

#define BITSHIFT 9
#define SCALE_FACTOR 1000.0
#define FREQUENCY_LIMIT 60000

#define PEAK_THRESHOLD 1000

#define Q15_SCALE 32768.0f

#define FLOAT_TO_Q15(x) ((q15_t)((x) >= 0.0f ? ((x) * Q15_SCALE + 0.5f) : ((x) * Q15_SCALE - 0.5f)))

#define NUM_STAGES 2
#define BLOCK_SIZE 256

#define fOrder 9
#define fOrder2 2

// Globals or #defines (set these once):
#define Fs 50000.0f       // sample rate
#define N SAMPLE_LENGTH   // FFT length
#define f_ping 12000.0f   // your target ping freq in Hz
#define deltaF 200.0f     // allow ±200 Hz of drift
#define SIDE_BAND_WIDTH 8 // bins on each side for noise floor
#define MARGIN_Q15 0x1000 // ~+6 dB margin

// FFT flags
#define BIT_REVERSE 1 
#define IFFT_FLAG 0
// Coefficients for 430kHz sampling and 50kHz cut-off
const float32_t sos_floats[NUM_STAGES][6] = {{0.00802494, 0.01604989, 0.00802494, 1, -0.92145, 0.23722397}, {1, 2, 1, 1, -1.18653637, 0.59315345}};


#ifdef __cplusplus

extern "C" {

#endif // __cplusplus




extern q15_t samples_filtered[SAMPLE_LENGTH];
extern q15_t fft_results_magnified[SAMPLE_LENGTH];
extern size_t num_peaks;


void filter_butterworth_4th_order_init(void); 

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
