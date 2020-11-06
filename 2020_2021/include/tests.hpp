/**
 * @brief Different functions for creating and 
 * managing tests
 * 
 * @warning It is possible to increase the 
 * efficiency by using premade functions from
 * for example alglib. Many statistical functions
 * can be found under alglib/statistics.h
 */

#ifndef ACOUSTICS_TESTS_HPP
#define ACOUSTICS_TESTS_HPP

#include <random>
#include <iostream>
#include <vector>
#include <chrono>

#include "../include/hydrophones.hpp"

/**
 * @brief Namespace to abstract the tests
 */
namespace TESTS{

/**
 * @brief Parameter to change if testing 
 * is wished.
 */
const bool testing = true;


/**
 * @brief Parameter that describes the number
 * of tests to be done before concluding.
 * 
 * 
 * @warning Should have at least 30 tests to 
 * have a decent estimate of whatever is to be
 * tested. 
 */
const int num_nests_const = 100;


/**
 * @brief Function to create a full test-array.
 * Length of the array is given by the variable
 * DSP::interval_total_len
 */
int* create_random_integers();


/**
 * @brief Function for testing the speed of the
 * calculations behind the lag.
 */
void test_lag_calculation(const int num_tests);



} // TESTS


#endif // ACOUSTICS_TEST_HPP