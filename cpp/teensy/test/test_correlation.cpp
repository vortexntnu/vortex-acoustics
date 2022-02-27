//#include "Arduino.h"
#include "unity.h"
#include "correlation.h"



int main(int argc, char **argv) {

    UNITY_BEGIN(); 
    TEST_ASSERT_EQUAL_INT(3, nimportequoi(3)); 
    UNITY_END(); 
}