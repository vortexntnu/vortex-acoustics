#include "trilateration.h"

import math
import parameters as param
import numpy as np



#Initializing the variables maximum_time_diff and max_hydrophone_distance.
#These values are updated in the function initialize_trilateration_globals
 
#Initialized to -1 such that it's easy to check if the variables are incorrect

max_hydrophone_distance = -1.0
max_time_diff = -1.0


#Functions for initializing 

def initialize_trilateration_globals():

    #Calculating the distances between the hydrophones 
    dist_port_starboard = float(math.sqrt(
            math.pow(param.HydrophoneDetails.STARBOARD_HYD_X - param.HydrophoneDetails.PORT_HYD_X, 2) + 
            math.pow(param.HydrophoneDetails.STARBOARD_HYD_Y - param.HydrophoneDetails.PORT_HYD_Y, 2) +
            math.pow(param.HydrophoneDetails.STARBOARD_HYD_Z - param.HydrophoneDetails.PORT_HYD_Z, 2)))
    dist_port_stern = float(math.sqrt(
                math.pow(param.HydrophoneDetails.STERN_HYD_X - param.HydrophoneDetails.PORT_HYD_X, 2) + 
                math.pow(param.HydrophoneDetails.STERN_HYD_Y - param.HydrophoneDetails.PORT_HYD_Y, 2) +
                math.pow(param.HydrophoneDetails.STERN_HYD_Z - param.HydrophoneDetails.PORT_HYD_Z, 2)))
    dist_starboard_stern = float(math.sqrt(
                math.pow(param.HydrophoneDetails.STERN_HYD_X - param.HydrophoneDetails.STARBOARD_HYD_X, 2) + 
                math.pow(param.HydrophoneDetails.STERN_HYD_Y - param.HydrophoneDetails.STARBOARD_HYD_Y, 2) +
                math.pow(param.HydrophoneDetails.STERN_HYD_Z - param.HydrophoneDetails.STARBOARD_HYD_Z, 2)))

        #Finding the maximum distance 
    max_hydrophone_distance = max(dist_port_starboard, max(dist_starboard_stern, dist_port_stern))
        
        #Calculating max time allowed over that distance 
    max_time_diff = (1 + param.SystemMargins.MARGIN_TIME_EPSILON) *(max_hydrophone_distance / param.PhysicalConstants.SOUND_SPEED)

        #Returning if both variables have been set correctly 
    return check_initialized_globals(); 


def initialize_A_matrix(): #return type Matrix_2_3_f
    A = np.zeros((2,3))
    return A


def initialize_B_vector():
    B = np.zeros((2,1))
    return B

#Functions to check if signals/data are valid
 
def check_initialized_globals(): #return type uint8_t
       return (max_hydrophone_distance != -1 and max_time_diff != 1)



def check_valid_time(time_lhs: np.uint32, time_rhs: np.uint32): #return type uint8_t
        
         
        #Calculating the time-difference and checking if it exceeds the maximum
        #allowed time for a valid signal
        
        time_diff = time_lhs - time_rhs #type int32_t
        return (abs(time_diff) * param.DSPConstants.SAMPLE_TIME) > max_time_diff


def check_valid_time(time_diff: np.uint32): #return type uint8_t

         #Checking if the time_diff exceeds the maximum allowed time for a valid signal
         
        return (abs(time_diff) * param.DSPConstants.SPSAMPLE_TIME) > max_time_diff



def check_valid_signals(p_lag_array: np.array, bool_time_error: np.uint32): 
    """
    arguments: 
      in the orginal code is was written as such: uint32_t* p_lag_array[NUM_HYDROPHONES],uint8_t& bool_time_error)
      
    returns:
      type uint8
    """

    #Evaluating if the signals are valid in time
    if(check_valid_time(p_lag_array[0]) or check_valid_time(p_lag_array[1]) or
    check_valid_time(p_lag_array[2])):
        bool_time_error = 1
        #Possible to add other tests here in the future

        #Returning the test-results
    return not (bool_time_error)



#Functions for trilateration based on TDOA 

