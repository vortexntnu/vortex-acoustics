

#include "Include/arm_math.h"
#include "Include/arm_const_structs.h"
#include "dsp.h"

// How fast the ADC samples, important to know for FFT, the max is 510 kHz, HOWEVER for some reason ADC can not go max, real value is lower at:
#define SAMPLE_RATE 430000 // 430.0 kHz
// How many samples we want from ADC
#define SAMPLE_LENGTH 1024

// How much should the signal should be amplified before filtering it
#define FILTER_AMPLIFICATION 2

// For FFT to shift bits
#define BITSHIFT 9
// How more pronounced the peaks of a frequencies will be in contrast with the lower
// dont have it to high as this will make the noise have high peaks as well
#define SCALE_FACTOR 1000.0
// The upper frequency limit of the frequency band we actually want to check
#define FREQUENCY_LIMIT 60000

// A manual variable to filter out small peaks that don't manage to get over the threshold, so called "fake peaks"
#define PEAK_THRESHOLD 1000

#define fOrder  9
#define fOrder2  2

// We do not care about frequencies up to 510k Hz, so we define a variable for
// indexes of indexes, go to the h file
const q15_t samplesOfInterest = FREQUENCY_LIMIT * SAMPLE_LENGTH / SAMPLE_RATE;

/*
Coefficients for filter found at https://www.meme.net.au/butterworth.html,
put 9th order filter, 510kHz sampling rate and 50kHz cut-off
put 2th order filter, 430kHz sampling rate and 50kHz cut-off
*/
const float32_t aFilterCoeffs[fOrder] = {
    5.4569203401896500,   -13.7047980216478000, 20.6476635308150000,
    -20.4748421533297000, 13.8143215886326000,  -6.3261752484730100,
    1.8924462642157100,   -0.3350397779275800,  0.0267111235596287};
const float32_t bFilterCoeffs[fOrder + 1] = {
    0.00000545381633879714, 0.00004908434704917420, 0.00019633738819669700,
    0.00045812057245895900, 0.00068718085868843900, 0.00068718085868843900,
    0.00045812057245895900, 0.00019633738819669700, 0.00004908434704917420,
    0.00000545381633879714};

const float32_t aFilterCoeffs2[fOrder2] = {0.00101196462632, -0.00035885208947};
const float32_t bFilterCoeffs2[fOrder2 + 1] = {
    0.000086700190740, 0.000173400381481, 0.000086700190740};

// Coefficients for 1th order filter, 430 kHz sampling rate, 50 kHz cut-off
// calculated manually with the help of this research paper
// https://www.researchgate.net/publication/338022014_Digital_Implementation_of_Butterworth_First-Order_Filter_Type_IIR
const float32_t aFilterCoeffs1[] = {1.0, -0.44669};
const float32_t bFilterCoeffs1[] = {0.27665, 0.27665};

/*
Bit reversing is applied in a lot of FFT
algorithms for increase efficiency.
*/
const uint32_t doBitReverse = 1;

// Constants in q_15 format done right
const q15_t PI_q15 = (q15_t)(PI * (1 << 15) + 0.5);


static q15_t q15_divide(q15_t a, q15_t b) {
    if (b == 0) {
        return (a >= 0) ? 0x7FFF : 0x8000;  
    }

    int32_t a_scaled = (int32_t)a << 15;
    int32_t result = a_scaled / b;

    if (result > 0x7FFF) {
        result = 0x7FFF;
    } else if (result < -0x8000) {
        result = -0x8000;
    }

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


static q15_t q15_taylor_atan(q15_t x) {
    const int TAYLOR_TERMS = 10;
    
    // Compute x^2 in Q15 with rounding.
    q15_t x_sq = (q15_t)(((int32_t)x * x + (1 << 14)) >> 15);

    q15_t term = x;
    q15_t result = x;

    for (int i = 1; i < TAYLOR_TERMS; i++) {
        term = (q15_t)(((int32_t)term * x_sq + (1 << 14)) >> 15);
        
        int divisor = 2 * i + 1;
        q15_t term_div = (q15_t)(((int32_t)term + divisor / 2) / divisor);

        if (i % 2 == 1) {
            result -= term_div;
        } else {
            result += term_div;
        }
    }

    return result;
}



q15_t *filter_butterwort_9th_order_50kHz(int16_t *samplesRaw) {
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
      input_influence += bFilterCoeffs[k] * (samplesRaw[i - k]);
      input_influence += bFilterCoeffs[k] * (samplesRaw[i - k]);
    }

    float influenceTotalFloat = output_influence + input_influence;

    // Convert float to q15 datatype in the correct way
    q15_t influenceTotalQ15 = (q15_t)influenceTotalFloat;
    samples[i] = influenceTotalQ15 * FILTER_AMPLIFICATION;
  }
  return samples;
}

