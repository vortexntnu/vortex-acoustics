#include "hydrophones.h"

HYDROPHONES::Hydrophones::Hydrophones(TRILITERATION::Pos pos) : 
        pos{pos}, last_lag{0}
{
    // Initialize the data as an array with garbage data 
    data.setlength(DSP::interval_total_len);
}

HYDROPHONES::Hydrophones::~Hydrophones(){}


void HYDROPHONES::Hydrophones::calculate_lag(uint16_t* c_arr){
    // Transforms the data from a pointer to a complex_1d_array.
    // Assumes the data to be 
    DSP::transfer_C_arr_to_alglib(c_arr, data);
    
    // Takes the FFT of the data
    alglib::fftc1d(data, DSP::interval_total_len);

    // Filters the data in SW between min and max freq in DSP
    DSP::freq_filtering(data);

    // Finds the intensity of the signal
    this->last_intensity = estimate_intensity(data);

    // Takes the IFFT to get the autocorrelation
    alglib::fftc1dinv(data, DSP::interval_total_len);

    // Finds the lag
    this->last_lag = DSP::find_lag(data);

}

double HYDROPHONES::Hydrophones::estimate_intensity(
    const alglib::complex_1d_array& x_arr){
    // Take the integral of the data

    // However, must make sure that we only capture the data 
    // of a given frequency - IDK how

    return -1.0;
}