def trilaterate_pinger_position(A,B,p_lag_array,x_estimate,y_estimate):
        """
        arg:
        A is a Matrix_2_3_f
        B is a Vector_2_1_f
        p_lag_array has length NUM_HYDROPHONES
        x_estimate and y_estimate is a float32

        returns:
        unit8
        """

        #Recovering the lags from the array 
        p_lag_port_starboard = p_lag_array[0]
        p_lag_port_stern = p_lag_array[1]
        p_lag_starboard_stern = p_lag_array[2]

        #Calculating TDOA and creating an array to hold the data 
        TDOA_port_starboard = float(
                param.DSPConstants.SAMPLE_TIME * param.PhysicalConstants.SOUND_SPEED * (p_lag_port_starboard))
        TDOA_port_stern =float(
                param.DSPConstants.SAMPLE_TIME * param.PhysicalConstants.SOUND_SPEED * (p_lag_port_stern))
        TDOA_starboard_stern =float(
                param.DSPConstants.SAMPLE_TIME * param.PhysicalConstants.SOUND_SPEED * (p_lag_starboard_stern))

        TDOA_array= [TDOA_port_starboard, TDOA_port_stern, TDOA_starboard_stern]
        
        #Calculating the matrices 
        calculate_tdoa_matrices(TDOA_array, A, B)

        #Calculating the transpose
        A_T = A.transpose() #Matrix_2_3_f 

        #Checking if A * A_T is invertible. Return 0 if not 
        if(not((A_T * A).determinant())):
                return 0

        #Calculating the solution-vector 
        solution_vec = (A_T * A).inverse() * A_T * B #Vector_2_1_f 

        #Extracting the values 
        x_estimate = solution_vec.coeff(0)
        y_estimate = solution_vec.coeff(1)

        return 1;       



def calculate_tdoa_matrices(TDOA_array,A,B):
        """
        Arg: 
        TDOA_array has len NUM_HYDROPHONES with type float32_t
        A is a Matrix_2_3f
        B is a Vector_2_1f
        """
                
        """
         * @brief Hydrophones are here labeled as a number
         *      Port hydrophone         : 0
         *      Starboard hydrophone    : 1
         *      Stern hydrophone        : 2
         * 
         * The positions and distances are therefore calculated using the
         * port hydrophone as a reference. Example:
         *      x_01 = x_0 - x_1        Difference in x-position between hyd 0 and 1
         *      x_02 = x_0 - x_2        Difference in x-position between hyd 0 and 2
         *      etc.
         * 
         * @note Only x and y is required as we are using 3 hydrophones and calculating
         * z will in most cases result in linear dependent equations 
         */
         """

        #Calculating the difference in position between the hydrophones 
        x_01 = param.HydrophoneDetails.PORT_HYD_X - param.HydrophoneDetails.STARBOARD_HYD_X; #float32
        x_02 = param.HydrophoneDetails.PORT_HYD_X - param.HydrophoneDetails.STERN_HYD_X #float32

        y_01 = param.HydrophoneDetails.PORT_HYD_Y - param.HydrophoneDetails.STARBOARD_HYD_Y #float32
        y_02 = param.HydrophoneDetails.PORT_HYD_Y - param.HydrophoneDetails.STERN_HYD_Y #float32

        #Extracting the data from the array 
        TDOA_port_starboard = TDOA_array[0] #float32
        TDOA_port_stern = TDOA_array[1] #float32
        TDOA_starboard_stern = TDOA_array[2] #float32

        #Using TDOA to calculate the distances 
        d_01 = param.PhysicalConstants.SOUND_SPEED * TDOA_port_starboard #float32
        d_02 = param.PhysicalConstants.SOUND_SPEED * TDOA_port_stern #float32

        #Setting A
        A = [[x_01, y_01, d_01], [x_02, y_02, d_02]]

        """
         * @brief Calculating the values of the B-vector
         * 
         * The variables refer to the position (1-indexed) in the vector.
         * 
         * Check the link in the .h file for a better explanation   
         """
        b1 = 1 / 2 * (
                math.pow(d_01, 2) +
                math.pow(param.HydrophoneDetails.PORT_HYD_X, 2) - math.pow(param.HydrophoneDetails.STARBOARD_HYD_X, 2) +
                math.pow(param.HydrophoneDetails.PORT_HYD_Y, 2) - math.pow(param.HydrophoneDetails.STARBOARD_HYD_Y, 2)) #float32
        
        b2 = 1 / 2 * (
                math.pow(d_02, 2) +
                math.pow(param.HydrophoneDetails.PORT_HYD_X, 2) - math.pow(param.HydrophoneDetails.STERN_HYD_X, 2) +
                math.pow(param.HydrophoneDetails.PORT_HYD_Y, 2) - math.pow(param.HydrophoneDetails.STERN_HYD_Y, 2)) #float32

        #Setting B 
        B = [b1, b2]
