
#include <arm_math.h>
#include <vector>
#include "add_timelag.h"

std::vector<float32_t> add_timelag(std::vector<float32_t> data, float32_t tdoa, float32_t sampling_frequency){
    int index_lag = static_cast<int>(tdoa * sampling_frequency);
    for (int i = 0; i < index_lag; i++){
        data.insert(data.begin(), 0);
        data.pop_back();
    }
    return data;
}




int* add_timelag_array(int* data, const int size, float32_t tdoa, float32_t sampling_frequency) {
    // Calculate the sample shift amount.
    int index_lag = static_cast<int>(tdoa * sampling_frequency);
    static int new_array[HYDROPHONE_DATA_SIZE_TESTING];

    // Handle positive timelag: shift right.
    if (index_lag > 0) {

        if (index_lag >= size) {
            for (int i = 0; i < size; i++) {
                new_array[i] = 0;
            }
        } else {
            // Fill the beginning with zeros.
            for (int i = 0; i < index_lag; i++) {
                new_array[i] = 0;
            }
            for (int i = index_lag; i < size; i++) {
                new_array[i] = data[i - index_lag];
            }
        }
    }
    // Handle negative timelag: shift left.
    else if (index_lag < 0) {
        int shift = -index_lag;  // Convert negative lag to positive shift.
        if (shift >= size) {
            // If the shift is greater than or equal to the size, fill with zeros.
            for (int i = 0; i < size; i++) {
                new_array[i] = 0;
            }
        } else {
            // Copy the data shifted to the left.
            for (int i = 0; i < size - shift; i++) {
                new_array[i] = data[i + shift];
            }
            // Fill the end with zeros.
            for (std::size_t i = size - shift; i < size; i++) {
                new_array[i] = 0;
            }
        }
    }
    // If index_lag is zero, simply copy the array.
    else {
        for (int i = 0; i < size; i++) {
            new_array[i] = data[i];
        }
    }

    return new_array;
}