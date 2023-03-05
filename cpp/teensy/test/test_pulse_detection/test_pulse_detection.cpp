#include "test_pulse_determination.h"

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(pulse_determination::test_short_time_fourier_transform);
  UNITY_END();
}
