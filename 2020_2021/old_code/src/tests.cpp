#include "../include/tests.hpp"

int* TESTS::create_random_integers(){
    static int test_array [DSP::interval_total_len];
    for(int i = 0; i < DSP::interval_total_len; i++){
        test_array[i] = rand() % 1024;
    }
    return test_array;
}


void TESTS::test_lag_calculation(const int num_tests){
    // Setting up for the test
    std::vector<double> test_times;
    TRILITERATION::Pos pos_test(0, 0, 0);
    HYDROPHONES::Hydrophones hyd_test(pos_test);

    // Doing the test #num_tests times. This is to make
    // sure that the tests have some statistical strength
    for(int i = 0; i < num_tests; i++){
        try{
            auto start = std::chrono::steady_clock::now();
            int* random_integers = TESTS::create_random_integers();
            hyd_test.calculate_lag(random_integers);
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
    avg /= test_times.size();
    std::cout << "Average test-time for the algoritm. Using " << 
            num_tests << " with a length of " << 
            DSP::interval_total_len << std::endl;
    std::cout << "The testing showed that the average is " <<
            avg << std::endl; 
}


