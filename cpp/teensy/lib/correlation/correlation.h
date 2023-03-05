#include "arm_math.h"
#include <unity.h>

int32_t findLag(float32_t sig1[], float32_t sig2[], uint32_t signalLength);

void computeTdoaArray(float32_t** signals, uint32_t numberOfSignals,
                      uint32_t signalLength, int32_t* p_tdoaArray);