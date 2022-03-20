#include "multilateration.h"

/*
TODO: 
- divide calaculate pinger position func in to three
- allocate only in setup func
- make structs outof hydrophones and celan up init functions thereafter
- give value to strcuts in calculate func befroe using them
- look at naming of vab and func
- compare results to those from python
- should I organixe things into classes? 

*/



void calculatePingerPosition(
    int32_t tdoaArray[], 
    const arm_matrix_instance_f32* pA, 
    const arm_matrix_instance_f32* pB, 
    arm_matrix_instance_f32* pResult
    ){
        compute_A(tdoaArray, pA->pData); 
        compute_B(tdoaArray, pB->pData); 

        arm_matrix_instance_f32 Atrans; 
        Atrans.numCols = NUM_DIMENTIONS+1; 
        Atrans.numRows = NUM_HYDROPHONES; 
        Atrans.pData = new float32_t[NUM_HYDROPHONES*(NUM_DIMENTIONS+1)];
        arm_status status = arm_mat_trans_f32(pA, &Atrans); 
        assert(status == ARM_MATH_SUCCESS); 
 
        arm_matrix_instance_f32 AtransXA; 
        AtransXA.numCols = NUM_HYDROPHONES; 
        AtransXA.numRows = NUM_HYDROPHONES; 
        AtransXA.pData = new float32_t[NUM_HYDROPHONES*NUM_HYDROPHONES];
        const arm_matrix_instance_f32* pAtrans = &Atrans;
        status = arm_mat_mult_f32(pAtrans, pA, &AtransXA);
        assert(status == ARM_MATH_SUCCESS); 

        arm_matrix_instance_f32 AtransXAinv; 
        AtransXAinv.numCols = NUM_HYDROPHONES; 
        AtransXAinv.numRows = NUM_HYDROPHONES; 
        AtransXAinv.pData = new float32_t[NUM_HYDROPHONES*NUM_HYDROPHONES]; 
        const arm_matrix_instance_f32* pAtransXA = &AtransXA; 
        status = arm_mat_inverse_f32(pAtransXA, &AtransXAinv);
        assert(status== ARM_MATH_SUCCESS);  
 
        arm_matrix_instance_f32 AtransXAinvXAtrans; 
        AtransXAinvXAtrans.numCols = NUM_HYDROPHONES; 
        AtransXAinvXAtrans.numRows = NUM_HYDROPHONES; 
        AtransXAinvXAtrans.pData = new float32_t[NUM_HYDROPHONES*NUM_HYDROPHONES]; 
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



arm_matrix_instance_f32 init_A_matrix(){
    float32_t* A_data = new float32_t[NUM_HYDROPHONES * 4]; 
    *(A_data + 0*4 + 0)= HYD_0_X_POS-HYD_1_X_POS; 
    *(A_data + 0*4 + 1)= HYD_0_Y_POS-HYD_1_Y_POS; 
    *(A_data + 0*4 + 2)= HYD_0_Z_POS-HYD_1_Z_POS; 
    *(A_data + 0*4 + 3)= 0.0; 

    *(A_data + 1*4 + 0) = HYD_0_X_POS-HYD_2_X_POS; 
    *(A_data + 1*4 + 1) = HYD_0_Y_POS-HYD_2_Y_POS; 
    *(A_data + 1*4 + 2) = HYD_0_Z_POS-HYD_2_Z_POS; 
    *(A_data + 1*4 + 3) = 0.0; 

    *(A_data + 2*4 + 0) = HYD_0_X_POS-HYD_3_X_POS; 
    *(A_data + 2*4 + 1) = HYD_0_Y_POS-HYD_3_Y_POS; 
    *(A_data + 2*4 + 2) = HYD_0_Z_POS-HYD_3_Z_POS; 
    *(A_data + 2*4 + 3) = 0.0; 

    *(A_data + 3*4 + 0) = HYD_0_X_POS-HYD_4_X_POS; 
    *(A_data + 3*4 + 1) = HYD_0_Y_POS-HYD_4_Y_POS; 
    *(A_data + 3*4 + 2)= HYD_0_Z_POS-HYD_4_Z_POS; 
    *(A_data + 3*4 + 3) = 0.0; 

    arm_matrix_instance_f32 A = {NUM_HYDROPHONES, NUM_DIMENTIONS+1, A_data}; 

    return A;
}

void compute_A(int32_t tdoaArray[], float32_t* A_data){
    for (int i = 0; i< NUM_HYDROPHONES-1; i++){
        *(A_data +i*4 + 3) = tdoaArray[i]*SOUND_SPEED/SAMPLING_FREQ;
    } 
}

arm_matrix_instance_f32 init_B_matrix(){
    float32_t* B_data = new float32_t[NUM_HYDROPHONES]; 
    for (int i= 0; i< NUM_HYDROPHONES-1; i++){
        B_data[i] = 0; 
    }
    arm_matrix_instance_f32 B; 
    arm_mat_init_f32(&B, NUM_HYDROPHONES, 1, B_data); 
    return B; 
}

void compute_B(int32_t tdoaArray[], float32_t* B_data){
    B_data[0] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_1_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_1_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_1_Z_POS, 2)
    +pow(tdoaArray[0]*SOUND_SPEED/SAMPLING_FREQ, 2)); 

    B_data[1] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_2_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_2_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_2_Z_POS, 2)
    +pow(tdoaArray[1]*SOUND_SPEED/SAMPLING_FREQ, 2));

    B_data[2] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_3_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_3_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_3_Z_POS, 2)
    +pow(tdoaArray[2]*SOUND_SPEED/SAMPLING_FREQ, 2));

    B_data[3] = 0.5*(
    pow(HYD_0_X_POS, 2)-pow(HYD_4_X_POS,2)
    +pow(HYD_0_Y_POS, 2)-pow(HYD_4_Y_POS, 2)
    +pow(HYD_0_Z_POS, 2)-pow(HYD_4_Z_POS, 2)
    +pow(tdoaArray[3]*SOUND_SPEED/SAMPLING_FREQ, 2));
}


arm_matrix_instance_f32 test_func(
    int32_t tdoaArray[], 
    const arm_matrix_instance_f32* pA, 
    const arm_matrix_instance_f32* pB
    ){
    compute_A(tdoaArray, (*pA).pData); 
    compute_B(tdoaArray, (*pB).pData); 


    arm_matrix_instance_f32 A_T; 
    A_T.pData = new float32_t[NUM_HYDROPHONES*(NUM_DIMENTIONS+1)];
    A_T.numCols = NUM_HYDROPHONES;
    A_T.numRows = NUM_DIMENTIONS+1;
    arm_status status = arm_mat_trans_f32(pA, &A_T);
    assert (status == ARM_MATH_SUCCESS); 

    return A_T;  
}

