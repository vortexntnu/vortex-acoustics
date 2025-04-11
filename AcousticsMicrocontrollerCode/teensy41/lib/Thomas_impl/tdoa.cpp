
#include <vector>
#include "tdoa.h"
#include <Arduino.h>
#include <string>


// calculate it for testing
float32_t calculate_tdoa(Pos pos, Pos pinger_pos, float32_t v){
    Pos relative_pos(pos.x-pinger_pos.x,pos.y-pinger_pos.y,pos.z-pinger_pos.z);
    return (1/v)*(relative_pos.dist-pinger_pos.dist);
}




// All this just to solve A*x=b
Pos tdoa_multilateration(std::vector<Pos> hydrophone_array, std::vector<float32_t> TDOA){
    // Firs element in hydrophone is zero
    // The first element in TDOA is the timedifference between the main and first hydrophone in hydrophone array,
    // the second element for the main and second hydrophone and so on.


    if (TDOA.size() < 4 || hydrophone_array.size() < 4) {
        return std::vector<float32_t>(4, 0.0f);  // Return zero vector
    }



    //d == c*TDOA
    float32_t const c = 1500;

    std::vector<float32_t> distances = TDOA;
    for (size_t i = 0; i < distances.size(); i++){
        distances.at(i) *= c;
    };


    float32_t A_data[4 * 4] = {
        hydrophone_array.at(0).x, hydrophone_array.at(0).y, hydrophone_array.at(0).z, -distances.at(0),
        hydrophone_array.at(1).x, hydrophone_array.at(1).y, hydrophone_array.at(1).z, -distances.at(1),
        hydrophone_array.at(2).x, hydrophone_array.at(2).y, hydrophone_array.at(2).z, -distances.at(2),
        hydrophone_array.at(3).x, hydrophone_array.at(3).y, hydrophone_array.at(3).z, -distances.at(3)
    };
    
    auto square = [](float32_t x) -> float32_t { return x * x; };

    float32_t b_data[4] = {
        0.5f * (square(hydrophone_array.at(0).x) + square(hydrophone_array.at(0).y) + square(hydrophone_array.at(0).z) - square(distances.at(0))),
        0.5f * (square(hydrophone_array.at(1).x) + square(hydrophone_array.at(1).y) + square(hydrophone_array.at(1).z) - square(distances.at(1))),
        0.5f * (square(hydrophone_array.at(2).x) + square(hydrophone_array.at(2).y) + square(hydrophone_array.at(2).z) - square(distances.at(2))),
        0.5f * (square(hydrophone_array.at(3).x) + square(hydrophone_array.at(3).y) + square(hydrophone_array.at(3).z) - square(distances.at(3)))
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





Pos::Pos(std::vector<float32_t> pos): x(pos.at(0)), y(pos.at(1)), z(pos.at(2)){
    arm_sqrt_f32(x*x+y*y+z*z, &dist);

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
}