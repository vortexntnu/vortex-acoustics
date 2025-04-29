

#include "dsp.h"
#include "Include/arm_const_structs.h"
#include "Include/arm_math.h"
#include "arm_math.h"
#include <cstdint>

#define SAMPLE_RATE 430000 // 430.0 kHz
#define SAMPLE_LENGTH 1024

#define FILTER_AMPLIFICATION 2

#define BITSHIFT 9
#define SCALE_FACTOR 1000.0
#define FREQUENCY_LIMIT 60000

#define PEAK_THRESHOLD 1000

#define fOrder 9
#define fOrder2 2

const q15_t samples_of_interest = FREQUENCY_LIMIT * SAMPLE_LENGTH / SAMPLE_RATE;

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
const q15_t PI_Q15 = (q15_t)(PI * (1 << 15) + 0.5);

static int16_t samplesRawForDSP[SAMPLE_LENGTH] = {0};
static size_t samples_interest = 32;
static q15_t FFTResultsRaw[2 * SAMPLE_LENGTH] = {0};


q15_t samplesFiltered[SAMPLE_LENGTH] = {0};
q15_t FFTResultsMagnified[SAMPLE_LENGTH] = {0};
Peak peaks_buffer[SAMPLE_LENGTH];
size_t num_peaks;

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

