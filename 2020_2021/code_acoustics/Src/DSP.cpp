#include "DSP.h"


double DSP::calculate_abs(const alglib::complex num){
    return sqrt(pow(num.x, 2) + pow(num.y, 2));
}


uint32_t DSP::find_lag(const alglib::complex_1d_array& x_arr){
    // N: size of the array
    // max_val: maximum value in x_arr
    // max_idx: idx of maximum value in x_arr   
    const uint32_t N = x_arr.length();
    double max_val = 0;
    uint32_t max_idx = 0;

    for(uint32_t i = 0; i < N; i++){
        double temp_abs = DSP::calculate_abs(x_arr[i]);
        if(temp_abs >= max_val){
            max_val = temp_abs;
            max_idx = i;
        }
    }
    return max_idx;
}


void DSP::freq_filtering(alglib::complex_1d_array& x_arr){
    /**
     * @warning Haven't found a good way to implement this
     * yet. The problem is that I've relied heavily on 
     * alglib's library, which uses a special class for
     * complex values and complex arrays. It makes it much
     * safer to work with, however normal filters like 
     * butterworth would not work with it.
     * 
     * Using a bit of if/else-hell to gradually reduce the
     * gain of the values. 
     * 
     * f \in [10, 50] kHz => K = 1
     * f \in [5, 10] kHz or [50, 55] kHz => K = 1/2
     * f \leq 5 kHZ or f \geq 55 kHz => K = 0   
     */

    const uint32_t N = x_arr.length();
    for(uint32_t i = 0; i < N; i++){
        double abs_val = DSP::calculate_abs(x_arr[i]);
        /*if(abs_val < DSP::MIN_FREQUENCY || 
                    abs_val > DSP::MAX_FREQUENCY){
            x_arr[i] = alglib::complex(0);
        }*/
        if(abs_val < (DSP::MIN_FREQUENCY / 2) ||
                abs_val > (DSP::MAX_FREQUENCY + 
                DSP::MIN_FREQUENCY / 2)){
            x_arr[i] = alglib::complex(0);
        }
        else if((abs_val >= (DSP::MIN_FREQUENCY / 2) &&
                abs_val < (DSP::MIN_FREQUENCY)) ||
                (abs_val > (DSP::MAX_FREQUENCY) && abs_val
                <= (DSP::MAX_FREQUENCY + 
                DSP::MIN_FREQUENCY / 2))){
            x_arr[i] = alglib::complex(1/2*x_arr[i].x, 
                1/2*x_arr[i].y);
        }
    }
}


void DSP::transfer_C_arr_to_alglib(uint16_t* c_arr, 
        alglib::complex_1d_array& x_arr){
    for(uint32_t i = 0; i < DSP::interval_total_len; i++){
        x_arr[i] = alglib::complex(*c_arr);
        c_arr++;
    }
}


bool TRILITERATION::check_valid_signals(uint32_t time_port, 
        uint32_t time_starboard, uint32_t time_stern){
    return true;
}





