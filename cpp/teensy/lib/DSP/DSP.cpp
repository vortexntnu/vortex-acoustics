/*
Code for digital signal processing
Processes raw sound data sampled in a way that can be used further down the line
*/

#include <DSP.h>

// Making sure the inside functions are seen
q15_t q15_divide(q15_t a, q15_t b);
q15_t q15_taylor_atan(q15_t x);

// We do not care about frequencies up to 510k Hz, so we define a variable for
// indexes of indexes, go to the h file
const q15_t samplesOfInterest = FREQUENCY_LIMIT * SAMPLE_LENGTH / SAMPLE_RATE;
const int fOrder = 9;

/*
Coefficients for filter found at https://www.meme.net.au/butterworth.html,
put 9th order filter, 510kHz sampling rate and 50kHz cut-off
*/
const float32_t aFilterCoeffs[fOrder] = {5.4569203401896500, -13.7047980216478000, 20.6476635308150000, -20.4748421533297000, 13.8143215886326000, -6.3261752484730100, 1.8924462642157100, -0.3350397779275800, 0.0267111235596287};
const float32_t bFilterCoeffs[fOrder + 1] = {0.00000545381633879714, 0.00004908434704917420, 0.00019633738819669700, 0.00045812057245895900, 0.00068718085868843900, 0.00068718085868843900, 0.00045812057245895900, 0.00019633738819669700, 0.00004908434704917420, 0.00000545381633879714};

/*
Bit reversing is applied in a lot of FFT
algorithms for increase efficiency.
*/
const uint32_t doBitReverse = 1;

// Constants in q_15 format done right
const q15_t PI_q15 = (q15_t)(PI * (1 << 15) + 0.5);

q15_t* filter_butterwort_9th_order_50kHz(int16_t* samplesRaw) {
    Serial.println("TestFilter");
    // Create array to store the filtered samples
    static q15_t samples[SAMPLE_LENGTH];

    /*
    Implement Butterwort filter of "fOrder"
    y = (a_1 * y_1 + .... + a_n * y_n) + (b_1 * x_1 + ... b_m * x_m)
    Se Wiki:
    http://vortex.a2hosted.com/index.php/Acoustics_Digital_Signal_Processing_(DSP)
    Se source: https://www.meme.net.au/butterworth.html
    */

    /*
    Iterate through each index of the raw samples, and apply filtering to
    them. Starting at fOrder because we can't use an index outside of the
    samples array.
    */
    for (int i = fOrder; i < SAMPLE_LENGTH; i++) {
        float32_t output_influence = 0;
        /* We iterate through the previous filtered samples for the
        filtering, as it is more clean and convenient. */
        for (int k = 0; k < fOrder; k++) {
            output_influence += aFilterCoeffs[k] * samples[i - (k + 1)];
        }

        float32_t input_influence = 0;
        /* We iterate through the previous unfilteredsamples for the
        filtering, as it is more clean and convenient.*/
        for (int k = 0; k < fOrder + 1; k++) {
            input_influence += bFilterCoeffs[k] * samplesRaw[i - k];
        }

        float influenceTotalFloat = output_influence + input_influence;

        // Convert float to q15 datatype in the correct way
        q15_t influenceTotalQ15 = (q15_t)influenceTotalFloat;
        samples[i] = influenceTotalQ15;
    }
    return samples;
}

/*
Instead of taking the full FFT in a signle function we split it
We calculating first the raw values out of FFT witch are "Real" and "Imaginary"
values these values are really interesting since this raw format can be used to
calculate both amplitude, frequencies and phase shift of a signal
*/
q15_t* FFT_raw(q15_t* samples) {
    /*
    To store the results of fft with
    complex numbers, need to have double the
    size of the sample length
    z = a + bi, (a1, b1, a2, b2, a3, b3 ... )
    */
    static q15_t resultsRaw[2 * SAMPLE_LENGTH];

    /* Forward transform, which is what we want,
    we want to go from time to frequency domain.*/
    uint32_t ifftFlag = 0;

    arm_rfft_instance_q15 fftInstance; // Must exist, nothing to say.

    // Initialize the rfft
    arm_rfft_init_q15(&fftInstance, SAMPLE_LENGTH, ifftFlag, doBitReverse);

    // Scale the samples for better contrasts.
    arm_scale_q15(samples, SCALE_FACTOR, BITSHiFT, samples, SAMPLE_LENGTH);

    // The FFT itself, output is the FFT complex array
    arm_rfft_q15(&fftInstance, samples, resultsRaw);

    return resultsRaw;
}

