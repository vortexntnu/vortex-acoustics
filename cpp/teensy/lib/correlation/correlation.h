#include "arm_math.h"
#include <unity.h>

int32_t find_lag(
    float32_t sig1[],  
    float32_t sig2[], 
    uint32_t signal_length
    ); 


void compute_tdoa_array(
    float32_t** signals, 
    uint32_t number_of_signals, 
    uint32_t signal_length, 
    int32_t* p_tdoa_array); 