
/*
Main Firmware code for Acoustics PCB

How the code works:
- Conncets to the Ethernet and waits for the Client
- Sets parameters specified by the Client. like frequency of interest
- Confgures ADC on the Acoustics PCB 

- Start endless loop of Sampling
- Does DSP (Digital Signal Processing) on the signals collected
- Checks for frequencies of interest in the signal
- Multilaterates the signal with the frequency of interest to find out the sound source it is comming from and how far away we are from it (OBS! NOT IMPLEMENTED YET, to be continued.....)
- Sends all the frequency of interest DSP data, Hydrophone recorded signal data and Multilaterated data back to the Client
- The infinite loop continues 

Code written by: Vortex NTNUs
All rights reserved to: Vortex NTNU
License: MIT
*/

// CMSIS Libraries
#include "Include/arm_const_structs.h"
#include "Include/arm_math.h"

// Arduino Libraries
#include <Arduino.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include <stdlib.h>

// Sampling Analog to Digital Converter (ADC) Libraries
#include "GPT.h"
#include "adc.h"
#include "arm_math.h"
#include "clock.h"
#include "gpio.h"
#include "gpio_interrupt.h"
#include "multilateration.h"
#include "pit.h"

// Digital Signal Processing (DSP) Libraries
#include "dsp.h"

// Libraries for Ethernet
#include "ethernet_module.h"
#include "stack/fnet_stdlib.h"
#include "teensy_udp.h"

#define MAX_LAG (2 * RAW_HYDROPHONE_SIZE - 1)

// Variables for Sampling ==========
float sample_period = 2.4;     // >= MIN_SAMP_PERIOD_BLOCKING, Recomended: 2.4
#define SAMPLING_TIMEOUT 10000 // [ms]

// Variables for Digital Signal Processing ==========
int16_t samplesRawForDSP[SAMPLE_LENGTH] = {0};
q15_t samplesFiltered[SAMPLE_LENGTH] = {0};
q15_t FFTResultsRaw[2 * SAMPLE_LENGTH] = {0};
q15_t FFTResultsMagnified[SAMPLE_LENGTH] = {0};
Peak peaks_buffer[SAMPLE_LENGTH];
size_t samples_interest = 32;
size_t num_peaks;

// Variables for Peak Detection ==========
int32_t frequenciesOfInterest[FREQUENCY_LIST_LENGTH];    // 0 Hz
int32_t frequencyVariances[FREQUENCY_LIST_LENGTH];       // +-0 Hz
int32_t frequenciesOfInterestMax[FREQUENCY_LIST_LENGTH]; // 0 Hz
int32_t frequenciesOfInterestMin[FREQUENCY_LIST_LENGTH]; // 0 Hz

// Variables for Multilateration ==========
#define TDOA_DATA_LENGTH 5                           // TODO: Should be moved into multilateration library once that is operational
#define POSITION_DATA_LENGTH 3 + 1                   // TODO: Should be moved into multilateration library once that is operational
float32_t timeDifferenceOfArrival[TDOA_DATA_LENGTH]; // time difference for hydrophone 1, 2, 3, 4, 5 [s]
float32_t soundLocation[POSITION_DATA_LENGTH];       // X, Y, Z [m]

// Variables for data transmission ==========
int32_t lastSendTime = 0;

void setup() {
    // Debugging Setup (START) ====================================================================================================
    Serial.begin(9600);
    delay(5000); //  pause to giving time to enter serial monitor
    Serial.println("1 - Debuging Setup");

    // Debugging Setup (STOP) ====================================================================================================

    // Ethernet Setup (START) ====================================================================================================
    /*
    NOTE: This code NEEDS to come befor "Sampling Setup", otherwise some PINS and values are configured incorrectly in in Comunications
    Why? I have no Idea, some configuration of the ISP protocol clock timer and PINS that both Comuniaction and Sampling codes uses from what it seems, probably... =_=
    */

    // Ethernet init
    Serial.println("2 - Ethernet Setup");
    UDP_init();

    // delay(20000); //  Test stuff

    // Wait until someone is connected and get their IP and Port address
    Serial.println("Waiting for client connection...");
    while (!UDP_check_if_connected())
        ;

    // Wait for client input into what frequencies we sould detect and send sound signals of
    Serial.println("Waiting for client configuration...");
    setupTeensyCommunication(frequenciesOfInterest, frequencyVariances);

    for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
        frequenciesOfInterestMax[i] = frequenciesOfInterest[i] + frequencyVariances[i];
        frequenciesOfInterestMin[i] = frequenciesOfInterest[i] - frequencyVariances[i];
    }
    Serial.println("Client CONNECTED");
    // Ethernet Setup (STOP) ====================================================================================================

    // Sampling Setup (START) ====================================================================================================

    Serial.println("3 - Sampling Setup");
    adc::init();

    const uint32_t ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_VREF);

    adc::config(ADC_reg_config);
    adc::setup();

    Serial.println("Sampling Setup complete");
    // Sampling Setup (STOP) ====================================================================================================

    // Digital Signal Processing Setup (START) ====================================================================================================
    Serial.println("4 - DSP Setup");
    // Fill up buffers with 0s first to not get unexpected errors

    Serial.println("DSP Setup Complete");
    // Digital Signal Processing Setup (STOP) ====================================================================================================

    Serial.println("==================================================");
    Serial.println("SETUP COMPLETE :D");
    Serial.println("==================================================");
}

