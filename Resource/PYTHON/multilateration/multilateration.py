#include "trilateration.h"

import math


#Initializing the variables maximum_time_diff and max_hydrophone_distance.
#These values are updated in the function initialize_trilateration_globals
 
#Initialized to -1 such that it's easy to check if the variables are incorrect

TRILATERATION.max_hydrophone_distance = float32(-1)
TRILATERATION::max_time_diff = float32(-1)


#Functions for initializing 

def initialize_trilateration_globals():

    #Calculating the distances between the hydrophones 
    dist_port_starboard = float32(math.sqrt(
            math.pow(HydrophoneDetails.STARBOARD_HYD_X - HydrophoneDetails.PORT_HYD_X, 2) + 
            math.pow(STARBOARD_HYD_Y - PORT_HYD_Y, 2) +
            math.pow(STARBOARD_HYD_Z - PORT_HYD_Z, 2)))
    dist_port_stern = <float32> math.sqrt(
                math.pow(STERN_HYD_X - PORT_HYD_X, 2) + 
                math.pow(STERN_HYD_Y - PORT_HYD_Y, 2) +
                math.pow(STERN_HYD_Z - PORT_HYD_Z, 2))
    dist_starboard_stern = <float32_t> math.sqrt(
                math.pow(STERN_HYD_X - STARBOARD_HYD_X, 2) + 
                math.pow(STERN_HYD_Y - STARBOARD_HYD_Y, 2) +
                math.pow(STERN_HYD_Z - STARBOARD_HYD_Z, 2))

        #Finding the maximum distance 
        TRILATERATION::max_hydrophone_distance = 
                std::max(dist_port_starboard, std::max(dist_starboard_stern, dist_port_stern));
        
        /* Calculating max time allowed over that distance */
        TRILATERATION::max_time_diff = (1 + MARGIN_TIME_EPSILON) *
                (TRILATERATION::max_hydrophone_distance / SOUND_SPEED);

        /* Returning if both variables have been set correctly */
        return TRILATERATION::check_initialized_globals(); 