q15_t *filter_butterwort_2th_order_50kHz(int16_t *samplesRaw) {
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
  them. Starting at fOrder2 because we can't use an index outside of the
  samples array.
  */
  for (int i = fOrder2; i < SAMPLE_LENGTH; i++) {
    float32_t output_influence = 0;
    /* We iterate through the previous filtered samples for the
    filtering, as it is more clean and convenient. */
    for (int k = 0; k < fOrder2; k++) {
      output_influence += aFilterCoeffs2[k] * samples[i - (k + 1)];
    }

    float32_t input_influence = 0;
    /* We iterate through the previous unfilteredsamples for the
    filtering, as it is more clean and convenient.*/
    for (int k = 0; k < fOrder2 + 1; k++) {
      input_influence +=
          bFilterCoeffs2[k] * (samplesRaw[i - k] * FILTER_AMPLIFICATION);
    }

    float influenceTotalFloat = output_influence + input_influence;

    // Convert float to q15 datatype in the correct way
    q15_t influenceTotalQ15 = (q15_t)influenceTotalFloat;
    samples[i] = influenceTotalQ15;
  }
  return samples;
}



q15_t *filter_butterworth_1st_order_50kHz(const int16_t *samplesRaw) {
    static q15_t samples[SAMPLE_LENGTH];

    static q15_t x_prev = 0;
    static q15_t y_prev = 0;

    for (int n = 0; n < SAMPLE_LENGTH; n++) {
        // Convert the raw sample to Q15 and scale it by the amplification factor.
        // Assuming the raw sample is in a similar Q15 range or is appropriately scaled.
        q15_t x_current = ((q15_t)samplesRaw[n] * FILTER_AMPLIFICATION);

        // Compute the filter output in two stages:
        // 1. Compute the numerator (b0*x[n] + b1*x[n-1])
        q15_t num = (bFilterCoeffs1[0] * x_current) + (bFilterCoeffs1[1] * x_prev);

        // 2. Compute the denominator term (a1*y[n-1]) and subtract from the numerator.
        q15_t den = (aFilterCoeffs1[1] * y_prev);
        q15_t temp = num - den;

        // Multiply by a0 (normally, a0 equals 1.0 in Q15, i.e., 0x7FFF, so this might be a no-op if normalized).
        q15_t y_current = (aFilterCoeffs1[0] * temp);

        // Store the output.
        samples[n] = y_current;

        // Update filter state.
        x_prev = x_current;
        y_prev = y_current;
    }

    return samples;
}

/*
Instead of taking the full FFT in a signle function we split it
We calculating first the raw values out of FFT witch are "Real" and "Imaginary"
values these values are really interesting since this raw format can be used to
calculate both amplitude, frequencies and phase shift of a signal
*/
q15_t *FFT_raw(q15_t *samples) {
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
  arm_scale_q15(samples, SCALE_FACTOR, BITSHIFT, samples, SAMPLE_LENGTH);

  // The FFT itself, output is the FFT complex array
  arm_rfft_q15(&fftInstance, samples, resultsRaw);

  return resultsRaw;
}

