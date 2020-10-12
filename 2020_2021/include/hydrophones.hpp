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

#include "../include/triliteration.hpp"

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