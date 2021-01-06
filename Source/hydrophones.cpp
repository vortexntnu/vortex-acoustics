#include "hydrophones.h"

HYDROPHONES::Hydrophones::Hydrophones(TRILATERATION::Pos pos) : 
    pos{pos}, last_lag{0}, last_intensity{0}
{
    /* Initial memory allocation */
    p_data = (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::IN_BUFFER_LENGTH);
    p_mag_data = (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::IN_BUFFER_LENGTH);
    p_autocorr_data = (float32_t*) malloc(sizeof(float32_t) * (DSP_CONSTANTS::IN_BUFFER_LENGTH * 2 - 1));
    p_max_val = (float32_t*) malloc(sizeof(float32_t));
    p_idx = (uint32_t*) malloc(sizeof(uint32_t));
}

HYDROPHONES::Hydrophones::~Hydrophones()
{
    /* Deleting the allocated memory */
    free(p_data);
    free(p_mag_data);
    free(p_max_val);
    free(p_idx);
    free(p_autocorr_data);
}

void HYDROPHONES::Hydrophones::analyze_data(float32_t *p_raw_data)
{
    /** 
     * Filters the data using an second-order IIR-filter
     * p_data is set to the filtered data
     */ 
    arm_biquad_cascade_df1_f32(&DSP_CONSTANTS::IIR_FILTER,
            p_raw_data, p_data, DSP_CONSTANTS::IIR_SIZE);

    /* Takes the complex FFT with length 4096 */
    arm_cfft_f32(&arm_cfft_sR_f32_len4096, p_data, 0, 1);

    /* Finds the complex magnitude output */
    arm_cmplx_mag_f32(p_data, p_mag_data, DSP_CONSTANTS::FFT_SIZE);

    /* Calculates the index and value for the maximum value */
    arm_max_f32(p_mag_data, DSP_CONSTANTS::FFT_SIZE, p_max_val, p_idx);

    /* Taking the autocorrelation of the filtered data */
    arm_correlate_f32(p_data, DSP_CONSTANTS::FFT_SIZE, p_data, 
            DSP_CONSTANTS::FFT_SIZE, p_autocorr_data);

    /* Iterating over the autocorrelation to find the lag */
    float32_t max = 0;
    last_lag = 0;
    for(int i = 0; i < 2 * DSP_CONSTANTS::IN_BUFFER_LENGTH - 1; i++){
        if(p_autocorr_data[i] > max){
            max = p_autocorr_data[i];
            last_lag = i;
        }
    }

    /* Calculates the intensity in dB */
    last_intensity = TRILATERATION::estimate_signal_intensity(p_data); 
}


