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
         cos_wave[i] = arm_cos_f32(2*3.14159265*SAMPLING_FREQUENCY*i); 
    }
}

void print_array(float32_t* array, int len){
    Serial.println(""); 
    for (int i = 0; i< len; i++){
        if (i%5 == 0){
            Serial.println("");
        }
        Serial.printf("%f, ", array[i]); 
    }
    Serial.println(""); 
}

int main(void) {
    Serial.begin(9600); 
    while (!Serial){}

    uint32_t pulseLength = 4; // [ms]
    float32_t* pulse = new float32_t[pulseLength*SAMPLING_FREQUENCY]; 
    generate_pulse(pulseLength, pulse);

    float32_t* mPointFft = new float32_t[FFT_SIZE];
    float32_t* nPointFft = new float32_t[FFT_SIZE]; //check if it should be FFT SIZE / 2

    arm_rfft_fast_instance_f32* fftStructure = new arm_rfft_fast_instance_f32; 
    arm_status initStatus = arm_rfft_fast_init_f32(fftStructure, FFT_SIZE); //check if it should be FFT SIZE / 2

    Serial.printf("\nInit status: %d \n", initStatus); 
    
    int M = (pulseLength * SAMPLING_FREQUENCY) / FFT_SIZE; 
    for (int i = 0; i < M; i ++){

        arm_rfft_fast_f32(fftStructure, (pulse + i*FFT_SIZE), nPointFft, 0); 

        for (int j = 0; j < FFT_SIZE; j++){
            mPointFft[j] += nPointFft[j]; //no complex values??
        }
    }

    float32_t* fft_abs = new float32_t[FFT_SIZE]; 
    arm_abs_f32(mPointFft, fft_abs, FFT_SIZE); 

    float32_t max = 0; 
    uint32_t argmax =0; 
    for (uint32_t i = 0; i < FFT_SIZE; i++){
        if (max < fft_abs[i]){
            argmax = i; 
            max = fft_abs[i]; 
        }
    } 

    Serial.print("FFT: "); 
    print_array(fft_abs, FFT_SIZE); 
    
    Serial.printf("Argmax: %d \n", argmax); //255


    float32_t binWidth = float32_t(SAMPLING_FREQUENCY) / FFT_SIZE; 

    Serial.printf("Binwidth: %f \n", binWidth); 

    float32_t tone = binWidth*argmax; 
    float32_t tone_juks = binWidth*15;  

    Serial.printf("Carrier freq: %f \n", tone); //425.33
    Serial.printf("Carrier freq juks: %f \n", tone_juks); //25.02

    delete[] fft_abs;
    delete[] nPointFft; 
    delete fftStructure;
    delete[] mPointFft; 
    delete[] pulse;

    while (true) {
    }
}
