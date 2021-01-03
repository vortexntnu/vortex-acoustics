/**
 * @file
 * 
 * @brief Basic functions to triliterate the position of
 * the acoustic pinger
 */
#ifndef ACOUSTICS_TRILITERATION_H
#define ACOUSTICS_TRILITERATION_H

#include <algorithm> 
#include "DSP_constants.h"

/**
 * @brief Namespace/wrapper for the triliteration
 */
namespace TRILITERATION{

/**
 * @brief Constants used for the triliterations 
 * 
 * @param sound_speed Speed of sound in water in m/s
 * 
 * @param source_power The power from each pinger in dB.
 * Warning: requires us to know the type of pinger 
 * and the battery-voltage as these are time varying
 * parameters
 * 
 * @param max_intensity_diff Number of dB we allow the detected
 * signals to differentiate from each other.
 * 
 * @param time_diff_epsilon Safety margin to reduce the 
 * likelyhood of an error. A value of 0.1 shows that we accept
 * signals that arrive 10 % later. 
 * NOTE: 0 <= time_diff_epsilon < 1
 * 
 * @param max_hydrophone_distance The maximum distance 
 * measured between the hydrophones. 
 * 
 * @param max_time_diff The maximum time-difference that
 * should be possible between the data signals
 * 
 * @warning max_hydrophone_distance and maximum_time_diff are
 * decleared as extern, as they are decleared in .cpp 
 */
const uint16_t sound_speed          = 1480;
const float32_t source_power        = 177;
const float32_t max_intensity_diff  = 20;
const float32_t time_diff_epsilon   = 0.1; 
extern float32_t max_hydrophone_distance; 
extern float32_t max_time_diff;

/**
 * @brief Struct to keep the position
 * for an object/hydrophone. Could also
 * have used the POS found inside ROS
 */
struct Pos{
    /**
     * @brief Describes the position in 
     * x, y and z relative to the center 
     * of the AUV.
     */
    float32_t x, y, z;

