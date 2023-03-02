#include <DSP.h>
const q15_t samplesOfInterest =
    FREQUENCY_LIMIT * SAMPLE_LENGTH /
    SAMPLE_RATE; // We do not care about frequencies up to 510k Hz, so we define
                 // a variable for indexes of indexes, go to the h file

q15_t* filterButterWorth9thOrder50kHz(int16_t* samplesRaw) {
    int fOrder = 9;
    static q15_t
        samples[SAMPLE_LENGTH]; // Create array to store the filtered samples

    // Coefficients found at https://www.meme.net.au/butterworth.html, put 9th
    // order filter, 510kHz sampling rate and 50kHz cut-off
    float32_t aFilterCoeffs[fOrder] = {
        5.4569203401896500,   -13.7047980216478000, 20.6476635308150000,
        -20.4748421533297000, 13.8143215886326000,  -6.3261752484730100,
        1.8924462642157100,   -0.3350397779275800,  0.0267111235596287};

    float32_t bFilterCoeffs[fOrder + 1] = {
        0.00000545381633879714, 0.00004908434704917420, 0.00019633738819669700,
        0.00045812057245895900, 0.00068718085868843900, 0.00068718085868843900,
        0.00045812057245895900, 0.00019633738819669700, 0.00004908434704917420,
        0.00000545381633879714};

    /*
      Implement Buterwirth filter of "fOrder"
      y = (a_1 * y_1 + .... + a_n * y_n) + (b_1 * x_1 + ... b_m * x_m)
      Se Wiki: [Wiki Page]
      Se source: https://www.meme.net.au/butterworth.html
    */

    // Iterate through each index of the raw samples, and apply filtering to
    // them. Starting at fOrder because we can't use an index outside of the
    // samples array.
    for (int i = fOrder; i < SAMPLE_LENGTH; i++) {
        float32_t output_influence = 0;
        for (int k = 0; k < fOrder;
             k++) // We iterate through the previous filtered samples for the
                  // filtering, as it is more clean and convenient.
        {
            output_influence += aFilterCoeffs[k] * samples[i - (k + 1)];
        }

        float32_t input_influence = 0;
        for (int k = 0; k < fOrder + 1;
             k++) // We iterate through the previous unfilteredsamples for the
                  // filtering, as it is more clean and convenient.
        {
            input_influence += bFilterCoeffs[k] * samplesRaw[i - k];
        }

        samples[i] =
            (q15_t)(output_influence +
                    input_influence); // Add together the influences at the end.
    }
    return samples;
}

q15_t* magFFT(q15_t* samples) {

    int16_t
        resultsRaw[2 * SAMPLE_LENGTH]; // To store the results of fft with
                                       // complex numbers, need to have double
                                       // the size of the sample length z = a +
                                       // bi, {a1, b1, a2, b2, a3, b3 ... }
    uint32_t ifftFlag = 0; // Forward transform, which is what we want, we want
                           // to go from time to frequency domain.
    uint32_t doBitReverse = 1; // Bit reversing is applied in a lot of FFT
                               // algorithms for increase efficiency.
    arm_rfft_instance_q15 fftInstance; // Must exist, nothing to say.
    static q15_t
        results[SAMPLE_LENGTH]; // Create an empty array to store the magnitue
                                // calculations of the FFT. As we are not
                                // dealing with complex numbers anymore, it is
                                // the size of the sample length

    arm_rfft_init_q15(&fftInstance, SAMPLE_LENGTH, ifftFlag,
                      doBitReverse); // Initialize the rfft

    arm_scale_q15(samples, SCALE_FACTOR, BITSHiFT, samples,
                  SAMPLE_LENGTH); // Scale the samples for better contrasts.
    arm_rfft_q15(&fftInstance, samples,
                 resultsRaw); // The FFT itself, output is the FFT complex array

    arm_cmplx_mag_q15(
        resultsRaw, results,
        SAMPLE_LENGTH); // Converts the complex array into a magnitude array.

    return results;
}