static q15_t *filter_butterwort_9th_order_50kHz(int16_t *samplesRaw) {
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

static q15_t *filter_butterwort_2th_order_50kHz(int16_t *samplesRaw) {
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

static void filter_butterworth_1st_order_50kHz(const int16_t *samplesRaw,
                                        q15_t *samples) {

  static q15_t x_prev = 0;
  static q15_t y_prev = 0;

  for (int n = 0; n < SAMPLE_LENGTH; n++) {
    // Convert the raw sample to Q15 and scale it by the amplification factor.
    // Assuming the raw sample is in a similar Q15 range or is appropriately
    // scaled.
    q15_t x_current = ((q15_t)samplesRaw[n] * FILTER_AMPLIFICATION);

    // Compute the filter output in two stages:
    // 1. Compute the numerator (b0*x[n] + b1*x[n-1])
    q15_t num = (bFilterCoeffs1[0] * x_current) + (bFilterCoeffs1[1] * x_prev);

    // 2. Compute the denominator term (a1*y[n-1]) and subtract from the
    // numerator.
    q15_t den = (aFilterCoeffs1[1] * y_prev);
    q15_t temp = num - den;

    // Multiply by a0 (normally, a0 equals 1.0 in Q15, i.e., 0x7FFF, so this
    // might be a no-op if normalized).
    q15_t y_current = (aFilterCoeffs1[0] * temp);

    // Store the output.
    samples[n] = y_current;

    // Update filter state.
    x_prev = x_current;
    y_prev = y_current;
  }
}

/*
Instead of taking the full FFT in a signle function we split it
We calculating first the raw values out of FFT witch are "Real" and "Imaginary"
values these values are really interesting since this raw format can be used to
calculate both amplitude, frequencies and phase shift of a signal
*/
static void FFT_raw(q15_t *samples, q15_t *resultsRaw) {
  /*
  To store the results of fft with
  complex numbers, need to have double the
  size of the sample length
  z = a + bi, (a1, b1, a2, b2, a3, b3 ... )
  */

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
}

static inline void FFT_mag(q15_t *resultsRaw, q15_t *results) {
  /*
  Create an empty array to store the magnitude
  calculations of the FFT.
  As we are not dealing with complex numbers
  anymore, it is the size of the sample length
  */

  // Converts the complex array into a magnitude array.
  arm_cmplx_mag_q15(resultsRaw, results, SAMPLE_LENGTH);
}

// Compute phase from real and imag in Q15 format
static inline q15_t compute_phase(q15_t real, q15_t imag) {
  if (real == 0 && imag == 0) {
    return 0;
  } else if (real == 0) {
    // +90 or -90 degrees
    return (imag > 0) ? q15_divide(PI_Q15, 2) : (q15_t)(-q15_divide(PI_Q15, 2));
  } else {
    // atan(imag/real)
    return q15_taylor_atan(q15_divide(imag, real));
  }
}

// Simple median via insertion sort on small array
// Median via Quickselect (Hoare's algorithm) for O(n) average complexity
static size_t partition(q15_t *arr, size_t left, size_t right,
                        size_t pivot_index) {
  q15_t pivot_value = arr[pivot_index];
  // Move pivot to end
  q15_t tmp = arr[pivot_index];
  arr[pivot_index] = arr[right];
  arr[right] = tmp;
  size_t store_index = left;
  for (size_t i = left; i < right; ++i) {
    if (arr[i] < pivot_value) {
      tmp = arr[store_index];
      arr[store_index] = arr[i];
      arr[i] = tmp;
      ++store_index;
    }
  }
  // Move pivot to its final place
  tmp = arr[right];
  arr[right] = arr[store_index];
  arr[store_index] = tmp;
  return store_index;
}

static q15_t quickselect_median(q15_t *arr, size_t n) {
  size_t left = 0, right = n - 1;
  size_t median_index = n / 2;
  while (1) {
    if (left == right) {
      return arr[left];
    }
    // Choose pivot as middle element
    size_t pivot_index = left + (right - left) / 2;
    pivot_index = partition(arr, left, right, pivot_index);
    if (pivot_index == median_index) {
      return arr[median_index];
    } else if (median_index < pivot_index) {
      right = pivot_index - 1;
    } else {
      left = pivot_index + 1;
    }
  }
}

// Median wrapper: handles even/odd length
static q15_t median_of(q15_t *arr, size_t n) {
  if (n == 0)
    return 0;
  q15_t median = quickselect_median(arr, n);
  if ((n & 1) == 0) {
    // Even count: average the two middle values
    // Find the max in the lower half
    q15_t max_lower = arr[0];
    for (size_t i = 1; i < n / 2; ++i) {
      if (arr[i] > max_lower)
        max_lower = arr[i];
    }
    int sum = (int)max_lower + (int)median;
    return (q15_t)(sum >> 1);
  }
  return median;
}
/*
 * peak_detection():
 *   - resultsRaw: pointer to interleaved FFT raw data (real, imag) in Q15,
 * length = 2*SAMPLE_LENGTH
 *   - results:    pointer to FFT magnitudes in Q15, length = SAMPLE_LENGTH
 *   - samplesOfInterest: number of low-frequency bins to use for median
 * threshold
 *   - outPeaks:   caller-allocated array of Peaks, size = outBufSize
 *   - outBufSize: capacity of outPeaks[]
 *   - outNumPeaks: pointer to size_t for number of peaks detected
 *
 * Returns 0 on success (outNumPeaks filled), -1 on error (outNumPeaks = 0).
 */
static int peak_detection(const q15_t *resultsRaw, const q15_t *results,
                   size_t samplesOfInterest, Peak *outPeaks, size_t outBufSize,
                   size_t *outNumPeaks) {
  if (!resultsRaw || !results || !outPeaks || !outNumPeaks) {
    return -1;
  }
  if (samplesOfInterest == 0 || samplesOfInterest > SAMPLE_LENGTH) {
    *outNumPeaks = 0;
    return -1;
  }

  // 1) Compute median-based threshold using first samplesOfInterest bins
  q15_t scratch[samplesOfInterest];
  memcpy(scratch, results, samplesOfInterest * sizeof(q15_t));
  q15_t med = median_of(scratch, samplesOfInterest);
  // threshold = med*3 + PEAK_THRESHOLD, in Q15
  int32_t thr_q31 = (int32_t)med * 3 + (int32_t)PEAK_THRESHOLD;
  if (thr_q31 > 0x7FFF)
    thr_q31 = 0x7FFF;
  else if (thr_q31 < -0x8000)
    thr_q31 = -0x8000;
  q15_t threshold = (q15_t)thr_q31;

  // 2) Single-pass peak detection and filtering
  size_t count = 0;
  for (size_t i = 1; i < SAMPLE_LENGTH - 1; ++i) {
    q15_t mag = results[i];
    // local max and above threshold
    if (mag >= results[i - 1] && mag >= results[i + 1] && mag > threshold) {
      if (count >= outBufSize)
        break;
      Peak *p = &outPeaks[count++];
      p->index = i;
      p->amplitude = (q31_t)mag;
      p->frequency = (q31_t)((i * SAMPLE_RATE) / SAMPLE_LENGTH);
      p->phase = compute_phase(resultsRaw[2 * i], resultsRaw[2 * i + 1]);
    }
  }

  *outNumPeaks = count;
  return 0;
}

int dsp_find_signal(uint8_t buffer_to_check) {

  filter_butterworth_1st_order_50kHz(samplesRawHydrophones[0] +
                                         (buffer_to_check * SAMPLE_LENGTH_ADC),
                                     samplesFiltered);

  FFT_raw(samplesFiltered, FFTResultsRaw);
  FFT_mag(FFTResultsRaw, FFTResultsMagnified);

  if (peak_detection(FFTResultsRaw, FFTResultsMagnified, samples_interest,
                     peaks_buffer, SAMPLE_LENGTH, &num_peaks)) {
    return 0;
  }

  for (size_t i = 0; i < num_peaks; i++) {
    int32_t peakFrequency = peaks_buffer[i].frequency;
    for (int j = 0; j < FREQUENCY_LIST_LENGTH; j++) {
      if ((peakFrequency < freq_interest_max[j]) &&
          (peakFrequency > freq_interest_min[j])) {
        return 1;
      }
    }
  }
  return 0;
}
