#include "multilateration.h"
#include "arm_math.h"
#include <cstddef>
#include <stdint.h>
#include <stddef.h>




// Helper function for squaring a number.
static inline float32_t square(float32_t x) {
    return x * x;
}




size_t find_peak_index(q15_t* signal, int size) {
    // If the signal is empty, return 0
    if (size == 0) {
        return 0;
    }

    int peakIndex = 0;
    int peakValue = signal[0];

    // Loop through the signal starting from the first element.
    for (int i = 1; i < size; ++i) {
        if (signal[i] > peakValue) {
            peakValue = signal[i];
            peakIndex = i;
        }
    }
    return peakIndex;
}



/*
 * tdoa_multilateration: solves A*x=b using 4 hydrophones.
 *
 * Parameters:
 *   hydrophone_array: a 4x3 array of float32_t where each row is {x, y, z}
 *   TDOA: an array of 4 float32_t values representing time differences of arrival.
 *
 * Returns:
 *   A pointer to a static array of 4 float32_t values (the solution vector).
 */
arm_status tdoa_multilateration(const float32_t hydrophone_array[4][3], const float32_t TDOA[4], float32_t* result) {

    // Speed of sound in water.
    const float32_t c = 1500.0f;

    // Compute distances = c * TDOA for each hydrophone.
    float32_t distances[4];
    for (size_t i = 0; i < 4; i++) {
        distances[i] = TDOA[i] * c;
    }

    // Build the A matrix (4x4) from hydrophone positions and distances.
    float32_t A_data[4 * 4] = {
        hydrophone_array[0][0], hydrophone_array[0][1], hydrophone_array[0][2], -distances[0],
        hydrophone_array[1][0], hydrophone_array[1][1], hydrophone_array[1][2], -distances[1],
        hydrophone_array[2][0], hydrophone_array[2][1], hydrophone_array[2][2], -distances[2],
        hydrophone_array[3][0], hydrophone_array[3][1], hydrophone_array[3][2], -distances[3]
    };

    // Build the b vector (4x1) using the formula:
    // b[i] = 0.5 * (x_i^2 + y_i^2 + z_i^2 - d_i^2)
    float32_t b_data[4] = {
        0.5f * ( square(hydrophone_array[0][0]) + square(hydrophone_array[0][1]) + square(hydrophone_array[0][2]) - square(distances[0]) ),
        0.5f * ( square(hydrophone_array[1][0]) + square(hydrophone_array[1][1]) + square(hydrophone_array[1][2]) - square(distances[1]) ),
        0.5f * ( square(hydrophone_array[2][0]) + square(hydrophone_array[2][1]) + square(hydrophone_array[2][2]) - square(distances[2]) ),
        0.5f * ( square(hydrophone_array[3][0]) + square(hydrophone_array[3][1]) + square(hydrophone_array[3][2]) - square(distances[3]) )
    };

    // Declare matrix instances.
    arm_matrix_instance_f32 A, b, A_inverse, result_vect;
    float32_t A_inverse_data[4 * 4];

    // Initialize the matrices.
    arm_mat_init_f32(&A, 4, 4, A_data);
    arm_mat_init_f32(&b, 4, 1, b_data);
    arm_mat_init_f32(&A_inverse, 4, 4, A_inverse_data);
    arm_mat_init_f32(&result_vect, 4, 1, result);

    // Invert A to get A_inverse.
    arm_status status = arm_mat_inverse_f32(&A, &A_inverse);
    if (status == ARM_MATH_SUCCESS) {
        // Multiply A_inverse and b to solve for x.
        status = arm_mat_mult_f32(&A_inverse, &b, &result_vect);
        // (Optional: you might want to handle status differently if multiplication fails.)
    }

    return status;
}



