/**
 * @file
 *
 * @brief Basic functions to trilaterate the position of
 * the acoustic pinger. Preferable to change it with the MANYEARS-
 * library
 */
#ifndef ACOUSTICS_TRILATERATION_H
#define ACOUSTICS_TRILATERATION_H

/**
 * @brief Defines used to specify the workflow from the Eigen-library
 */
#ifndef EIGEN_DEFINES
#define EIGEN_DEFINES

#ifndef EIGEN_NO_STATIC_ASSERT
#define EIGEN_NO_STATIC_ASSERT /* Prevents static assertion */
#endif                         /* EIGEN_NO_STATIC_ASSERT    */

#endif /* EIGEN_DEFINES */

#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/LU>
#include <algorithm>

#include "parameters.h"

/**
 * @brief Typedef used during trilateration
 */
typedef Eigen::Matrix<float32_t, 2, 3> Matrix_2_3_f;
typedef Eigen::Matrix<float32_t, 2, 1> Vector_2_1_f;

/**
 * @brief Namespace/wrapper for the trilateration
 */
namespace TRILATERATION {

/**
 * @brief Constants used for the trilaterations
 *
 * @param max_hydrophone_distance The maximum distance
 * measured between the hydrophones.
 *
 * @param max_time_diff The maximum time-difference that
 * should be possible between the data signals
 */
extern float32_t max_hydrophone_distance;
extern float32_t max_time_diff;

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
uint8_t initialize_trilateration_globals();

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
Vector_2_1_f initialize_B_vector();

/**
 * @brief Function that check if the global variables
 * @p max_hydrophone_distance and @p max_time_diff have been
 * initialized correctly
 *
 * @retval Returns true if set up correctly
 */
uint8_t check_initialized_globals();

/**
 * @brief Function to check the validy of each signal
 *
 * @retval Returns true if the values are valid, and false if not.
 * If false is returned, @p bool_time_error is set to 1
 *
 * @param p_lag_array Array containing the measured lags.
 * @p lag_array expands to
 *      p_lag_array = { *p_lag_port_starboard,
 *                      *p_lag_port_stern,
 *                      *p_lag_starboard_stern }
 *
 * @param bool_time_error Int used to indicate time-error
 */
uint8_t check_valid_signals(uint32_t *p_lag_array[NUM_HYDROPHONES],
                            uint8_t &bool_time_error);

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
uint8_t check_valid_time(const uint32_t &time_lhs, const uint32_t &time_rhs);

/**
 * @brief Helper function. Checks if a given time-difference is valid.
 * The time difference between the signals are checked
 * against TRILATERATION::maximum_time_diff
 *
 * @retval Returns false/true (0/1) depending on the result of the test
 *
 * @param time_diff One of the time-samples to check against
 */
uint8_t check_valid_time(const uint32_t &time_diff);

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
 * @param A A @c Matrix_2_3_f that holds the positions of, and the distances
 * between the hydrophones. Size: 4x4
 *
 * @param B A @c Vector_2_1_f that holds the minimal solutions to the equations.
 * Size: 4x1
 *
 * @param p_lag_array Array containing pointers to the cross-correlated lags.
 * @p lag_array expands to
 *      p_lag_array = { *p_lag_port_starboard,
 *                      *p_lag_port_stern,
 *                      *p_lag_starboard_stern }
 *
 * @param x_estimate Reference to the estimated x-position. Used to return
 * the x-position indirectly
 *
 * @param y_estimate Reference to the estimated y-position. Used to return
 * the y-position indirectly
 */
uint8_t trilaterate_pinger_position(Matrix_2_3_f &A, Vector_2_1_f &B,
                                    uint32_t *p_lag_array[NUM_HYDROPHONES],
                                    float32_t &x_estimate,
                                    float32_t &y_estimate);

/**
 * @brief Helper-function that set the matrices @p A and @p B to the desired
 * values specified in @p TDOA_array
 *
 * The values of @p A and @p B are calculated from @p TDOA_array and the
 * position of the hydrophones. Calcualtions are described at
 * https://math.stackexchange.com/questions/1722021/trilateration-using-tdoa
 *
 * @param TDOA_array Array containg the calculated TDOA
 * The array is given as
 *      @p TDOA_array = { TDOA_port_starboard, TDOA_port_stern,
 * TDOA_starboard_stern } where ex. TDOA_port_starboard is the time-difference
 * between port and starboard
 *
 * @param A The matrix containing the parameters to the linear equations
 *
 * @param B A vector containing the solutions to the linear equations
 */
void calculate_tdoa_matrices(float32_t TDOA_array[NUM_HYDROPHONES],
                             Matrix_2_3_f &A, Vector_2_1_f &B);

} /* namespace TRILATERATION */

#endif /* ACOUSTICS_TRILATERATION_H */
