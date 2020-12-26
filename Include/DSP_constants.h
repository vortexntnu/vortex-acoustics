/**
 * @file
 * @brief Small library that implements some
 * constants for the DSP 
 */
#ifndef ACOUSTICS_DSP_CONSTANTS_H
#define ACOUSTICS_DSP_CONSTANTS_H

/**
 * @brief Important defines that unlocks multiple of the functions
 * and defines from the include-files
 */
#ifndef STM32_DEFINES
#define STM32_DEFINES
  
  #define STM32F767xx               /* Current MCU used for the project           */
  #define ARM_MATH_CM7              /* Processor-version used on the STM32        */
  #define _USE_MATH_DEFINES         /* Gives access to math-defines such as M_PI  */
  #define __SOFTFP__                /* Allows assert by software                  */
  #define HAL_ADC_MODULE_ENABLED    /* Enables the HAL_ADC                        */

#endif /* STM32_DEFINES */

#include <stdint.h>
#include <math.h>
#include <cmath>
#include <utility>
#include "arm_math.h"
#include "arm_const_structs.h" 


/**
 * @brief Defines that indicate the setup of the hardware on the AUV
 * 
 * NOTE: It may be possible to transfer more of the global/extern variables
 * over to macros. Will require a lot of work, and pherhaps not be optimal
 * in the short run, however would be optimzed better by the compiler and
 * could result in a better code
 */
#ifndef VORTEX_AUV_SETUP
#define VORTEX_AUV_SETUP

  #define NUM_HYDROPHONES 3           /* Number of hydrophones used on the AUV      */

#endif /* VORTEX_AUV_SETUP */

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
const uint32_t SAMPLE_FREQUENCY = 150000;
const float32_t SAMPLE_TIME = (float32_t) 1 / SAMPLE_FREQUENCY;   


/**
 * @brief An IIR-filter to filter the data before 
 * processing it. By using this filter, we should (hopefully)
 * eliminate unwanted frequencies. 
 * 
 * @warning Must have abs(filter) < 1 to prevent overflow
 * 
 * @warning Filter coefficients are represented as fractional 
 * values and restricted to lie in the range [-1, +1)
 * 
 * @warning The filter is designed with the filterdesigner 
 * in MATLAB, using
 *      Fs = 150000     Sampling frequency
 *      f0 = 30000      Passband center
 *      f1 = 15000      Lower cut-off frequency
 *      f2 = 45000      Upper cut-off frequency
 *      Q  = 1          Quality-factor
 *      K  = 1          Gain 
 * 
 * The filter's transferfunction is given as 
 *      H(z) = B(z) * 1 / A(z)
 * where
 *      B(z) = b0 + b1 * z^(-1) + b2 * z^(-2) + b3 * z^(-3) + ...
 *      A(z) = 1 + a1 * z^(-1) + a2 * z^(-2) + a3 * z^(-3) + ...
 * 
 * NOTE: Since the function uses a biquad-filter, every filter with order > 2 is split 
 * into multiple serial second-order filters, leaving at most one filter with order = 1 
 * 
 * Example:
 *      4th order filter with the original filter function given as
 *      H(z) = B(z)/A(z) with
 *
 *          B(z) = b0 + b1 * z^(-1) + b2 * z^(-2) + b3 * z^(-3) + b4 * z^(-4)
 *          A(z) = 1 + a1 * z^(-1) + a2 * z^(-2) + a3 * z^(-3) + a4 * z^(-4)
 *    
 *      is instead given as H(z) = B_1(z)/A_1(z) * B_2(z)/A_2(z), where
 *      H_1(z) = B_1(z)/A_1(z)
 * 
 *          B_1(z) = b10 + b11 * z^(-1) + b12 * z^(-2)
 *          A_1(z) = 1 + a11 * z^(-1) + a12 * z^(-2)
 * 
 *      and equivalent for B_2(z) and A_2(z)
 * 
 * NOTE: For more information, see 
 * https://arm-software.github.io/CMSIS_5/DSP/html/group__BiquadCascadeDF1__32x64.html
 * 
 * NOTE: The MATLAB-script used to test the filter can be found under Resource/MATLAB
 * 
 * @param num_stages            Number of second order cascade-filters. Determines the
 *                              filter order. Order = 2 * num_stages
 * 
 * @param state_coefficients    Initial values for x[n-1], x[n-2], ..., x[n-(2*num_stages)]
 *                              y[n-1], y[n-2], ..., y[n-(2*num_stages)]
 * 
 * @param filter_coefficients   Filter coefficients given as {b10, b11, b12, a11, a12
 *                              b20, b21, b22, a21, a22, ...}
 * 
 * @param IIR_filter            A struct describing a biquad DF1 IIR filter
 * 
 * @param post_shift            Scaling-factor to keep all filter coefficients in the range
 *                              [-1, +1). Scales all of the filter coefficients! 
 *                              IMPORTANT: post_shift is a factor of 2, so post_shift = 1 
 *                              scales by 2
 */
const uint32_t num_stages = 2;
const float32_t state_coefficients[4 * num_stages] = {0.0, 0.0, 
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const float32_t filter_coefficients[5 * num_stages] = {
        0.22725101, 0.0, -0.22725101,   /* Numerator filter 1 */ 
        -0.58226175, 0.26593912,        /* Denominator filter 1 */
        0.45450202, 0.0, -0.45450202,   /* Numerator filter 2 */
        0.25944456, 0.36815892}         /* Denominator filter 2 */
const uint8_t post_shift = 1; 
const arm_biquad_casd_df1_inst_f32 IIR_filter = {
    .numStages = num_stages, 
    .pState = &filter_coefficients[0],
    .pCoeffs = &filter_coefficients[0], 
    .postShift = post_shift};

} /* namespace DSP_CONSTANTS */

#endif /* ACOUSTICS_DSP_CONSTANTS_H */