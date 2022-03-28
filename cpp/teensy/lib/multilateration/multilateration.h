#pragma once

#include "arm_math.h"
#include <Arduino.h>

#ifndef HYDROPHONE_DETAILS
#define HYDROPHONE_DETAILS

const int32_t NUM_HYDROPHONES  = 5; 
const int32_t NUM_DIMENSIONS  = 3; 

const float32_t HYD_0_X_POS = -0.11; 
const float32_t HYD_0_Y_POS = 0.31; 
const float32_t HYD_0_Z_POS = 0.10; 

const float32_t HYD_1_X_POS = 0.11; 
const float32_t HYD_1_Y_POS = 0.31; 
const float32_t HYD_1_Z_POS = 0.10; 

const float32_t HYD_2_X_POS = 0.0; 
const float32_t HYD_2_Y_POS = -0.24; 
const float32_t HYD_2_Z_POS = 0.0; 

const float32_t HYD_3_X_POS = 0.5; 
const float32_t HYD_3_Y_POS = -0.1; 
const float32_t HYD_3_Z_POS = 0.4; 

const float32_t HYD_4_X_POS = 0.4; 
const float32_t HYD_4_Y_POS = 0.0; 
const float32_t HYD_4_Z_POS = -0.4; 

#endif

#ifndef PHYSICAL_CONSTANTS
#define PHYSICAL_CONSTANTS

const float32_t SOUND_SPEED = 1480.0;  //[m/s]

#endif

#ifndef DSP_CONSTANTS
#define DSP_CONSTANTS

const int SAMPLING_FREQ = 300000;             //[Hz]

#endif

struct HydrophonePositions {
    float32_t PosX;
    float32_t PosY;
    float32_t PosZ;
};

arm_status calculatePingerPosition(int32_t TdoaArray[],
                                   HydrophonePositions HydrophonePositions[],
                                   const arm_matrix_instance_f32* pA,
                                   const arm_matrix_instance_f32* pB,
                                   arm_matrix_instance_f32* pResult);

void initialComputationA(float32_t* AData,
                         HydrophonePositions HydrophonePositions[]);
void computeA(int32_t TdoaArray[], float32_t* AData);
void computeB(int32_t TdoaArray[], HydrophonePositions HydrophonePositions[],
               float32_t* BData);

arm_status leastSquareEstimation(const arm_matrix_instance_f32* pA,
               const arm_matrix_instance_f32* pB,
               arm_matrix_instance_f32* pResult);

void initHydrophonePositions(HydrophonePositions* HydrophonePositions);
