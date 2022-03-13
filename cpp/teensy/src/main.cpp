#include <Arduino.h>
#include "arm_math.h"
#include "stdio.h"
#include "algorithm"


int main(void) {
    Serial.begin(9660);
    while (!Serial){} 
    Serial.println("Serial connected"); 

    float32_t arr1[] = {3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7, 0.0, 0.0}; 
    float32_t arr2[] = {0.0, 0.0, 3.0, 5.2, 5.2, 6.7, 8.9, 2.0, 6.7, 6.7, 5.7};  

    uint32_t size =  2 * 11 - 1; 
    float32_t result_from_teensy[size]; 
    arm_correlate_f32(arr1, 11, arr2, 11, result_from_teensy); 
    Serial.println("Post Correlation");


    for (uint32_t i = 0; i < size; i++){
        //int corr = static_cast<int>(result_from_teensy[i]); 
        Serial.printf("%.3f, ", result_from_teensy[i]);
    }
    Serial.println("");


    while(true){
        Serial.printf("Loop"); 
        delay(1000); 
    }
    return 0;
}
