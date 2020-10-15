#include "../include/fourier.hpp"

void FOURIER::fft(CArray& x_arr, bool take_abs_value=false)
{
    const size_t N = x_arr.size();
    if (N <= 1) return;
 
    /*
    * Dividing the array into even and odd parts. 
    * The fft is called recursivly on each part
    */
    CArray even = x_arr[std::slice(0, N/2, 2)];
    CArray odd = x_arr[std::slice(1, N/2, 2)];

    FOURIER::fft(even, take_abs_value);
    FOURIER::fft(odd), take_abs_value;
 
    /*
    * Combining the smaller solutions into a
    * larger solution.
    */
    for (size_t k = 0; k < N/2; k++)
    {
        FOURIER::Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        FOURIER::Complex val_pos = even[k] + t;
        FOURIER::Complex val_neg = even[k] - t;
        if(take_abs_value){
            val_pos = std::abs(val_pos);
            val_neg = std::abs(val_neg);
        }
        x_arr[k] = val_pos;
        x_arr[k+N/2] = val_neg;
    }
}
 

void FOURIER::ifft(CArray& x_arr)
{
    /**
     * IDFT is done by taking the DFT of
     * the conjugated values
     */
    x_arr = x_arr.apply(std::conj);
    FOURIER::fft(x_arr, false);
 
    /**
     * Conjugating the values back again,
     * before normalizing the values.
     */
    x_arr = x_arr.apply(std::conj);
    x_arr /= x_arr.size();
}


FOURIER::Complex sum_complex_arr(const FOURIER::CArray& x_arr){
    const int N = x_arr.size();
    FOURIER::Complex sum;
    for(int i = 0; i < N; i++){
        sum += x_arr[i];
    }
    return sum;
}


FOURIER::CArray FOURIER::freq_estimation(const FOURIER::CArray& x_arr){
    /**
     * values: an array of double that holds the frequency
     *          response for all of the arrays
     * N: the size of the x_arr 
     */
    double values[num_freq_intervals];
    const int N = x_arr.size();

    for(int i = 0; i < num_freq_intervals; i++){
        FOURIER::CArray temp_complex_array (len_freq_intervals);

        for(int k = 0; k < len_freq_intervals; k++){
            FOURIER::Complex fourier;       //Need to find a way to define
                                            //this as a complex value e = jw.
                                            //This will depend entirely on the
                                            //frequency w

            FOURIER::Complex temp_complex_val = x_arr[i * num_freq_intervals 
                        + k] * fourier;
            
            temp_complex_array[k] = temp_complex_val;   
        }
        values[i] = (std::pow(abs(sum_complex_arr(temp_complex_array)), 2)) 
                    / len_freq_intervals; 
    }

    double sum = 0;
    for(int i = 0; i < num_freq_intervals; i++){
        sum += values[i];
    }
    sum /= num_freq_intervals;

    //Problems with this algoritm:
        //How the fuck to convert this to an array
                //because it requires an array to take
                //the ifft later to find the lag

    return *(new FOURIER::CArray (N)); //Update this when it is determined 
                                        //how to calculate the CArray 
}



