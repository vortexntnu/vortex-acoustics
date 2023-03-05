#include "unity_testframework.h"

// remember to compile CMSIS lib with -Ofast

int summation(int a) {
  int n = 0;
  for (int i = 0; i < a; i++) {
    n += 1;
  }
  return n;
}

bool return_true() { return true; }

void test_summation() {
  TEST_ASSERT_EQUAL_INT(7, summation(7));
  TEST_ASSERT_TRUE(7 == summation(7));
}

void test_return_true() {
  TEST_ASSERT_TRUE(return_true());
  // TEST_ASSERT_FALSE(return_true);
}
