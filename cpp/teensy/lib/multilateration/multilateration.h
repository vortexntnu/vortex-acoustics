#pragma once

#include "arm_math.h"
#include <Arduino.h>

const int32_t NUM_HYDROPHONES = 5;
const int32_t NUM_DIMENSIONS = 3;

const float32_t SOUND_SPEED = 1480.0; //[m/s]
const int SAMPLING_FREQ = 300000;     //[Hz]

struct Position {
    float32_t X;
    float32_t Y;
    float32_t Z;
};

// OBS: use same x, y, z system as autonomous
const Position hydrophonePositions[NUM_HYDROPHONES] = {
    {-0.11, 0.31, 0.10}, // Hyd0 {x,y,z}
    {0.11, 0.31, 0.10},  // Hyd1 {x,y,z}
    {0.0, -0.24, 0.0},   // Hyd2 {x,y,z}
    {0.5, -0.1, 0.4},    // Hyd3 {x,y,z}
    {0.4, 0.0, -0.4}     // Hyd4 {x,y,z}
};

arm_status calculatePingerPosition(int32_t TdoaArray[],
                                   const Position hydrophonePositions[],
                                   const arm_matrix_instance_f32* pA,
                                   const arm_matrix_instance_f32* pB,
                                   Position* pSourcePosition);

void initialComputationA(float32_t* AData,
                         const Position hydrophonePositions[]);
void computeA(int32_t TdoaArray[], float32_t* AData);
void computeB(int32_t TdoaArray[], const Position hydrophonePositions[],
              float32_t* BData);

arm_status leastSquareEstimation(const arm_matrix_instance_f32* pA,
                                 const arm_matrix_instance_f32* pB,
                                 Position* pSourcePosition);
