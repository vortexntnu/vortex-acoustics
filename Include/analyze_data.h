/**
 * @file
 * 
 * @brief File that contains functions to analyze data.
 * Functions earlier implemented inside HYDROPHONES, however changed to 
 * reduce memory-usage. 
 * 
 * This .h and .cpp changes the code more towards c, by dropping the class
 * Hydrophones   
 */
#ifndef ACOUSTICS_ANALYZE_DATA_H
#define ACOUSTICS_ANALYZE_DATA_H

#include "triliteration.h"
#include "DSP_constants.h"

namespace ANALYZE_DATA{


/**
 * @brief Function to analyze the data
 * 
 * The raw data is filtered using a fourth-order biquad DF1 IIR filter 
 * to eliminate unwanted frequencies. Thereafter the frequencies 
 * magnitude and autocorrelation are found. The lag, intensity and 
 * distance are thereafter estimated from these measurements.
 * 
 * @retval Returns all values indirectly using pointers
 * 
 * @warning This function is equal to the function given in 
 * HYDROPHONES::Hydrophone::analyze_data(). Transfered as it's own function
 * to reduce the memory-usage from the class. It reduces the readability of
 * the code, and requires more arrays to be initialized beforehand.
 * 
 * @param p_raw_data        Pointer to an array of raw data from the ADC
 * @param p_data            Pointer to an array of filtered data
 * @param p_autocorr_data   Pointer to an array holding the autocorrelation of
 *                            the filtered data
 * @param p_intensity       Pointer indicating the intensity of a measurment
 * @param p_lag             Pointer indicating the index/lag of the strongest 
 *                            signal in @p p_autocorr_data
 */
void analyze_data(
            float32_t* p_raw_data, 
            float32_t* p_data,
            float32_t* p_autocorr_data, 
            float32_t* p_intensity,
            uint32_t* p_lag);

} /* namespace ANALYZE_DATA */



#endif /* ACOUSTICS_ANALYZE_DATA_H */