#include <vector>
#include <algorithm>
#include <arm_math.h>
#include "corelation.h"
#include <Arduino.h>




namespace Multilateration{
// Custom correlation with 64-bit accumulation to avoid overflow.

// signal1 and signal2 are arrays of length n.

// 'result' must have space for (2*n - 1) elements.

// simple correlation
void crosscorelation(const int* signal1, const int* signal2, int size, int* result) {
    // Use convolution to corelate the signals
    // This is because all signals are LTI system
    // This makes it possible that corelation and convolution are the same
    // Convolution is a lot easier to do than corelation itself
    // Works the same tested :)
    int outputLength = 2 * size - 1;
    for (int lag = 0; lag < outputLength; lag++) {
        int64_t sum = 0;
        for (int i = 0; i < size; i++) {
            int idx = lag + i;
            int val = 0;
            // Zero padding: valid indices for signal2 are [n-1, n-1+n-1]
            if (idx > size && idx < 2*size) {
                int j = idx - size;
                val = signal2[j];
            }
            sum += (int64_t)signal1[i] * val;
        }

    // Divide by 1000 with rounding (add half of 1000) to minimize precision loss
    result[lag] = (int)((sum + 500) / 1000);
  }
}
 


int find_peak_index(int32_t* signal, int size) {
    // If the signal is empty, return 0 
    if (size == 0) {
        return 0;
    }

    int peakIndex = 0;
    int peakValue = signal[0];

    // Loop through the signal starting from the first element.
    for (int i = 1; i < size; ++i) {
        if (signal[i] > peakValue) {
            peakValue = signal[i];
            peakIndex = i;
        }
    }
    return peakIndex;
}






// Compute a cross-correlation lag between two signals using a brute-force approach.
void crosscorelation_2(const int* x, const int* y, const int size, int32_t* result){

    
    int output_size = 2*size-1;

    // makes a new arrays with twise-1 the length of the original for correlation.
    int long_x[output_size] = {0}; 
    int long_y[output_size] = {0};

    for (int i = 0; i < size; i++) {
        long_x[i] = x[i];
        long_y[size + i-1] = y[i];
    }

    

    
    for (int i = 0; i < (output_size); i++){
        result[i] = 0; // To make it ready for input

        // this shifts every element in x one space to the right starting from the back
        for (int j = output_size-1; j > 0; j--){
            long_x[j] = long_x[j - 1];
        }
        long_x[0] = 0;



        for (int k = 0; k < output_size; k++){
            result[i] += long_x[k]*long_y[k];
        }
    }
             
        
}





// Converts int arrays to float32_t, to do arm correlation the only correlation part is this arm_correlate_f32(pSrcA, srcALen, pSrcB, srcBLen, pDst);
void arm_correlation(const int *pSrcA_int, uint32_t srcALen,
                     const int *pSrcB_int, uint32_t srcBLen,
                     int32_t *pDst_int)
{
    // Allocate temporary float arrays for inputs.
    float32_t *pSrcA = (float32_t *) malloc(srcALen * sizeof(float32_t));
    float32_t *pSrcB = (float32_t *) malloc(srcBLen * sizeof(float32_t));
    if (pSrcA == NULL || pSrcB == NULL) {
        if (pSrcA) free(pSrcA);
        if (pSrcB) free(pSrcB);
        return;
    }

    // Convert the integer input arrays to float arrays.
    for (uint32_t i = 0; i < srcALen; i++) {
        pSrcA[i] = (float32_t)pSrcA_int[i];
    }
    for (uint32_t i = 0; i < srcBLen; i++) {
        pSrcB[i] = (float32_t)pSrcB_int[i];
    }

    // Output length for correlation
    uint32_t outLen = srcALen + srcBLen - 1;
    float32_t *pDst = (float32_t *) malloc(outLen * sizeof(float32_t));
    if (pDst == NULL) {
        free(pSrcA);
        free(pSrcB);
        return;
    }

    // Compute correlation using the CMSIS-DSP function:
    arm_correlate_f32(pSrcA, srcALen, pSrcB, srcBLen, pDst);

    // Determine the maximum absolute value from the correlation output.
    // This is used to compute the scaling factor.
    float32_t maxVal = 0.0f;
    for (uint32_t i = 0; i < outLen; i++) {
        float32_t absVal = fabsf(pDst[i]);
        if (absVal > maxVal) {
            maxVal = absVal;
        }
    }

    // If maxVal is zero then no scaling is needed.
    if (maxVal == 0.0f) {
        maxVal = 1.0f;
    }

    // Calculate the scale factor so that the maximum value maps to INT32_MAX.
    // INT32_MAX is 2147483647.
    float32_t scale = 2147483647.0f / maxVal;

    // Convert the scaled float correlation result to int32_t.
    // This step prevents overflow/clipping by ensuring the scaled values fit within int32_t.
    for (uint32_t i = 0; i < outLen; i++) {
        pDst_int[i] = (int32_t)(pDst[i] * scale);
    }

    // Clean up: free the dynamically allocated memory.
    free(pDst);
    free(pSrcA);
    free(pSrcB);
}



}