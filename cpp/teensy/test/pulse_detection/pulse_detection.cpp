#include "Arduino.h"
#include "pulse_determination.h"
#include "unity.h"

/*
TODO: 
- add white noise to signal
- test compute tone 

*/

void generate_pulse(
    uint32_t pulseLength,
    float32_t* cos_wave
){
    for (uint32_t i = 0; i < pulseLength*SAMPLING_FREQUENCY; i++){
         cos_wave[i] = arm_cos_f32(2*3.14*SAMPLING_FREQUENCY*i); 
    }
}

void test_short_time_fourier_transform(){
    float32_t* window = new float32_t[FFT_SIZE]; 
    makeBartlettWindow(FFT_SIZE, window); 

    uint32_t pulseLength = 4; // [ms]
    float32_t* pulse = new float32_t[pulseLength*SAMPLING_FREQUENCY]; 
    generate_pulse(pulseLength, pulse); 

    float32_t* fft = new float32_t[FFT_SIZE]; 
    bool  status = shortTimeFourierTransform(pulse, pulseLength, window, fft); 
    TEST_ASSERT_TRUE(status); 


    delete[] window; 
    delete[] pulse; 
    delete[] fft; 

}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_short_time_fourier_transform);
    UNITY_END();
}