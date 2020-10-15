/**
 * @file
 * @brief Small library that implements some
 * DSP-functions to the acoustics for Vortex NTNU
 */
#ifndef ACOUSTICS_DSP_HPP
#define ACOUSTICS_DSP_HPP

#include "../include/fourier.hpp"

/**
 * @brief Namespace/wrapper for basic DSP
 * (digital signal processing) functions. 
 */
namespace DSP{

/**
 * @brief Constants. 
 * The frequencie (in robosub) will be 
 * in the range 20 KHz - 40 KHz.
 * The minimum sampling-frequency is therefore
 * 80 KHz, but increased to 100 KHz for safety.
 * 
 * The MAX_FREQUENCY and MIN_FREQUENCY are used 
 * for filtering the data in software. Every 
 * frequency above max or below min is noise
 */
const int SAMPLE_FREQUENCY = 100000;
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
double correlation(double x_arr, double y_arr);


/**
 * @brief Function to calculate the lag of an
 * array. Finds the lag by detecting the highest
 * value in the argument.
 * 
 * @param x_arr Array of signal 1 to find the
 * lag
 */
int find_lag(const FOURIER::CArray& x_arr);


/**
 * @brief Function to filter the signal between
 * the min and max frequency. Will therefore 
 * implement a band-pass filter in SW.
 * 
 * @param x_arr Reference to an array to be BP-
 * filtered
 */
void freq_filtering(FOURIER::CArray& x_arr);



} //DSP

#endif //ACOUSTICS_DSP_HPP