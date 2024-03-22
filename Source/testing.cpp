#include "testing.h"

#if CURR_TESTING_BOOL

/**
 * Test-functions for acoustic trilateration
 */
void TESTING::calculate_toa_array(uint32_t lag_array[NUM_HYDROPHONES],
                                  uint8_t &bool_valid_parameters) {

  /**
   * Fail-checks
   */
  if (!CURR_TESTING_BOOL || NUM_HYDROPHONES < 3) {
    bool_valid_parameters = 0;
    return;
  }

  /**
   * Calculating the distance between the sound-source and the hydrophones
   */
  float32_t dist_src_port = std::sqrt(std::pow(SOURCE_POS_X - PORT_HYD_X, 2) +
                                      std::pow(SOURCE_POS_Y - PORT_HYD_Y, 2) +
                                      std::pow(SOURCE_POS_Z - PORT_HYD_Z, 2));
  float32_t dist_src_starboard =
      std::sqrt(std::pow(SOURCE_POS_X - STARBOARD_HYD_X, 2) +
                std::pow(SOURCE_POS_Y - STARBOARD_HYD_Y, 2) +
                std::pow(SOURCE_POS_Z - STARBOARD_HYD_Z, 2));
  float32_t dist_src_stern = std::sqrt(std::pow(SOURCE_POS_X - STERN_HYD_X, 2) +
                                       std::pow(SOURCE_POS_Y - STERN_HYD_Y, 2) +
                                       std::pow(SOURCE_POS_Z - STERN_HYD_Z, 2));

  /**
   * Calculating the time the sound will arrive at x hyd
   *
   * Since the TOA uses lag (direct measuremend), these values are uint32_t
   */
  uint32_t lag_port =
      (uint32_t)(SAMPLE_FREQUENCY * (dist_src_port / SOUND_SPEED));
  uint32_t lag_starboard =
      (uint32_t)(SAMPLE_FREQUENCY * (dist_src_starboard / SOUND_SPEED));
  uint32_t lag_stern =
      (uint32_t)(SAMPLE_FREQUENCY * (dist_src_stern / SOUND_SPEED));

  /**
   * Setting the values into the array
   */
  lag_array[0] = lag_port;
  lag_array[1] = lag_starboard;
  lag_array[2] = lag_stern;

  bool_valid_parameters = 1;
}

void TESTING::test_trilateration_algorithm() {

  /**
   * Creating the TOA-intervals.
   * Writing out an error-msg if the parameters are wrong
   */
  uint32_t lag_array[NUM_HYDROPHONES];
  uint8_t bool_valid_parameters = 1;

  TESTING::calculate_toa_array(lag_array, bool_valid_parameters);
  if (bool_valid_parameters) {
    printf("\nAt least one parameter in parameter.h is invalid");
    return;
  }

  /**
   * Initializing the system matrices
   */
  Matrix_2_3_f A_matrix = TRILATERATION::initialize_A_matrix();
  Vector_2_1_f B_vector = TRILATERATION::initialize_B_vector();

  /**
   * Initializing the estimate for x-pos and y-pos
   */
  float32_t x_pos_es, y_pos_es;

  if (!TRILATERATION::trilaterate_pinger_position(A_matrix, B_vector, lag_array,
                                                  x_pos_es, y_pos_es)) {
    printf("\nA-matrix is not invertible")
  }

  /**
   * Calculating the distance between the estimated position and the
   * actual position. Only considering x and y, since we only have
   * three hydrophones
   */
  float32_t distance_diff = std::sqrt(std::pow(x_pos_es - SOURCE_POS_X, 2) +
                                      std::pow(y_pos_es - SOURCE_POS_Y, 2));

  printf("\nThe estimated pinger position is at (x,y) = (%f, %f)", x_pos_ex,
         y_pos_es);
  printf("\nThe actual pinger position is at (x,y) = (%f, %f)", SOURCE_POS_X,
         SOURCE_POS_Y);
  printf("\nThe difference between the actual position and the estimated "
         "position is %f m",
         distance_diff);
}

#endif /* CURR_TESTING_BOOL */