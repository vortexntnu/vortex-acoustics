/**
 * @file
 * 
 * @brief Basic functions to trilaterate the position of
 * the acoustic pinger. Preferable to change it with the MANYEARS-
 * library
 */
#ifndef ACOUSTICS_TRILATERATION_H
#define ACOUSTICS_TRILATERATION_H

#include <algorithm> 
#include <Eigen/Dense>
#include <Eigen/LU> 
#include <Eigen/Core>

#include "DSP_constants.h"

/**
 * @brief Typedef used during trilateration
 */
typedef Eigen::Matrix<float32_t, 2, 3> Matrix_2_3_f;
typedef Eigen::Matrix<float32_t, 2, 1> Vector_2_1_f;

/**
 * @brief Namespace/wrapper for the trilateration
 */
namespace TRILATERATION{

/**
 * @brief Constants used for the trilaterations 
 * 
 * @param sound_speed Speed of sound in water in m/s
 * 
 * @param source_power The power from each pinger in dB.
 * Warning: requires us to know the type of pinger 
 * and the battery-voltage as these are time varying
 * parameters
 * 
 * @param max_intensity_diff Number of dB we allow the detected
 * signals to differentiate from each other.
 * 
 * @param time_diff_epsilon Safety margin to reduce the 
 * likelyhood of an error. A value of 0.1 shows that we accept
 * signals that arrive 10 % later. 
 * NOTE: 0 <= time_diff_epsilon < 1
 * 
 * @param max_hydrophone_distance The maximum distance 
 * measured between the hydrophones. 
 * 
 * @param max_time_diff The maximum time-difference that
 * should be possible between the data signals
 * 
 * @warning max_hydrophone_distance and maximum_time_diff are
 * decleared as extern, as they are decleared in .cpp 
 */
const uint16_t sound_speed          = 1480;
const float32_t source_power        = 177;
extern float32_t max_hydrophone_distance; 
extern float32_t max_time_diff;

/**
 * @brief Struct to keep the position
 * for an object/hydrophone. Could also
 * have used the POS found inside ROS
 */
struct Pos{
    /**
     * @brief Describes the position in 
     * x, y and z relative to the center 
     * of the AUV.
     */
    float32_t x, y, z;

    /**
     * @brief Constructor.
     */
    Pos(float32_t x, float32_t y, float32_t z);
}; /* struct Pos */


/**
 * @brief Helper function. Calculates the distance betweeen
 * two Pos
 * 
 * @retval Returns the distance between two positions
 * 
 * @param pos_lhs First position
 * 
 * @param pos_rhs Second position
 */
float32_t calculate_pos_distance(
            const Pos& pos_lhs, 
            const Pos& pos_rhs);


/**
 * @brief Function that calculates the maximum distance
 * between the hydrophones, and use it to calculate the
 * allowed time_difference. See the overview in hydrophones.h
 * for a better explenation of the hydrophones positioning
 * 
 * @retval Returns 1/0 to indicate whether the values are initialized
 * correctly. Returns 0 if either max_hydrophone_distance or
 * maximum_time_diff is unchanged
 * 
 * @param pos_hyd_port Position of port hydrophone
 * 
 * @param pos_hyd_starboard Position of starboard hydrophone
 * 
 * @param pos_hyd_stern Position of stern hydrophone 
 */
uint8_t initialize_trilateration_globals(
            const Pos& pos_hyd_port,
            const Pos& pos_hyd_starboard, 
            const Pos& pos_hyd_stern);


/**
 * @brief Initializes the matrix @p A to a 2x3 0-matrix
 * 
 * @retval Returns a 2x3 matrix with all entries set to 0
 */
Matrix_2_3_f initialize_A_matrix();

/**
 * @brief Initializes the vector @p B to a 2x3 0-matrix
 * 
 * @retval Returns a 2x1 vector with both entries set to 0
 */
Vector_2_1_f initialize_B_matrix();


/**
 * @brief Function to check the validy of each signal 
 * 
 * @retval Returns true if the values are valid, and false if not. 
 * If false is returned, @p p_bool_time_error is set to 1
 * 
 * @param lag_array Array containing the measured lags. 
 * @p p_lag_array expands to 
 *      *p_lag_array = { lag_port, lag_starboard, lag_stern }
 *
 * @param bool_time_error Int used to indicate time-error
 */
uint8_t check_valid_signals(
            uint32_t* p_lag_array,
            float32_t* p_intensity_array,
            uint8_t& p_bool_time_error); 


/**
 * @brief Helper function. Checks if the time-difference between two 
 * signals are valid. The time difference between the signals are checked 
 * against TRILATERATION::maximum_time_diff
 * 
 * @retval Returns false/true (0/1) depending on the result of the test
 * 
 * @param time_lhs One of the time-samples to check against
 * 
 * @param time_rhs The other time-sample to check 
 */
uint8_t valid_time_check(
            const uint32_t& time_lhs, 
            const uint32_t& time_rhs);


/**
 * @brief Function to trilaterate the position of the acoustic pinger based
 * on the time of arrival. The function uses the TDOA and linear algebra to 
 * calculate the position using minimal square error.
 * 
 * The mathematics are copy/paste from 
 * https://math.stackexchange.com/questions/1722021/trilateration-using-tdoa
 * 
 * Abbrivations used:
 *      TOA: Time of arrival
 *      TDOA: Time-difference of arrival
 * 
 * @retval Returns the estimated x- and y-value indirectly using references
 * 
 * @warning The code assumes that the hydrophones are on the same plane/level 
 * as the acoustic pinger. The estimates will therefore exceed the actual 
 * position of the acoustic pinger somewhat.
 * 
 * @warning The code does not take into consideration any pitch/roll which will
 * affect the hydrophones' position
 * 
 * @param A A @c Matrix_2_3_f that holds the positions of, and the distances between
 * the hydrophones. Size: 4x4
 * 
 * @param B A @c Vector_2_1_f that holds the minimal solutions to the equations. 
 * Size: 4x1
 * 
 * @param p_lag_array Pointer to an array containing the measured
 * lags. @p p_lag_array expands to 
 *      *p_lag_array = { lag_port, lag_starboard, lag_stern }
 * 
 * @param x_estimate Reference to the estimated x-position. Used to return
 * the x-position indirectly
 * 
 * @param y_estimate Reference to the estimated y-position. Used to return
 * the y-position indirectly
 */
uint8_t trilaterate_pinger_position(
            Matrix_2_3_f& A,
            Vector_2_1_f& B,
            uint32_t* p_lag_array,
            float32_t& x_estimate,
            float32_t& y_estimate); 
            
/**
 * @brief Helper-function that set the matrices @p A and @p B to the desired
 * values specified in @p TDOA_array
 * 
 * The values of @p A and @p B are calculated from @p TDOA_array and the position
 * of the hydrophones. Calcualtions are described at
 * https://math.stackexchange.com/questions/1722021/trilateration-using-tdoa
 * 
 * @param TDOA_array Array containg the calculated TDOA 
 * The array is given as 
 *      @p TDOA_array = { TDOA_port_starboard, TDOA_port_stern, TDOA_starboard_stern }
 * where ex. TDOA_port_starboard is the time-difference between port and starboard
 * 
 * @param A The matrix containing the parameters to the linear equations
 * 
 * @param B A vector containing the solutions to the linear equations
 */
void calculate_tdoa_matrices(
            float32_t* TDOA_array, 
            Matrix_2_3_f& A,
            Vector_2_1_f& B);



} /* namespace TRILATERATION */

#endif /* ACOUSTICS_TRILATERATION_H */
