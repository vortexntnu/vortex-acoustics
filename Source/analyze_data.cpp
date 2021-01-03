#include "analyze_data.h"

void ANALYZE_DATA::analyze_data(float32_t* p_raw_data, float32_t* p_data,
      float32_t* p_autocorr_data, float32_t* p_intensity,
      uint32_t* p_lag)
{
    /** 
     * Filters the data using an second-order IIR-filter
     * p_data is set to the filtered data
     */ 
    arm_biquad_cascade_df1_f32(&DSP_CONSTANTS::IIR_FILTER,
            p_raw_data, p_data, DSP_CONSTANTS::IIR_SIZE);

    /* Takes the complex FFT with length 4096 */
    arm_cfft_f32(&arm_cfft_sR_f32_len4096, p_data, 0, 1);

    /* Taking the autocorrelation of the filtered data */
    arm_correlate_f32(p_data, DSP_CONSTANTS::FFT_SIZE, p_data, 
            DSP_CONSTANTS::FFT_SIZE, p_autocorr_data);

    /* Iterating over the autocorrelation to find the lag */
    float32_t max = 0;
    *p_lag = 0;
    for(int i = 0; i < 2 * DSP_CONSTANTS::IN_BUFFER_LENGTH - 1; i++){
        if(p_autocorr_data[i] > max){
            max = p_autocorr_data[i];
            *p_lag = i;
        }
    }

    /* Calculates the intensity in dB */
    *p_intensity = TRILITERATION::estimate_signal_intensity(p_data); 
}