
#ifndef CORRELATION_H
#define CORRELATION_H






#include "arm_math.h"
#include <cstdint>



size_t findLag(q15_t* sig1, q15_t* sig2, uint32_t signalLength); 




#endif // !CORRELATION_H
