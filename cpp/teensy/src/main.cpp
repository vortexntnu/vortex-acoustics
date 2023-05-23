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
#include <vector>

// Sampling Analog to Digital Converter (ADC) Libraries
#include "GPT.h"
#include "PIT.h"
#include "adc.h"
#include "clock.h"
#include "gpio.h"
#include "gpioInterrupt.h"

// Digital Signal Processing (DSP) Libraries
#include "DSP.h"

// Libraries for Ethernet
#include "ethernetModule.h"
#include "teensyUDP.h"

// Variables for Debugging ==========
/*
These are just to measure the time it takes to run the entire code.
*/
unsigned long timeDiff;
unsigned long startTime;
unsigned long endTime;

// Variables for Sampling ==========
// to be safe should be a bit under 1500. If it sampled more than 1500 for some reason, the data gathered will be inconsistent.
uint16_t number_samples = SAMPLE_LENGTH * 3;
uint32_t sample_period = 2.3; // >= MIN_SAMP_PERIOD_TIMER
int16_t samplesRawHydrophone1[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH * 3];
#define SAMPLING_TIMEOUT 60000 // [60 s] If sampling takes to long before finding a frequency of interest we exit the loop and later try again

// Variables for Digital Signal Processing ==========
int16_t samplesRawForDSP[SAMPLE_LENGTH];
q15_t* samplesFiltered;
q15_t* FFTResultsRaw;
q15_t* FFTResults;
std::vector<std::vector<q31_t>> peaks;
int16_t lengthOfPeakArray;

// Variables for Peak Detection ==========
int32_t frequencyOfInterest = 0; // 0 Hz
int32_t frequencyVariance = 0;   // +-0 Hz

// Variables for data transmission ==========
void communicationTeensy();

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("1 - Serial connected");
    Serial.println();

    // Ethernet Setup PART 1 (START) ====================================================================================================
    /*
    NOTE: This code HAS to come befor "Sampling Setup", otherwise some values are configured incorrectly
    Why? I have no Idea, some memory magic probably =_=
    */
    // Ethernet init
    Serial.print("2 - ");
    ethernetModule::UDP_init();
    Serial.println();
    // Ethernet Setup PART 1 (STOP) ====================================================================================================

    // Sampling Setup (START) ====================================================================================================
    // initializing ADC before being able to use it
    Serial.println("3 - Initializing ADC");
    adc::init();
    // Setup parameters for ADC
    uint32_t ADC_reg_config;
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_VREF);
    // Configure ADC
    adc::config(ADC_reg_config);
    adc::setup();
    // Double check that the number of samples we want to take in doesn't overflow the ADC ring buffer space
    if (number_samples > 3 * SAMPLE_LENGTH) {
        number_samples = 3 * SAMPLE_LENGTH;
    }
    Serial.println("3 - ADC setup done");
    Serial.println();
    // Sampling Setup (STOP) ====================================================================================================

    // Digital Signal Processing Setup (START) ====================================================================================================
    // Fill up buffers with 0s first to not get unexpected errors
    samplesFiltered = filter_butterwort_2th_order_50kHz(samplesRawForDSP);
    FFTResultsRaw = FFT_raw(samplesFiltered);
    FFTResults = FFT_mag(FFTResultsRaw);
    peaks = peak_detection(FFTResultsRaw, FFTResults);
    lengthOfPeakArray = peaks[0][0];
    Serial.println("4 - DSP Setup Complete");
    Serial.println();
    // Digital Signal Processing Setup (STOP) ====================================================================================================

    // Ethernet Setup PART 2 (START) ====================================================================================================
    /*
    NOTE: This code HAS to come after "Sampling Setup" otherwise some values are configured incorrectly
    Why? I have no Idea, some memory magic probably =_=
    */
    // Wait until someone is coected and sends SKIP request to indicate they are ready to start receiving data
    Serial.println("5 - Waiting for client connection...");
    communicationTeensy();
    Serial.println("5 - Client CONNECTED");
    Serial.println();
    // Ethernet Setup PART 2 (STOP) ====================================================================================================

    Serial.println();
    Serial.println("==================================================");
    Serial.println("SETUP COMPLETE :D");
    Serial.println("==================================================");
    Serial.println();
}