q31_t**
peakDetection(q15_t* results,
              int16_t countTest) { // We will be returning q31_t datatypes as
                                   // the frequency numbers become too great to
                                   // handle for q15_t

    // Dynamically allocate the 2D array
    q31_t** peaks = new q31_t*[SAMPLE_LENGTH];
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        peaks[i] = new q31_t[2]; // We make a 2 dimensional array
    }

    // Once we allocated the memory to the 2d array, the memory that we have
    // allocated was already in use, so it had values from before, from other
    // tasks. In order to have a clean slate we must iterate through the array
    // to make sure we don't read wrong values at the end.
    for (int i = 0; i < SAMPLE_LENGTH;
         i++) { // Fill the array with 0s, this is important so we don't end up
                // having memory that is filled with other stuff.
        peaks[i][0] = 0;
        peaks[i][1] = 0;
    }

    for (int i = 1; i < SAMPLE_LENGTH - 2; i++) {
        if (results[i] >= results[i - 1] &&
            results[i] >= results[i + 1]) { // Check if a sample is greater than
                                            // both of its neighboring samples.
            peaks[i][0] = i * SAMPLE_RATE / SAMPLE_LENGTH;
            peaks[i][1] = results[i]; // Fill the
        }
    }
    q15_t resultsSort[samplesOfInterest]; /*We make an array with the results
    sorted for use in the median calculation. A big part of the 510k Hz zone is
    either 0 or very low, so not only is is performance wise wasteful, it is
    also for calculating a proper median*/

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

    q15_t avgMedian = (resultsSort[(samplesOfInterest / 2) - 1] +
                       resultsSort[samplesOfInterest / 2]) /
                          2 +
                      1; /*As we may easily have an even number
of results, we calculate the median using the average of the two middle values*/

    // The next section is nothing short of a crime against all those who want
    // optimized code, we sincerely apologize for leaving you with this The
    // following code is so that we return a 2D array only the size of the
    // number of peaks we actually have.
    int numNonNull = 0;
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        if (peaks[i][1] <=
            avgMedian * 2) { // We filter by the median*2 because it works, the
                             // x2 is there just because mediean is too small to
                             // filter out all the "false" peaks
            peaks[i][0] = 0; // If the magnitue does not pass the threshold we
                             // bring the entire index to 0
            peaks[i][1] = 0;
        } else {
            numNonNull++; // If it passes we count it towards the number of
                          // peaks we have.
        }
    }
    // This is something that needs to be resolved ASAP, we are putting in the
    // length of the array in the very front of the array This is because q31_t
    // does not work seem well with the sizeof() function for arrays. This is
    // why the array has 1 extra index.
    numNonNull++;
    q31_t** peaksReturn =
        new q31_t*[numNonNull]; // Dynamically make a new array for the peaks we
                                // actually want to return, notice the use of
                                // nomNonNull
    for (int i = 0; i < numNonNull; i++) {
        peaksReturn[i] = new q31_t[2];
    }

    int tempCount = 0;
    // We send the length of the array through the 1st index of the array. It is
    // very important to find another solution. Maybe look into using the
    // uint16_t datatype
    peaksReturn[tempCount][0] = 0;
    peaksReturn[tempCount][1] = numNonNull;
    tempCount++;

    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        if (peaks[i][1] >
            avgMedian * 2) { // We filter by the median*2 because it works, the
                             // x2 is there just because mediean is to small to
                             // filter out all the "false" peaks
            peaksReturn[tempCount][0] =
                peaks[i][0]; // Fill the return array with the peaks.
            peaksReturn[tempCount][1] = peaks[i][1];
            tempCount++; // As our first index is reserved for the length of the
                         // array we use tempCount, which is 1 ahead of i,
                         // instead of i
        }
    }

    return peaksReturn;
}
