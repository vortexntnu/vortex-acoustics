/**
 * @file
 * 
 * @brief Small library that implements the 
 * hydrophones for the AUV for Vortex NTNU
 */
#ifndef ACOUSTICS_ANALYZE_DATA_H
#define ACOUSTICS_ANALYZE_DATA_H

#include "trilateration.h"

namespace ANALYZE_DATA{

/**
 * @brief Class for the hydrophones. This allows us to easier analyze
 * the data-points, and access the results from each hydrophone.
 */
class Hydrophones{
private:
    /**
    * @brief The lag calculated for the last sample
    */
    uint32_t last_lag;

    /**
    * @brief The dataset for one hydrophone 
    */
    float32_t* p_data;

    /**
     * @brief The autocorrelated data-sequence
     * 
     * @warning The length of the autocorrelation will be
     * 2 * length(p_data) - 1
     */
    float32_t* p_autocorr_data;
     
public:
    /**
    * @brief Constructor and destructor 
    */
    Hydrophones();
    ~Hydrophones();
    
    /**
    * @brief Function to return the last calculated lag 
    */
    uint32_t get_measured_lag() const { return last_lag; } 

    /**
    * @brief Function to analyze the data
    * 
    * The raw data is filtered using a second-order biquad DF1 
    * IIR filter to eliminate unwanted frequencies. Thereafter
    * the autocorrelation is found and used to calculate the lag, 
    * intensity and distance are thereafter estimated 
    * from the filtered data
    */
    void analyze_hydrophone_data(float32_t* p_raw_data);
}; /* class Hydrophones */

} /* namespace ANALYZE_DATA */

#endif // ACOUSTICS_ANALYZE_DATA_H



