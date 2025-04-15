
#ifndef CORRELATION_H
#define CORRELATION_H






#include "arm_math.h"
#include <cstdint>



size_t findLag(q15_t* sig1, q15_t* sig2, uint32_t signalLength); 

void computeTdoaArray(float32_t** signals, uint32_t numberOfSignals, uint32_t signalLength, int32_t* p_tdoaArray);



#endif // !CORRELATION_H
