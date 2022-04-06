#include "pulse_determination.h"


void shortTimeFourierTransform(
    float32_t* pulse, 
    uint32_t pulseLength, 
    float32_t* window,
    float32_t* dstFft
    ){
    float32_t* windowed_pulse = new float32_t[pulseLength]; 
    float32_t* nPointFft = new float32_t[FFT_SIZE]; //check if it should be FFT SIZE / 2

    arm_rfft_fast_instance_f32* fftStructure = new arm_rfft_fast_instance_f32; 
    arm_status initStatus = arm_rfft_fast_init_f32(fftStructure, FFT_SIZE); //check if it should be FFT SIZE / 2
    
    int M = (pulseLength * SAMPLING_FREQUENCY) / FFT_SIZE; 
    for (int i = 0; i < M; i++){
        arm_status convolutionStatus = arm_conv_partial_f32(
            (pulse+i*pulseLength), pulseLength, //dobbel check if this is correct 
            window, pulseLength, windowed_pulse, pulseLength/2, pulseLength); 
        
        arm_rfft_fast_f32(fftStructure, windowed_pulse, nPointFft, 0); 

        for (int j = 0; j < FFT_SIZE; j++){
            dstFft[j] += nPointFft[j]; 
        }
    }

    delete[] windowed_pulse; 
    delete[] nPointFft; 
    delete fftStructure; 
}

float32_t computeTone(float32_t* fft){
    float32_t* fft_abs = new float32_t[FFT_SIZE]; 
    arm_abs_f32(fft, fft_abs, FFT_SIZE); 

    float32_t max = 0; 
    uint32_t argmax; 
    for (int i = FFT_SIZE/2; i < FFT_SIZE; i++){
        if (max < fft_abs[i]){
            argmax = i; 
        }
    }

    float32_t binWidth = SAMPLING_FREQUENCY / FFT_SIZE; 
    float32_t tone = binWidth*argmax; 

    delete[] fft_abs; 

    return tone; 
} 

void makeBartlettWindow(float32_t* array, uint32_t arrayLength, float32_t* dstWindowedPulse){ 
    for (int n = 0; n < arrayLength; n++){
        dstWindowedPulse[n] = (2/(arrayLength-1))*(((arrayLength-1)/2)-abs(n-(arrayLength-1)/2)); 
    }
}

int abs(int x){
    if (x < 0){
        return -x; 
    }
    else{
        return x; 
    }
}