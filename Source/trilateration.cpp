#include "trilateration.h"

/**
 * Initializing the variables maximum_time_diff and max_hydrophone_distance.
 * These values are updated in the function initialize_trilateration_globals
 *
 * Initialized to -1 such that it's easy to check if the variables are incorrect
 */
float32_t TRILATERATION::max_hydrophone_distance = -1;
float32_t TRILATERATION::max_time_diff = -1;

/**
 * Functions for initializing
 */
uint8_t TRILATERATION::initialize_trilateration_globals() {

  /* Calculating the distances between the hydrophones */
  float32_t dist_port_starboard =
      (float32_t)std::sqrt(std::pow(STARBOARD_HYD_X - PORT_HYD_X, 2) +
                           std::pow(STARBOARD_HYD_Y - PORT_HYD_Y, 2) +
                           std::pow(STARBOARD_HYD_Z - PORT_HYD_Z, 2));
  float32_t dist_port_stern =
      (float32_t)std::sqrt(std::pow(STERN_HYD_X - PORT_HYD_X, 2) +
                           std::pow(STERN_HYD_Y - PORT_HYD_Y, 2) +
                           std::pow(STERN_HYD_Z - PORT_HYD_Z, 2));
  float32_t dist_starboard_stern =
      (float32_t)std::sqrt(std::pow(STERN_HYD_X - STARBOARD_HYD_X, 2) +
                           std::pow(STERN_HYD_Y - STARBOARD_HYD_Y, 2) +
                           std::pow(STERN_HYD_Z - STARBOARD_HYD_Z, 2));

  /* Finding the maximum distance */
  TRILATERATION::max_hydrophone_distance = std::max(
      dist_port_starboard, std::max(dist_starboard_stern, dist_port_stern));

  /* Calculating max time allowed over that distance */
  TRILATERATION::max_time_diff =
      (1 + MARGIN_TIME_EPSILON) *
      (TRILATERATION::max_hydrophone_distance / SOUND_SPEED);

  /* Returning if both variables have been set correctly */
  return TRILATERATION::check_initialized_globals();
}

Matrix_2_3_f TRILATERATION::initialize_A_matrix() {
  Matrix_2_3_f A;
  A << 0, 0, 0, 0, 0, 0;
  return A;
}

Vector_2_1_f TRILATERATION::initialize_B_vector() {
  Vector_2_1_f B;
  B << 0, 0;
  return B;
}

/**
 * Functions to check if signals/data are valid
 */
uint8_t TRILATERATION::check_initialized_globals() {
  return (TRILATERATION::max_hydrophone_distance != -1 &&
          TRILATERATION::max_time_diff != 1);
}

uint8_t TRILATERATION::check_valid_time(const uint32_t &time_lhs,
                                        const uint32_t &time_rhs) {

  /**
   * Calculating the time-difference and checking if it exceeds the maximum
   * allowed time for a valid signal
   */
  int32_t time_diff = time_lhs - time_rhs;
  return (std::abs(time_diff) * SAMPLE_TIME) > TRILATERATION::max_time_diff;
}

uint8_t TRILATERATION::check_valid_time(const uint32_t &time_diff) {

  /**
   * Checking if the time_diff exceeds the maximum allowed time for a valid
   * signal
   */
  return (std::abs(time_diff) * SAMPLE_TIME) > TRILATERATION::max_time_diff;
}

uint8_t
TRILATERATION::check_valid_signals(uint32_t *p_lag_array[NUM_HYDROPHONES],
                                   uint8_t &bool_time_error) {

  /**
   * Evaluating if the signals are valid in time
   */
  if (TRILATERATION::check_valid_time(*p_lag_array[0]) ||
      TRILATERATION::check_valid_time(*p_lag_array[1]) ||
      TRILATERATION::check_valid_time(*p_lag_array[2]))
    bool_time_error = 1;

  /**
   * Possible to add other tests here in the future
   */

  /**
   * Returning the test-results
   */
  return !(bool_time_error);
}

