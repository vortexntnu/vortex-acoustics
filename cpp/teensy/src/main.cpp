#include "algorithm"
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

    int32_t tdoaAray[NUM_HYDROPHONES-1] = {2,3,4,5}; 

    HydrophonePositions* hydrophonePositions = new HydrophonePositions[NUM_HYDROPHONES]; 
    initHydrophonePositions(hydrophonePositions); 

    arm_matrix_instance_f32 A = {NUM_HYDROPHONES, NUM_DIMENTIONS+1, new float32_t [NUM_HYDROPHONES * (NUM_DIMENTIONS+1)]}; 
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES, 1, new float32_t[NUM_HYDROPHONES]}; 
    arm_matrix_instance_f32 Result = {NUM_HYDROPHONES, 1, new float32_t[NUM_HYDROPHONES]}; 

    initialComputationA(A.pData, hydrophonePositions); 
    calculatePingerPosition(tdoaAray, &A, &B, &Result);

    delete[] A.pData; 
    delete[] B.pData; 
    delete[] Result.pData;
    delete[] hydrophonePositions;  
    
    while(true){}
}
