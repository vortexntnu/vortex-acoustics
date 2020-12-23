/**
 * @file
 * @brief Small library that implements some
 * constants for the DSP 
 */
#ifndef ACOUSTICS_DSP_CONSTANTS_H
#define ACOUSTICS_DSP_CONSTANTS_H

#define ARM_MATH_CM7
#define _USE_MATH_DEFINES
#define __SOFTFP__

#include <stdint.h>
#include <math.h>
#include <cmath>
#include <utility>
#include "arm_math.h"
#include "arm_const_structs.h" 


/**
 * @brief Namespace/wrapper for basic DSP functions. 
 */
namespace DSP_CONSTANTS{

/**
 * @brief Constants for sampling and DSP.
 * These values could also be defines/macros
 */
const uint16_t DMA_BUFFER_LENGTH = 4096;
const uint16_t WORKING_BUFFER_LENGTH = DMA_BUFFER_LENGTH / 2;
const uint16_t IN_BUFFER_LENGTH = DMA_BUFFER_LENGTH;
const uint16_t FFT_SIZE = DMA_BUFFER_LENGTH;
const uint16_t IIR_SIZE = DMA_BUFFER_LENGTH;


/**
 * @note The frequencies (in robosub) will be in the 
 * range 20 KHz - 40 KHz. The minimum sampling-frequency 
 * is therefore 80 KHz, but should minimum be 100 KHz due to
 * safety. The sampling-frequency is set to 112500 since that
 * is the closest the ADC can sample
 * 
 * The SAMPLE_TIME is used to validate the signals
 */
const uint32_t SAMPLE_FREQUENCY = 1125000;
const float32_t SAMPLE_TIME = (float32_t) 1 / SAMPLE_FREQUENCY;   


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
 * 
 * NOTE: The function void TESTING::test_filter_coefficients(void)
 * is under development for testing the filter
 * 
 * @param num_stages            Number of second order cascade-filters
 * 
 * @param state_coefficients    Initial values for x[n-1], x[n-2], y[n-1] and y[n-2]
 * 
 * @param filter_coefficients   Filter coefficients given as {b0, b1, b2, a0, a1}
 * 
 * @param IIR_filter            A struct describing a DF1 IIR filter
 */
const uint32_t num_stages = 1;
const float32_t state_coefficients[4*num_stages] = {0.0, 0.0, 
        0.0, 0.0};
const float32_t filter_coefficients[5*num_stages] = {0.32227662, 
        0.0, -0.32227662, 0.41885608, 0.35544676};
const arm_biquad_casd_df1_inst_f32 IIR_filter = {
    .numStages = num_stages, .pState = &filter_coefficients[0],
    .pCoeffs = &filter_coefficients[0]};

} // namespace DSP_CONSTANTS

#endif // ACOUSTICS_DSP_CONSTANTS_H