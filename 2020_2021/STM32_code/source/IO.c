#include "../STM32_code/include/IO.h"


int* create_random_data(int pin, int num_data_points){
    int new_int_ptr[num_data_points];
    for(int i = 0; i < num_data_points; i++){
        new_int_ptr[i] = rand();
    }
    return &new_int_ptr;
}


void read_multiple_pins(int* data_hyd_1_ptr,
        int* data_hyd_2_ptr, int* data_hyd_3_ptr,
        int pin_hyd_1, int pin_hyd_2,
        int pin_hyd_3, int num_data_points); 