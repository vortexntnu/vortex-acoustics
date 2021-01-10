/**
 * @file
 * 
 * @brief Parameters and constants used for the system. 
 * Included parameters:
 *    DSP_CONSTANTS: 
 *        Sampling-frequency
 *        Buffer-sizes for DMA, ADC, FFT and filter
 * 
 *    HYDROPHONE_DETAILS:
 *        Number hydrophones
 *        Hydrophone amplification
 *        Hydrophone position
 * 
 *    PHYSICAL_CONSTANTS:
 *        Speed of sound in water
 * 
 *    ARM_MATH_CM7:
 *        Enables specialized math-functions to the Cortex M7 MCU
 * 
 *    _USE_MATH_DEFINES:
 *        Enables math-defines and math-functions from <cmath> and <math>
 * 
 *    FILTER_SETUP
 *        Parameters for the arm_biquad_casd_df1_inst_f32 struct
 */
#ifndef ACOUSTICS_PARAMETERS_H
#define ACOUSTICS_PARAMETERS_H

/**
 * @brief 
 * Sampling frequency
 * 
 * Different buffer-length used 
 */
#ifndef DSP_CONSTANTS
#define DSP_CONSTANTS

  #define SAMPLE_FREQUENCY    112500.0f             /* Sample frequency                     [Hz]      */
  #define SAMPLE_TIME         1 / SAMPLE_FREQUENCY  /* Sample time                          [s]       */

  #define IN_BUFFER_LENGTH    4096u                 /* Total number (real + imaginary) measurements   */
                                                    /* per hydrophone                                 */
  #define DMA_BUFFER_LENGTH   IN_BUFFER_LENGTH / 2  /* Number real measurements transferred with DMA  */
  #define FFT_SIZE            IN_BUFFER_LENGTH      /* Number of data-points to FFT                   */
  #define IIR_SIZE            IN_BUFFER_LENGTH      /* Number of data-points to filter                */

#endif /* DSP_CONSTANTS */


/**
 * @brief Defines that indicate the setup of the hydrophones
 * 
 * These variables must be changed according to the hydrophones used.
 * As of 03.01.21, three hydrophones of the type Benthowave BII-7014FG 
 * is used. See the datasheet at 
 * https://www.benthowave.com/products/Specs/BII-7014FGPGDatasheet.pdf
 * for more information
 * 
 * @note It is thought that the hydrophones are
 * placed in a form of a triangle. Therefore, we
 * have one on front starboard, one on front port
 * and one on the stern in the middle. Could be changed
 * in the future
 * 
 *                    BOW
 *                      
 *      Port                       Starboard
 *                      
 *       HYD ''''''''''''''''''''' HYD
 *           '''''''''''''''''''''
 *           ''''''''' A '''''''''
 *           ''''''''' U '''''''''
 *           ''''''''' V '''''''''
 *           '''''''''''''''''''''
 *           '''''''''''''''''''''
 *           '''''''''HYD'''''''''
 * 
 *                   STERN
 * 
 * @note The position of the hydrophones are relative
 * to the center of the AUV
 */
#ifndef HYDROPHONE_DETAILS
#define HYDROPHONE_DETAILS

  #define NUM_HYDROPHONES     3                     /* Number of hydrophones used on the AUV          */
  #define HYD_PREAMP_DB       40                    /* Number of dB the signal is preamplifies        */
  #define HYD_FFVS           -173                   /* Average FFVS for 20 - 40 kHz [dB V/μPa]        */  

  #define SIGNAL_GAIN         1.0f                  /* Gain of signal (unknown as of 06.01)           */

  #define PORT_HYD_X         -1.0f                  /* x - position of port hydrophone      [m]       */
  #define PORT_HYD_Y          0.0f                  /* y - position of port hydrophone      [m]       */
  #define PORT_HYD_Z          0.0f                  /* z - position of port hydrophone      [m]       */

  #define STARBOARD_HYD_X     1.0f                  /* x - position of starboard hydrophone [m]       */
  #define STARBOARD_HYD_Y     0.0f                  /* y - position of starboard hydrophone [m]       */
  #define STARBOARD_HYD_Z     0.0f                  /* z - position of starboard hydrophone [m]       */

  #define STERN_HYD_X         0.0f                  /* x - position of stern hydrophone     [m]       */
  #define STERN_HYD_Y        -1.0f                  /* y - position of stern hydrophone     [m]       */
  #define STERN_HYD_Z         0.0f                  /* z - position of stern hydrophone     [m]       */

#endif /* HYDROPHONE_DETAILS */


/**
 * @brief Defines that indicate the errors that we can allow and still 
 * get an acceptable result 
 */
#ifndef SYSTEM_MARGINS
#define SYSTEM_MARGINS

  #define MARGIN_POS_ESTIMATE   0.5f                /* Error tolerable in estimating the position [m] */
  #define MARGIN_POS_SEARCH     0.25f               /* How much the search will deviate in x      [m] */          

  #define MARGIN_INTENSITY      20                  /* Difference between the intensity-measurements  */       
  #define MARGIN_TIME_EPSILON   0.1f                /* Determines the difference allowed between the  */
                                                    /* detected lags. With LAG_DIFF_EPSILON = 0.1 we  */
                                                    /* allow signals to arrive 1.1 * max_time and     */
                                                    /* still count. max_time is the maximum           */
                                                    /* possible time for sound to pass from one to    */
                                                    /* another hydrophone                             */


#endif /* SYSTEM_MARGINS */


/**
 * @brief Defines that indicate physical characteristics/constants 
 * of the system
 */
#ifndef PHYSICAL_CONSTANTS
#define PHYSICAL_CONSTANTS

  #define SOUND_SPEED         1480.0f               /* Speed of sound in water [m/s]                  */

#endif /* PHYSICAL_CONSTANTS */


#ifndef ARM_MATH_CM7
  #define ARM_MATH_CM7                              /* Processor-version used on the STM32            */
#endif /* ARM_MATH_CM7 */

#ifndef _USE_MATH_DEFINES
  #define _USE_MATH_DEFINES                         /* Gives access to math-defines such as M_PI      */
#endif /* _USE_MATH_DEFINES */


#include <cstdint>
#include <math.h>
#include <cmath>
#include <utility>

#include "arm_math.h"
#include "arm_const_structs.h" 

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
#ifndef FILTER_SETUP
#define FILTER_SETUP

  const uint32_t num_stages = 2;
  
  float32_t state_coefficients[4 * num_stages] = 
  {
          0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
  };

  float32_t filter_coefficients[5 * num_stages] = 
  {
          0.56942484, 0.0, -0.56942484,             /* Numerator filter 1                               */ 
          -1.12551866, 0.46469620,                  /* Denominator filter 1                             */
          0.56942484, 0.0, -0.56942484,             /* Numerator filter 2                               */
          -0.83226204, 0.3694894                    /* Denominator filter 2                             */
  };
  
  const arm_biquad_casd_df1_inst_f32 IIR_FILTER = 
  {
      .numStages = num_stages, 
      .pState = &state_coefficients[0],
      .pCoeffs = &filter_coefficients[0]
  };

#endif /* FILTER_SETUP */


#endif /* ACOUSTICS_PARAMETERS_H */
