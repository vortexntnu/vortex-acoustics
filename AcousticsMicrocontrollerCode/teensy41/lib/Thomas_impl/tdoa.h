#pragma once

#include <vector>
#include "arm_math.h"



class Pos {
    public:
        float32_t x;
        float32_t y;
        float32_t z;

        float32_t dist; // distance

        Pos(float32_t x, float32_t y, float32_t z): x(x), y(y), z(z){arm_sqrt_f32(x*x+y*y+z*z, &dist);}
        Pos(std::vector<float32_t> pos);
};






std::vector<double> multilateration(std::vector<std::vector<double>> hydrophone_array, std::vector<double> TDOA);