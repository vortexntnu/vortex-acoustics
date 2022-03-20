#include "algorithm"
#include "arm_math.h"
#include "stdio.h"

#include "algorithm"
#include "../lib/multilateration/multilateration.h"

void print_A(float32_t** A){
    Serial.println(" ");
    for (int i = 0; i < NUM_HYDROPHONES-1; i++){
        for (int j = 0; j < 4; j++){
            Serial.printf("%f, ", A[i][j]); 
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

    float32_t** A = init_A_matrix(); 
    float32_t* B = init_B_matrix(); 
    print_A(A);
    print_B(B); 

    compute_A(tdoa_array, A); 
    compute_B(tdoa_array, B); 

    print_A(A); 
    print_B(B); 

    for (int i = 0; i<NUM_HYDROPHONES-1; i++){
        delete[] A[i]; }
    delete[] A; 
    delete[] B; 
    
    while(true){}
}
