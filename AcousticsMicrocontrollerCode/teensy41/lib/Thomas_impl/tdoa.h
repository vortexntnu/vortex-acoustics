#pragma once

#include <vector>
#include "arm_math.h"
#include <iostream>



class Pos {
    public:
        float32_t x;
        float32_t y;
        float32_t z;

        float32_t dist; // distance

        Pos(float32_t x, float32_t y, float32_t z): x(x), y(y), z(z){arm_sqrt_f32(x*x+y*y+z*z, &dist);}
        Pos(std::vector<float32_t> pos);
};

std::ostream& operator <<(std::ostream& os, Pos& pos);





std::vector<double> multilateration(std::vector<Pos> hydrophone_array, std::vector<double> TDOA);