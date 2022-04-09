#include "pulse_determination.h"

bool shortTimeFourierTransform(
    float32_t* pulse, 
    uint32_t pulseLength, 
    float32_t* dstFft
    ){
    float32_t* windowed_pulse = new float32_t[pulseLength]; 
    float32_t* nPointFft = new float32_t[FFT_SIZE]; //check if it should be FFT SIZE / 2

    arm_rfft_fast_instance_f32* fftStructure = new arm_rfft_fast_instance_f32; 
    arm_status initStatus = arm_rfft_fast_init_f32(fftStructure, FFT_SIZE); //check if it should be FFT SIZE / 2
    
    int M = (pulseLength * SAMPLING_FREQUENCY) / FFT_SIZE; 
    for (int i = 0; i < M; i ++){

        arm_rfft_fast_f32(fftStructure, (pulse + i*FFT_SIZE), nPointFft, 0); 

        for (int j = 0; j < FFT_SIZE; j++){
            dstFft[j] += nPointFft[j]; 
        }
    }

    delete[] windowed_pulse; 
    delete[] nPointFft; 
    delete fftStructure; 

    return (initStatus == ARM_MATH_SUCCESS); 

}

float32_t computeTone(float32_t* fft){
    float32_t* fft_abs = new float32_t[FFT_SIZE]; 
    arm_abs_f32(fft, fft_abs, FFT_SIZE); 

    float32_t max = 0; 
    uint32_t argmax = 0; 
    for (uint32_t i = 0; i < FFT_SIZE; i++){ //(uint32_t i = FFT_SIZE/2
        if (max < fft_abs[i]){
            argmax = i; 
            max = fft_abs[i]; 
        }
    }

    float32_t binWidth = static_cast<float32_t>(SAMPLING_FREQUENCY) / FFT_SIZE; 
    float32_t tone = binWidth*argmax; 

    delete[] fft_abs; 

    return tone; 
} 

void makeBartlettWindow(uint32_t arrayLength, float32_t* dstWindow){ 
    for (uint32_t n = 0; n < arrayLength; n++){
        dstWindow[n] = (2/(arrayLength-1))*(((arrayLength-1)/2)-abss(n-(arrayLength-1)/2)); 
    }
}

int abss(int x){
    if (x < 0){
        return -x; 
    }
    else{
        return x; 
    }
}