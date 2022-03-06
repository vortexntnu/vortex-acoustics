#include "unity.h"
#include "correlation.h"

void test_cosinus(){
    float_t exprected_val = 0.0; 
    float_t cos_output = cosinus(3.14); 
    TEST_ASSERT_EQUAL_FLOAT(exprected_val, cos_output); 
}

int main(int argc, char **argv) {

    UNITY_BEGIN(); 
    RUN_TEST(test_cosinus); 
    UNITY_END(); 
}