#include "hydrophones.h"

HYDROPHONES::Hydrophones::Hydrophones(TRILITERATION::Pos pos) : 
    pos{pos}, last_lag{0}
{
    /* Initial memory allocation */
    p_data = (float32_t*) malloc(4*DSP_CONSTANTS::DMA_BUFFER_LENGTH);
    p_mag_data = (float32_t*) malloc(4*DSP_CONSTANTS::DMA_BUFFER_LENGTH);
    p_max_val = (float32_t*) malloc(4);
    p_idx = (uint32_t*) malloc(4);
}

HYDROPHONES::Hydrophones::~Hydrophones()
{
    /* Deleting the allocated memory */
    free(p_data);
    free(p_mag_data);
    free(p_max_val);
    free(p_idx);
}

void HYDROPHONES::Hydrophones::analyze_data(float32_t *p_data_arr)
{
    /**
     * What is interesting to get the data from:
     *  -correlation (could be found under arm_correlate_f32.h)
     *  -FFT/IFFT (could be found under arm_cfft_f32.h)
     *  -power of the array (could be found under arm_power_f32.h, but must be filtered first)
     *  -intensity of the array (could be found under arm_cmplx_mag)
     */
    
    // Saves the data
    p_data = p_data_arr;

    // Takes the complex FFT with length 1024
    arm_cfft_f32(&arm_cfft_sR_f32_len1024, pdata, 0, 1);

    // Finds the complex magnitude output
    arm_cmplx_mag_f32(p_data, p_mag_data, DSP_CONSTANTS::FFT_SIZE);

    // Calculates the maxValue and returns corresponding value
    arm_max_f32(p_mag_data, DSP_CONSTANTS::FFT_SIZE, p_max_val, p_idx);


    /**
     * What is left to implement as of 09.12.2020:
     *  -filtering of the signal, to prevent engine-noise etc. to dominate 
     *          the data-signals
     *  -intensity-estimates of the desired signals
     */
}


