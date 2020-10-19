/**
 * @file
 * 
 * @brief File for using the basic I/O-operations
 * with the STM32F767ZI
 * 
 * @warning Not implemented anything as of 19.10.20.
 * More as a thought-process to the future
 * development. 
 */
#ifndef STM32_IO_H
#define STM32_IO_H

#include "../STM32_code/include/setup.h"

/**
 * @brief Function to create random data
 * 
 * @param pin Future reference to the pin-number
 * we would like to generate/read data from
 */
int* create_random_data(int pin, 
        const int num_data_points);


/**
 * @brief Read all of the pins that the hydrophones
 * are connected to, and store the data using int
 * pointers. 
 * 
 * @warning Crucial that each int-pointer has a 
 * fixed length, such that the system does not 
 * use invalid memory.
 * 
 * 
 * @note The pin-numbers could instead be included
 * as a define in the header.  
 */
void read_multiple_pins(int* data_hyd_1_ptr,
        int* data_hyd_2_ptr, int* data_hyd_3_ptr,
        int pin_hyd_1, int pin_hyd_2,
        int pin_hyd_3, int num_data_points); 


#endif //STM32_IO_H

