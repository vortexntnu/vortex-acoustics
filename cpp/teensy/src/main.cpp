#include "algorithm"
#include "arm_math.h"
#include "stdio.h"
#include "Arduino.h"

#include "algorithm"

#include "../pulse_detection/pulse_determination.h"

void generate_pulse(
    uint32_t pulseLength,
    float32_t* cos_wave
){
    for (uint32_t i = 0; i < pulseLength*SAMPLING_FREQUENCY; i++){
         cos_wave[i] = arm_cos_f32(2*3.14*SAMPLING_FREQUENCY*i); 
    }
}

int main(void) {
    Serial.begin(9600); 
    while (!Serial){}

    uint32_t pulseLength = 4; // [ms]
    float32_t* pulse = new float32_t[pulseLength*SAMPLING_FREQUENCY]; 
    generate_pulse(pulseLength, pulse); 

    float32_t* fft = new float32_t[FFT_SIZE]; 
    bool status = shortTimeFourierTransform(pulse, pulseLength, fft); 
    if (!status){
        Serial.print("Short time fourier transform went wrong, \n"); 
    }

    float32_t tone = computeTone(fft); 
    Serial.printf("The carrier frequency is: ", tone); 


    delete[] pulse; 
    delete[] fft; 
    while (true) {
    }
}