void loop() {
    // Sampling (START) ====================================================================================================
    /*
    !IMPORTANT!
    ! Digital Signal Processing (DSP) MUST be FASTER than Sampling !
    ! Now the whole DSP + peak detection (PD) is less than 700 us, this is good !
    ! DSP + PD should NEVER be over 2000 us, this will CRASH the system !
    */
    Serial.println("1 - SAMPLING: Start Sampling");

    bool not_found = true;
    uint8_t buffer_to_check = 0;
    unsigned long samplingStartTime = millis();

    adc::startConversion(sample_period, adc::BLOCKING);

    while (not_found) {
        while (!(adc::buffer_filled & (1 << buffer_to_check)))
            ;
        // Digital Signal Processing (START) ====================================================================================================

        filter_butterworth_1st_order_50kHz(adc::samplesRawHydrophones[0] + (buffer_to_check * SAMPLE_LENGTH_ADC), samplesFiltered);

        FFT_raw(samplesFiltered, FFTResultsRaw);
        FFT_mag(FFTResultsRaw, FFTResultsMagnified);

        if (peak_detection(FFTResultsRaw, FFTResultsMagnified, samples_interest, peaks_buffer, SAMPLE_LENGTH, &num_peaks)) {
            continue;
        }

        for (size_t i = 0; i < num_peaks; i++) {
            int32_t peakFrequency = peaks_buffer[i].frequency;
            for (int j = 0; j < FREQUENCY_LIST_LENGTH; j++) {
                if ((peakFrequency < frequenciesOfInterestMax[j]) && (peakFrequency > frequenciesOfInterestMin[j])) {
                    not_found = false;
                    break;
                }
            }
        }

        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);

        if (millis() - samplingStartTime > SAMPLING_TIMEOUT) {
            break;
        }
    }

    // We make sure the last buffer that we are interested in is filled before continuing
    // This ensures we have the not only the data signal of the peak, but also what happens after the peaks in the signal frequency we are interested in
    // adc::startConversion(sample_period, adc::BLOCKING);
    for (int i = 1; i < BUFFER_PER_CHANNEL; i++) {
        while (!(adc::buffer_filled & (1 << buffer_to_check)))
            ;
        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);
    }

    adc::stopConversion();
    Serial.println("1 - SAMPLING: Stoped sampling");

    adc::buffer_filled = 0;

    // Sampling (STOP) ====================================================================================================

    // Multilateration (START) ====================================================================================================

    Serial.println("2 - MULTILATERATION: Started the Calculations");

    timeDifferenceOfArrival[0] = 0;
    q15_t correlation_array[MAX_LAG];
    q15_t max_number;
    size_t peak_index;

    for (int i = 1; i < NUM_HYDROPHONES; i++) {
        arm_correlate_q15(adc::samplesRawHydrophones[0], RAW_HYDROPHONE_SIZE, adc::samplesRawHydrophones[i], RAW_HYDROPHONE_SIZE, correlation_array);

        arm_max_q15(correlation_array, MAX_LAG, &max_number, &peak_index);

        timeDifferenceOfArrival[i] = ((float32_t)(peak_index - RAW_HYDROPHONE_SIZE) / SAMPLE_RATE);
    }

    if (tdoa_multilateration(hydrophonePositions, timeDifferenceOfArrival + 1, soundLocation)) {
        Serial.println("Multilateration failed");
    }

    Serial.println("2 - MULTILATERATION: Got the results");
    // Multilateration (STOP) ====================================================================================================

    // Send data (START) ====================================================================================================
    Serial.println("3 - DATA SEND: Start sending data");

    for (int i = 0; i < NUM_HYDROPHONES; i++) {
        send_data_udp(adc::samplesRawHydrophones[i], sizeof(int16_t) * RAW_HYDROPHONE_SIZE);
    }

    send_data_udp(samplesFiltered, sizeof(q15_t) * SAMPLE_LENGTH);
    send_data_udp(FFTResultsMagnified, sizeof(q15_t) * SAMPLE_LENGTH);

    // send_peak_data(peaks, lengthOfPeakArray);

    send_data_udp(timeDifferenceOfArrival, sizeof(float32_t) * TDOA_DATA_LENGTH);
    send_data_udp(soundLocation, sizeof(float32_t) * POSITION_DATA_LENGTH);

    UDP_clean_message_memory();
    Serial.println("3 - DATA SEND: Data sent sucsessfully");
    // Send data (STOP) ====================================================================================================

    Serial.println("--------------------------------------------------");
}
