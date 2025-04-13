
#include <vector>
#include "tdoa.h"
#include "corelation.h"
#include <Arduino.h>


namespace Multilateration{

// for testing
float32_t calculate_tdoa(Pos pos, Pos pinger_pos, float32_t v){
    Pos relative_pos(pos.x-pinger_pos.x,pos.y-pinger_pos.y,pos.z-pinger_pos.z);
    return (1/v)*(relative_pos.dist-pinger_pos.dist);
}




// All this just to solve A*x=b like this x = A⁻¹b
Pos tdoa_multilateration(const Pos hydrophone_pos_array[NUM_HYDROPHONES], const double TDOA[TDOA_DATA_LENGHT]){
    // Firs element in hydrophone is zero
    // The first element in TDOA is the timedifference between the main and first hydrophone in hydrophone array,
    // the second element for the main and second hydrophone and so on.


    //d == c*TDOA
    float32_t const c = 1500;

    double distances[TDOA_DATA_LENGHT];
    for (size_t i = 0; i < TDOA_DATA_LENGHT; i++){
        distances[i] = c*TDOA[i];
    };


    float32_t A_data[4 * 4] = {
        hydrophone_pos_array[1].x, hydrophone_pos_array[1].y, hydrophone_pos_array[1].z, -distances[0],
        hydrophone_pos_array[2].x, hydrophone_pos_array[2].y, hydrophone_pos_array[2].z, -distances[1],
        hydrophone_pos_array[3].x, hydrophone_pos_array[3].y, hydrophone_pos_array[3].z, -distances[2],
        hydrophone_pos_array[4].x, hydrophone_pos_array[4].y, hydrophone_pos_array[4].z, -distances[3]
    };
    
    auto square = [](float32_t x) -> float32_t { return x * x; };

    float32_t b_data[4] = {
        0.5f * (square(hydrophone_pos_array[1].x) + square(hydrophone_pos_array[1].y) + square(hydrophone_pos_array[1].z) - square(distances[0])),
        0.5f * (square(hydrophone_pos_array[2].x) + square(hydrophone_pos_array[2].y) + square(hydrophone_pos_array[2].z) - square(distances[1])),
        0.5f * (square(hydrophone_pos_array[3].x) + square(hydrophone_pos_array[3].y) + square(hydrophone_pos_array[3].z) - square(distances[2])),
        0.5f * (square(hydrophone_pos_array[4].x) + square(hydrophone_pos_array[4].y) + square(hydrophone_pos_array[4].z) - square(distances[3]))
    };


    arm_matrix_instance_f32 A;
    arm_mat_init_f32(&A, 4, 4, A_data);

    arm_matrix_instance_f32 b;
    arm_mat_init_f32(&b, 4, 1, b_data);

    float32_t A_inverse_data[4 * 4];
    arm_matrix_instance_f32 A_inverse;
    arm_mat_init_f32(&A_inverse, 4, 4, A_inverse_data);

    float32_t resulting_data[4];
    arm_matrix_instance_f32 result_vect;
    arm_mat_init_f32(&result_vect, 4, 1, resulting_data);
    std::vector<float32_t> result_vect_format(4, 0.0f); // for returning
    


    arm_status status =  arm_mat_inverse_f32(&A, &A_inverse);

    if (status == ARM_MATH_SUCCESS){
        status = arm_mat_mult_f32(&A_inverse, &b, &result_vect);

        
        if (status == ARM_MATH_SUCCESS){
            // Store the result in a std::vector
            for (int i = 0; i < 4; i++) {

                result_vect_format[i] = resulting_data[i];
            }
        }
    }

    return Pos(result_vect_format); // this will return all zeros if some calculationes failed
}








// TODO: Solve for x = (A^T A)^{-1} A^T b, which gives a more presise and reliable estimated position.
Pos multilateration_least_square(const std::vector<Pos>& hydrophones,
                                 const std::vector<float32_t>& tdoa,
                                 float32_t c)
{
    return Pos(0, 0, 0);
}





Pos find_pinger(int hydrophone_value_array[NUM_HYDROPHONES][HYDROPHONE_DATA_SIZE], double TDOA[TDOA_DATA_LENGHT], const int sampling_frequency){

    for (int i = 0; i < TDOA_DATA_LENGHT; i++) {

        // Compute cross-correlation (convolution) for all possible lags
        int max_lag = 2 * HYDROPHONE_DATA_SIZE - 1;  // Total number of possible shifts
        int32_t correlation_array[max_lag];
        crosscorelation(hydrophone_value_array[0], hydrophone_value_array[i+1], HYDROPHONE_DATA_SIZE, correlation_array);

        // Find peak of the cross corelation, thats where TDOA is located
        int peek_index = find_peak_index(correlation_array, max_lag);

        // Calculate the TDOA in seconds
        // Explain here in TDOA why subtract by HYDROPHONE_DATA_SIZE_TESTING (Because cross corelation produces lag in positive and negative +- HYDROPHONE_DATA_SIZE_TESTING), we want it in time format to be corret thus we subtract
        double tdoa = ((float)(peek_index - HYDROPHONE_DATA_SIZE)/sampling_frequency);


        TDOA[i] = tdoa;
    }

    Pos pinger = tdoa_multilateration(hydrophonePositions, TDOA);
    Serial.println("Calculated: ");
    pinger.display();
    return pinger;
}





Pos::Pos(std::vector<float32_t> pos): x(pos.at(0)), y(pos.at(1)), z(pos.at(2)){
    arm_sqrt_f32(x*x+y*y+z*z, &dist);
}


float32_t Pos::dist_to(const Pos& other){
    float32_t result;
    arm_sqrt_f32((other.x - x)*(other.x - x) + (other.y - y)*(other.y - y) + (other.z - z)*(other.z - z), &result);
    return result;
}



void Pos::display(){
    Serial.println();
    Serial.println("Position info: ");
    Serial.print("x: ");Serial.print(x, 5);Serial.print(", y: "); Serial.print(y, 5);Serial.print(", z: "); Serial.print(z, 5); Serial.print(", distance: "); Serial.print(dist, 5);
    Serial.println();
    Pos dir = (*this).direction();
    Serial.println("Direction: ");
    Serial.print("x: ");Serial.print(dir.x, 5);Serial.print(", y: "); Serial.print(dir.y, 5);Serial.print(", z: "); Serial.print(dir.z, 5); Serial.print(", distance: "); Serial.println(dir.dist);
    Serial.println();
}}