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
  #define __SOFTFP__                /* Allows assert by softwa                    */

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
 * @warning The sign of the filter-coefficients could possible
 * be wrong. Requires further testing to detect if the filter 
 * is stable/unstable with the given coefficients, as the 
 * documentation of the CMSISI-library is lacking at times...
 * 
 * @warning Filter coefficients are represented as fractional 
 * values and restricted to lie in the range [-1 +1)
 * 
 * @warning The calculations for the filter should be automated
 * such that the parameters changes according to the desired 
 * frequencies. The filter's parameters are calculated using
 *      Fs = 150000     Sampling frequency
 *      f0 = 30000      Passband center
 *      f1 = 15000      Lower cut-off frequency
 *      f2 = 45000      Upper cut-off frequency
 *      Q  = 1          Quality-factor
 *      K  = 1          Gain 
 * by using the calculator found at 
 * https://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/
 * 
 * The calculator reports the values as {a0, a1, a2, b1, b2}, instead
 * of {b0, b1, b2, a1, a2}. It is assumed that the calculator has 
 * swapped names between the denominator and the numerator, because of
 * the number of filter coefficients given. (The first filter coefficient
 * in the denominator is trivially assigned to 1). This must nonetheless 
 * be tested in the future
 * 
 * 
 * NOTE: For more information, see 
 * https://arm-software.github.io/CMSIS_5/DSP/html/group__BiquadCascadeDF1__32x64.html
 * 
 * NOTE: The function void TESTING::test_filter_coefficients(void)
 * is under development for testing the filter
 * 
 * @param num_stages            Number of second order cascade-filters. Determines the
 *                              filter order. Order = 2 * num_stages
 * 
 * @param state_coefficients    Initial values for x[n-1], x[n-2], y[n-1] and y[n-2]
 * 
 * @param filter_coefficients   Filter coefficients given as {b0, b1, b2, a1, a2}
 * 
 * @param IIR_filter            A struct describing a biquad DF1 IIR filter
 */
const uint32_t num_stages = 1;
const float32_t state_coefficients[4*num_stages] = {0.0, 0.0, 
        0.0, 0.0};
const float32_t filter_coefficients[5*num_stages] = {0.32227662, 
        0.0, -0.32227662, -0.41885608, 0.35544676};
const arm_biquad_casd_df1_inst_f32 IIR_filter = {
    .numStages = num_stages, .pState = &filter_coefficients[0],
    .pCoeffs = &filter_coefficients[0]};

} /* namespace DSP_CONSTANTS */

#endif /* ACOUSTICS_DSP_CONSTANTS_H */