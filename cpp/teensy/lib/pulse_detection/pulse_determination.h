//#include <unity.h>
#include "arm_math.h"

#define SAMPLING_FREQUENCY 427 // [kHz]
#define FFT_SIZE 256
#define PULSE_LENGTH 4 //[ms]

bool shortTimeFourierTransform(
    float32_t* pulse, 
    uint32_t pulseLength, 
    float32_t* window,
    float32_t* dstFft); 
    
float32_t computeTone(float32_t* fft); //remeber here you must find the exact freq after using argmax
void makeBartlettWindow(uint32_t arrayLength, float32_t* dstWindow); 

int abss(int x); 