q15_t* FFT_mag(q15_t* resultsRaw) {
    /*
    Create an empty array to store the magnitude
    calculations of the FFT.
    As we are not dealing with complex numbers
    anymore, it is the size of the sample length
    */
    static q15_t results[SAMPLE_LENGTH];

    // Converts the complex array into a magnitude array.
    arm_cmplx_mag_q15(resultsRaw, results, SAMPLE_LENGTH);

    return results;
}

/*
We will be returning q31_t datatypes as
the frequency numbers become too great to
handle for q15_t
Here we take inn FFT response and return the peaks we find
we return the peaks:
    - Amplitude
    - Frequency
    - Phase shift
*/
q31_t** peak_detection(q15_t* resultsRaw, q15_t* results, q15_t threshold) {
    // Dynamically allocate the 2D array
    q31_t** peaks = new q31_t*[SAMPLE_LENGTH];
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        peaks[i] = new q31_t[2]; // We make a 2 dimensional array
    }

    /*
    Once we allocated the memory to the 2d array, the memory that we have
    allocated was already in use, so it had values from before, from other
    tasks. In order to have a clean slate we must iterate through the array to
    make sure we don't read wrong values at the end
    */

    /*
    Fill the array with 0s, this is important so we don't end up
    having memory that is filled with other stuff
    */
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        peaks[i][0] = 0;
        peaks[i][1] = 0;
    }

    for (int i = 1; i < SAMPLE_LENGTH - 2; i++) {
        // Check if a sample is greater than both of its neighboring samples.
        if (results[i] >= results[i - 1] && results[i] >= results[i + 1]) {
            peaks[i][0] = results[i];
            // Fill the array with the frequency and attached magnitude
            peaks[i][1] = i * SAMPLE_RATE / SAMPLE_LENGTH;
        }
    }

    /*
    We make an array with the results sorted for use in the median
    calculation. A big part of the 510k Hz zone is either 0 or very low, so not
    only is is performance wise wasteful, it is also for calculating a proper
    median
    */
    q15_t resultsSort[samplesOfInterest];

    // Sorting algorithm
    for (int i = 0; i < samplesOfInterest; i++) {
        resultsSort[i] = results[i];
    }

    for (int i = 1; i < samplesOfInterest; i++) {
        int key = resultsSort[i];
        int j = i - 1;

        while (j >= 0 && resultsSort[j] > key) {
            resultsSort[j + 1] = resultsSort[j];
            j--;
        }

        resultsSort[j + 1] = key;
    }

    /*
    As we may easily have an even number
    of results, we calculate the median using the average of the two middle
    values. We are aware the formatting is wonky, but that's how it
    has to be
    */
    q15_t avgMedian = (resultsSort[(samplesOfInterest / 2) - 1] + resultsSort[samplesOfInterest / 2]) / 2;
    avgMedian += threshold;

    /*
    The next section is nothing short of a crime against all those who want
    optimized code, we sincerely apologize for leaving you with this The
    following code is so that we return a 2D array only the size of the number
    of peaks we actually have
    */
    int numNotNullValuesInList = 0;
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        /*
        We filter by the median*3 because it works, the x3
        is there just because mediean is too small to filter
        out all the "false" peaks
        */
        if (peaks[i][0] <= avgMedian * 3) {
            // if the magnitue does not pass the threshold we bring the entire
            // index to 0
            peaks[i][0] = 0;
            peaks[i][1] = 0;
        } else {
            // If it passes we count it towards the number of peaks we have
            numNotNullValuesInList++;
        }
    }
    /*
    This is something that needs to be resolved ASAP, we are putting in the
    length of the array in the very front of the array This is because q31_t
    does not work seem well with the sizeof() function for arrays. This is why
    the array has 1 extra index
    */
    numNotNullValuesInList++;

    /*
    Dynamically make a new array for the peaks we
    actually want to return, notice the use of nomNonNull
    */
    q31_t** peaksReturn = new q31_t*[numNotNullValuesInList];
    for (int i = 0; i < numNotNullValuesInList; i++) {
        peaksReturn[i] = new q31_t[3];
    }

    /*
    We send the length of the array through the 1st index of the array. It is
    very important to find another solution. Maybe look into using the uint16_t
    datatype
    */
    peaksReturn[0][0] = numNotNullValuesInList;
    peaksReturn[0][1] = numNotNullValuesInList;
    peaksReturn[0][2] = numNotNullValuesInList;

    // Start with 1 because length of the list is in the first element
    // Again, we need to find a better way to return length of the array then
    // putting it in the first element of the array :/
    int tempCount = 1;

    q15_t real = 0;
    q15_t imag = 0;
    q15_t phase = 0;

    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        /*
        We already filtered out non peak values
        So all values will be 0 except the peaks that are left unchanged

        if (peaks[i][0] > avgMedian * 2) {
        */
        if (peaks[i][0] > 0) {
            // Fill the return array with the peaks.
            peaksReturn[tempCount][0] = peaks[i][0];
            peaksReturn[tempCount][1] = peaks[i][1];

            /*
            Calculate phase shift of the peak frequency

            Since we took in the raw values of the FFT
            We now know the:
            Real      -> x-axis
            Imaginary -> y-axis
            arctan(y/x) = angle
            angle = phase shift
            */
            real = resultsRaw[i * 2];
            imag = resultsRaw[i * 2 + 1];

            /*
            First we check if our Real value is 0 to insure we dont divide by 0
            If real is 0 we know that angle must be either 0 or +-pi/2
            */
            if ((real == 0) && (imag == 0)) {
                phase = 0;
            } else if ((real == 0) && (imag > 0)) {
                phase = q15_divide(PI_q15, 2);
            } else if ((real == 0) && (imag < 0)) {
                phase = -q15_divide(PI_q15, 2);
            } else {
                phase = q15_taylor_atan(q15_divide(imag, real));
            }

            peaksReturn[tempCount][2] = phase;

            /*
            As our first index is reserved for the length of the array
            we use tempCount, which is 1 ahead of i, instead of i
            */
            tempCount++;
        }
    }

    return peaksReturn;
}

