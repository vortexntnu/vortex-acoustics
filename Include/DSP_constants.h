/**
 * @file
 * @brief Small library that implements some
 * constants for the DSP 
 */
#ifndef ACOUSTICS_DSP_CONSTANTS_H
#define ACOUSTICS_DSP_CONSTANTS_H

#include <stdint.h>
#include <math.h>
#include <cmath>
#include <utility>
#include "arm_math.h"
#include "arm_const_structs.h" 

/**
 * @brief Duplicating code is terrible code-practice! These constants somehow 
 * throws an error during compilation. Therefore tried to move the variables 
 * to eliminate such error 
 */
float32_t pi   = 3.14159265358979323846;        /* PI (obviously)               */
float32_t pi_2 = 1.57079632679489661923;        /* PI / 2                       */

/**
 * @brief Namespace/wrapper for basic DSP functions. 
 */
namespace DSP_CONSTANTS{

/**
 * @brief Constants for sampling and DSP
 *     
 * @param IN_BUFFER_LENGHT     Number of measurements for each hydrophone. 
 *                               This includes both real and imaginary measurements
 * @param DMA_BUFFER_LENGTH    Number of (real) measurements transferred over DMA  
 * @param FFT_SIZE             Number of data-points to fourier-transform
 * @param IIR_SIZE             Number of data-points to filter
 */
const uint16_t IN_BUFFER_LENGTH    = 4096;
const uint16_t DMA_BUFFER_LENGTH   = IN_BUFFER_LENGTH / 2;
const uint16_t FFT_SIZE            = IN_BUFFER_LENGTH;
const uint16_t IIR_SIZE            = IN_BUFFER_LENGTH;


/**
 * @brief The frequencies (in robosub) will be in the range 20 KHz - 40 KHz. 
 * The minimum sampling-frequency is therefore 80 KHz, but should minimum 
 * be 100 KHz due to safety. The sampling-frequency is set to 112500 Hz since that
 * is the closest the ADC can sample
 * 
 * @param SAMPLE_FREQUENCY  Frequency of sampling
 * @param SAMPLE_TIME       Time between each sample. Inverse of sample-frequency. 
 *                            Used to validate the signals
 */
const uint32_t SAMPLE_FREQUENCY = 112500;
const float32_t SAMPLE_TIME = (float32_t) 1 / SAMPLE_FREQUENCY;   


/**
 * @brief A fourth-order IIR-filter to filter the data before 
 * processing it. By using this filter, we should (hopefully)
 * eliminate unwanted frequencies. 
 * 
 * @warning Must have abs(filter) < 1 to prevent overflow
 * 
 * @warning The filter is designed with the filterdesigner 
 * in MATLAB, using
 *      Fs    = 112500        Sampling frequency
 *      Fc1   = 15000         Lower cut-off frequency
 *      Fc2   = 45000         Upper cut-off frequency
 *      
 *      Order = 4             Filter order
 *      Type  = Butterworth   Filter type
 *      
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
 * @param IIR_FILTER            A struct describing a biquad DF1 IIR filter
 */
const uint32_t num_stages = 2;
float32_t state_coefficients[4 * num_stages] = 
{
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};
const float32_t filter_coefficients[5 * num_stages] = 
{
        0.56942484, 0.0, -0.56942484,     /* Numerator filter 1                 */ 
        -1.12551866, 0.46469620,          /* Denominator filter 1               */
        0.56942484, 0.0, -0.56942484,     /* Numerator filter 2                 */
        -0.83226204, 0.3694894            /* Denominator filter 2               */
};
const arm_biquad_casd_df1_inst_f32 IIR_FILTER = 
{
    .numStages = num_stages, 
    .pState = &state_coefficients[0],
    .pCoeffs = &filter_coefficients[0]
};

} /* namespace DSP_CONSTANTS */

#endif /* ACOUSTICS_DSP_CONSTANTS_H */