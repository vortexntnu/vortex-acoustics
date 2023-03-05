#include "correlation.h"

int32_t findLag(float32_t sig1[], float32_t sig2[], uint32_t signalLength) {
  uint32_t outputSize =
      2 * signalLength - 1; // assuming signal always will have the same length

  float32_t resultFromTeensy[outputSize];
  arm_correlate_f32(sig1, signalLength, sig2, signalLength, resultFromTeensy);

  float32_t maxVal;
  uint32_t maxValIndex;
  arm_max_f32(resultFromTeensy, outputSize, &maxVal, &maxValIndex);

  return maxValIndex;
}

void computeTdoaArray(float32_t **signals, uint32_t numberOfSignals,
                      uint32_t signalLength, int32_t *p_tdoaArray) {

  for (uint32_t i = 0; i < numberOfSignals - 1; i++) {
    p_tdoaArray[i] = findLag(signals[0], signals[i + 1], signalLength);
  }
}