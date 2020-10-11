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
 * Defines and typedefs
 */
const double PI = 3.141592653589793238460;
 
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
 * It requires a lot of memory!
 */
void fourier_fft(CArray& x_arr, bool take_abs_value);


/**
 * @brief Function to calculate the inverse DFT
 * in O(n lg(n)) time
 * 
 * @param x_arr Array of data to take the IDFT 
 */
void fourier_ifft(CArray& x_arr);

#endif //ACOUSTICS_FOURIER_HPP