void loop() {
    // Sampling (START) ====================================================================================================
    // Start sampling
    adc::startConversion(sample_period, adc::BLOCKING);
    // Start sampling into the buffer
    uint8_t buffer_to_check = adc::active_buffer;

    /*
    !IMPORTANT!
    ! Digital Signal Processing (DSP) MUST be FASTER than Sampling !
    ! Now the whole DSP + peak detection (PD) is less than 700 us, this is good !
    ! DSP + PD should NEVER be over 2000 us, this will CRASH the system !

    Have a endless loop that samples signals
    Process signal and check for any peaks
    If peaks of interest detected exit loop and process data further down the line

    If sampling takes to long to find a peak we exit the loop and try again later
    */
    int32_t frequencyOfInterestMax = frequencyOfInterest + frequencyVariance;
    int32_t frequencyOfInterestMin = frequencyOfInterest - frequencyVariance;
    uint8_t found = 0;
    unsigned long samplingStartTime = millis();
    while (!found) {
        // Wait until first ring buffer is filled
        while (!adc::buffer_filled[buffer_to_check])
            ;

        // Save raw sampled data
        for (uint16_t i = 0; i < SAMPLE_LENGTH; i++) {
            samplesRawForDSP[i] = (int16_t)adc::channel_buff_ptr[1][buffer_to_check][i];
        }

        // Digital Signal Processing (START) ====================================================================================================
        // Filter raw samples
        samplesFiltered = filter_butterwort_2th_order_50kHz(samplesRawForDSP);

        // Preform FFT calculations on filtered samples
        FFTResultsRaw = FFT_raw(samplesFiltered);
        FFTResults = FFT_mag(FFTResultsRaw);

        // Get peaks of frequencies that might be of interest and their useful information like amplitude, frequency and phase
        peaks = peak_detection(FFTResultsRaw, FFTResults);

        /*
        Since we are storing the length of the array in the first index, we do not start from 0 in the array when printing out. 
        Find out how to get length of a 2D array of a q31_t datatype. 
        For now we return the length of the array in the first index of 2D array, This must be solved
        this is NOT a good solution.
        */
        lengthOfPeakArray = peaks[0][0];

        /*
        TIPS: For getting phase of the peak FFTs from q31 format that we dont understand to radians in floats, use this:
        phaseQ31_to_radianFloat32(peaks[x][2]);
        */
        // Digital Signal Processing (STOP) ====================================================================================================

        // Check if any of the peaks are of interest
        for (int i = 1; i < lengthOfPeakArray; i++) {
            int32_t peakFrequency = peaks[i][1];
            if ((peakFrequency < frequencyOfInterestMax) && (peakFrequency > frequencyOfInterestMin)) {
                found++;
            }
        }

        // Increment buffer to check
        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);

        // Check if sampling has taken to long and if so exit the loop and try again later
        if ((millis() - samplingStartTime) > SAMPLING_TIMEOUT) {
            break;
        }
    }
    // After finding peaks of interest let the last sampling sequence finish
    while (!adc::buffer_filled[buffer_to_check])
        ;
    // Stop Sampling
    adc::stopConversion();

    // Process data from the ring-buffer
    // Saving data into array we will use further down the line
    uint16_t index = 0;
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        // Combine all 3 buffers from chanels into one BIG array
        for (uint16_t u = 0; u < SAMPLE_LENGTH; u++) {
            index = (SAMPLE_LENGTH * i) + u;

            samplesRawHydrophone1[index] = (int16_t)adc::channel_buff_ptr[1][buffer_to_check][index];
            samplesRawHydrophone2[index] = (int16_t)adc::channel_buff_ptr[2][buffer_to_check][index];
            samplesRawHydrophone3[index] = (int16_t)adc::channel_buff_ptr[3][buffer_to_check][index];
            samplesRawHydrophone4[index] = (int16_t)adc::channel_buff_ptr[4][buffer_to_check][index];
            samplesRawHydrophone5[index] = (int16_t)adc::channel_buff_ptr[0][buffer_to_check][index];
        }
        buffer_to_check = (buffer_to_check + 1) % BUFFER_PER_CHANNEL;
    }
    // Clean ring-buffers
    // this is done so that next time we can add new data into the ring-buffers
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        adc::buffer_filled[i] = 0;
    }
    // Sampling (STOP) ====================================================================================================

    // Send data (START) ====================================================================================================
    Serial.println("Waiting for clients requests...");
    communicationTeensy();
    Serial.println("Data transfer complete");
    Serial.println();
    // Send data (STOP) ====================================================================================================
}

void communicationTeensy() {
    // Necessary ethernet variables ==========
    char* messageToReceive;
    char tempCharA = '0';
    char tempCharB = '0';
    int32_t* frequencyDataFromClient;

    // Endless loop until SKIP is sent back
    while (true) {
        // wait until a request is sent from client
        while (!ethernetModule::UDP_check_if_connected())
            ;
        messageToReceive = ethernetModule::UDP_read_message();
        tempCharA = messageToReceive[0];
        tempCharB = messageToReceive[1];

        // Check witch request is being sent
        // ss - Send SKIP request
        if ((tempCharA == 's') && (tempCharB == 's')) {
            break;
        }
        // sf - Send Frequency data
        if ((tempCharA == 's') && (tempCharB == 'f')) {
            frequencyDataFromClient = teensyUDP::frequency_data_from_client();
            frequencyOfInterest = frequencyDataFromClient[0];
            frequencyVariance = frequencyDataFromClient[1];
        }
        // gh - Get Hydrophone data
        if ((tempCharA == 'g') && (tempCharB == 'h')) {
            teensyUDP::send_hydrophone_data(samplesRawHydrophone1, (SAMPLE_LENGTH * 3));
            teensyUDP::send_hydrophone_data(samplesRawHydrophone2, (SAMPLE_LENGTH * 3));
            teensyUDP::send_hydrophone_data(samplesRawHydrophone3, (SAMPLE_LENGTH * 3));
            teensyUDP::send_hydrophone_data(samplesRawHydrophone4, (SAMPLE_LENGTH * 3));
            teensyUDP::send_hydrophone_data(samplesRawHydrophone5, (SAMPLE_LENGTH * 3));
        }
        // gd - Get Digital Signal Processing (DSP) data
        if ((tempCharA == 'g') && (tempCharB == 'd')) {
            teensyUDP::send_samples_raw_data(samplesRawForDSP, SAMPLE_LENGTH);
            teensyUDP::send_samples_filtered_data(samplesFiltered, SAMPLE_LENGTH);
            teensyUDP::send_FFT_data(FFTResults, SAMPLE_LENGTH);
            teensyUDP::send_peak_data(peaks, lengthOfPeakArray);
        }
        ethernetModule::UDP_clean_message_memory();
    }
    ethernetModule::UDP_clean_message_memory();
}