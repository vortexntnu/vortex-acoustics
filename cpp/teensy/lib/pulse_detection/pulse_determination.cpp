#include "pulse_determination.h"

arm_status shortTimeFourierTransform(float32_t* pulse, uint32_t pulseLength,
                                     float32_t* mnPointFft) {

    float32_t* nPointFft = new float32_t[FFT_SIZE];

    arm_rfft_fast_instance_f32* fftStructure = new arm_rfft_fast_instance_f32;
    arm_status initStatus = arm_rfft_fast_init_f32(fftStructure, FFT_SIZE);

    if (initStatus != ARM_MATH_SUCCESS)
        return initStatus;

    int M = (pulseLength * SAMPLING_FREQUENCY) / FFT_SIZE;
    for (int i = 0; i < M; i++) {

        arm_rfft_fast_f32(fftStructure, (pulse + i * FFT_SIZE), nPointFft, 0);

        for (int j = 0; j < FFT_SIZE; j++) {
            mnPointFft[j] += nPointFft[j];
        }
    }

    delete[] nPointFft;
    delete fftStructure;

    return ARM_MATH_SUCCESS;
}

float32_t computeCarrierFrequency(float32_t* fft) {

    float32_t max = -1;
    uint32_t argmax = 0;
    for (uint32_t i = 0; i < FFT_SIZE; i += 2) {
        float32_t real = fft[i];
        float32_t imag = fft[i + 1];

        float32_t abs_val = real * real + imag * imag;
        if (max < abs_val) {
            argmax = i;
            max = abs_val;
        }
    }

    float32_t binWidth = static_cast<float32_t>(SAMPLING_FREQUENCY) / FFT_SIZE;
    float32_t carrierFrequency = binWidth * argmax / 2;

    return carrierFrequency;
}