    /**
     * @brief Constructor.
     */
    Pos(float32_t x, float32_t y, float32_t z);
}; /* struct Pos */


/**
 * @brief Helper function. Calculates the distance betweeen
 * two Pos
 * 
 * @retval Returns the distance between two positions
 * 
 * @param pos_lhs First position
 * 
 * @param pos_rhs Second position
 */
float32_t calculate_pos_distance(const Pos& pos_lhs, const Pos& pos_rhs);


/**
 * @brief Function that calculates the maximum distance
 * between the hydrophones, and use it to calculate the
 * allowed time_difference. See the overview in hydrophones.h
 * for a better explenation of the hydrophones positioning
 * 
 * @retval Returns 1/0 to indicate whether the values are initialized
 * correctly. Returns 0 if either max_hydrophone_distance or
 * maximum_time_diff is unchanged
 * 
 * @param pos_hyd_port Position of port hydrophone
 * 
 * @param pos_hyd_starboard Position of starboard hydrophone
 * 
 * @param pos_hyd_stern Position of stern hydrophone 
 */
uint8_t initialize_triliteration_globals(const Pos& pos_hyd_port,
        const Pos& pos_hyd_starboard, const Pos& pos_hyd_stern);


/**
 * @brief Function to calculate an estimate for the distance
 * given a measurement
 * 
 * @param intensity The strenght of the last measurement
 */
float32_t estimate_distance(float32_t intensity);


/**
 * @brief Function to calculate a rough estimate for the angle
 * 
 * @param time_difference The time-difference between two signals
*/
float32_t estimate_rough_angle(uint32_t time_difference);


/**
 * @brief Function to calculate the absolute angle and tell whether
 * the target is to the starboard or to the port side of the AUV
 * 
 * @note See the start of hydrophones.hpp for more information
 * 
 * @param time_port Time the signal was measured on the port side
 * 
 * @param time_starboard Time the signal was measured on the 
 * starboard side
 */
std::pair<float32_t, uint8_t> estimate_latitude(
            int32_t time_port, uint32_t time_starboard);


/**
 * @brief Function to estimate whether the point is to the bow or
 * at the stern of the AUV.
 * 
 * @note See the start of hydrophones.hpp for more information
 * 
 * @param time_port Time the signal was measured on the port side
 * 
 * @param time_starboard Time the signal was measured on the 
 * starboard side  
 * 
 * @param time_stern Time the signal was measured at the stern 
 */
uint8_t estimate_longitude(uint32_t time_port, uint32_t time_starboard,
            uint32_t time_stern);


/**
 * @brief Function to estimate the intensity of a signal 
 * 
 * @param p_signal_data A pointer to the signal-data to be estimated
 * 
 * @warning From Parseval's theorem, both the signal in the frequency- and
 * the signal in the time-domain can be used. This function uses the data
 * in the time-domain 
 * 
 * @warning The function assumes that the signal is already filtered
 */
float32_t estimate_signal_intensity(float32_t* p_signal_data);


/**
 * @brief Function to estimate the position of the acoustic pinger
 * 
 * @retval Returns an std::pair containing the estimates of x and y 
 * compared to the position of the AUV at the time of measurement.
 * 
 * @note See the start of hydrophones.hpp for more information
 * 
 * @param time_port Time the signal was measured on the port side
 * 
 * @param time_starboard Time the signal was measured on the 
 * starboard side  
 * 
 * @param time_stern Time the signal was measured at the stern 
 * 
 * @param intensity_port Intensity of the signal measured at the
 * port side
 * 
 * @param intensity_starboard Intensity of the signal measured at the
 * starboard side
 * 
 * @param intensity_stern Intensity of the signal measured at the
 * stern side
 */
std::pair<float32_t, float32_t> estimate_pinger_position(
            uint32_t time_port, uint32_t time_starboard, 
            uint32_t time_stern, float32_t intensity_port, 
            float32_t intensity_starboard, float32_t intensity_stern);


/**
 * @brief Function to calculate distance and angle to the estimated
 * position of the acoustic pinger
 * 
 * NOTE: Not directly necessary to calculate this twice as both the angle
 * and the distance is used to calculate the @p position_estimate however
 * it is easier to use a helper-function to implement this, compared to 
 * cluttering the original function. By separating into two functions, it 
 * gives us more flexibility to choose the optimal way of reporting the 
 * position-estimate to the Xavier
 * 
 * @retval Returns the values indirectly using the pointers 
 * @p p_distance_estimate and @p p_angle_estimate 
 * 
 * @param p_distance_estimate Pointer to return the calculated distance 
 * 
 * @param p_angle_estimate Pointer to return the calculated angle
 * 
 * @param position_estimate The position estimated 
 * 
 * @warning Not implemented as of 23.10.2020, as I suddenly realized that 
 * the original estimate also takes into account the distance in z. Must 
 * be discussed on how important this is to be included, however it will 
 * obstruct the usage of an estimated position. 
 */
void calculate_distance_and_angle(
            const std::pair<float32_t, float32_t>& position_estimate,
            float32_t* p_distance_estimate, 
            float32_t* p_angle_estimate);


/**
 * @brief Function to check the validy of each signal 
 * 
 * @retval Returns true if the values are valid, and false if not. 
 * If false is returned, at least one of @p p_bool_time_error or 
 * @p p_bool_intensity_error is set to 1. Otherwise both are set to
 * 0
 * 
 * @param time_port Time the signal was measured on the port side
 * 
 * @param time_starboard Time the signal was measured on the 
 * starboard side
 * 
 * @param time_stern Time the signal was measured at the stern
 *
 * @param intensity_port Intensity of the signal detected at the
 * port side
 * 
 * @param intensity_starboard Intensity of the signal detected at 
 * the starboard side
 * 
 * @param intensity_stern Intensity of the signal detected at the
 * stern of the AUV
 * 
 * @param p_bool_time_error Pointer to indicate time-error
 * 
 * @param p_bool_intensity_error Pointer to indicate intensity-error
 */
uint8_t check_valid_signals(const uint32_t& time_port, 
    const uint32_t& time_starboard, const uint32_t& time_stern, 
    const float32_t& intensity_port, const float32_t& intensity_starboard, 
    const float32_t& intensity_stern, uint8_t* p_bool_time_error,
    uint8_t* p_bool_intensity_error);


/**
 * @brief Helper function. Checks if the time-difference between two 
 * signals are valid. The time difference between the signals are checked 
 * against TRILITERATION::maximum_time_diff
 * 
 * @retval Returns false/true (0/1) depending on the result of the test
 * 
 * @param time_lhs One of the time-samples to check against
 * 
 * @param time_rhs The other time-sample to check 
 */
uint8_t valid_time_check(const uint32_t& time_lhs, const uint32_t& time_rhs);


/**
 * @brief Helper function. Checks if the measured/estimated intensity of 
 * two signals appears valid.
 * 
 * @retval Returns true/false, with the retval depending on the result of the
 * test. 
 * 
 * @param intensity_lhs The first signal to check against
 * 
 * @param intensity_rhs The second signal to check against 
 */
uint8_t valid_intensity_check(const float32_t& intensity_lhs, 
    const float32_t& intensity_rhs);


/**
 * @brief Function prototype as of 03.01.21. The function should transfer 
 * the measured data to values given in the unit Pa.
 * This is to make sure that the calculated intensity is correct. Takes
 * into account pre-amplification and sensitivity of the hydrophone at the
 * given frequency
 * 
 * At first, the values are given in intervals of 0 to 4095, where 0 
 * represents 0.0 V and 4095 represents 3.3 V measured. This is thereafter
 * shifted to accomodate the preamplifier, and divided by the sensitivity 
 * to get measured pressure.
 * 
 * NOTE: Is it really required to do this? The intensity should be calculculated
 * well enough by using Parseval's theorem(?)  
 * 
 * @param p_data Pointer to filtered data that is to be transformed 
 */
void transform_data(float32_t* p_data);

} /* namespace TRILITERATION */

#endif /* ACOUSTICS_TRILITERATION_H */
