/**
 * @file
 * 
 * @brief The main-file is primarely designed to combine
 * the different files and functions at the moment
 * 
 */

#include "../include/hydrophones.hpp"
#include <random>
#include <iostream>


// Get the data from one spesific hydrophone.
// Not implemented, and written here for the thought-process
int* get_c_data(int hyd_x);


// Position of each hydrophone
TRILITERATION::Pos pos_hyd1(0, 0, 0);
TRILITERATION::Pos pos_hyd2(1, 0, 0);
TRILITERATION::Pos pos_hyd3(0, 1, 0);

int main(){
    HYDROPHONES::Hydrophones hyd1(pos_hyd1);
    HYDROPHONES::Hydrophones hyd2(pos_hyd2);
    HYDROPHONES::Hydrophones hyd3(pos_hyd3);

    // Lag from each hydrophone
    int lag_hyd1, lag_hyd2, lag_hyd3;

    // Range-estimate based on some calculation
    double range_es_1, range_es_2, range_es_3;

    while(true){
        // The raw data from the hydrophones
        int* c_data_hyd1 = get_c_data(1);
        int* c_data_hyd2 = get_c_data(2);
        int* c_data_hyd3 = get_c_data(3);

        // Calculating the lag
        hyd1.calculate_lag(c_data_hyd1);
        hyd2.calculate_lag(c_data_hyd2);
        hyd3.calculate_lag(c_data_hyd3);

        lag_hyd1 = hyd1.get_lag();
        lag_hyd2 = hyd2.get_lag();
        lag_hyd3 = hyd3.get_lag();

        // Calculate an estimate for the lag
        range_es_1 = 0; // Update 
        range_es_2 = 0; // Update 
        range_es_3 = 0; // Update

        // Triliterating the possible positions 
        std::pair<TRILITERATION::Pos, TRILITERATION::Pos>
            possible_pos = TRILITERATION::triliterate(
                pos_hyd1, pos_hyd2, pos_hyd3,
                range_es_1, range_es_2, range_es_3);

        // Do somethin with the estimates

        // Send the data to the Xavier to get the possible direction and range
    }

    return 0;
}