#include "unity.h"
#include "multilateration.h"
#include "stdio.h"
#include "Arduino.h"

void test_calculate_pinger_position(){
    int32_t tdoaAray[NUM_HYDROPHONES-1] = {610,-67,549,290}; 

    HydrophonePositions* hydrophonePositions = new HydrophonePositions[NUM_HYDROPHONES]; 
    initHydrophonePositions(hydrophonePositions); 

    arm_matrix_instance_f32 A = {NUM_HYDROPHONES-1, NUM_DIMENTIONS+1, new float32_t [NUM_HYDROPHONES * (NUM_DIMENTIONS+1)]}; 
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES]}; 
    arm_matrix_instance_f32 Result = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES]}; 

    initialComputationA(A.pData, hydrophonePositions); 
    arm_status status = calculatePingerPosition(tdoaAray, hydrophonePositions, &A, &B, &Result);
    TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS); 


    float32_t expectedResult[NUM_DIMENTIONS] = {3.909, 2.123, -0.607};
    float32_t acctualResult[NUM_DIMENTIONS]; 
    for (int i = 0; i < NUM_DIMENTIONS; i++){
        acctualResult[i] = *(Result.pData+i); 
    } 

    for (int i = 0; i < NUM_DIMENTIONS; i++) {
        TEST_ASSERT_FLOAT_WITHIN(0.01, expectedResult[i], acctualResult[i]); 
    } 

    delete[] A.pData; 
    delete[] B.pData; 
    delete[] Result.pData;
    delete[] hydrophonePositions; 

}

void test_with_small_values_for_tdoa(){
    float32_t toleranse = 0.01; 
    int32_t tdoaAray[NUM_HYDROPHONES-1] = {1,0,2,0}; 

    HydrophonePositions* hydrophonePositions = new HydrophonePositions[NUM_HYDROPHONES]; 
    initHydrophonePositions(hydrophonePositions); 

    arm_matrix_instance_f32 A = {NUM_HYDROPHONES-1, NUM_DIMENTIONS+1, new float32_t [NUM_HYDROPHONES * (NUM_DIMENTIONS+1)]}; 
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES]}; 
    arm_matrix_instance_f32 Result = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES]}; 

    initialComputationA(A.pData, hydrophonePositions); 
    arm_status status = calculatePingerPosition(tdoaAray, hydrophonePositions, &A, &B, &Result);
    TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS); 

    float32_t expectedResult[NUM_DIMENTIONS] = {0.587, 0.113, 0.326};
    float32_t acctualResult[NUM_DIMENTIONS]; 
    for (int i = 0; i < NUM_DIMENTIONS; i++){
        acctualResult[i] = *(Result.pData+i); 
    } 

    for (int i = 0; i < NUM_DIMENTIONS; i++) {
        TEST_ASSERT_FLOAT_WITHIN(toleranse, expectedResult[i], acctualResult[i]); 
    } 

    delete[] A.pData; 
    delete[] B.pData; 
    delete[] Result.pData;
    delete[] hydrophonePositions; 

}

int main(int argc, char **argv) {
    UNITY_BEGIN(); 
    RUN_TEST(test_calculate_pinger_position); 
    RUN_TEST(test_with_small_values_for_tdoa); 
    UNITY_END(); 
}