float32_t phaseQ31_to_radianFloat32(q31_t phaseQ15) {
    float32_t pi = 3.141592653589793f;
    float32_t conversionRatio = 32768.0f;

    return (phaseQ15 / conversionRatio) * pi;
}

// Function for inside use only
// ==================================================
q15_t q15_divide(q15_t a, q15_t b) {
    // Cast the dividend and divisor to int32_t to avoid overflow
    int32_t a_scaled = (int32_t)a;
    int32_t b_scaled = (int32_t)b;

    // Scale the dividend by the appropriate factor (2^15 for q15_t)
    a_scaled <<= 15;

    // Perform the integer division
    int32_t result = a_scaled / b_scaled;

    // Cast the result back to q15_t, if necessary
    return (q15_t)result;
}

/*
Since the CMSIS ARM libary uses FIXED pointer arithmetic we cant use
conventional means Normal Arduino math uses FLOAT pointer arithmetic which are
slower and not compatible with CMSIS q_15 data type This is why we make a FIXED
pointer arithmetic function to do "arctan" to get angle This method of arctan is
a aproximation algorithm using taylor series

Check wiki for more info:
https://proofwiki.org/wiki/Power_Series_Expansion_for_Real_Arctangent_Function
*/
q15_t q15_taylor_atan(q15_t x) {
    // Change this variable, the bigger the more accurate the value but the
    // longer computational time required
    int TAYLOR_SERIES_TERMS = 10; //

    // Variables
    q15_t x_squared, term, result;
    int32_t temp;

    x_squared = (q15_t)(((int32_t)x * x) >> 15);
    term = x;
    result = x;

    for (int i = 1; i < TAYLOR_SERIES_TERMS; i++) {
        term = (q15_t)((((int32_t)term * x_squared) + (1 << 14)) >> 15);
        temp = (int32_t)term / ((2 * i + 1) << 15);
        if (i % 2 == 0) {
            result += temp;
        } else {
            result -= temp;
        }
    }

    return result;
}
