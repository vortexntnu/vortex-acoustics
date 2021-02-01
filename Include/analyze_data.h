/**
 * @file
 * 
 * @brief Small library that implements the 
 * hydrophones for the AUV for Vortex NTNU
 */
#ifndef ACOUSTICS_ANALYZE_DATA_H
#define ACOUSTICS_ANALYZE_DATA_H

#include "trilateration.h"

namespace ANALYZE_DATA{

/**
 * @brief Function to find the index and the maximum abs value in an
 * array. The function returns the index and the maximum value 
 * indirectly as references
 * 
 * If @p array_length is invalid (negative), the function sets both
 * @p max_val and @p idx to -1
 * 
 * @param data_array The array to find the maximum of
 * 
 * @param array_length The length of the array
 * 
 * @param idx The index containing the maximum abs value
 * 
 * @param max_val Max absolute value
 */
void array_max_value(
        float32_t* data_array,
        const uint32_t& array_length,
        uint32_t& idx,
        float32_t& max_val);


/**
 * @brief The function takes in raw data-signals, and uses the ARM 
 * Biquad IIR-filter to filter the data
 * 
 * @param p_raw_data_array Raw data to be filtered
 * It is assumed that
 *      @p p_raw_data_array = {p_raw_data_port,
 *                             p_raw_data_starboard,
 *                             p_raw_data_stern}
 * 
 * @param p_filtered_data_array The filtered data
 * Returned as
 *      @p p_filtered_data_array = {p_filtered_data_port,
 *                                  p_filtered_data_starboard,
 *                                  p_filtered_data_stern}
 */
void filter_raw_data(
        float32_t* p_raw_data_array[NUM_HYDROPHONES],
        float32_t* p_filtered_data_array[NUM_HYDROPHONES]);


/**
 * @brief A function that crosscorrelates the filtered data arrays and
 * returns a TDOA_array which indicates the time-difference of arrival
 * between the signals
 * 
 * The function uses crosscorrelation to determine the TDOA, since the
 * autocorrelation is prone to detect external noise as the signal  
 * 
 * @param p_filtered_data_array The array containing the 
 * It is assumed that
 *      @p p_filtered_data_array = {p_filtered_data_port, 
 *                                  p_filtered_data_starboard,
 *                                  P_filtered_data_stern}
 * 
 * @param p_TDOA_array The array to hold the calculated TDOA
 * It is assumed that
 *      @p p_TDOA_array = {p_TDOA_port_starboard,
 *                         p_TDOA_port_stern,
 *                         p_TDOA_starboard_stern}
 */
void calculate_TDOA_array(
        float32_t* p_filtered_data_array[NUM_HYDROPHONES],
        uint32_t* p_TDOA_array[NUM_HYDROPHONES]);

} /* namespace ANALYZE_DATA */

#endif // ACOUSTICS_ANALYZE_DATA_H



