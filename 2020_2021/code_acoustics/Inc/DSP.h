/**
 * @file
 * @brief Small library that implements some
 * constants for the DSP 
 */
#ifndef ACOUSTICS_DSP_H
#define ACOUSTICS_DSP_H

/*
These libraries were from an earlier attempt, where I
tried to implement the DSP using proper FFT-libraries.
The problem was that the libraries were too large for 
the STM32, such that all of the RAM was "deleted". Tried
to implement the data using the built in FFT from ARM
*/
//#include "../alglib/src/fasttransforms.h"
//#include "../kiss_fft130/tools/kiss_fftr.h"

#include "arm_math.h"
#include "arm_const_structs.h" 
#include <stdint.h>


/**
 * @brief Namespace/wrapper for basic DSP functions. 
 */
namespace DSP_CONSTANTS{

/**
 * @brief Constants for sampling and DSP
 */
const int DMA_BUFFER_LENGTH = 1024;
const int WORKING_BUFFER_LENGTH = DMA_BUFFER_LENGTH/2;
const int IN_BUFFER_LENGTH = DMA_BUFFER_LENGTH;
const int FFT_SIZE = DMA_BUFFER_LENGTH;

/**
 * @note The frequencies (in robosub) will be 
 * in the range 20 KHz - 40 KHz.
 * The minimum sampling-frequency is therefore
 * 80 KHz, but increased to 100 KHz for safety.
 * 
 * The MAX_FREQUENCY and MIN_FREQUENCY are used 
 * for filtering the data in software. Every 
 * frequency above max or below min is noise
 */
const uint32_t SAMPLE_FREQUENCY = 100000;
const uint32_t MAX_FREQUENCY = SAMPLE_FREQUENCY / 2;
const uint32_t MIN_FREQUENCY = SAMPLE_FREQUENCY / 10;

} // namespace DSP_CONSTANTS

#endif // ACOUSTICS_DSP_H