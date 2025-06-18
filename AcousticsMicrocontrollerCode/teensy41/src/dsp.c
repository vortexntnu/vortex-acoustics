#include "dsp.h"



const float32_t sos_floats[NUM_STAGES][6] = {{0.00802494, 0.01604989, 0.00802494, 1, -0.92145, 0.23722397}, {1, 2, 1, 1, -1.18653637, 0.59315345}};
static q15_t biquadCoeffsQ15[NUM_STAGES * 6];
static q15_t biquadStateQ15[4 * NUM_STAGES] = {0};
static arm_biquad_casd_df1_inst_q15 S_q15;


// Constants in q_15 format done right
const q15_t PI_Q15 = (q15_t)(PI * (1 << 15) + 0.5);

static q15_t fft_results_raw[2 * SAMPLE_LENGTH] = {0};

q15_t samples_filtered[SAMPLE_LENGTH] = {0};
q15_t fft_results_magnified[SAMPLE_LENGTH] = {0};
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

/**
 *@brief Converting Coefficients from float to q15
 */
static inline void buildCoeffs(void) {
    // Section 1:
    biquadCoeffsQ15[0] = FLOAT_TO_Q15(sos_floats[0][0]); // b0_1
    biquadCoeffsQ15[1] = FLOAT_TO_Q15(sos_floats[0][1]); // b1_1
    biquadCoeffsQ15[2] = FLOAT_TO_Q15(sos_floats[0][2]); // b2_1
    biquadCoeffsQ15[3] = 0x7FFF;                         // a0_1 = +1.0
    biquadCoeffsQ15[4] = FLOAT_TO_Q15(sos_floats[0][4]); // a1_1
    biquadCoeffsQ15[5] = FLOAT_TO_Q15(sos_floats[0][5]); // a2_1

    // Section 2:
    biquadCoeffsQ15[6] = FLOAT_TO_Q15(sos_floats[1][0]);  // b0_2
    biquadCoeffsQ15[7] = FLOAT_TO_Q15(sos_floats[1][1]);  // b1_2
    biquadCoeffsQ15[8] = FLOAT_TO_Q15(sos_floats[1][2]);  // b2_2
    biquadCoeffsQ15[9] = 0x7FFF;                          // a0_2 = +1.0
    biquadCoeffsQ15[10] = FLOAT_TO_Q15(sos_floats[1][4]); // a1_2
    biquadCoeffsQ15[11] = FLOAT_TO_Q15(sos_floats[1][5]); // a2_2
}

void filter_butterwort_4th_order_init(void) {

    buildCoeffs();

    const int8_t postShift = 0;
    arm_biquad_cascade_df1_init_q15(&S_q15, NUM_STAGES, biquadCoeffsQ15, biquadStateQ15, postShift);
}

/**
 *@brief 4th order butterworth filter
 *@param raw signal
 *@param output filtered sigal
 *@param block size
 */
static inline void process_block(int16_t* rawADC, q15_t* filteredQ15, uint32_t blockSize) { arm_biquad_cascade_df1_q15(&S_q15, rawADC, filteredQ15, blockSize); }


static void fft_raw(q15_t* samples, q15_t* resultsRaw) {
    q15_t temp[SAMPLE_LENGTH];

    arm_rfft_instance_q15 fftInstance;

    arm_rfft_init_q15(&fftInstance, SAMPLE_LENGTH, IFFT_FLAG, BIT_REVERSE);

    arm_scale_q15(samples, SCALE_FACTOR, BITSHIFT, temp, SAMPLE_LENGTH);

    arm_rfft_q15(&fftInstance, temp, resultsRaw);
}

static inline void fft_mag(q15_t* resultsRaw, q15_t* results) { arm_cmplx_mag_q15(resultsRaw, results, SAMPLE_LENGTH); }

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

static int detect_ping_in_spectrum(const q15_t* fftMag) {
    // 1) Compute target bin ± spread:
    float binWidth = Fs / (float)N;
    int k0 = (int)(f_ping / binWidth);
    int spread = (int)(deltaF / binWidth) + 1;

    int k_min = k0 - spread;
    int k_max = k0 + spread;
    if (k_min < 1)
        k_min = 1;
    if (k_max > N / 2 - 1)
        k_max = N / 2 - 1;

    // 2) Find best peak within [k_min..k_max]:
    q15_t bestMag = fftMag[k_min];
    int bestBin = k_min;
    for (int k = k_min + 1; k <= k_max; k++) {
        if (fftMag[k] > bestMag) {
            bestMag = fftMag[k];
            bestBin = k;
        }
    }

    // 3) Build noise‐floor from [k_min-W..k_min-1] and [k_max+1..k_max+W]:
    int low1 = k_min - SIDE_BAND_WIDTH;
    if (low1 < 1)
        low1 = 1;
    int high1 = k_min - 1;
    if (high1 < low1)
        high1 = low1;
    int low2 = k_max + 1;
    if (low2 > N / 2 - 1)
        low2 = N / 2 - 1;
    int high2 = k_max + SIDE_BAND_WIDTH;
    if (high2 > N / 2 - 1)
        high2 = N / 2 - 1;

    uint32_t sum = 0;
    int count = 0;
    for (int k = low1; k <= high1; k++) {
        sum += (uint16_t)fftMag[k];
        count++;
    }
    for (int k = low2; k <= high2; k++) {
        sum += (uint16_t)fftMag[k];
        count++;
    }
    q15_t noiseFloor = (q15_t)(sum / count);

    // 4) Threshold = noiseFloor + MARGIN_Q15 (clamped)
    int32_t thr = (int32_t)noiseFloor + (int32_t)MARGIN_Q15;
    if (thr > 0x7FFF)
        thr = 0x7FFF;
    q15_t threshold = (q15_t)thr;

    // 5) Compare
    return (bestMag > threshold);
}

int dsp_found_signal(uint8_t bufferToCheck) {

    process_block(samples_raw_hydrophones[0] + (bufferToCheck * SAMPLE_LENGTH_ADC), samples_filtered, SAMPLE_LENGTH_ADC);
    fft_raw(samples_filtered, fft_results_raw);
    fft_mag(fft_results_raw, fft_results_magnified);

    return detect_ping_in_spectrum(fft_results_magnified);
}
