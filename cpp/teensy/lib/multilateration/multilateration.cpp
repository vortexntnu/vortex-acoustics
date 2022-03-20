#include "multilateration.h"

/*
float32_t calculatePingerPosition(int32_t tdoaArray[]){
        //compute A
        //compute B
        //MSE
    }*/

//sjould it be a class? 

float32_t** init_A_matrix(){
    float32_t** A = new float32_t*[NUM_HYDROPHONES]; 

    for (int i = 0; i<NUM_HYDROPHONES-1; i++){
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

void compute_A(int32_t tdoaArray[], float32_t** A){
    for (int i = 0; i< NUM_HYDROPHONES-1; i++){
        A[i][3] = tdoaArray[i]*SOUND_SPEED/SAMPLING_FREQ;
    }
}

float32_t* init_B_matrix(){
    float32_t* B = new float32_t[NUM_HYDROPHONES]; 
    for (int i= 0; i< NUM_HYDROPHONES-1; i++){
        B[i] = 0; 
    }
    return B; 
}

void compute_B(int32_t tdoaArray[], float32_t* B){
    B[0] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_1_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_1_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_1_Z_POS, 2)
    +pow(tdoaArray[0]*SOUND_SPEED/SAMPLING_FREQ, 2)); 

    B[1] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_2_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_2_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_2_Z_POS, 2)
    +pow(tdoaArray[1]*SOUND_SPEED/SAMPLING_FREQ, 2));

    B[2] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_3_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_3_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_3_Z_POS, 2)
    +pow(tdoaArray[2]*SOUND_SPEED/SAMPLING_FREQ, 2));

    B[3] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_4_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_4_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_4_Z_POS, 2)
    +pow(tdoaArray[3]*SOUND_SPEED/SAMPLING_FREQ, 2));
}

