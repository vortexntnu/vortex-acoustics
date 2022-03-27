#include "multilateration.h"

/*
TODO:
- clean up main (delete all)

*/

arm_status calculatePingerPosition(int32_t TdoaArray[],
                                   HydrophonePositions HydrophonePositions[],
                                   const arm_matrix_instance_f32* pA,
                                   const arm_matrix_instance_f32* pB,
                                   arm_matrix_instance_f32* pResult) {
    compute_A(TdoaArray, pA->pData);
    compute_B(TdoaArray, HydrophonePositions, pB->pData);
    arm_status Status = LSE(pA, pB, pResult);
    return Status;
}

arm_status LSE(const arm_matrix_instance_f32* pA,
               const arm_matrix_instance_f32* pB,
               arm_matrix_instance_f32* pResult) {
    arm_matrix_instance_f32 Atrans;
    Atrans.numCols = NUM_HYDROPHONES - 1;
    Atrans.numRows = NUM_DIMENTIONS + 1;
    Atrans.pData = new float32_t[(NUM_HYDROPHONES - 1) * (NUM_DIMENTIONS + 1)];
    arm_status Status = arm_mat_trans_f32(pA, &Atrans);
    if (Status != ARM_MATH_SUCCESS) {
        return Status;
    }

    arm_matrix_instance_f32 AtransXA;
    AtransXA.numCols = NUM_HYDROPHONES - 1;
    AtransXA.numRows = NUM_HYDROPHONES - 1;
    AtransXA.pData =
        new float32_t[(NUM_HYDROPHONES - 1) * (NUM_HYDROPHONES - 1)];
    const arm_matrix_instance_f32* pAtrans = &Atrans;
    Status = arm_mat_mult_f32(pAtrans, pA, &AtransXA);
    if (Status != ARM_MATH_SUCCESS) {
        return Status;
    }

    arm_matrix_instance_f32 AtransXAinv;
    AtransXAinv.numCols = NUM_HYDROPHONES - 1;
    AtransXAinv.numRows = NUM_HYDROPHONES - 1;
    AtransXAinv.pData =
        new float32_t[(NUM_HYDROPHONES - 1) * (NUM_HYDROPHONES - 1)];
    const arm_matrix_instance_f32* pAtransXA = &AtransXA;
    Status = arm_mat_inverse_f32(pAtransXA, &AtransXAinv);
    if (Status != ARM_MATH_SUCCESS) {
        return Status;
    }

    arm_matrix_instance_f32 AtransXAinvXAtrans;
    AtransXAinvXAtrans.numCols = NUM_HYDROPHONES - 1;
    AtransXAinvXAtrans.numRows = NUM_HYDROPHONES - 1;
    AtransXAinvXAtrans.pData =
        new float32_t[(NUM_HYDROPHONES - 1) * (NUM_HYDROPHONES - 1)];
    const arm_matrix_instance_f32* pAtransAinv = &AtransXAinv;
    Status = arm_mat_mult_f32(pAtransAinv, pAtrans, &AtransXAinvXAtrans);
    if (Status != ARM_MATH_SUCCESS) {
        return Status;
    }

    const arm_matrix_instance_f32* pAtransXAinvXAtrans = &AtransXAinvXAtrans;
    Status = arm_mat_mult_f32(pAtransXAinvXAtrans, pB, pResult);
    if (Status != ARM_MATH_SUCCESS) {
        return Status;
    }

    delete[] Atrans.pData;
    delete[] AtransXA.pData;
    delete[] AtransXAinv.pData;
    delete[] AtransXAinvXAtrans.pData;

    return Status;
}

void initialComputationA(float32_t* AData,
                         HydrophonePositions HydrophonePositions[]) {
    for (int i = 0; i < (NUM_HYDROPHONES - 1); i++) {
        *(AData + i * (NUM_HYDROPHONES - 1) + 0) =
            HydrophonePositions[0].PosX - HydrophonePositions[i + 1].PosX;
        *(AData + i * (NUM_HYDROPHONES - 1) + 1) =
            HydrophonePositions[0].PosY - HydrophonePositions[i + 1].PosY;
        *(AData + i * (NUM_HYDROPHONES - 1) + 2) =
            HydrophonePositions[0].PosZ - HydrophonePositions[i + 1].PosZ;
        *(AData + i * (NUM_HYDROPHONES - 1) + 3) = 0.0;
    }
}

void compute_A(int32_t TdoaArray[], float32_t* AData) {
    for (int i = 0; i < (NUM_HYDROPHONES - 1); i++) {
        *(AData + i * (NUM_HYDROPHONES - 1) + 3) =
            TdoaArray[i] * SOUND_SPEED / SAMPLING_FREQ;
    }
}

void compute_B(int32_t TdoaArray[], HydrophonePositions HydrophonePositions[],
               float32_t* Bdata) {
    for (int i = 0; i < (NUM_HYDROPHONES - 1); i++) {
        *(Bdata + i) =
            0.5 * (pow(HydrophonePositions[0].PosX, 2) -
                   pow(HydrophonePositions[i + 1].PosX, 2) +
                   pow(HydrophonePositions[0].PosY, 2) -
                   pow(HydrophonePositions[i + 1].PosY, 2) +
                   pow(HydrophonePositions[0].PosZ, 2) -
                   pow(HydrophonePositions[i + 1].PosZ, 2) +
                   pow(TdoaArray[i] * SOUND_SPEED / SAMPLING_FREQ, 2));
    }
}

void initHydrophonePositions(HydrophonePositions* hydrophonePositions) {
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