/**
 * Functions for trilateration based on TDOA
 */
uint8_t TRILATERATION::trilaterate_pinger_position(
    Matrix_2_3_f &A, Vector_2_1_f &B, uint32_t *p_lag_array[NUM_HYDROPHONES],
    float32_t &x_estimate, float32_t &y_estimate) {

  /* Recovering the lags from the array */
  uint32_t *p_lag_port_starboard = p_lag_array[0];
  uint32_t *p_lag_port_stern = p_lag_array[1];
  uint32_t *p_lag_starboard_stern = p_lag_array[2];

  /* Calculating TDOA and creating an array to hold the data */
  float32_t TDOA_port_starboard =
      (float32_t)SAMPLE_TIME * SOUND_SPEED * (*p_lag_port_starboard);
  float32_t TDOA_port_stern =
      (float32_t)SAMPLE_TIME * SOUND_SPEED * (*p_lag_port_stern);
  float32_t TDOA_starboard_stern =
      (float32_t)SAMPLE_TIME * SOUND_SPEED * (*p_lag_starboard_stern);

  float32_t TDOA_array[NUM_HYDROPHONES] = {TDOA_port_starboard, TDOA_port_stern,
                                           TDOA_starboard_stern};

  /* Calculating the matrices */
  TRILATERATION::calculate_tdoa_matrices(TDOA_array, A, B);

  /* Calculating the transpose */
  Matrix_2_3_f A_T = A.transpose();

  /* Checking if A * A_T is invertible. Return 0 if not */
  if (!((A_T * A).determinant())) {
    return 0;
  }

  /* Calculating the solution-vector */
  Vector_2_1_f solution_vec = (A_T * A).inverse() * A_T * B;

  /* Extracting the values */
  x_estimate = solution_vec.coeff(0);
  y_estimate = solution_vec.coeff(1);

  return 1;
}

void TRILATERATION::calculate_tdoa_matrices(
    float32_t TDOA_array[NUM_HYDROPHONES], Matrix_2_3_f &A, Vector_2_1_f &B) {

  /**
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
   * @note Only x and y is required as we are using 3 hydrophones and
   * calculating z will in most cases result in linear dependent equations
   */

  /* Calculating the difference in position between the hydrophones */
  float32_t x_01 = PORT_HYD_X - STARBOARD_HYD_X;
  float32_t x_02 = PORT_HYD_X - STERN_HYD_X;

  float32_t y_01 = PORT_HYD_Y - STARBOARD_HYD_Y;
  float32_t y_02 = PORT_HYD_Y - STERN_HYD_Y;

  /* Extracting the data from the array */
  float32_t TDOA_port_starboard = TDOA_array[0];
  float32_t TDOA_port_stern = TDOA_array[1];
  float32_t TDOA_starboard_stern = TDOA_array[2];

  /* Using TDOA to calculate the distances */
  float32_t d_01 = SOUND_SPEED * TDOA_port_starboard;
  float32_t d_02 = SOUND_SPEED * TDOA_port_stern;

  /* Setting A */
  A << x_01, y_01, d_01, x_02, y_02, d_02;

  /**
   * @brief Calculating the values of the B-vector
   *
   * The variables refer to the position (1-indexed) in the vector.
   *
   * Check the link in the .h file for a better explanation
   */
  float32_t b1 = 1 / 2 *
                 (std::pow(d_01, 2) + std::pow(PORT_HYD_X, 2) -
                  std::pow(STARBOARD_HYD_X, 2) + std::pow(PORT_HYD_Y, 2) -
                  std::pow(STARBOARD_HYD_Y, 2));

  float32_t b2 =
      1 / 2 *
      (std::pow(d_02, 2) + std::pow(PORT_HYD_X, 2) - std::pow(STERN_HYD_X, 2) +
       std::pow(PORT_HYD_Y, 2) - std::pow(STERN_HYD_Y, 2));

  /* Setting B */
  B << b1, b2;
}