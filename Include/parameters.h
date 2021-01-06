/**
 * @file
 * 
 * @brief Parameters and constants used for the system/AUV that a lot of the code
 * must know 
 */
#ifndef ACOUSTICS_PARAMETERS_H
#define ACOUSTICS_PARAMETERS_H

/**
 * @brief Defines that indicate the setup of the hardware on the AUV
 * 
 * These variables must be changed according to the hydrophones used.
 * As of 03.01.21, three hydrophones of the type Benthowave BII-7014FG 
 * is used. See the datasheet at 
 * https://www.benthowave.com/products/Specs/BII-7014FGPGDatasheet.pdf
 * for more information
 * 
 * @note Position of the hydrophones are relative to the center of the 
 * AUV
 */
#ifndef HYDROPHONE_DETAILS
#define HYDROPHONE_DETAILS

  #define NUM_HYDROPHONES   3         /* Number of hydrophones used on the AUV        */
  #define HYD_PREAMP_DB     40        /* Number of dB the signal is preamplifies      */
  #define HYD_FFVS          -173      /* Average FFVS for 20 - 40 kHz [dB V/μPa]      */  

  #define SIGNAL_GAIN       1.0f      /* Gain of signal (unknown as of 06.01)         */

  #define PORT_HYD_X        -1.0f     /* x - position of port hydrophone      [m]     */
  #define PORT_HYD_Y        0.0f      /* y - position of port hydrophone      [m]     */
  #define PORT_HYD_Z        0.0f      /* z - position of port hydrophone      [m]     */

  #define STARBOARD_HYD_X   1.0f      /* x - position of starboard hydrophone [m]     */
  #define STARBOARD_HYD_Y   0.0f      /* y - position of starboard hydrophone [m]     */
  #define STARBOARD_HYD_Z   0.0f      /* z - position of starboard hydrophone [m]     */

  #define STERN_HYD_X       0.0f      /* x - position of stern hydrophone     [m]     */
  #define STERN_HYD_Y       -1.0f     /* y - position of stern hydrophone     [m]     */
  #define STERN_HYD_Z       0.0f      /* z - position of stern hydrophone     [m]     */

#endif /* HYDROPHONE_DETAILS */

#endif /* ACOUSTICS_PARAMETERS_H */
