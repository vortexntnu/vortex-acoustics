#include <Arduino.h>
#include "arm_math.h"
#include "stdio.h"
#include "algorithm"
#include "../lib/multilateration/multilateration.h"

void print_matrix(float32_t* A, uint16_t n, uint16_t m){
    Serial.println(" ");
    for (int i = 0; i < n; i++){
        for (int j = 0; j < m; j++){
            Serial.printf("%f, ",*(A + i*m + j) ); 
        }
        Serial.println(" "); 
    } 
    Serial.println(" ");
}

void print_A_T(float32_t* A){
    Serial.println(" ");
    for (int i = 0; i < NUM_DIMENTIONS; i++){
        for (int j = 0; j < NUM_HYDROPHONES-1; j++){
            Serial.printf("%f, ",*(A + i*4 + j) ); 
        }
        Serial.println(" "); 
    } 
    Serial.println(" ");
}

void print_B(float32_t* B){
    for (int i = 0; i < NUM_HYDROPHONES-1; i++){
        Serial.printf("%f, ", B[i]); 
    }
    Serial.println(" ");
}

int main(void) {
    Serial.begin(9600); 
    while(!Serial){}
    Serial.println("Serial connected"); 

    int32_t tdoa_array[NUM_HYDROPHONES] = {3,4,5,6};
    
    arm_matrix_instance_f32 A = init_A_matrix();
    const arm_matrix_instance_f32* pA = &A; 
    arm_matrix_instance_f32 B = init_B_matrix();
    const arm_matrix_instance_f32* pB = &B; 
    arm_matrix_instance_f32 Result = {NUM_HYDROPHONES, 1, new float32_t[NUM_HYDROPHONES]}; 

    test_func(tdoa_array, pA, pB); 

    calculatePingerPosition(tdoa_array, pA, pB, &Result); 

    Serial.println("Printing matrices");
    print_matrix(A.pData, NUM_HYDROPHONES, NUM_DIMENTIONS+1);  

    delete[] A.pData; 
    delete[] B.pData; 
    
    while(true){}
}
