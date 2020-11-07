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
#include "../include/tests.hpp"


/**
 * @brief Namespace to hold the future
 * implementations. It exists mostly for
 * me to think through the process
 */
namespace FUTURE_IMPLEMENTATION{

/**
 * @brief Get the data from one spesific hydrophone.
 * Not implemented, and written here for the 
 * thought-process
 * 
 * @warning Has been mostly eliminated by the 
 * function get_data_all_pins()
 */ 
int* get_c_data(int hyd_x);


/**
 * @brief Read the data from all of the pins. 
 * In other words, read data on pin 1, then pin 2 
 * and then pin 3. Do this until all of the arrays 
 * have been filled.
 * Not implemented. Just for thought-process
 */ 

void get_data_all_pins(int* raw_data_hyd1, 
        int* raw_data_hyd2, int* raw_data_hyd3,
        int sample_frequency, int num_data_points);

/**
 * @brief Function to analyze if the calculated 
 * lags could be valid. Returns a true/false 
 */
bool check_valid_lag(const double& lag1,
        const double& lag2, const double& lag3,
        double time_margin);




}; // FUTURE_IMPLEMENTATION 





/**
 * @brief Namespace of globals used in the file
 * 
 * Might be a bit overkill to have this as a 
 * namespace as well
 */
namespace GLOBALS{

/**
 * @brief Globals that describes the
 * hydrophones position compared to the 
 * center of the AUV
 */
TRILITERATION::Pos pos_hyd1(0, 0, 0);
TRILITERATION::Pos pos_hyd2(1, 0, 0);
TRILITERATION::Pos pos_hyd3(0, 1, 0);


/**
 * @brief Constants that describes the error-
 * margin for the signals to be valid. Will
 * depend on the position and the speed of
 * sound in water
 * 
 * @warning Has not been calculated!
 */
const double time_error = 0;
const int speed_of_sound = 1480;

}; // GLOBALS


/**
 * @brief The main-function for the acoustics
 * 
 * @warning It exists two semi-states for this function.
 * Testing and normal running. If you would like to test
 * something, change the variable tests::testing in tests.hpp
 */
int main(){
    
    if(TESTS::testing){
        TESTS::test_lag_calculation(TESTS::num_nests_const);
        return 0;
    }

    HYDROPHONES::Hydrophones hyd1(GLOBALS::pos_hyd1);
    HYDROPHONES::Hydrophones hyd2(GLOBALS::pos_hyd2);
    HYDROPHONES::Hydrophones hyd3(GLOBALS::pos_hyd3);

    // Lag from each hydrophone
    int lag_hyd1, lag_hyd2, lag_hyd3;

    // Range-estimate based on some calculation
    double range_es_1, range_es_2, range_es_3;

    // Intializing the raw-data-arrays
    int c_data_hyd1[DSP::interval_total_len];
    int c_data_hyd2[DSP::interval_total_len];
    int c_data_hyd3[DSP::interval_total_len];


    /** Should have a function that decleares that the
    * MCU is ready to read data from the pins.
    * May the deleted in the future. Function takes
    * an infinite loop while the MCU sets up.
    * Better to just eliminate this by integrating
    * the functions, but works as a remainder of what
    * to implement in the future
    */
    bool is_ready = false; 
    while(!is_ready){
        is_ready = true;
    }

    while(true){
        // Getting data from the pins
        FUTURE_IMPLEMENTATION::get_data_all_pins(
            c_data_hyd1, c_data_hyd2, c_data_hyd3, 
            DSP::SAMPLE_FREQUENCY, 
            DSP::interval_total_len);

        // Calculating the lag
        hyd1.calculate_lag(c_data_hyd1);
        hyd2.calculate_lag(c_data_hyd2);
        hyd3.calculate_lag(c_data_hyd3);

        lag_hyd1 = hyd1.get_lag();
        lag_hyd2 = hyd2.get_lag();
        lag_hyd3 = hyd3.get_lag();

        if(!FUTURE_IMPLEMENTATION::check_valid_lag(lag_hyd1,
                lag_hyd2, lag_hyd3, GLOBALS::time_error)){
            // The lags are invalid
            // Should make a log-file to measure how often 
            // this happens
            continue;
        }


        // Calculate an estimate for the lag
        range_es_1 = 0; // Update 
        range_es_2 = 0; // Update 
        range_es_3 = 0; // Update

        // Triliterating the possible positions 
        std::pair<TRILITERATION::Pos, TRILITERATION::Pos>
            possible_pos = TRILITERATION::triliterate(
                GLOBALS::pos_hyd1, GLOBALS::pos_hyd2, 
                GLOBALS::pos_hyd3, range_es_1, 
                range_es_2, range_es_3);



        // Do something with the estimates

        // Send the data to the Xavier to get the possible direction and range
    }

    return 0;
}