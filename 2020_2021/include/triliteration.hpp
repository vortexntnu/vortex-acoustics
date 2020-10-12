/**
 * @file
 * 
 * @brief Basic functions to triliterate the position
 */

#ifndef ACOUSTICS_TRILITERATE_HPP
#define ACOUSTICS_TRILITERATE_HPP

#include <math.h>

#include "../include/DSP.hpp"
//Find the ROS pose-header/functions

/**
 * @brief Struct to keep the position
 * and heading of each object/hydrophone
 * 
 * @warning Should be replaced with the 
 * Pose included in ROS!
 */
struct Pose{
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
     * @brief Two different constructors.
     * Depends whether the heading should
     * be included or not
     */
    Pose(double x, double y, 
         double z, double h_x, 
         double h_y, double h_z);

    Pose(double x, double y,
        double z);

};


/**
 * @brief Operator overload of - for Pose
 */
Pose& operator-(const Pose& pos1, const Pose& pos2);

/**
 * @brief Operator overload of / for Pose.
 * The function takes in a position, and how
 * much the point should be normalized by
 * 
 * @param pose The postion to be normalized
 * 
 * @param den Denominator. How much the pose
 * should be normalized
 */
Pose& operator/(const Pose& pose, const double& den);


//Need to implement the dot and cross-
//product for pose in some way



/**
 * @brief Function to find the intersections between
 * three spheres. The function could therefore be used
 * to triliterate the position
 * 
 * @param pos1 Center for sphere 1
 * @param pos2 Center for sphere 2
 * @param pos3 Center for sphere 3
 *  
 * @param rad1 Radius for sphere 1
 * @param rad2 Radius for sphere 2
 * @param rad3 Radius for sphere 3   
 */
void triliterate(Pose pos1, Pose pos2, Pose pos3, 
            double rad1, double rad2, double rad3);


#endif //ACOUSTICS_TRILITERATE_HPP
