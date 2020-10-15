/**
 * @file
 * 
 * @brief Basic functions to triliterate the position
 */

#ifndef ACOUSTICS_TRILITERATE_HPP
#define ACOUSTICS_TRILITERATE_HPP

#include <math.h>

#include "../include/DSP.hpp"
//Find the ROS pos-header/functions

/**
 * @brief Namespace/wrapper for the triliteration
 */
namespace TRILITERATION{

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
     */
    double h_x, h_y, h_z;

    /**
     * @brief Constructor.
     */
    Pos(double x, double y,
        double z);
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
 */
std::pair<Pos, Pos> triliterate(Pos pos1, Pos pos2, 
        Pos pos3, double rad1, double rad2, double rad3);


} //TRILITERATION

#endif //ACOUSTICS_TRILITERATE_HPP
