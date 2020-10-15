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

namespace HYDROPHONES{

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
    TRILITERATION::Pos pos;

public:
    Hydrophones(TRILITERATION::Pos pos);
    ~Hydrophones();
};

} //HYDROPHONES

#endif //ACOUSTICS_HYDROPHONES_HPP