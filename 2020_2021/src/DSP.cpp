#include "../include/DSP.hpp"
 
double DSP::correlation_coefficient(int x_arr[], int y_arr[], int n) 
{ 
    int sum_x = 0, sum_y = 0, sum_xy = 0; 
    int square_sum_x = 0, square_sum_y = 0; 
  
    /**
     * Summing each variable that is present
     * in the correlation of two arrays, before
     * the correlation-coefficient is 
     * calculated.  
     */
    for (int i = 0; i < n; i++) 
    { 
        sum_x = sum_x + x_arr[i]; 
        sum_y = sum_y + y_arr[i]; 
  
        sum_xy = sum_xy + x_arr[i] * y_arr[i]; 

        square_sum_x = square_sum_x + x_arr[i] * x_arr[i]; 
        square_sum_y = square_sum_y + y_arr[i] * y_arr[i]; 
    } 
   
    double corr = (double)(n * sum_xy - sum_x * sum_y)  
                  / sqrt((n * square_sum_x - sum_x * sum_x)  
                      * (n * square_sum_y - sum_y * sum_y)); 
  
    return corr; 
} 



int DSP::find_lag(const FOURIER::CArray& x_arr){
    // N: size of the array
    // max_val: maximum value in x_arr
    // max_idx: idx of maximum value in x_arr   
    const int N = x_arr.size();
    double max_val = 0;
    int max_idx = 0;

    // Could be a potential problem by using the
    // absolute value.
    for(int i = 0; i < N; i++){
        //FOURIER::Complex temp = x_arr[i];
        double temp_abs = abs(x_arr[i]);
        if(temp_abs >= max_val){
            max_val = temp_abs;
            max_idx = i;
        }
    }
    return max_idx;
}


void DSP::freq_filtering(FOURIER::CArray& x_arr){
    const int N = x_arr.size();
    for(int i = 0; i < N; i++){
        if(abs(x_arr[i]) < DSP::MIN_FREQUENCY || abs(x_arr[i]) >  MAX_FREQUENCY){
            x_arr[i] = FOURIER::Complex(0, 0);
        }
    }
}
