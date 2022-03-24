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

    int32_t tdoaArray[NUM_HYDROPHONES-1] = {2,3,4,5}; 

    HydrophonePositions* hydrophonePositions = new HydrophonePositions[NUM_HYDROPHONES]; 
    initHydrophonePositions(hydrophonePositions); 

    arm_matrix_instance_f32 A = {NUM_DIMENTIONS+1, NUM_HYDROPHONES, new float32_t [NUM_HYDROPHONES * (NUM_DIMENTIONS+1)]}; 
    arm_matrix_instance_f32 B = {1, NUM_HYDROPHONES,new float32_t[NUM_HYDROPHONES]}; 
    arm_matrix_instance_f32 Result = {1, NUM_HYDROPHONES, new float32_t[NUM_HYDROPHONES]}; 

    Serial.println("Done allocating"); 

    initialComputationA(A.pData, hydrophonePositions); 

    Serial.println("Initial comp done"); 

    compute_A(tdoaArray, A.pData);
    Serial.println("A computed.");  
    compute_B(tdoaArray, B.pData); 
    Serial.println("B computed."); 

    arm_matrix_instance_f32 Atrans; 
    Atrans.numCols = NUM_DIMENTIONS+1; 
    Atrans.numRows = NUM_HYDROPHONES; 
    Atrans.pData = new float32_t[NUM_HYDROPHONES*(NUM_DIMENTIONS+1)];
    arm_status status = arm_mat_trans_f32(&A, &Atrans); 
    Serial.print(status); //assert(status == ARM_MATH_SUCCESS); - 0

    arm_matrix_instance_f32 AtransXA; 
    AtransXA.numCols = NUM_HYDROPHONES; 
    AtransXA.numRows = NUM_HYDROPHONES; 
    AtransXA.pData = new float32_t[NUM_HYDROPHONES*NUM_HYDROPHONES];
    const arm_matrix_instance_f32* pAtrans = &Atrans;
    status = arm_mat_mult_f32(pAtrans, &A, &AtransXA);
    Serial.print(status); //assert(status == ARM_MATH_SUCCESS); - 0

    print_matrix(AtransXA.pData, NUM_HYDROPHONES, NUM_HYDROPHONES); 

    arm_matrix_instance_f32 AtransXAinv; 
    AtransXAinv.numCols = NUM_HYDROPHONES; 
    AtransXAinv.numRows = NUM_HYDROPHONES; 
    AtransXAinv.pData = new float32_t[NUM_HYDROPHONES*NUM_HYDROPHONES]; 
    const arm_matrix_instance_f32* pAtransXA = &AtransXA; 
    status = arm_mat_inverse_f32(pAtransXA, &AtransXAinv);
    Serial.print(status); //assert(status== ARM_MATH_SUCCESS); - 5

    arm_matrix_instance_f32 AtransXAinvXAtrans; 
    AtransXAinvXAtrans.numCols = NUM_HYDROPHONES; 
    AtransXAinvXAtrans.numRows = NUM_HYDROPHONES; 
    AtransXAinvXAtrans.pData = new float32_t[NUM_HYDROPHONES*NUM_HYDROPHONES]; 
    const arm_matrix_instance_f32* pAtransAinv = &AtransXAinv;
    status = arm_mat_mult_f32(pAtransAinv, pAtrans, &AtransXAinvXAtrans);
    Serial.print(status); //assert(status== ARM_MATH_SUCCESS);

    const arm_matrix_instance_f32* pAtransXAinvXAtrans = &AtransXAinvXAtrans;  
    status = arm_mat_mult_f32(pAtransXAinvXAtrans, &B, &Result); 
    Serial.print(status); //assert(status == ARM_MATH_SUCCESS); 

    delete[] Atrans.pData;
    delete[] AtransXA.pData;
    delete[] AtransXAinv.pData;
    delete[] AtransXAinvXAtrans.pData;

    LSE(&A, &B, &Result); 
    Serial.println("LSE good."); 
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
