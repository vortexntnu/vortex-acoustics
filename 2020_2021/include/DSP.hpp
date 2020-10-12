/**
 * @file
 * @brief Small library that implements some
 * DSP-functions to the acoustics for Vortex NTNU
 */
#ifndef ACOUSTICS_DSP_HPP
#define ACOUSTICS_DSP_HPP

#include "../include/fourier.hpp"

/**
 * @brief Constants. 
 * Maximum frequency (in robosub) will be 50 KHz.
 * The minimum sampling-frequency is therefore
 * 100KHz. The sample-frequency is increased to
 * 120KHz to prevent any other aliasing.
 */
const int SAMPLE_FREQUENCY = 120000;
const int MAX_FREQUENCY = SAMPLE_FREQUENCY / 2;
const int MIN_FREQUENCY = SAMPLE_FREQUENCY / 10;

/**
 * @brief Function to calculate the correlation
 * coeffisient between two different signals.
 * Runs in O(n) time.
 * Small modifications from 
 * https://www.geeksforgeeks.org/program-find-correlation-coefficient/
 * 
 * @param x_arr Array of signal 1
 * 
 * @param y_arr Array of signal 2
 * 
 * @param n Number of elements in each array
 * 
 * @warning Unsure if the function handles
 * complex values
 * 
 * @warning Must make sure that the lengths of
 * each array are identical
 */
double correlation_coefficient(
    int x_arr, int y_arr, int n);


/**
 * @brief Function to calculate the correlation
 * between to different signals
 * 
 * @param x_arr Array of signal 1
 * 
 * @param y_arr Array of signal 2
 * 
 * @param n Number of elements in each array
 * 
 * @warning Must be updated to apprehend 
 * lists of different lengths
 * 
 * @warning Not implemented as of 12.10.2020
 */
double correlation(
    double x_arr, double y_arr);

// Required functions:
    //X-correlation
    //A way to calculate the lag
    //A way to filter the data in SW
        //by using the frequency-components 
        //from the fft


#endif //ACOUSTICS_DSP_HPP