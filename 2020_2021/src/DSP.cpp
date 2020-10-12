#include "../include/DSP.hpp"
 
double correlation_coefficient(int x_arr[], int y_arr[], int n) 
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