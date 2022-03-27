#include <Arduino.h>
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

    int32_t tdoaArray[NUM_HYDROPHONES-1] = {2,3,4,5}; 

    HydrophonePositions* hydrophonePositions = new HydrophonePositions[NUM_HYDROPHONES]; 
    initHydrophonePositions(hydrophonePositions); 

    arm_matrix_instance_f32 A = {NUM_HYDROPHONES-1, NUM_DIMENTIONS+1, new float32_t [(NUM_HYDROPHONES-1) * (NUM_DIMENTIONS+1)]}; 
    arm_matrix_instance_f32 B = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES-1]}; 
    arm_matrix_instance_f32 Result = {NUM_HYDROPHONES-1, 1, new float32_t[NUM_HYDROPHONES-1]}; 

    initialComputationA(A.pData, hydrophonePositions);  
    compute_A(tdoaArray, A.pData);
    Serial.println("A computed.");  
    compute_B(tdoaArray, B.pData); 
    Serial.println("B computed."); 

    Serial.println("A matrix:"); 
    print_matrix(A.pData, NUM_HYDROPHONES, NUM_DIMENTIONS+1);
    //husk å endre i cpp også!!

    arm_matrix_instance_f32 Atrans; 
    Atrans.numCols = NUM_HYDROPHONES-1; 
    Atrans.numRows = NUM_DIMENTIONS+1; 
    Atrans.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_DIMENTIONS+1)];
    arm_status status = arm_mat_trans_f32(&A, &Atrans); 
    Serial.printf("Status from transpose: %d \n", status); //assert(status == ARM_MATH_SUCCESS); - 0

    arm_matrix_instance_f32 AtransXA; 
    AtransXA.numCols = NUM_HYDROPHONES-1; 
    AtransXA.numRows = NUM_HYDROPHONES-1; 
    AtransXA.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_HYDROPHONES-1)];
    const arm_matrix_instance_f32* pAtrans = &Atrans;
    status = arm_mat_mult_f32(pAtrans, &A, &AtransXA);
    Serial.printf("Status from multiplication: %d \n", status);//assert(status == ARM_MATH_SUCCESS); - 0

    Serial.println("Atrans times A matrix:");
    print_matrix(AtransXA.pData, NUM_HYDROPHONES-1, NUM_HYDROPHONES-1); 

    arm_matrix_instance_f32 AtransXAinv; 
    AtransXAinv.numCols = NUM_HYDROPHONES-1; 
    AtransXAinv.numRows = NUM_HYDROPHONES-1; 
    AtransXAinv.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_HYDROPHONES-1)]; 
    const arm_matrix_instance_f32* pAtransXA = &AtransXA; 
    status = arm_mat_inverse_f32(pAtransXA, &AtransXAinv);
    Serial.printf("Status from inverse: %d \n", status); //assert(status== ARM_MATH_SUCCESS); - 0

    arm_matrix_instance_f32 AtransXAinvXAtrans; 
    AtransXAinvXAtrans.numCols = NUM_HYDROPHONES-1; 
    AtransXAinvXAtrans.numRows = NUM_HYDROPHONES-1; 
    AtransXAinvXAtrans.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_HYDROPHONES-1)]; 
    const arm_matrix_instance_f32* pAtransAinv = &AtransXAinv;
    status = arm_mat_mult_f32(pAtransAinv, pAtrans, &AtransXAinvXAtrans);
    Serial.printf("Status from multiplication: %d \n", status);//assert(status== ARM_MATH_SUCCESS); - 0

    const arm_matrix_instance_f32* pAtransXAinvXAtrans = &AtransXAinvXAtrans;  
    status = arm_mat_mult_f32(pAtransXAinvXAtrans, &B, &Result); 
    Serial.printf("Status from multiplication with B: %d \n", status); //assert(status == ARM_MATH_SUCCESS); - 0

    delete[] Atrans.pData;
    delete[] AtransXA.pData;
    delete[] AtransXAinv.pData;
    delete[] AtransXAinvXAtrans.pData;

    calculatePingerPosition(tdoaArray, &A, &B, &Result);

    Serial.println("Pinger pos calculated."); 

    delete[] A.pData; 
    delete[] B.pData; 
    delete[] Result.pData;
    delete[] hydrophonePositions; 

    Serial.println("Mem deallocated");  
    
    while(true){
        Serial.println("Loopoing"); 
        delay(1000); 
    }
}
