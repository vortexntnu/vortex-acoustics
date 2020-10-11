#include "fourier.hpp"

void fourier_fft(CArray& x_arr, bool take_abs_value=false)
{
    const size_t N = x_arr.size();
    if (N <= 1) return;
 
    /*
    * Dividing the array into even and odd parts. 
    * The fourier_fft is called recursivly on each part
    */
    CArray even = x_arr[std::slice(0, N/2, 2)];
    CArray odd = x_arr[std::slice(1, N/2, 2)];

    fourier_fft(even, take_abs_value);
    fourier_fft(odd), take_abs_value;
 
    /*
    * Combining the smaller solutions into a
    * larger solution.
    */
    for (size_t k = 0; k < N/2; k++)
    {
        Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
        Complex val_pos = even[k] + t;
        Complex val_neg = even[k] - t;
        if(take_abs_value){
            val_pos = std::abs(val_pos);
            val_neg = std::abs(val_neg);
        }
        x_arr[k] = val_pos;
        x_arr[k+N/2] = val_neg;
    }
}
 

void fourier_ifft(CArray& x_arr)
{
    /**
     * IDFT is done by taking the DFT of
     * the conjugated values
     */
    x_arr = x_arr.apply(std::conj);
    fourier_fft(x_arr, false);
 
    /**
     * Conjugating the values back again,
     * before normalizing the values.
     */
    x_arr = x_arr.apply(std::conj);
    x_arr /= x_arr.size();
}
