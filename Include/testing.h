/**
 * @file
 *
 * @brief File to implement tests for code analysis
 *
 * Required tests:
 *      1. Memory usage and leakage
 *      2. DMA correctly set up
 *      3. ADC correctly set up
 *      4. ETH correctly set up
 *      5. Filter coefficients
 *      6. ...
 *
 * @warning For optimum performance, the code should be "public", such
 * that this file has access to the code. This is not done as of 22.12.2020,
 * however should pherhaps be implemented over the following days/weeks
 */
#ifndef ACOUSTICS_TESTING_H
#define ACOUSTICS_TESTING_H

#include "analyze_data.h"

#if CURR_TESTING_BOOL

#include <stdio.h>

/**
 * @brief Namespace to hold the testing-functions
 */
namespace TESTING {

/**
 * @brief Function that generates TOA of the sound-signals for the different
 * hydrophones. Returns the calculated TOA indirectly via @p lag_array
 *
 * The TOA is calculated based on the following parameters:
 *    @p HYD_xxx_POS
 *    @p SOURCE_POS
 *    @p SOUND_SPEED
 *    @p SAMPLE_FREQUENCY
 * all of which are given in "parameters.h"
 *
 * @param lag_array An array giving the time the sound will be detected
 * by the hydrophones. The array expands to
 *    @p lag_array = { lag_port, lag_starboard, lag_stern }
 *
 * @param bool_valid_parameters An int describing if the parameters
 * given in parameters.h are valid.
 */
void calculate_toa_array(uint32_t lag_array[NUM_HYDROPHONES],
                         uint8_t &bool_valid_parameters);

/**
 * @brief Function that checks if the fictional sound-source position is  within
 * @p MARGIN_POS_ESTIMATE distance from the estimated position
 *
 * Writes the distance estimated and the actual distance to the terminal
 */
void test_trilateration_algorithm();

} /* namespace TESTING */

#endif /* CURR_TESTING_BOOL */

#endif /* ACOUSTICS_TESTING_H */
