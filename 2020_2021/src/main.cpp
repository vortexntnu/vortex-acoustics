/**
 * @file
 * 
 * @brief The main-file is primarely designed to combine
 * the different files and functions at the moment.
 * 
 * As of 05.11.2020, the code is being developed into
 * testing. This is to make sure that there are no errors
 * occuring.
 */

#include "../include/hydrophones.hpp"
#include <random>
#include <iostream>
#include <vector>
#include <cassert>
#include <chrono>



// Function for creating a full test-array
// Length of the array is given by 
// DSP::interval_total_len
int* create_random_integers(){
    static int test_array [DSP::interval_total_len];
    for(int i = 0; i < DSP::interval_total_len; i++){
        test_array[i] = rand() % 1024;
    }
    return test_array;
}


// Bool-parameter to check if the system should
// be testing something
const bool testing true

// Parameter to determine the number of tests
// to be performed
const int num_nests_const = 100;


// Function for testing the speed and functionality
// of the code. Runs the FFT and IFFT 100 times and
// calculates the time used. This could be used to 
// evaluate the efficientcy of the C++-code, and
// later determine if pherhaps python is a better
// language 
void test_drive_time(int num_tests){
    // Setting up for the test
    std::vector<double> test_times;
    TRILITERATION::Pos pos_test(0, 0, 0);
    HYDROPHONES::Hydrophones hyd_test(pos_test);

    // Doing the test @num_tests times. This is to make
    // sure that the tests have some statistical strength
    for(int i = 0; i < num_tests; i++){
        try{
            auto start = std::chrono::steady_clock::now();
            int* random_integers = create_random_integers();
            int lag = hyd_test.get_lag(random_integers);
            auto end = std::chrono::steady_clock::now();
            std::chrono::duration<double> diff = end - start;
            test_times.push_back(diff.count());
        }
        catch(...){
            std::cout << "Something went wrong for test number " 
                << i << std::endl; 
        }
    }

    // Calculating the average value 
    double avg = 0;
    for(int i = 0; i < num_tests; i++){
        avg += test_times[i];
    }
    avg /= test_times;
    std::cout << "Average test-time for the algoritm. Using " << 
            num_tests << " with a length of " << 
            DSP::interval_total_len << std::endl;
    std::cout << "The testing showed that the average is " <<
            avg << std::endl; 
}




// Get the data from one spesific hydrophone.
// Not implemented, and written here for the thought-process
int* get_c_data(int hyd_x);


// Position of each hydrophone
TRILITERATION::Pos pos_hyd1(0, 0, 0);
TRILITERATION::Pos pos_hyd2(1, 0, 0);
TRILITERATION::Pos pos_hyd3(0, 1, 0);

int main(){
    if(testing){
        test_drive_time(num_nests_const)
        return 0;
    }

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