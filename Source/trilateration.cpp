#include "trilateration.h"

/**
 * Initializing the variables maximum_time_diff and max_hydrophone_distance.
 * These values are updated in the function initialize_trilateration_globals
 * 
 * Initialized to -1 such that it's easy to check if the variables are incorrect
 */
float32_t TRILATERATION::max_hydrophone_distance = -1;
float32_t TRILATERATION::max_time_diff = -1;


/**
 * Constructor for Pos
 */
TRILATERATION::Pos::Pos(float32_t x, float32_t y, float32_t z) : 
        x{x}, y{y}, z{z} {}


/**
 * Calculating distance between postions and the global variables
 */
float32_t TRILATERATION::calculate_pos_distance(
                const Pos& pos_lhs, const Pos& pos_rhs){
        return (float32_t)(std::sqrt(
                std::pow(pos_lhs.x - pos_rhs.x, 2) +
                std::pow(pos_lhs.y - pos_rhs.y, 2) +
                std::pow(pos_lhs.z - pos_rhs.z, 2)));
}


uint8_t TRILATERATION::initialize_trilateration_globals(
                const Pos& pos_hyd_port, const Pos& pos_hyd_starboard,
                const Pos& pos_hyd_stern){
        float32_t dist_port_starboard = 
                TRILATERATION::calculate_pos_distance(pos_hyd_port, pos_hyd_starboard);
        float32_t dist_port_stern = 
                TRILATERATION::calculate_pos_distance(pos_hyd_port, pos_hyd_stern);
        float32_t dist_starboard_stern =
                TRILATERATION::calculate_pos_distance(pos_hyd_starboard, pos_hyd_stern);
        TRILATERATION::max_hydrophone_distance = 
                std::max(dist_port_starboard, std::max(dist_starboard_stern, dist_port_stern));
        TRILATERATION::max_time_diff = (1 + TRILATERATION::time_diff_epsilon) *
                (TRILATERATION::max_hydrophone_distance / TRILATERATION::sound_speed);
        return (TRILATERATION::max_hydrophone_distance != -1 && 
                TRILATERATION::max_time_diff != 1);
}


/**
 * Functions for trilateration, estimation and calculation for
 * the position and angles
 */
float32_t TRILATERATION::estimate_distance(float32_t intensity){
    /**
     * The proper line using _MATH_H::M_PI throws an error. IDK why
     */

    /* return (float32_t)(sqrt(TRILATERATION::source_power / (4*_MATH_H_::M_PI*intensity))); */
    return (float32_t)(sqrt(TRILATERATION::source_power / (4*pi*intensity)));
}


float32_t TRILATERATION::estimate_rough_angle(uint32_t time_difference){
    /** 
     * M_PI_2 is a macro for M_PI / 2
     * The proper line using _MATH_H::M_PI_2 throws an error. IDK why
     */

    /* return (float32_t)((_MATH_H_::M_PI_2) * (time_difference / TRILATERATION::max_time_diff)); */
    return (float32_t)(pi_2 * (time_difference / TRILATERATION::max_time_diff));
}


std::pair<float32_t, uint8_t> TRILATERATION::estimate_latitude(
                uint32_t lag_port, 
                uint32_t lag_starboard){

    uint32_t closest_time = std::min(lag_port, lag_starboard);
    uint32_t farthest_time = std::max(lag_port, lag_starboard);

    uint32_t time_diff = farthest_time - closest_time;
    float32_t rough_angle = TRILATERATION::estimate_rough_angle(time_diff);

    uint8_t bool_starboard = (closest_time == lag_starboard);
    return std::pair<float32_t, uint8_t> (rough_angle, bool_starboard);
}


uint8_t TRILATERATION::estimate_longitude(
                uint32_t lag_port, 
                uint32_t lag_starboard, 
                uint32_t lag_stern){

    uint32_t time_longitude = (uint32_t)(std::max(lag_port, lag_starboard) -
            std::min(lag_port, lag_starboard))/2.0;
    return (time_longitude <= lag_stern);
}


float32_t TRILATERATION::estimate_signal_intensity(float32_t* p_signal_data){
        /* Initializating variables */
        float32_t signal_energy = 0;
        float32_t signal_mean_energy, signal_intensity;

        /** 
         * Using Parseval's theorem
         * Total energy = sum(abs(data)^2)/len(data)
         */ 
        for(int i = 0; i < DSP_CONSTANTS::DMA_BUFFER_LENGTH; i++){
                signal_energy += std::pow(p_signal_data[i], 2);
        }
        signal_energy /= DSP_CONSTANTS::DMA_BUFFER_LENGTH;

        /* Calculating signal intensity in dB */
        signal_mean_energy = signal_energy / 
                (DSP_CONSTANTS::SAMPLE_TIME * DSP_CONSTANTS::DMA_BUFFER_LENGTH);
        signal_intensity = 20 * log10(signal_mean_energy);
        
        return signal_intensity;
}



