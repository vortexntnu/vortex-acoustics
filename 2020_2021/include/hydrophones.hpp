/**
 * @file
 * @brief Small library that implements the 
 * hydrophones for the AUV for Vortex NTNU
 * 
 * @note Could be improved by using the pose
 * from ROS. Should be implemented in the 
 * future
 */
#ifndef ACOUSTICS_HYDROPHONES_HPP
#define ACOUSTICS_HYDROPHONES_HPP

#include "DSP.hpp"
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

    Pose(double x, double y, 
         double z, double h_x, 
         double h_y, double h_z);
};

/**
 * @brief Class for the hydrophones. 
 * Combines the basic functions for
 * each hydrophone. Makes it a lot 
 * easier to implement
 * 
 * @warning To be improved in the 
 * future! Still work in progress 
 */
class Hydrophones{
private:
    Pose pose;

public:
    Hydrophones(Pose pose);
    ~Hydrophones();
};

#endif //ACOUSTICS_HYDROPHONES_HPP