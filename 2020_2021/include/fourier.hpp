/**
 * @file
 * 
 * @brief Small library that implements the some
 * fourier-functions for the acoustics for Vortex
 * NTNU.
 * 
 * Modified version of 
 * https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
 * 
 * @note Could also use the "kiss_fft"-library.
 */

#ifndef ACOUSTICS_FOURIER_HPP
#define ACOUSTICS_FOURIER_HPP

#include <complex>
#include <iostream>
#include <valarray>


/**
 * @brief Namespace/wrapper for basic-fourier
 * functions
 */
namespace FOURIER{

/**
 * @brief Important values
 * 
 * @param PI Value for the math-constant pi
 * 
 * @param num_freq_intervals How many intervals is used
 * for estimating the frequency measured. More 
 * intervals gives lower variance in the estimated
 * frequency. This improves the estimate and therefore 
 * how good the system can tolerate noise and other
 * disturbances. 
 * 
 * @param len_freq_intervals Describes how much data each
 * interval can hold. 
 */
const double PI = 3.141592653589793238460;
const int num_freq_intervals = 1024;
const int len_freq_intervals = 128;

 
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;


/**
 * @brief Function for calculating the DFT in
 * O(n lg(n)) time. A variant of the Cooley-Tukey 
 * FFT, implemented in-place, divide & conquer.
 *  
 * @param x_arr Array of data to take the DFT
 * 
 * @param take_abs_value Bool to determine if
 * the resulting signal should have 
 * 
 * @warning The default value for take_abs_value
 * is false. Need to specify to true if the function
 * should return the absolute-values!
 * 
 * @warning The algorithm could be improved. 
 * It requires a lot of memory, so if implemented
 * on a µ-controler the function may result in a
 * memory overflow! 
 */
void fft(CArray& x_arr, bool take_abs_value);


/**
 * @brief Function to calculate the inverse DFT
 * in O(n lg(n)) time
 * 
 * @param x_arr Array of data to take the IDFT 
 */
void ifft(CArray& x_arr);


/**
 * @brief Function to estimate the frequency-response
 * from the samples. 
 * 
 * @note Mainly a helper-function to the function
 * freq_estimation. Will sum up the entire array.
 * Results in more overhead, but makes the code
 * more readable.
 *  
 * @param x_arr Array of data to estimate the 
 * correct frequency-response 
 */
Complex sum_complex_arr(const CArray& x_arr);


/**
 * @brief Function to estimate the frequency-response
 * from the samples. 
 * 
 * @note The estimation is another 
 * possible technique to get the frequency from 
 * the data. This method should result in less noise
 * compared to taking the fft of the autocorrelation.
 *  
 * @param x_arr Array of data to estimate the 
 * correct frequency-response 
 * 
 * @warning Not implemented! Too many inaccuracies
 * and uncertanties atm (15.10.2020)
 */
CArray freq_estimation(const CArray& x_arr);


} //FOURIER

#endif //ACOUSTICS_FOURIER_HPP