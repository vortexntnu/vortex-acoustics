/*
IMPORTANT INFORMATION!

Some libraries are not imported properly when downloading from GitHub
This is because they are to big and so they get auto deleted
For now the libraries that need to be manually installed are
- CMSIS
To import libraries properly do this:
Copy file from ".\vortex-acoustics\Resource\CMSIS" to ".\vortex-acoustics\cpp\teensy\.pio\libdeps\teensy41\CMSIS"

Code written by: Vortex NTNU
*/

// CMSIS Libraries
#include "arm_const_structs.h"
#include "arm_math.h"

// Arduino Libraries
#include <Arduino.h>

// Sampling Analog to Digital Converter (ADC) Libraries
#include "GPT.h"
#include "PIT.h"
#include "adc.h"
#include "clock.h"
#include "gpio.h"
#include "gpioInterrupt.h"

// Digital Signal Processing (DSP) Libraries
#include "DSP.h"

// Variables for Debugging ==========
/*
These are just to measure the time it takes to run the entire code.
The new calculations show 13300 us (micro seconds) (13.3 ms) for the whole algorithm
*/
unsigned long timeDiff;
unsigned long startTime;
unsigned long endTime;

// Variables for Sampling ==========
// to be safe should be a bit under 1500. If it sampled more than 1500 for some reason, the data gathered will be inconsistent.
uint16_t number_samples = SAMPLE_LENGTH;
uint32_t sample_period = 2.3; // >= MIN_SAMP_PERIOD_TIMER
int16_t samplesRawHydrophone1[SAMPLE_LENGTH];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH];

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial connected\r\n");

    // Sampling Setup (START) ====================================================================================================
    // initializing ADC before being able to use it
    Serial.println("Initialize ADC");
    adc::init();
    // Setup parameters for ADC
    uint32_t ADC_reg_config;
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_VREF);
    // Configure ADC
    adc::config(ADC_reg_config);
    adc::setup();
    // Double check that the number of samples we want to take in doesn't overflow the ADC ring buffer space
    if (number_samples > 3 * SAMPLE_LENGTH_ADC) {
        number_samples = 3 * SAMPLE_LENGTH_ADC;
    }
    // Sampling Setup (STOP) ====================================================================================================
}

void loop() {
    // Start timer to see time it takes for everything to run
    startTime = micros();

    // Sampling (START) ====================================================================================================
    // Start sampling
    adc::startConversion(sample_period, adc::BLOCKING);
    // Start sampling into the buffer
    uint8_t buffer_to_check = adc::active_buffer;
    while (!adc::buffer_filled[buffer_to_check])
        ;
    // Stop Sampling
    adc::stopConversion();

    // Process data from the ring-buffer
    // Saving data into array we will be used further down the line
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone1[i] = (int16_t)adc::channel_buff_ptr[1][buffer_to_check][i];
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone2[i] = (int16_t)adc::channel_buff_ptr[2][buffer_to_check][i];
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone3[i] = (int16_t)adc::channel_buff_ptr[3][buffer_to_check][i];
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone4[i] = (int16_t)adc::channel_buff_ptr[4][buffer_to_check][i];
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone5[i] = (int16_t)adc::channel_buff_ptr[0][buffer_to_check][i];
    }

    // Clean ring-buffers
    // this is done so that next time we can add new data into the ring-buffers
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        adc::buffer_filled[i] = 0;
    }
    // Sampling (STOP) ====================================================================================================

    // Digital Signal Processing (START) ====================================================================================================
    // Filter raw samples
    q15_t* samplesFiltered = filter_butterwort_2th_order_50kHz(samplesRawHydrophone1);

    // Preform FFT calculations on filtered samples
    q15_t* FFTResultsRaw = FFT_raw(samplesFiltered);
    q15_t* FFTResults = FFT_mag(FFTResultsRaw);

    // Get peaks of frequencies that might be of interest and their useful information like amplitude, frequency and phase
    q31_t** peaks = peak_detection(FFTResultsRaw, FFTResults);

    /*
    Since we are storing the length of the array in the first index, we do not start from 0 in the array when printing out. 
    Find out how to get length of a 2D array of a q31_t datatype. 
    For now we return the length of the array in the first index of 2D array, This must be solved
    this is NOT a good solution.
    */
    int lengthOfPeakArray = peaks[0][0];

    /*
    TIPS: For getting phase of the peak FFTs from q31 format that we dont understand to radians in floats, use this:
    phaseQ31_to_radianFloat32(peaks[x][2]);
    */
    // Digital Signal Processing (STOP) ====================================================================================================

    // End timer for testing speed of algorithm
    endTime = micros();

    // Debugging (START) ====================================================================================================
    // Print out how long it takes to run the whole algorithm
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Timer");
    timeDiff = endTime - startTime;
    Serial.print("StartTime: ");
    Serial.println(startTime);
    Serial.print("EndTime: ");
    Serial.println(endTime);
    Serial.print("Time: ");
    Serial.println(timeDiff);

    // Print raw sampled signal
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Raw data from hydrophone 1");
    for (uint16_t i = 0; i < number_samples; i++) {
        Serial.print(samplesRawHydrophone1[i]);
        Serial.print(",");
    }

    // Print Filtered signal response
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Filtered samples");
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        Serial.print(samplesFiltered[i]);
        Serial.print(",");
    }

    // Print FFT
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("FFT");
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        Serial.print(FFTResults[i]);
        Serial.print(",");
    }

    // Print peaks of FFT
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Peaks");
    Serial.println(lengthOfPeakArray);
    Serial.println("[Amplitude, Frequency, Phase in radians]");
    for (int i = 1; i < lengthOfPeakArray; i++) {
        Serial.print("[");
        Serial.print(peaks[i][0]);
        Serial.print(", ");
        Serial.print(peaks[i][1]);
        Serial.print(", ");

        // Calculate phase in comprehensible manner
        // peaks[i][2] is q31_t type but in the taylor expansion it is actually
        // q15_t, the rest of the 16 MSB are just 0, The reason it is q31_t is
        // just because we send it with all the frequencies, witch are really
        // big and require q31_t to be stored properly
        float32_t phase_in_radians = phaseQ31_to_radianFloat32(peaks[i][2]);

        Serial.print(phase_in_radians);

        Serial.println("],");
    }

    // 20 second delay
    // This is here just so that we can observe data with human eyes and copy it
    // Because other wise it goes to fast
    delay(20000);
    // Debugging (STOP) ====================================================================================================
}