q15_t *FFT_mag(q15_t *resultsRaw) {
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
 * peak_detection():
 *   - resultsRaw: pointer to interleaved FFT raw data (real and imaginary parts) in Q15.
 *   - results: pointer to an array of FFT magnitudes in Q15.
 *   - out_num_peaks: output parameter that will contain the number of detected peaks.
 *
 * Returns:
 *   A dynamically allocated array of Peak structures (or NULL if none detected or on error).
 *   The caller is responsible for freeing the returned array.
 */
Peak* peak_detection(const q15_t *resultsRaw, const q15_t *results, size_t *out_num_peaks) {
    size_t i;
    size_t candidate_count = 0;

    // Allocate worst-case candidate array (each bin might be a candidate)
    Peak *candidates = (Peak *) malloc(SAMPLE_LENGTH * sizeof(Peak));
    if (!candidates) {
        *out_num_peaks = 0;
        return NULL;
    }

    // First pass: detect local maxima from index 1 to SAMPLE_LENGTH-2.
    for (i = 1; i < SAMPLE_LENGTH - 1; i++) {
        if (results[i] >= results[i - 1] && results[i] >= results[i + 1]) {
            // Store this candidate.
            candidates[candidate_count].index = i;
            // Promote amplitude from Q15 to Q31 (simple cast; adjust scaling if needed)
            candidates[candidate_count].amplitude = (q31_t)results[i];
            // Frequency computation: bin index to frequency.
            candidates[candidate_count].frequency = (q31_t)((i * SAMPLE_RATE) / SAMPLE_LENGTH);
            candidates[candidate_count].phase = 0;
            candidate_count++;
        }
    }

    // Prepare to compute a median from the first samplesOfInterest values of 'results'.
    q15_t *resultsSort = (q15_t *) malloc(samplesOfInterest * sizeof(q15_t));
    if (!resultsSort) {
        free(candidates);
        *out_num_peaks = 0;
        return NULL;
    }
    for (i = 0; i < samplesOfInterest; i++) {
        resultsSort[i] = results[i];
    }
    // Insertion sort (or qsort, if available) on the subset.
    for (i = 1; i < samplesOfInterest; i++) {
        q15_t key = resultsSort[i];
        int j = i - 1;
        while (j >= 0 && resultsSort[j] > key) {
            resultsSort[j + 1] = resultsSort[j];
            j--;
        }
        resultsSort[j + 1] = key;
    }
    q15_t median;
    if (samplesOfInterest % 2 == 0) {
        median = (resultsSort[samplesOfInterest/2 - 1] + resultsSort[samplesOfInterest/2]) / 2;
    } else {
        median = resultsSort[samplesOfInterest/2];
    }
    free(resultsSort);

    // Define a threshold: median * 3 + PEAK_THRESHOLD.
    q15_t threshold = median * 3 + PEAK_THRESHOLD;

    // Allocate the final peaks array.
    // Worst-case, all candidates are valid.
    Peak *final_peaks = (Peak *) malloc(candidate_count * sizeof(Peak));
    if (!final_peaks) {
        free(candidates);
        *out_num_peaks = 0;
        return NULL;
    }
    size_t final_count = 0;

    // For each candidate, if amplitude exceeds the threshold, compute phase.
    for (i = 0; i < candidate_count; i++) {
        if (candidates[i].amplitude > threshold) {
            size_t idx = candidates[i].index;
            q15_t real = resultsRaw[idx * 2];
            q15_t imag = resultsRaw[idx * 2 + 1];
            q15_t phase;
            // Compute phase safely.
            if ((real == 0) && (imag == 0)) {
                phase = 0;
            } else if ((real == 0) && (imag > 0)) {
                phase = q15_divide(PI_q15, 2);
            } else if ((real == 0) && (imag < 0)) {
                phase = -q15_divide(PI_q15, 2);
            } else {
                phase = q15_taylor_atan(q15_divide(imag, real));
            }
            candidates[i].phase = phase;
            final_peaks[final_count++] = candidates[i];
        }
    }
    free(candidates);

    // Optionally, shrink the allocated array using realloc.
    if (final_count == 0) {
        free(final_peaks);
        *out_num_peaks = 0;
        return NULL;
    } else {
        Peak *resized = (Peak *) realloc(final_peaks, final_count * sizeof(Peak));
        if (resized != NULL) {
            final_peaks = resized;
        }
        *out_num_peaks = final_count;
        return final_peaks;
    }
}
