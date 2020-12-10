/**
 * @file
 * 
 * @brief Basic functions to triliterate the position
 */

#ifndef ACOUSTICS_TRILITERATE_H
#define ACOUSTICS_TRILITERATE_H

#include <math.h>
#include "DSP.h"

/**
 * @brief Namespace/wrapper for the triliteration
 */
namespace TRILITERATION{

/**
 * @brief Constants used for the triliterations 
 * 
 * @param sound_speed Speed of sound in m/s
 * 
 * @param source_power The power from each pinger in dB.
 * Warning: requires us to know the type of pinger 
 * and the battery-voltage as these are time varying
 * parameters
 * 
 * @param hydrophone_distance The distance between the 
 * hydrophones. Required to be changed later on! Could
 * potentially be set using the hydrophone-positioning
 * 
 * @param time_error Error margin to help classify valid
 * vs. invalid signals
 */
const uint16_t sound_speed = 1480;
const uint16_t source_power = 177;  
const float32_t hydrophone_distance = 0.57; 
const float32_t maximum_time_diff = hydrophone_distance/sound_speed;

/**
 * @brief Struct to keep the position
 * for an object/hydrophone
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

} // namespace TRILITERATION

#endif // ACOUSTICS_TRILITERATE_H
