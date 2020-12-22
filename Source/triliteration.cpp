#include "triliteration.h"

/**
 * Initializing the variables maximum_time_diff and max_hydrophone_distance.
 * These values are updated in the function initialize_triliteration_globals
 * 
 * Initialized to -1 such that it's easy to check if the variables are incorrect
 */
float32_t TRILITERATION::max_hydrophone_distance = -1;
float32_t TRILITERATION::max_time_diff = -1;


/**
 * Constructor for Pos
 */
TRILITERATION::Pos::Pos(float32_t x, float32_t y, float32_t z) : 
        x(x), y(y), z(z) {}


/**
 * Calculating distance between postions and the global variables
 */
float32_t TRILITERATION::calculate_distance(
                const Pos& pos_lhs, const Pos& pos_rhs){
        return (float32_t)(std::sqrt(
                std::pow(pos_lhs.x - pos_rhs.x, 2) +
                std::pow(pos_lhs.y - pos_rhs.y, 2) +
                std::pow(pos_lhs.z - pos_rhs.z, 2)));
}


uint8_t TRILITERATION::initialize_triliteration_globals(
                const Pos& pos_hyd_port, const Pos& pos_hyd_starboard,
                const Pos& pos_hyd_stern){
        float32_t dist_port_starboard = 
                TRILITERATION::calculate_distance(pos_hyd_port, pos_hyd_starboard);
        float32_t dist_port_stern = 
                TRILITERATION::calculate_distance(pos_hyd_port, pos_hyd_stern);
        float32_t dist_starboard_stern =
                TRILITERATION::calculate_distance(pos_hyd_starboard, pos_hyd_stern);
        TRILITERATION::max_hydrophone_distance = 
                std::max(dist_port_starboard, std::max(dist_starboard_stern, dist_port_stern));
        TRILITERATION::max_time_diff = (1 + TRILITERATION::time_diff_epsilon) *
                (TRILITERATION::max_hydrophone_distance / TRILITERATION::sound_speed);
        return (TRILITERATION::max_hydrophone_distance != -1 && 
                TRILITERATION::max_time_diff != 1);
}


/**
 * Functions for triliteration, estimation and calculation for
 * the position and angles
 */
float32_t TRILITERATION::estimate_distance(float32_t intensity){
    return (float32_t)(sqrt(TRILITERATION::source_power / (4*_MATH_H_::M_PI*intensity)));
}


float32_t TRILITERATION::estimate_rough_angle(uint32_t time_difference){
    return (float32_t)((_MATH_H_::M_PI_2) * (time_difference / TRILITERATION::max_time_diff));
}


std::pair<float32_t, uint8_t> TRILITERATION::estimate_lateral(
            uint32_t time_port, uint32_t time_starboard){

    uint32_t closest_time = std::min(time_port, time_starboard);
    uint32_t farthest_time = std::max(time_port, time_starboard);

    uint32_t time_diff = farthest_time - closest_time;
    float32_t rough_angle = TRILITERATION::estimate_rough_angle(time_diff);

    uint8_t bool_starboard = (closest_time == time_starboard);
    return std::pair<float32_t, uint8_t> (rough_angle, bool_starboard);
}


uint8_t TRILITERATION::estimate_longitude(uint32_t time_port, 
            uint32_t time_starboard, uint32_t time_stern){

    uint32_t time_longitude = (uint32_t)(std::max(time_port, time_starboard) -
            std::min(time_port, time_starboard))/2.0;
    return (time_longitude <= time_stern);
}


std::pair<float32_t, float32_t> TRILITERATION::estimate_pinger_position(
            uint32_t time_port, uint32_t time_starboard, uint32_t time_stern, 
            float32_t intensity_port, float32_t intensity_starboard,
            float32_t intensity_stern){

    // Estimating the likely lateral/longitude for the acoustic pinger 
    std::pair<float32_t, uint8_t> lateral_estimate = 
            TRILITERATION::estimate_lateral(time_port, time_starboard);
    uint8_t longitude_estimate = TRILITERATION::estimate_longitude(
            time_port, time_starboard, time_stern);
        
    // Estimating the distances
    float32_t distance_port = TRILITERATION::estimate_distance(
            intensity_port);
    float32_t distance_starboard = TRILITERATION::estimate_distance(
            intensity_starboard);
    float32_t distance_stern = TRILITERATION::estimate_distance(
            intensity_stern);

    // Averaging the distance-estimates to triliterate the 
    // position of the pinger
    float32_t distance_source = (distance_port + distance_starboard
            + distance_stern) / 3.0;

    // Using the likely lateral/longitude to calculate the estimated
    // position for the acoustic pinger
    float32_t x, y;
    if(!longitude_estimate)
        y = distance_source * std::sin(lateral_estimate.first);
    else
        y = distance_source * std::sin(-1 * lateral_estimate.first);
    if(!lateral_estimate.second)
        x = distance_source * std::cos(lateral_estimate.first) * (-1);
    else
        x = distance_source * std::cos(lateral_estimate.second);
    return std::pair<float32_t, float32_t> (x, y);
}


/**
 * Functions to check if signals/data are valid
 */
uint8_t valid_time_check(const uint32_t& time_lhs, const uint32_t& time_rhs){
        return ((float32_t)(abs(time_lhs - time_rhs) * DSP_CONSTANTS::SAMPLE_TIME) 
                > TRILITERATION::max_time_diff);
}


uint8_t valid_intensity_check(const float32_t& intensity_lhs, 
        const float32_t& intensity_rhs){
    return 1;                
}


uint8_t TRILITERATION::check_valid_signals(
        const uint32_t& time_port, const uint32_t& time_starboard, 
        const uint32_t& time_stern, const float32_t& intensity_port, 
        const float32_t& intensity_starboard, const float32_t& intensity_stern){

        /**
         * Here it is difficult to estimate the signals, as the signals must be
         * evaluated in both regards to the signal intensity and the signal time 
        */
        if(valid_time_check(time_port, time_starboard) || 
        valid_time_check(time_port, time_stern) || 
        valid_time_check(time_starboard, time_stern))
                return (uint8_t) 0;

        // Intensity is not implemented as of 12.12.2020, however time is more
        // important in the beginning - and time is more straightforward to 
        // implement, while the intensity might require more data from the AUV.

        return (uint8_t) 1;
}