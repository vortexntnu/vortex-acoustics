#include "Arduino.h"
#include "correlation.h"
#include "stdio.h"
#include "unity.h"

void test_arm_lib() {
  float_t expectedVal = -1.0;
  float_t cosOutput = arm_cos_f32(3.14);
  TEST_ASSERT_EQUAL_FLOAT(expectedVal, cosOutput);
}

void test_correlation_with_simple_arrays() {
  float32_t result_from_correlation_in_python[] = {
      17.1,   49.74,  84.58,  113.87, 167.56, 192.7,  221.34,
      251.39, 313.45, 251.39, 221.34, 192.7,  167.56, 113.87,
      84.58,  49.74,  17.1,   0.,     0.,     0.,     0.};

  float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0};
  float32_t arr2[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7};

  int inputSize = 11;
  int outputSize = 2 * inputSize - 1;

  float32_t result_from_correlation_on_teensy[outputSize];
  arm_correlate_f32(arr1, inputSize, arr2, inputSize,
                    result_from_correlation_on_teensy);

  TEST_ASSERT_EQUAL_FLOAT_ARRAY(result_from_correlation_in_python,
                                result_from_correlation_on_teensy, outputSize);
}

void test_find_lag() {
  float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0};
  float32_t arr2[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7};
  uint32_t signalLength = sizeof(arr1) / sizeof(arr1[0]);
  int offset = 2;

  int computedLag = findLag(arr1, arr2, signalLength);
  TEST_ASSERT_EQUAL_INT32((signalLength - 1 - offset), computedLag);
}

void test_compute_tdoa_array() {
  float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7,
                      6.7, 5.7, 0.0, 0.0, 0.0, 0.0};
  float32_t arr2[] = {0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0,
                      6.7, 6.7, 5.7, 0.0, 0.0, 0.0};
  float32_t arr3[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9,
                      2.0, 6.7, 6.7, 5.7, 0.0, 0.0};
  float32_t arr4[] = {0.0, 0.0, 0.0, 3.0, 5.2, 5.2, 6.7,
                      8.9, 2.0, 6.7, 6.7, 5.7, 0.0};

  uint32_t numberOfSignals = 4;
  uint32_t signalLength = sizeof(arr1) / sizeof(arr1[0]);

  float32_t *signals[] = {arr1, arr2, arr3, arr4};

  int32_t tdoaArray[numberOfSignals - 1];
  computeTdoaArray(signals, numberOfSignals, signalLength, tdoaArray);

  int32_t expectedTdoaVaules[] = {
      (signalLength - 1 - 1), (signalLength - 1 - 2), (signalLength - 1 - 3)};

  TEST_ASSERT_EQUAL_INT32_ARRAY(expectedTdoaVaules, tdoaArray, 3);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_arm_lib);
  RUN_TEST(test_correlation_with_simple_arrays);
  RUN_TEST(test_find_lag);
  RUN_TEST(test_compute_tdoa_array);
  UNITY_END();
}
