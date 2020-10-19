#include "../include/hydrophones.hpp"

HYDROPHONES::Hydrophones::Hydrophones(TRILITERATION::Pos pos) : 
        pos{pos}, last_lag{0}
{
    // Initialize the data as an array with garbage data 
    data.setlength(DSP::interval_total_len);
}

HYDROPHONES::Hydrophones::~Hydrophones(){}


void HYDROPHONES::Hydrophones::calculate_lag(int* c_arr){
    //Transforms the data from a pointer to a complex_1d_array.
    //Assumes the data to be 
    DSP::transfer_C_arr_to_alglib(c_arr, data);
    
    //Takes the FFT of the data
    alglib::fftc1d(data);

    //Filters the data in SW between min and max freq in DSP
    DSP::freq_filtering(data);

    //Takes the IFFT to get the autocorrelation
    alglib::fftc1dinv(data);

    //Finds the lag
    last_lag = DSP::find_lag(data);
}

