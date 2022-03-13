#include "unity.h"
#include "unity_testframework.h"



int main(int argc, char **argv) {

    UNITY_BEGIN(); 
    RUN_TEST(test_summation); 
    RUN_TEST(test_return_true); 
    UNITY_END(); 
}


