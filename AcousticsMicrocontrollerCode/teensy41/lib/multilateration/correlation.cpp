#include "correlation.h"



size_t findLag(q15_t* sig1, q15_t* sig2, uint32_t signalLength) {

    q15_t resultFromTeensy[2 * signalLength  -1];
    arm_correlate_q15(sig1, signalLength, sig2, signalLength, resultFromTeensy);

    q15_t maxVal;
    uint32_t maxValIndex;
    arm_max_q15(resultFromTeensy, outputSize, &maxVal, &maxValIndex);

    return maxValIndex;
}



void computeTdoaArray(float32_t** signals, uint32_t numberOfSignals, uint32_t signalLength, int32_t* p_tdoaArray) {

    for (uint32_t i = 0; i < numberOfSignals - 1; i++) {
        p_tdoaArray[i] = findLag(signals[0], signals[i + 1], signalLength);
    }
}
