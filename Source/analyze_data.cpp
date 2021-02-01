#include "analyze_data.h"

/**
 * Initializing the extern filter-variables. 
 * 
 * See FILTER_SETUP in parameters.h for more information on the variables
 */
float32_t state_coefficients[4 * num_stages] = 
{
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0
};

float32_t filter_coefficients[5 * num_stages] =
{
        0.56942484, 0.0, -0.56942484,             /* Numerator filter 1   { b10, b11, b12 }           */
        -1.12551866, 0.46469620,                  /* Denominator filter 1 {      a11, a12 }           */
        0.56942484, 0.0, -0.56942484,             /* Numerator filter 2   { b20, b21, b22 }           */
        -0.83226204, 0.3694894                    /* Denominator filter 2 {      a21, a22 }           */
};

const arm_biquad_casd_df1_inst_f32 IIR_FILTER = 
{
    .numStages = num_stages, 
    .pState = &state_coefficients[0],
    .pCoeffs = &filter_coefficients[0]
};


/**
 * Functions for analyzing the data
 */
void ANALYZE_DATA::array_max_value(
        float32_t* data_array,
        const uint32_t& array_length,
        uint32_t& idx,
        float32_t& max_val){

    /* Checking if invalid length */
    if(array_length < 0){
        idx = -1;
        max_val = -1;
        return;
    }
    
    /* Initializing values */
    idx = 0;
    max_val = 0;

    /* Iterating over the array */
    for(uint32_t i = 0; i < array_length; i++){
        if(std::abs(data_array[i]) > max_val){
            idx = i;
            max_val = std::abs(data_array[i]);
        }
    }
}



void ANALYZE_DATA::filter_raw_data(
        float32_t* p_raw_data_array[NUM_HYDROPHONES];
        float32_t* p_filtered_data_array[NUM_HYDROPHONES]){
    
    /* Getting the values from the arrays */
    float32_t* p_raw_data_port = p_filtered_data_array[0];
    float32_t* p_raw_data_starboard = p_filtered_data_array[1];
    float32_t* p_raw_data_stern = p_filtered_data_array[2];

    float32_t* p_filtered_data_port = p_filtered_data_array[0];
    float32_t* p_filtered_data_starboard = p_filtered_data_array[1];
    float32_t* p_filtered_data_stern = p_filtered_data_array[2];

    /* Filters the data using an fourth-order IIR-filter */
    arm_biquad_cascade_df1_f32(
            &IIR_FILTER,
            p_raw_data_port, 
            p_filtered_data_port, 
            IIR_SIZE);

    arm_biquad_cascade_df1_f32(
            &IIR_FILTER,
            p_raw_data_starboard, 
            p_filtered_data_starboard, 
            IIR_SIZE);

    arm_biquad_cascade_df1_f32(
            &IIR_FILTER,
            p_raw_data_stern, 
            p_filtered_data_stern, 
            IIR_SIZE);
}


void ANALYZE_DATA::calculate_TDOA_array(
        float32_t* p_filtered_data_array[NUM_HYDROPHONES],
        uint32_t TDOA_array[NUM_HYDROPHONES]){
    
    /* Getting the values from the arrays */
    float32_t* p_filtered_data_port = p_filtered_data_array[0];
    float32_t* p_filtered_data_starboard = p_filtered_data_array[1];
    float32_t* p_filtered_data_stern = p_filtered_data_array[2];

    /* Creating temporary arrays to hold the result */
    float32_t cross_corr_port_starboard[2 * IN_BUFFER_LENGTH - 1];
    float32_t cross_corr_port_stern[2 * IN_BUFFER_LENGTH - 1];
    float32_t cross_corr_starboard_port[2 * IN_BUFFER_LENGTH - 1];

    /* Crosscorrelating the data */
    arm_correlate_f32(
            p_filtered_data_port, IN_BUFFER_LENGTH, 
            p_filtered_data_starboard, IN_BUFFER_LENGTH, 
            cross_corr_port_starboard);

    arm_correlate_f32(
            p_filtered_data_port, IN_BUFFER_LENGTH, 
            p_filtered_data_stern, IN_BUFFER_LENGTH, 
            cross_corr_port_stern);

    arm_correlate_f32(
            p_filtered_data_starboard, IN_BUFFER_LENGTH, 
            p_filtered_data_stern, IN_BUFFER_LENGTH, 
            cross_corr_starboard_stern);

    /* Calculating TDOA */
    uint32_t TDOA_port_starboard;
    uint32_t TDOA_port_stern;
    uint32_t TDOA_starboard_stern;

    float32_t max_val_port_starboard;
    float32_t max_val_port_stern;
    float32_t max_val_starboard_stern;

    ANALYZE_DATA::array_max_value(
            cross_corr_port_starboard,
            2 * IN_BUFFER_LENGTH - 1,
            TDOA_port_starboard,
            max_val_port_starboard);

    ANALYZE_DATA::array_max_value(
            cross_corr_port_stern,
            2 * IN_BUFFER_LENGTH - 1,
            TDOA_port_stern,
            max_val_port_stern);

    ANALYZE_DATA::array_max_value(
            cross_corr_starboard_stern,
            2 * IN_BUFFER_LENGTH - 1,
            TDOA_starboard_stern,
            max_val_starboard_stern);

    /* Inserting calculated TDOA into array */
    TDOA_array[0] = TDOA_port_starboard;
    TDOA_array[1] = TDOA_port_stern;
    TDOA_array[2] = TDOA_starboard_stern;
}


