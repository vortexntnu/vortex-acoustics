#include "algorithm"
#include "arm_math.h"
#include "stdio.h"

#include "algorithm"
#include "../lib/multilateration/multilateration.h"

void print_matrix(float32_t* A, uint16_t n, uint16_t m){
    Serial.println(" ");
    for (int i = 0; i < n-1; i++){
        for (int j = 0; j < m; j++){
            Serial.printf("%f, ",*(A + i*m + j) ); 
        }
        Serial.println(" "); 
    } 
    Serial.println(" ");
}



int main(void) {
    Serial.begin(9600); 
    while(!Serial){}
    Serial.println("Serial connected"); 

    int32_t tdoaArray[NUM_HYDROPHONES-1] = {610, -67, 549, 290}; 

    HydrophonePositions* hydrophonePositions = new HydrophonePositions[NUM_HYDROPHONES]; 
    initHydrophonePositions(hydrophonePositions); 

    arm_matrix_instance_f32 A = {NUM_HYDROPHONES-1, NUM_DIMENTIONS+1, new float32_t [(NUM_HYDROPHONES-1) * (NUM_DIMENTIONS+1)]}; 
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES-1]}; 
    arm_matrix_instance_f32 Result = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES-1]}; 

    initialComputationA(A.pData, hydrophonePositions); 
    print_matrix(A.pData, NUM_HYDROPHONES, NUM_DIMENTIONS+1);  
    compute_A(tdoaArray, A.pData);
    Serial.println("A computed.");  
    compute_B(tdoaArray, hydrophonePositions, B.pData); 
    Serial.println("B computed."); 

    print_matrix(A.pData, NUM_HYDROPHONES, NUM_DIMENTIONS+1); 

    calculatePingerPosition(tdoaArray, hydrophonePositions, &A, &B, &Result);

    Serial.println("Pinger pos calculated."); 

    delete[] A.pData; 
    delete[] B.pData; 
    delete[] Result.pData;
    delete[] hydrophonePositions; 

    Serial.println("Mem deallocated");  
    
    while(true){
        delay(1000); 
    }
}
