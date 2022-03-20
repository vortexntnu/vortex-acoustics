#include "multilateration.h"

/*
float32_t calculatePingerPosition(int32_t tdoaArray[]){
        //compute A
        //compute B
        //MSE
    }*/

float32_t** init_A_matrix(){
    float32_t** A = new float32_t*[NUM_HYDROPHONES]; 

    for (int i = 0; i<NUM_HYDROPHONES; i++){
        A[i] = new float32_t[4]; }

    A[0][0] = HYD_0_X_POS-HYD_1_X_POS; 
    A[0][1] = HYD_0_Y_POS-HYD_1_Y_POS; 
    A[0][2] = HYD_0_Z_POS-HYD_1_Z_POS; 
    A[0][3] = 0.0; 

    A[1][0] = HYD_0_X_POS-HYD_2_X_POS; 
    A[1][1] = HYD_0_Y_POS-HYD_2_Y_POS; 
    A[1][2] = HYD_0_Z_POS-HYD_2_Z_POS; 
    A[1][3] = 0.0; 

    A[2][0] = HYD_0_X_POS-HYD_3_X_POS; 
    A[2][1] = HYD_0_Y_POS-HYD_3_Y_POS; 
    A[2][2] = HYD_0_Z_POS-HYD_3_Z_POS; 
    A[2][3] = 0.0; 

    A[3][0] = HYD_0_X_POS-HYD_4_X_POS; 
    A[3][1] = HYD_0_Y_POS-HYD_4_Y_POS; 
    A[3][2] = HYD_0_Z_POS-HYD_4_Z_POS; 
    A[3][3] = 0.0; 

    return A; 
}



float32_t* init_B_matrix(){
    float32_t* B = new float32_t[NUM_HYDROPHONES]; 
    return B; 
}

