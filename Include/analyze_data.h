/**
 * @file
 *
 * @brief Small library that implements the
 * hydrophones for the AUV for Vortex NTNU
 */
#ifndef ACOUSTICS_ANALYZE_DATA_H
#define ACOUSTICS_ANALYZE_DATA_H

#include "trilateration.h"

namespace ANALYZE_DATA {

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
void array_max_value(float32_t *data_array, const uint32_t &array_length,
                     uint32_t &idx, float32_t &max_val);

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
void filter_raw_data(float32_t *p_raw_data_array[NUM_HYDROPHONES],
                     float32_t *p_filtered_data_array[NUM_HYDROPHONES]);

/**
 * @brief A function that crosscorrelates the filtered data arrays and
 * returns an array which gives the number of samples between the
 * measured signals at hyd1, hyd2, hyd3
 *
 * @warning The values contained within @p p_lag_array are
 * extremely sensitive to the sign. If the sign is negative, the
 * hydrophone on the "first"-side measured the signal first.
 * Example:
 *
 *      Assuming that *p_lag_port_stern = -10
 *
 *      That means the port hydrophone measured the signal 10 samples
 *          BEFORE the stern hydrophone
 *
 *
 *
 * @param p_filtered_data_array The array containing the
 * It is assumed that
 *      @p p_filtered_data_array = {p_filtered_data_port,
 *                                  p_filtered_data_starboard,
 *                                  P_filtered_data_stern}
 *
 * @param p_lag_array The array to hold the cross-correlated lags.
 * It is assumed that
 *      @p p_lag_array = {p_lag_port_starboard,
 *                        p_lag_port_stern,
 *                        p_lag_starboard_stern}
 */
void calculate_xcorr_lag_array(
    float32_t *p_filtered_data_array[NUM_HYDROPHONES],
    uint32_t *p_lag_array[NUM_HYDROPHONES]);

} /* namespace ANALYZE_DATA */

#endif // ACOUSTICS_ANALYZE_DATA_H
