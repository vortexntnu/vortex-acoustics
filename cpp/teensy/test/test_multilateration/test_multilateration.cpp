#include "Arduino.h"
#include "multilateration.h"
#include "unity.h"

void test_calculate_pinger_position() {
    float32_t toleranse = 0.01;
    int32_t tdoaAray[NUM_HYDROPHONES - 1] = {610, -67, 549, 290};

    arm_matrix_instance_f32 A = {
        NUM_HYDROPHONES - 1, NUM_DIMENSIONS + 1,
        new float32_t[NUM_HYDROPHONES * (NUM_DIMENSIONS + 1)]};
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES - 1, 1,
                                 new float32_t[NUM_HYDROPHONES]};


    Positions* sourcePosition = new Positions;

    initialComputationA(A.pData, hydrophonePositions);
    arm_status status = calculatePingerPosition(
        tdoaAray, hydrophonePositions, &A, &B, sourcePosition);
    TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS);

    Positions expectedSourcePosition = {3.909, 2.123, -0.607};

    TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedSourcePosition.X,
                             (*sourcePosition).X);
    TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedSourcePosition.Y,
                             (*sourcePosition).Y);
    TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedSourcePosition.Z,
                             (*sourcePosition).Z);

    delete[] A.pData;
    delete[] B.pData;
}

void test_with_small_values_for_tdoa() {
    float32_t toleranse = 0.01;
    int32_t tdoaAray[NUM_HYDROPHONES - 1] = {1, 0, 2, 0};

    arm_matrix_instance_f32 A = {
        NUM_HYDROPHONES - 1, NUM_DIMENSIONS + 1,
        new float32_t[NUM_HYDROPHONES * (NUM_DIMENSIONS + 1)]};
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES - 1, 1,
                                 new float32_t[NUM_HYDROPHONES]};

    Positions* sourcePosition = new Positions;

    initialComputationA(A.pData, hydrophonePositions);
    arm_status status = calculatePingerPosition(
        tdoaAray, hydrophonePositions, &A, &B, sourcePosition);
    TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS);

    Positions expectedSourcePosition = {0.587, 0.113, 0.326};

    TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedSourcePosition.X,
                             (*sourcePosition).X);
    TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedSourcePosition.Y,
                             (*sourcePosition).Y);
    TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedSourcePosition.Z,
                             (*sourcePosition).Z);

    delete[] A.pData;
    delete[] B.pData;
}

void test_tdoa_with_zero_values() {
    int32_t tdoaAray[NUM_HYDROPHONES - 1] = {0, 0, 0, 0};

    arm_matrix_instance_f32 A = {
        NUM_HYDROPHONES - 1, NUM_DIMENSIONS + 1,
        new float32_t[NUM_HYDROPHONES * (NUM_DIMENSIONS + 1)]};
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES - 1, 1,
                                 new float32_t[NUM_HYDROPHONES]};
    Positions* sourcePosition = new Positions;

    initialComputationA(A.pData, hydrophonePositions);
    arm_status status = calculatePingerPosition(
        tdoaAray, hydrophonePositions, &A, &B, sourcePosition);
    TEST_ASSERT_FALSE(status == ARM_MATH_SUCCESS);

    delete[] A.pData;
    delete[] B.pData;
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_with_small_values_for_tdoa);
    RUN_TEST(test_tdoa_with_zero_values);
    RUN_TEST(test_calculate_pinger_position);
    UNITY_END();
}