#include "Arduino.h"
#include "pulse_determination.h"
#include "unity.h"


void generatePulse(
    uint32_t pulseLength, // [ms]
    float32_t* cosWave, 
    float32_t carrierFrequency // [kHz]
){
    float32_t* x = new float32_t[pulseLength*SAMPLING_FREQUENCY]; 
    x[0] = 0.0; 
    for (uint32_t i = 0; i < pulseLength*SAMPLING_FREQUENCY-1; i++){
        x[i+1] = x[i] + 1/(static_cast<float32_t>(SAMPLING_FREQUENCY)*1000);  
    }

    for (uint32_t i = 0; i < pulseLength*SAMPLING_FREQUENCY; i++){
         cosWave[i] = arm_cos_f32(2*3.14*carrierFrequency*1000*x[i]); 
    }

    delete[] x; 
}

void test_short_time_fourier_transform(){
    float32_t carrierFrequency = 25; // [kHz]
    uint32_t pulseLength = 4; // [ms]

    float32_t* pulse = new float32_t[pulseLength*SAMPLING_FREQUENCY]; 
    generatePulse(pulseLength, pulse, carrierFrequency); 

    float32_t* fft = new float32_t[FFT_SIZE]; 
    bool status = shortTimeFourierTransform(pulse, pulseLength, fft); 
    TEST_ASSERT_TRUE(status); 

    float32_t computedCarrierFrequency = computeCarrierFrequency(fft); 

    float32_t tolerance =  static_cast<float32_t>(SAMPLING_FREQUENCY) / FFT_SIZE; 
    TEST_ASSERT_FLOAT_WITHIN(tolerance, carrierFrequency, computedCarrierFrequency); 
 
    delete[] pulse; 
    delete[] fft; 

}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(test_short_time_fourier_transform);
    UNITY_END();
}