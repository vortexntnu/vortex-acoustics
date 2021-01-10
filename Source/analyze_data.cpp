#include "analyze_data.h"

/**
 * Initializing the extern filter-variables
 */
float32_t state_coefficients[4 * num_stages] = 
{
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

float32_t filter_coefficients[5 * num_stages] =
{
        0.56942484, 0.0, -0.56942484,             /* Numerator filter 1                               */
        -1.12551866, 0.46469620,                  /* Denominator filter 1                             */
        0.56942484, 0.0, -0.56942484,             /* Numerator filter 2                               */
        -0.83226204, 0.3694894                    /* Denominator filter 2                             */
};

const arm_biquad_casd_df1_inst_f32 IIR_FILTER = 
{
    .numStages = num_stages, 
    .pState = &state_coefficients[0],
    .pCoeffs = &filter_coefficients[0]
};


/**
 * Class Hydrophones w/ functions
 */
ANALYZE_DATA::Hydrophones::Hydrophones() : 
    last_lag{0}
{
    /* Initial memory allocation */
    p_data = (float32_t*) malloc(sizeof(float32_t) * IN_BUFFER_LENGTH);
    p_autocorr_data = (float32_t*) malloc(sizeof(float32_t) * (IN_BUFFER_LENGTH * 2 - 1));
}

ANALYZE_DATA::Hydrophones::~Hydrophones()
{
    /* Deleting the allocated memory */
    free(p_data);
    free(p_autocorr_data);
}

void ANALYZE_DATA::Hydrophones::analyze_hydrophone_data(float32_t *p_raw_data)
{
    /** 
     * Filters the data using an second-order IIR-filter
     * p_data is set to the filtered data
     */ 
    arm_biquad_cascade_df1_f32(&IIR_FILTER,
            p_raw_data, p_data, IIR_SIZE);

    /* Taking the autocorrelation of the filtered data */
    arm_correlate_f32(p_data, FFT_SIZE, p_data, 
            FFT_SIZE, p_autocorr_data);

    /* Iterating over the autocorrelation to find the lag */
    float32_t max_val = 0;
    last_lag = 0;
    for(uint i = 0; i < 2 * IN_BUFFER_LENGTH - 1; i++){
        if(p_autocorr_data[i] > max_val){
            max_val = p_autocorr_data[i];
            last_lag = i;
        }
    }
}


