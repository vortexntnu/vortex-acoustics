#include "correlation.h"

int find_lag(float32_t sig1[], float32_t sig2[], uint32_t signal_length){
    uint32_t output_size = 2*signal_length-1; //assuming signal always will have the same length

    float32_t result_from_teensy[output_size]; 
    arm_correlate_f32(sig1, signal_length, sig2, signal_length, result_from_teensy); 

    float32_t max_val; 
    uint32_t max_val_index; 
    arm_max_f32(result_from_teensy, output_size, &max_val, &max_val_index); 
    
    return max_val_index; 
}

void compute_tdoa_array(
    float32_t** signals, 
    uint32_t number_of_signals, 
    uint32_t signal_length,
    int32_t* p_tdoa_array
    ){
  
    for (uint32_t i = 0; i < number_of_signals-1; i++){ 
        p_tdoa_array[i] = find_lag(signals[0], signals[i+1], signal_length);
    }
}