#ifndef TDOA_H
#define TDOA_H
#include <vector>
#include "/home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Include/arm_math.h"
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

std::ostream& operator <<(std::ostream& os, const Pos& pos);





std::vector<float32_t> multilateration(std::vector<Pos> hydrophone_array, std::vector<float32_t> TDOA);

#endif