std::pair<float32_t, float32_t> TRILATERATION::estimate_pinger_position(
            uint32_t* p_lag_array,
            float32_t* p_intensity_array){

        /**
         * Recovering the values from the arrays
         */
        uint32_t lag_port, lag_starboard, lag_stern;
        float32_t intensity_port, intensity_starboard, intensity_stern;

        lag_port = p_lag_array[0];
        lag_starboard = p_lag_array[1];
        lag_stern = p_lag_array[2];

        intensity_port = p_intensity_array[0];
        intensity_starboard = p_intensity_array[1];
        intensity_stern = p_intensity_array[2];


    /* Estimating the likely lateral/longitude for the acoustic pinger */
    std::pair<float32_t, uint8_t> lateral_estimate = 
            TRILATERATION::estimate_latitude(lag_port, lag_starboard);
    uint8_t longitude_estimate = TRILATERATION::estimate_longitude(
            lag_port, lag_starboard, lag_stern);
        
    /* Estimating the distances */
    float32_t distance_port = TRILATERATION::estimate_distance(
            intensity_port);
    float32_t distance_starboard = TRILATERATION::estimate_distance(
            intensity_starboard);
    float32_t distance_stern = TRILATERATION::estimate_distance(
            intensity_stern);

    /** 
     * Averaging the distance-estimates to trilaterate the 
     * position of the pinger
     */
    float32_t distance_source = (distance_port + distance_starboard
            + distance_stern) / 3.0;

    /** 
     * Using the likely lateral/longitude to calculate the estimated
     * position for the acoustic pinger
     */
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


void TRILATERATION::calculate_distance_and_angle(
        const std::pair<float32_t, float32_t>& position_estimate,
        float32_t* p_distance_estimate,
        float32_t* p_angle_estimate){
        
        /* Calculating the distance */
        Pos current_pos{0, 0, 0};
        Pos estimated_pos{position_estimate.first, position_estimate.second, 0};
        *p_distance_estimate = calculate_pos_distance(current_pos, estimated_pos);

        /* Calculating the angle from the AUV */
        *p_angle_estimate = _MATH_H_::atan2(position_estimate.second, position_estimate.first);
}



/**
 * Functions to check if signals/data are valid
 */
uint8_t TRILATERATION::valid_time_check(const uint32_t& time_lhs, const uint32_t& time_rhs){
        int32_t time_diff = time_lhs - time_rhs;
        return (std::abs(time_diff) * DSP_CONSTANTS::SAMPLE_TIME)
		> TRILATERATION::max_time_diff;
}


uint8_t TRILATERATION::valid_intensity_check(const float32_t& intensity_lhs, 
        const float32_t& intensity_rhs){
    return std::abs(intensity_lhs - intensity_rhs) > TRILATERATION::max_intensity_diff;
                
}


uint8_t TRILATERATION::check_valid_signals(
        uint32_t* p_lag_array,
        float32_t* p_intensity_array,
        uint8_t* p_bool_time_error, 
        uint8_t* p_bool_intensity_error){
        
        /**
         * Recovering the values from the arrays
         */
        uint32_t lag_port, lag_starboard, lag_stern;
        float32_t intensity_port, intensity_starboard, intensity_stern;

        lag_port = p_lag_array[0];
        lag_starboard = p_lag_array[1];
        lag_stern = p_lag_array[2];

        intensity_port = p_intensity_array[0];
        intensity_starboard = p_intensity_array[1];
        intensity_stern = p_intensity_array[2];

        /**
         * Evaluating if the signals are valid in time
        */
        if(TRILATERATION::valid_time_check(lag_port, lag_starboard) || 
        TRILATERATION::valid_time_check(lag_port, lag_stern) || 
        TRILATERATION::valid_time_check(lag_starboard, lag_stern))
                *p_bool_time_error = 1;

        /**
         * Evaluating if the signals are valid in intensity
         */
        if(TRILATERATION::valid_intensity_check(intensity_port, intensity_starboard) ||
        TRILATERATION::valid_intensity_check(intensity_port, intensity_stern) ||
        TRILATERATION::valid_intensity_check(intensity_starboard, intensity_stern))
                *p_bool_intensity_error = 1;

        /**
         * Returning the test-results
         */
        return !((*p_bool_intensity_error) || (*p_bool_time_error));
}


/**
 * Functions to tranform the data such that the intensity calculated is
 * given in the correct units
 */
void TRILATERATION::transform_data(float32_t* p_data);