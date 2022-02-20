#include <unity.h>
#include "correlation.h"


void test_nimportequoi(){
    TEST_ASSERT_TRUE(nimportequoi); 

}

int main(int argc, char **argv) {
    UNITY_BEGIN(); 
    RUN_TEST(test_nimportequoi); 
    UNITY_END(); 
}