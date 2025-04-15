#include "correlation.h"



size_t findLag(q15_t* sig1, q15_t* sig2, uint32_t signalLength) {

    
    q15_t resultFromTeensy[2 * signalLength  -1];
    arm_correlate_q15(sig1, signalLength, sig2, signalLength, resultFromTeensy);

    q15_t maxVal;
    uint32_t maxValIndex;
    arm_max_q15(resultFromTeensy, 2 * signalLength -1, &maxVal, &maxValIndex);

    return maxValIndex;
}



