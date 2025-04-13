#pragma once
#include <vector>
#include "arm_math.h"
#include "DSP.h" // for defines
#include "adc.h" // for defines



#define NUM_HYDROPHONES 5
#define NUM_DIMENSIONS 3
#define HYDROPHONE_DATA_SIZE (SAMPLE_LENGTH * BUFFER_PER_CHANNEL)

#define TDOA_DATA_LENGHT (NUM_HYDROPHONES-1)// Difference between a main hydrophone and the rest
#define POSITION_DATA_LENGHT 3




const float32_t SOUND_SPEED = 1500.0; //[m/s]
const int SAMPLING_FREQ = 300000;     //[Hz]

// double TDOA_VALUES[TDOA_DATA_LENGHT]; // time difference between hydrophone 0 and 1, 2, 3, 4 [s]
// double soundLocation[NUM_DIMENSIONS]; // Pinger position




namespace Multilateration{

class Pos {
    public:
        float32_t x;
        float32_t y;
        float32_t z;

        float32_t dist; // distance

        

        Pos(float32_t x, float32_t y, float32_t z): x(x), y(y), z(z){arm_sqrt_f32(x*x+y*y+z*z, &dist);}
        Pos(std::vector<float32_t> pos);

        Pos direction(){return Pos(x/dist, y/dist, z/dist);}
        float32_t dist_to(const Pos& other);
        void display(); // This is just for testing
};

const Pos hydrophonePositions[NUM_HYDROPHONES] {
    {0.0, 0.0, 0.0},
    {1.0, 1.0, 1.0},
    {1.2, 0.8, 1.1},
    {0.9, 1.3, 1.2},
    {1.1, 1.4, 0.9}
};


float32_t calculate_tdoa(Pos pos, Pos pinger_pos, float32_t v);


Pos tdoa_multilateration(const Pos hydrophone_pos_array[NUM_HYDROPHONES], const std::vector<float32_t>& TDOA);

Pos find_pinger(int hydrophone_value_array[NUM_HYDROPHONES][HYDROPHONE_DATA_SIZE], const int sampling_frequency);

}