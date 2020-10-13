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
 * @warning Must be evaluated if this
 * class is really necessary
 */
class Hydrophones{
private:
    Pos pos;

public:
    Hydrophones(Pos pos);
    ~Hydrophones();
};

#endif //ACOUSTICS_HYDROPHONES_HPP