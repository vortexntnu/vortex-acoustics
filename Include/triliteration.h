/**
 * @file
 * 
 * @brief Basic functions to triliterate the position of
 * the acoustic pinger
 */
#ifndef ACOUSTICS_TRILITERATION_H
#define ACOUSTICS_TRILITERATION_H

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
 * @param time_diff_epsilon Safety margin to reduce the 
 * likelyhood of an error. A value of 0.1 shows that we accept
 * signals that arrive 10 % later. 
 * NOTE: 0 <= time_diff_epsilon < 1
 * 
 * @param max_hydrophone_distance The maximum distance 
 * measured between the hydrophones. 
 * 
 * @param maximum_time_diff The maximum time-difference that
 * should be possible between the data signals
 * 
 * @warning max_hydrophone_distance and maximum_time_diff are
 * decleared as extern, as they are decleared in .cpp 
 */
const uint16_t sound_speed = 1480;
const uint16_t source_power = 177; 
const float32_t time_diff_epsilon = 0.1; 
extern float32_t max_hydrophone_distance; 
extern float32_t maximum_time_diff;

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
};


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
float32_t calculate_distance(const Pos& pos_lhs, const Pos& pos_rhs);


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
std::pair<float32_t, uint8_t> estimate_lateral(
        uint32_t time_port, uint32_t time_starboard);


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
 * @brief Function to estimate the position of the acoustic pinger
 * Returns an estimate of x and y compared to the position of the 
 * AUV
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
std::pair<float32_t, float32_t> estimate_pinger_position(uint32_t time_port,
            uint32_t time_starboard, uint32_t time_stern, 
            float32_t intensity_port, float32_t intensity_starboard,
            float32_t intensity_stern);

/**
 * @brief Function to check the validy of each signal 
 * 
 * @retval Returns true if the values are valid, and false if not
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
 * @warning Not fully implemented as of 09.12.2020. Required to
 * find a way to use the intensity of the signal to validate the 
 * data, however that could become difficult without knowing how
 * the sound-signals are detected with the AUV
 */
uint8_t check_valid_signals(const uint32_t& time_port, 
    const uint32_t& time_starboard, const uint32_t& time_stern, 
    const float32_t& intensity_port, const float32_t& intensity_starboard, 
    const float32_t& intensity_stern);


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
 * 
 * @warning Not implemented as og 12.12.2020, as I have no idea how to 
 * implement or calculate the intensity of the signals. The function will 
 * hopefully be updated/implemented in the upcoming weeks.
 */
uint8_t valid_intensity_check(const float32_t& intensity_lhs, 
    const float32_t& intensity_rhs);


} // namespace TRILITERATION

#endif // ACOUSTICS_TRILITERATION_H
