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

// Variables for Debugging ==================================================
/*
These are just to measure the time it takes to run the entire code.
The new calculations show 13300 us (micro seconds) (13.3 ms) for the whole
algorithm
*/
unsigned long timeDiff;
unsigned long startTime;
unsigned long endTime;

// Variables for Sampling ==================================================
// to be safe should be a bit under 1500. If it sampled more than 1500 for some reason, the data gathered will be inconsistent.
uint16_t number_samples = SAMPLE_LENGTH;
uint32_t sample_period = 11; // >= MIN_SAMP_PERIOD_TIMER
int16_t samplesRawHydrophone1[SAMPLE_LENGTH];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH];

<<<<<<< HEAD
=======
// Variables for Digital Signal Processing ==================================================
// A manual variable to filter out small peaks that dont manage to get over the threshold
const q15_t peakThreshold = 200;
>>>>>>> 2b135a629537a30021d051019ccb2a531b53aa68

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial connected\r\n");

    // Start timer to see time it takes for everything to run
    startTime = micros();

    // Sampling (START) ==================================================
    // (1) - First part: initializing ADC before being able to use it
    Serial.println("Initialize ADC");
    // clock of teensy is 600MHz after normal boot
    clockSampling::setup();
    adc::init();
    // defining value of register
    uint32_t ADC_reg_config;
    // WRITE_EN needs to be set to update REG, internal clock, BUSY mode active
    // high, powering off channel D because we don't need it, internal ref
    // because nothing external connected, reference voltage to 2.5V
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC);
    // value of channel A doubles by dividing range by 2 (works as expected)
    // ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 <<
    // CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_RANGE_A);
    adc::config(ADC_reg_config);
    adc::setup();
    // adc is now ready to use

    // (2) - Second Part: Sampling the data that is then put into the
    // ringbuffers to not overfill ringbuffer, just set maximum hard set limit
    // of number of samples to 1500
    if (number_samples > 1500) {
        number_samples = 1500;
    }
    // Start sampling
    adc::startConversion(sample_period, adc::TIMER);
    // will sample "number_samples" samples
    delayMicroseconds(sample_period * number_samples);
    // Stop Sampling
    adc::stopConversion();

    // (3) - Third Part:  The data is now in the ringbuffers. First the
    // timestamps are processed and the each of the channels
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Getting data");
    // Saving data into array we will use further down the line
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone1[i] = (int16_t)adc::ChannelA0.get();
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone2[i] = (int16_t)adc::ChannelA1.get();
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone3[i] = (int16_t)adc::ChannelB0.get();
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone4[i] = (int16_t)adc::ChannelB1.get();
    }
    for (uint16_t i = 0; i < number_samples; i++) {
        samplesRawHydrophone5[i] = (int16_t)adc::ChannelC0.get();
    }
    /*
        Do sample and process repeatedly:
            1: reset all the ringbuffer so that the new values are written from the start 
            2: Sample again for the wanted number of samples 
            3: process the new values
            4: go back to 1 and reset the ringbuffers Following can be implemented to sample "continuously":
                - setup adc
                - define sample number and frequency
            loop:
                - sample
                - process the data in the ringbuffer
                - reset ringbuffer for new sampling
            end of loop
    */
    // Sampling (STOP) ==================================================

    // Digital Signal Processing (START) ==================================================
    // Filter raw samples
    q15_t* samplesFiltered = filter_butterwort_9th_order_50kHz(samplesRawHydrophone1);

    // Preform FFT calculations on filtered samples
    q15_t* FFTResultsRaw = FFT_raw(samplesFiltered);
    q15_t* FFTResults = FFT_mag(FFTResultsRaw);

    // Get peaks of frequencies that might be of interest and their useful
    // information like amplitude, frequency and phase
    q31_t** peaks = peak_detection(FFTResultsRaw, FFTResults);

    /*
    Since we are storing the length of the array in the first index, we do
    not start from 0 in the array when printing out. Find out how to get
    length of a 2D array of a q31_t datatype. For now we return the length of
    the array in the first index of 2D array, This must be solved, this is
    not a good solution.
    */
    int lengthOfPeakArray = peaks[0][0];

    // TIPS: For getting phase of the peak FFTs from q31 format that we dont
    // understand to radians in floats, use this:
    // phaseQ31_to_radianFloat32(peaks[x][2]);
    // Digital Signal Processing (STOP) ==================================================

    // End timer for testing speed of algorithm
    endTime = micros();

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
        Serial.print(", ");
    }

    // Print Filtered signal response
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("Filtered samples");
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        Serial.print(samplesFiltered[i]);
        Serial.print(", ");
    }

    // Print FFT
    Serial.println("");
    Serial.println("=====================================================================================");
    Serial.println("FFT");
    for (int i = 0; i < SAMPLE_LENGTH; i++) {
        Serial.print(FFTResults[i]);
        Serial.print(", ");
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
}

void loop() {
    // Serial.println("Test");
}
