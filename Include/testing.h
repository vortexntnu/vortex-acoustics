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

/**
 * @brief Namespace to hold the testing-functions
 */
namespace TESTING{


/**
 * @brief Function to test if there exists any problems with memory
 * leakage. Valgrind should be used in parallell with this function.
 * 
 * The function should run on the code developed for the main-program.
 */
void test_memory_leakage(void);


/**
 * @brief Function to test the filter parameters. The function should 
 * be used alongside GDB to analyze the results being calculated. 
 * The results should then be compared to another filter-software such as
 * a matlab-script to make sure that the results are roughly equal (small
 * errors may be accepted)
 * 
 * NOTE: It may seem strange to test the filter coefficients, however the
 * documentation was ambigous:
 *    1. It didn't specify whether the numerator was before or after the
 *        denominator in the param - list
 *    2. It hinted that some functions swap polarity of the filter's
 *        denominator, however didn't specify what itself did 
 * Both these points must be tested, such that problem could be resolved 
 */
void test_filter_coefficients(void);

} /* namespace TESTING */


#endif /* ACOUSTICS_TESTING_H */
