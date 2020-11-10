#include "triliteration.h"

/**
 * Constructor
 */
TRILITERATION::Pos::Pos(double x, double y, double z) : 
        x(x), y(y), z(z) {}


/**
 * Operator overload for Pos
 */
TRILITERATION::Pos operator*(double val, const TRILITERATION::Pos& pos){
    return TRILITERATION::Pos(pos.x * val, pos.y * val, pos.z * val);
}

TRILITERATION::Pos operator/(const TRILITERATION::Pos& pos, const double& den){
    return TRILITERATION::Pos(pos.x / den, pos.y / den, pos.z / den);
}


TRILITERATION::Pos operator-(const TRILITERATION::Pos& pos1, 
        const TRILITERATION::Pos& pos2){
    return TRILITERATION::Pos(pos2.x - pos1.x, pos2.y - pos1.y, 
        pos2.z - pos1.z);
}

TRILITERATION::Pos operator+(const TRILITERATION::Pos& pos1, 
        const TRILITERATION::Pos& pos2){
    return TRILITERATION::Pos(pos1.x + pos2.x, pos1.y + pos2.y, 
        pos1.z + pos2.z);
}



/**
 * Helper functions for Pos and linalg
 */
TRILITERATION::Pos pos_cross(const TRILITERATION::Pos& pos1, 
        const TRILITERATION::Pos& pos2){
    //Find a good way to implement this
    return TRILITERATION::Pos(0, 0, 0);
}


double TRILITERATION::pos_norm(const TRILITERATION::Pos& pos){
    return sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2));
}


double TRILITERATION::pos_dot(const TRILITERATION::Pos& pos1, 
        const TRILITERATION::Pos& pos2){
    return (pos1.x * pos2.x + 
        pos1.y * pos2.y + pos1.z * pos2.z);
}



/**
 * Functions for triliteration, estimation and calculation for
 * the position and angles. Must be updated with better names!
 */

// Some outdated code
/*
std::pair<TRILITERATION::Pos, TRILITERATION::Pos> 
        TRILITERATION::triliterate(TRILITERATION::Pos pos1, 
        TRILITERATION::Pos pos2, TRILITERATION::Pos pos3,
                double rad1, double rad2, double rad3){
    
    //Just what the fuck does these varible-names mean???
    // Change them, please!
    
    using namespace TRILITERATION;

    Pos temp1 = pos2 - pos1;
    Pos e_x = temp1 / pos_norm(temp1);
    Pos temp2 = pos3 - pos1;

    double i = pos_dot(e_x, temp2);
    Pos temp3 = temp2 - i * e_x;
    Pos e_y = temp3 / pos_norm(temp3);

    Pos e_z = pos_cross(e_x, e_y);
    double d = pos_norm(pos2 - pos1);
    double j = pos_dot(e_y, temp2);
    double x = (pow(rad1, 2) - pow(rad2, 2) + pow(d, 2)) /
                (2 * d);
    double y = (pow(rad1, 2) - pow(rad3, 2) - 2 * i * x +
                 pow(i, 2) + pow(j, 2)) / (2 * d);
    double temp4 = pow(rad1, 2) - pow(x, 2) - pow(y, 2);
    if(temp4 < 0){
        throw("The three spheres do not intersect");
    }
    double z = sqrt(temp4);
    Pos p_12_a = pos1 + x * e_x + y * e_y + z * e_z;
    Pos p_12_b = pos2 + x * e_x + y * e_y - z * e_z;

    return std::make_pair(p_12_a, p_12_b);
}
*/

double TRILITERATION::estimate_distance(double intensity){
    return sqrt(TRILITERATION::source_power/(4*DSP::PI*intensity));
}


double TRILITERATION::estimate_rough_angle(uint32_t time_difference){
    return (double)((DSP::PI/2)*(time_difference/TRILITERATION::maximum_time_diff));
}

std::pair<double, bool> TRILITERATION::estimate_lateral(
            uint32_t time_port, uint32_t time_starboard){

    uint32_t closest_time = std::min(time_port, time_starboard);
    uint32_t farthest_time = std::max(time_port, time_starboard);

    uint32_t time_diff = farthest_time - closest_time;
    uint32_t rough_angle = TRILITERATION::estimate_rough_angle(time_diff);

    bool bool_starboard = (closest_time == time_starboard);
    return std::pair<double, bool> (rough_angle, bool_starboard);
}


bool TRILITERATION::estimate_longitude(uint32_t time_port, 
            uint32_t time_starboard, uint32_t time_stern){

    uint32_t time_longitude = (std::max(time_port, time_starboard) -
            std::min(time_port, time_starboard))/2.0;
    return (time_longitude <= time_stern);
}


std::pair<double, double> TRILITERATION::estimate_pinger_position(
            uint32_t time_port, uint32_t time_starboard, uint32_t time_stern, 
            double intensity_port, double intensity_starboard,
            double intensity_stern){

    // Estimating the likely lateral/longitude for the acoustic pinger 
    std::pair<double, bool> lateral_estimate = 
            TRILITERATION::estimate_lateral(time_port, time_starboard);
    bool longitude_estimate = TRILITERATION::estimate_longitude(
            time_port, time_starboard, time_stern);
        
    // Estimating the distances
    double distance_port = TRILITERATION::estimate_distance(
            intensity_port);
    double distance_starboard = TRILITERATION::estimate_distance(
            intensity_starboard);
    double distance_stern = TRILITERATION::estimate_distance(
            intensity_stern);

    // Averaging the distance-estimates to triliterate the 
    // position of the pinger
    double distance_source = (distance_port + distance_starboard
            + distance_stern) / 3.0;

    // Using the likely lateral/longitude to calculate the estimated
    // position for the acoustic pinger
    double x, y;
    if(!longitude_estimate)
        y = distance_source * sin(lateral_estimate.first);
    else
        y = distance_source * sin(-1 * lateral_estimate.first);
    if(!lateral_estimate.second)
        x = distance_source * cos(lateral_estimate.first) * (-1);
    else
        x = distance_source * cos(lateral_estimate.second);
    return std::pair<double, double> (x, y);
}


