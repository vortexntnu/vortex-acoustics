#include "multilateration.h"

/*
TODO: 
- compare results to those from python

*/


void calculatePingerPosition(
    int32_t tdoaArray[], 
    const arm_matrix_instance_f32* pA, 
    const arm_matrix_instance_f32* pB, 
    arm_matrix_instance_f32* pResult
    ){
        compute_A(tdoaArray, pA->pData); 
        compute_B(tdoaArray, pB->pData); 
        LSE(pA, pB, pResult); 
    }

void LSE(
    const arm_matrix_instance_f32* pA, 
    const arm_matrix_instance_f32* pB, 
    arm_matrix_instance_f32* pResult
){
    arm_matrix_instance_f32 Atrans; 
    Atrans.numCols = NUM_HYDROPHONES-1; 
    Atrans.numRows = NUM_DIMENTIONS+1; 
    Atrans.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_DIMENTIONS+1)];
    arm_status status = arm_mat_trans_f32(pA, &Atrans); 
    assert(status == ARM_MATH_SUCCESS); 

    arm_matrix_instance_f32 AtransXA; 
    AtransXA.numCols = NUM_HYDROPHONES-1; 
    AtransXA.numRows = NUM_HYDROPHONES-1; 
    AtransXA.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_HYDROPHONES-1)];
    const arm_matrix_instance_f32* pAtrans = &Atrans;
    status = arm_mat_mult_f32(pAtrans, pA, &AtransXA);
    assert(status == ARM_MATH_SUCCESS); 

    arm_matrix_instance_f32 AtransXAinv; 
    AtransXAinv.numCols = NUM_HYDROPHONES-1; 
    AtransXAinv.numRows = NUM_HYDROPHONES-1; 
    AtransXAinv.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_HYDROPHONES-1)]; 
    const arm_matrix_instance_f32* pAtransXA = &AtransXA; 
    status = arm_mat_inverse_f32(pAtransXA, &AtransXAinv);
    assert(status== ARM_MATH_SUCCESS);  

    arm_matrix_instance_f32 AtransXAinvXAtrans; 
    AtransXAinvXAtrans.numCols = NUM_HYDROPHONES-1; 
    AtransXAinvXAtrans.numRows = NUM_HYDROPHONES-1; 
    AtransXAinvXAtrans.pData = new float32_t[(NUM_HYDROPHONES-1)*(NUM_HYDROPHONES-1)]; 
    const arm_matrix_instance_f32* pAtransAinv = &AtransXAinv;
    status = arm_mat_mult_f32(pAtransAinv, pAtrans, &AtransXAinvXAtrans);
    assert(status== ARM_MATH_SUCCESS);

    const arm_matrix_instance_f32* pAtransXAinvXAtrans = &AtransXAinvXAtrans;  
    status = arm_mat_mult_f32(pAtransXAinvXAtrans, pB, pResult); 
    assert(status == ARM_MATH_SUCCESS); 

    delete[] Atrans.pData;
    delete[] AtransXA.pData;
    delete[] AtransXAinv.pData;
    delete[] AtransXAinvXAtrans.pData;
}



void initialComputationA(float32_t* AData, HydrophonePositions hydrophonePositions[]){ 
    for (int i = 0; i< (NUM_HYDROPHONES-1); i++){
        *(AData + i*(NUM_HYDROPHONES-1) + 0) = hydrophonePositions[0].pos_x - hydrophonePositions[i+1].pos_x; 
        *(AData + i*(NUM_HYDROPHONES-1) + 1) = hydrophonePositions[0].pos_y - hydrophonePositions[i+1].pos_y; 
        *(AData + i*(NUM_HYDROPHONES-1) + 2) = hydrophonePositions[0].pos_z - hydrophonePositions[i+1].pos_z; 
        *(AData + i*(NUM_HYDROPHONES-1) + 3) = 0.0; 
    }
}

void compute_A(int32_t tdoaArray[], float32_t* AData){
    for (int i = 0; i< (NUM_HYDROPHONES-1); i++){
        *(AData + i*(NUM_HYDROPHONES-1) + 3) = tdoaArray[i]*SOUND_SPEED/SAMPLING_FREQ;
    } 
}

void compute_B(int32_t tdoaArray[], float32_t* Bdata){ //fiks denne
    Bdata[0] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_1_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_1_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_1_Z_POS, 2)
    +pow(tdoaArray[0]*SOUND_SPEED/SAMPLING_FREQ, 2)); 

    Bdata[1] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_2_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_2_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_2_Z_POS, 2)
    +pow(tdoaArray[1]*SOUND_SPEED/SAMPLING_FREQ, 2));

    Bdata[2] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_3_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_3_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_3_Z_POS, 2)
    +pow(tdoaArray[2]*SOUND_SPEED/SAMPLING_FREQ, 2));

    Bdata[3] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_4_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_4_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_4_Z_POS, 2)
    +pow(tdoaArray[3]*SOUND_SPEED/SAMPLING_FREQ, 2));
}

void initHydrophonePositions(HydrophonePositions* hydrophonePositions){
    HydrophonePositions Hyd0 = {HYD_0_X_POS, HYD_0_Y_POS, HYD_0_Z_POS}; 
    HydrophonePositions Hyd1 = {HYD_1_X_POS, HYD_1_Y_POS, HYD_1_Z_POS};
    HydrophonePositions Hyd2 = {HYD_2_X_POS, HYD_2_Y_POS, HYD_2_Z_POS};
    HydrophonePositions Hyd3 = {HYD_3_X_POS, HYD_3_Y_POS, HYD_3_Z_POS};
    HydrophonePositions Hyd4 = {HYD_4_X_POS, HYD_4_Y_POS, HYD_4_Z_POS};

    hydrophonePositions[0] = Hyd0; 
    hydrophonePositions[1] = Hyd1; 
    hydrophonePositions[2] = Hyd2; 
    hydrophonePositions[3] = Hyd3; 
    hydrophonePositions[4] = Hyd4; 
       
}

