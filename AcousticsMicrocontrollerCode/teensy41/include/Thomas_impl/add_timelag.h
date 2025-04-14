#pragma once
#include <vector>
#include "arm_math.h"

#define HYDROPHONE_DATA_SIZE_TESTING 6144

std::vector<float32_t> add_timelag(std::vector<float32_t> data, float32_t tdoa, float32_t sampling_frequency);
int* add_timelag_array(int* data, const int size, float32_t tdoa, float32_t sampling_frequency);