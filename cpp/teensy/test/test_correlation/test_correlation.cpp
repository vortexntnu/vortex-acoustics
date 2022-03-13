#include "unity.h"
#include "correlation.h"
#include "stdio.h"
#include "Arduino.h"

//be consistent. Either you care about differnt lengths or you dont


/***** just checking that I can use the arm_math.h lib" ********/

void test_arm_lib(){
    float_t exprected_val = -1.0; 
    float_t cos_output = arm_cos_f32(3.14); 
    TEST_ASSERT_EQUAL_FLOAT(exprected_val, cos_output); 
}

void test_correlation_with_simple_arrays(){
    float32_t result_correlation_in_py[] = {17.1, 49.74, 84.58, 113.87, 167.56, 192.7, 221.34, 251.39, 313.45, 251.39, 221.34, 192.7, 167.56, 113.87, 84.58, 49.74, 17.1, 0., 0., 0., 0.}; 

    float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0}; 
    float32_t arr2[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7};  

    int input_size = 11; 
    int output_size = 2*input_size-1; 

    float32_t result_from_teensy[output_size]; 
    arm_correlate_f32(arr1, input_size, arr2, input_size, result_from_teensy); 

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(result_correlation_in_py, result_from_teensy, output_size); 
 } 

void test_find_lag(){
    float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0}; 
    float32_t arr2[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7}; 
    uint32_t length_arr1 = sizeof(arr1)/sizeof(arr1[0]); 
    uint32_t length_arr2 = sizeof(arr2)/sizeof(arr2[0]);
    int offset = 2; 

    int computed_lag = find_lag(arr1, length_arr1, arr2, length_arr2); 
    TEST_ASSERT_EQUAL_INT32((length_arr1-1-offset), computed_lag);

}

void test_compute_tdoa_array(){
    float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0, 0.0, 0.0};
    float32_t arr2[] = {0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0, 0.0};
    float32_t arr3[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0};
    float32_t arr4[] = {0.0, 0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0};

    uint32_t number_of_signals = 4; 
    uint32_t signal_length = sizeof(arr1)/sizeof(arr1[0]); 

    float32_t* signals[]= {arr1, arr2, arr3, arr4}; 

    int32_t tdoa_array[number_of_signals-1]; 
    compute_tdoa_array(signals, number_of_signals, signal_length, tdoa_array); 

    int32_t expected_tdoa_vaules[] = {(signal_length-1-1),(signal_length-1-2),(signal_length-1-3)}; 

    TEST_ASSERT_EQUAL_INT32_ARRAY(expected_tdoa_vaules, tdoa_array, 3); 

}

int main(int argc, char **argv) {
    UNITY_BEGIN(); 
    RUN_TEST(test_arm_lib); 
    RUN_TEST(test_correlation_with_simple_arrays); 
    RUN_TEST(test_find_lag); 
    RUN_TEST(test_compute_tdoa_array); 
    UNITY_END(); 
}

