#include "testing.h"

/* Test-function for memory */
void TESTING::test_memory_leakage(void);


/* Test-function for the filter-coefficients */
void TESTING::test_filter_coefficients(void){
  /* Allocating memory */
  float32_t* p_raw_data = (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::IN_BUFFER_LENGTH);
  float32_t* p_data = (float32_t*) malloc(sizeof(float32_t) * DSP_CONSTANTS::IN_BUFFER_LENGTH);

  /** 
   * Generating raw data
   * Must make sure that we test the filter for the different
   * frequencies that it can experience:
   *    - f < 0.15
   *    - 0.15 <= f <= 0.45
   *    - f > 0.45
   */
  for(int i = 0; i < DSP_CONSTANTS::DMA_BUFFER_LENGTH; i++){
    p_raw_data[i] = 0;
  }

  /* Filtering the data */
  arm_biquad_cascade_df1_f32(&DSP_CONSTANTS::IIR_FILTER,
        p_raw_data, p_data, DSP_CONSTANTS::IIR_SIZE);

  /* Releasing the memory */
  free(p_raw_data);
  free(p_data);
}


