#include "arm_math.h"

const uint32_t SAMPLING_FREQUENCY = 427; // [kHz]
const uint32_t FFT_SIZE = 256;
const uint32_t PULSE_LENGTH = 4; //[ms]

arm_status shortTimeFourierTransform(float32_t* pulse, uint32_t pulseLength,
                                     float32_t* dstFft);

float32_t computeCarrierFrequency(float32_t* fft);
