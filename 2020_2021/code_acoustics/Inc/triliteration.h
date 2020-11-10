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
 * @param source_power The power from each pinger.
 * Warning: requires us to know the type of pinger 
 * and the battery-voltage 
 * 
 * @param hydrophone_distance The distance between the 
 * hydrophones. Required to be changed later on! Could
 * potentially be set using the hydrophone-positioning
 * 
 * @param time_error Error margin to help classify valid
 * vs. invalid signals
 */
const uint16_t sound_speed = 1480;
const uint8_t source_power = 177;  
const double hydrophone_distance = 0.57; 
const double maximum_time_diff = hydrophone_distance/sound_speed;
const double time_error = 0;

/**
 * @brief Struct to keep the position
 * for an object/hydrophone
 * 
 * @warning Should be replaced with the 
 * Pos included in ROS!
 */
struct Pos{
    /**
     * @brief Describes the position in 
     * x, y and z relative to the center 
     * of the AUV.
     */
    double x, y, z;
    
    /**
     * @brief Describes the heading, 
     * compared to the heading of the
     * AUV
     * 
     * @warning Not used or implemented. Just
     * included for the future
     */
    double h_x, h_y, h_z;

    /**
     * @brief Constructor.
     */
    Pos(double x, double y, double z);
};


/**
 * @brief Function to take the norm
 * of a point pos. Takes the norm from
 * the zero-vector, which would be the 
 * mass-center of the AUV
 * 
 * @param pos The position to take
 * the norm
 */
double pos_norm(const Pos& pos);


/**
 * @brief Function to take the dot-
 * product of a position pos.
 * 
 * @param pos1 The first position to 
 * take the dot-product
 * 
 * @param pos2 The second position to
 * take the dot-product
 */
double pos_dot(const Pos& pos1, const Pos & pos2);


/**
 * @brief Function to take the cross-
 * product of a position pos.
 * 
 * @param pos The position to take
 * the cross-product of
 */
Pos pos_cross(const Pos& pos1, const Pos& pos2);


/**
 * @brief Operator overload of - for Pos
 */
Pos operator-(const Pos& pos1, const Pos& pos2);


/**
 * @brief Operator overload of * for Pos
 */
Pos operator*(double val, const Pos& pos);


/**
 * @brief Operator overload of + for Pos
 */
Pos operator+(const Pos& pos1, const Pos& pos2);


/**
 * @brief Operator overload of / for Pos.
 * The function takes in a position, and how
 * much the point should be normalized by
 * 
 * @param pos The postion to be normalized
 * 
 * @param den Denominator. How much the pos
 * should be normalized
 */
Pos operator/(const Pos& pos, const double& den);


/**
 * @brief Function to find the intersections between
 * three spheres. The function could therefore be used
 * to triliterate the position. Returns a pair of values
 * 
 * @param pos1 Center for sphere 1
 * @param pos2 Center for sphere 2
 * @param pos3 Center for sphere 3
 *  
 * @param rad1 Radius for sphere 1
 * @param rad2 Radius for sphere 2
 * @param rad3 Radius for sphere 3
 * 
 * @warning NEEDS TO HAVE UNDERSTANDABLE VARIABLE-NAMES!!
 * 
 * @warning Outdated!   
 */
//std::pair<Pos, Pos> triliterate(Pos pos1, Pos pos2, 
//       Pos pos3, double rad1, double rad2, double rad3);


/**
 * @brief Function to calculate an estimate for the distance
 * given a measurement
 * 
 * @param intensity The strenght of the last measurement
 */
double estimate_distance(double intensity);


/**
 * @brief Function to calculate a rough estimate for the angle
 * 
 * @param time_difference The time-difference between two signals
*/
double estimate_rough_angle(uint32_t time_difference);


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
std::pair<double, bool> estimate_lateral(uint32_t time_port, 
                uint32_t time_starboard);


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
bool estimate_longitude(uint32_t time_port, uint32_t time_starboard,
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
std::pair<double, double> estimate_pinger_position(uint32_t time_port,
            uint32_t time_starboard, uint32_t time_stern, 
            double intensity_port, double intensity_starboard,
            double intensity_stern);

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
 * @warning Not implemented as of 10.11.2020
 */
bool check_valid_signals(uint32_t time_port, uint32_t time_starboard,
            uint32_t time_stern);


} // namespace TRILITERATION

#endif // ACOUSTICS_TRILITERATE_H
