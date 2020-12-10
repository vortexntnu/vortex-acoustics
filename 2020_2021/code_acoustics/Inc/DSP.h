/**
 * @file
 * @brief Small library that implements some
 * constants for the DSP 
 */
#ifndef ACOUSTICS_DSP_H
#define ACOUSTICS_DSP_H

/*
These libraries were from an earlier attempt, where I
tried to implement the DSP using proper FFT-libraries.
The problem was that the libraries were too large for 
the STM32, such that all of the RAM was "deleted". Tried
to implement the data using the built in FFT from ARM
*/
//#include "../alglib/src/fasttransforms.h"
//#include "../kiss_fft130/tools/kiss_fftr.h"

#include "arm_math.h"
#include "arm_const_structs.h" 
#include <stdint.h>


/**
 * @brief Namespace/wrapper for basic DSP functions. 
 */
namespace DSP_CONSTANTS{

/**
 * @brief Constants for sampling and DSP
 */
const int DMA_BUFFER_LENGTH = 4096;
const int WORKING_BUFFER_LENGTH = DMA_BUFFER_LENGTH/2;
const int IN_BUFFER_LENGTH = DMA_BUFFER_LENGTH;
const int FFT_SIZE = DMA_BUFFER_LENGTH;
const int IIR_SIZE = DMA_BUFFER_LENGTH;

/**
 * @note The frequencies (in robosub) will be 
 * in the range 20 KHz - 40 KHz.
 * The minimum sampling-frequency is therefore
 * 80 KHz, but increased to 100 KHz for safety.
 * 
 * The MAX_FREQUENCY and MIN_FREQUENCY are used 
 * for filtering the data in software. Every 
 * frequency above max or below min is assumed to
 * be noise
 * 
 * The SAMPLE_TIME is used to validate the signals
 */
const uint32_t SAMPLE_FREQUENCY = 100000;
const uint32_t MAX_FREQUENCY = SAMPLE_FREQUENCY / 2;
const uint32_t MIN_FREQUENCY = SAMPLE_FREQUENCY / 10;
const float32_t SAMPLE_TIME = 1/SAMPLE_FREQUENCY;   


/**
 * @brief An IIR-filter to filter the data before 
 * processing it. By using this filter, we should (hopefully)
 * eliminate unwanted frequencies. 
 * 
 * @warning Must have abs(filter) < 1 to prevent overflow
 * 
 * @warning The calculations for the filter should be automated
 * such that the parameters changes according to the desired 
 * frequencies. The filter's parameters are calculated using
 *      f0 = 30000      // Passband center
 *      Fs = 100000     // Sampling frequency
 *      f1 = 15000      // Lower cut-off frequency
 *      f2 = 45000      // Upper cut-off frequency
 *      Q  = 1          // Quality-factor
 *      K  = 1          // Gain
 * 
 * NOTE: For more information, see 
 * https://arm-software.github.io/CMSIS_5/DSP/html/group__BiquadCascadeDF1__32x64.html
 */
// Number of second order cascade-filters
const uint32_t num_stages = 1;
// Initial values for x[n-1], x[n-2], y[n-1] and y[n-2]
float32_t state_coefficients[4*num_stages] = {0.0, 0.0, 
        0.0, 0.0};
// Filter coefficients given as {b0, b1, b2, a0, a1}
float32_t filter_coefficients[5*num_stages] = {0.32227662, 
        0.0, -0.32227662, 0.41885608, 0.35544676};
// Initialization of the struct
const arm_biquad_casd_df1_inst_f32 IIR_filter = {
    .numStages = num_stages, .pState = &filter_coefficients,
    .pCoeffs = &filter_coefficients};

} // namespace DSP_CONSTANTS

#endif // ACOUSTICS_DSP_H