#include "arm_math.h"

#define SAMPLING_FREQUENCY 427 // [kHz]
#define FFT_SIZE 256
#define PULSE_LENGTH 4 //[ms]

bool shortTimeFourierTransform(float32_t* pulse, uint32_t pulseLength,
                               float32_t* dstFft);

float32_t computeCarrierFrequency(float32_t* fft);
