#include <unity.h>
#include "arm_math.h"

#define SAMPLING_FREQUENCY 427 // [kHz]
#define FFT_SIZE 256
#define PULSE_LENGTH 4 //[ms]

void shortTimeFourierTransform(float32_t* pulse,  float32* dstFft); 
float32_t computeTone(float32_t* fft); //remeber here you must find the exact freq after using argmax
void applyBartlettWindow(float32t* pulse, float32* dstWindowedPulse); 