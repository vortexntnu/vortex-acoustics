#pragma once
#include <vector>
#include "arm_math.h"
#include <string>





class Pos {
    public:
        float32_t x;
        float32_t y;
        float32_t z;

        float32_t dist; // distance

        

        Pos(float32_t x, float32_t y, float32_t z): x(x), y(y), z(z){arm_sqrt_f32(x*x+y*y+z*z, &dist);}
        Pos(std::vector<float32_t> pos);

        Pos direction(){return Pos(x/dist, y/dist, z/dist);}
        void display(); // This is just for testing
};

float32_t calculate_tdoa(Pos pos, Pos pinger_pos, float32_t v);


Pos tdoa_multilateration(std::vector<Pos> hydrophone_array, std::vector<float32_t> TDOA);

