#ifndef CORELATION_H
#define CORELATION_H
#include <vector>
#include "/home/thomas/acoustics_ws/src/vortex-acoustics/AcousticsMicrocontrollerCode/teensy41/lib/Include/arm_math.h"



std::vector <float32_t> still_brute_force_but_better_best_crosscorelation_lag(std::vector <float32_t> x, std::vector <float32_t> y);

#endif