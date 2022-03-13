#include "correlation.h"

float32_t cosinus(float32_t argument){
    return arm_cos_f32(argument); 
}

int find_lag(float32_t sig1[], uint32_t length_sig1, float32_t sig2[], uint32_t length_sig2){
    uint32_t output_size; 
    if (length_sig1 >= length_sig2){
        output_size = 2*length_sig1-1;
    }
    else{
        output_size = 2*length_sig2-1; 
    }

    float32_t result_from_teensy[output_size]; 
    arm_correlate_f32(sig1, length_sig1, sig2, length_sig2, result_from_teensy); 

    float32_t max_val; 
    uint32_t max_val_index; 
    arm_max_f32(result_from_teensy, output_size, &max_val, &max_val_index); 
    
    return max_val_index; 
}