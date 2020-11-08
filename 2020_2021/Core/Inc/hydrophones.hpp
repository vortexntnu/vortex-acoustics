/**
 * @file
 * 
 * @brief Small library that implements the 
 * hydrophones for the AUV for Vortex NTNU
 * 
 * @note Could be improved by using the pose
 * from ROS. Should be implemented in the 
 * future
 * 
 * @note It is thought that the hydrophones are
 * placed in a form of a triangle. Therefore, we
 * have one on front starboard, one on front port
 * and one on the back in the middle
 * 
 *                    BOW
 *                      
 *      Port                       Starboard
 *                      
 *       HYD ''''''''''''''''''''' HYD
 *           '''''''''''''''''''''
 *           ''''''''' A '''''''''
 *           ''''''''' U '''''''''
 *           ''''''''' V '''''''''
 *           '''''''''''''''''''''
 *           '''''''''''''''''''''
 *           '''''''''HYD'''''''''
 * 
 *                   STERN
 */
#ifndef ACOUSTICS_HYDROPHONES_HPP
#define ACOUSTICS_HYDROPHONES_HPP

#include "../Core/Inc/triliteration.hpp"
#include "../Core/Inc/DSP.hpp"

// Pseudokode to solve the problem
// 1. Read in N number of datapoints to calculate from each hydrophone 
// 2. Transfer those datapoints to a complex_1d_array for an easier way to
//      take the DSP and fourier
// 3. Take the fft for each sample
// 4. BP each sample in SW to get the interesting values
// 5. Take the ifft to get the autocorrelation
// 6. Use the autocorrelation to get the lag
// 7. Calculate the time using the lag between the samples

/*
What is required for each signal: the lag 

To get the lag: requires the autocorrelation 

To get the autocorrelation: takes the IDFT (IFFT) of the frequency

To get the frequency: take the DTFT (FFT) of the sample
*/

namespace HYDROPHONES{

/**
 * @brief Class for the hydrophones. 
 * Combines the basic functions for
 * each hydrophone. Makes it a lot 
 * easier to implement
 * 
 * @warning Must be evaluated if this
 * class is really necessary.
 */
class Hydrophones{
private:
    /*
    * Position of each hydrophone 
    */
    TRILITERATION::Pos pos;

    /*
    * The lag calculated for the last sample
    */
    int last_lag;

    /*
    * The dataset for one hydrophone 
    */
    alglib::complex_1d_array data;

public:
    /*
    * Constructor and destructor 
    */
    Hydrophones(TRILITERATION::Pos pos);
    ~Hydrophones();
    
    /*
    * Function to return the last calculated lag 
    */
    int get_lag() const { return last_lag; } 

    /*
    * Function to return the current data-set 
    */
    alglib::complex_1d_array get_data() { return data; }

    /*
    * Function to calculate everything.
    * 
    * Takes in a pointer to a C-array, calculates the 
    * complex_1d_array, takes the fft and the ifft and
    * then calculates the lag from the autocorrelation. 
    */
    void calculate_lag(int* c_arr);

};

} // HYDROPHONES

#endif // ACOUSTICS_HYDROPHONES_HPP



