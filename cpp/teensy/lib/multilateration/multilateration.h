
#include "arm_math.h"

#ifndef HYDROPHONE_DETAILES
#define HYDROPHONE_DETAILES

    #define NUM_HYDROPHONES 5

    #define HYD_0_X_POS -0.11
    #define HYD_0_Y_POS 0.31
    #define HYD_0_Z_POS 0.10
    
    #define HYD_1_X_POS 0.11
    #define HYD_1_Y_POS 0.31
    #define HYD_1_Z_POS 0.10

    #define HYD_2_X_POS 0.0
    #define HYD_2_Y_POS -0.24
    #define HYD_2_Z_POS 0.0

    #define HYD_3_X_POS 0.5
    #define HYD_3_Y_POS -0.1
    #define HYD_3_Z_POS 0.4

    #define HYD_4_X_POS 0.4
    #define HYD_4_Y_POS 0.0
    #define HYD_4_Z_POS 0.4

#endif

#ifndef PHYSICAL_CONSTANTS
#define PHYSICAL_CONSTANTS

  #define SOUND_SPEED         1480.0         

#endif

#ifndef DSP_CONSTANTS
#define DSP_CONSTANTS

  #define SAMPLING_FREQ         300000 //[Hz]
  #define SIGNAL_LENGTH         40*SAMPLING_FREQ //[sampels]      

#endif

float32_t calculatePingerPosition(int32_t tdoaArray[]); 

float32_t** init_A_matrix(); 

float32_t* init_B_matrix(); 

void compute_A(int32_t tdoaArray[], float32_t** A); 
void compute_B(int32_t tdoaArray[], float32_t* B); 

//mabye an init func                                                                      