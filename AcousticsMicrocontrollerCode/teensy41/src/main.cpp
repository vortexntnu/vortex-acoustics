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



// Variables for Sampling ==========
float sample_period = 2.4; // >= MIN_SAMP_PERIOD_BLOCKING, Recomended: 2.4
#define SAMPLING_TIMEOUT 10000 // [ms]
int16_t samplesRawHydrophone1[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];

// Variables for Digital Signal Processing ==========
int16_t samplesRawForDSP[SAMPLE_LENGTH];
q15_t* samplesFiltered;
q15_t* FFTResultsRaw;
q15_t* FFTResultsMagnified;
std::vector<std::vector<q31_t>> peaks;
int16_t lengthOfPeakArray;

// Variables for Peak Detection ==========
int32_t frequenciesOfInterest[FREQUENCY_LIST_LENGTH]; // 0 Hz
int32_t frequencyVariances[FREQUENCY_LIST_LENGTH]; // +-0 Hz
int32_t frequenciesOfInterestMax[FREQUENCY_LIST_LENGTH]; // 0 Hz
int32_t frequenciesOfInterestMin[FREQUENCY_LIST_LENGTH]; // 0 Hz

// Variables for Multilateration ==========
#define TDOA_DATA_LENGHT 5 // TODO: Should be moved into multilateration library once that is operational
#define POSITION_DATA_LENGHT 3 // TODO: Should be moved into multilateration library once that is operational
double timeDifferenceOfArrival[TDOA_DATA_LENGHT]; // time difference for hydrophone 1, 2, 3, 4, 5 [s]
double soundLocation[POSITION_DATA_LENGHT]; // X, Y, Z [m]

// Variables for data transmission ==========
int32_t lastSendTime = 0;
void setupTeensyCommunication();
void sendDataToClient();



void setup() {
    // Debugging Setup (START) ====================================================================================================
    Serial.begin(9600);
    delay(5000); //  pause to giving time to enter serial monitor
    Serial.println("1 - Debuging Setup");
    Serial.println();
    // Debugging Setup (STOP) ====================================================================================================



    // Ethernet Setup (START) ====================================================================================================
    /*
    NOTE: This code NEEDS to come befor "Sampling Setup", otherwise some PINS and values are configured incorrectly in in Comunications
    Why? I have no Idea, some configuration of the ISP protocol clock timer and PINS that both Comuniaction and Sampling codes uses from what it seems, probably... =_=
    */



    // Ethernet init
    Serial.println("2 - Ethernet Setup");
    ethernetModule::UDP_init();

    // delay(20000); //  Test stuff

    // Wait until someone is connected and get their IP and Port address
    Serial.println("Waiting for client connection...");
    while (!ethernetModule::UDP_check_if_connected());

    // Wait for client input into what frequencies we sould detect and send sound signals of
    Serial.println("Waiting for client configuration...");
    teensyUDP::setupTeensyCommunication(frequenciesOfInterest, frequencyVariances);

    for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
        frequenciesOfInterestMax[i] = frequenciesOfInterest[i] + frequencyVariances[i];
        frequenciesOfInterestMin[i] = frequenciesOfInterest[i] - frequencyVariances[i];
    }
    Serial.println("Client CONNECTED");
    Serial.println();
    // Ethernet Setup (STOP) ====================================================================================================



    // Sampling Setup (START) ====================================================================================================
    // initializing ADC before being able to use it
    Serial.println("3 - Sampling Setup");
    adc::init();

    // Setup parameters for ADC
    uint32_t ADC_reg_config;
    ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_VREF);
    // Configure ADC
    adc::config(ADC_reg_config);
    adc::setup();


    Serial.println("Sampling Setup complete");
    Serial.println();
    // Sampling Setup (STOP) ====================================================================================================



    // Digital Signal Processing Setup (START) ====================================================================================================
    Serial.println("4 - DSP Setup");
    // Fill up buffers with 0s first to not get unexpected errors
    samplesFiltered = DigitalSignalProcessing::filter_butterwort_9th_order_50kHz(samplesRawForDSP);
    FFTResultsRaw = DigitalSignalProcessing::FFT_raw(samplesFiltered);
    FFTResultsMagnified = DigitalSignalProcessing::FFT_mag(FFTResultsRaw);
    peaks = DigitalSignalProcessing::peak_detection(FFTResultsRaw, FFTResultsMagnified);
    lengthOfPeakArray = peaks[0][0];
    Serial.println("DSP Setup Complete");
    Serial.println();
    // Digital Signal Processing Setup (STOP) ====================================================================================================



    Serial.println();
    Serial.println("==================================================");
    Serial.println("SETUP COMPLETE :D");
    Serial.println("==================================================");
    Serial.println();
}



void loop() {
    // Sampling (START) ====================================================================================================
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
    Serial.println("1 - SAMPLING: Start Sampling");

    uint8_t found = 0;
    uint8_t buffer_to_check = 0; // Reset buffer to start filling to 0 // this is the NOW buffer
    unsigned long samplingStartTime = millis(); // For sampling timeout, in case we sample for to long, we want to break the loop

    // Start sampling ADC data imediately
    adc::startConversion(sample_period, adc::BLOCKING);

    while (!found) {
        // Start sampling into the buffer and wait until the latest one is filled before moving on and leting it continue to fill up into the next buffer
        while (!adc::buffer_filled[buffer_to_check]);

        // Save raw sampled data from ADC
        for (uint16_t i = 0; i < SAMPLE_LENGTH; i++) {
            samplesRawForDSP[i] = (int16_t)adc::channel_buff_ptr[1][buffer_to_check][i];
        }

        // Digital Signal Processing (START) ====================================================================================================
        // Filter raw samples
        samplesFiltered = DigitalSignalProcessing::filter_butterwort_1th_order_50kHz(samplesRawForDSP);

        // Preform FFT calculations on filtered samples
        FFTResultsRaw = DigitalSignalProcessing::FFT_raw(samplesFiltered);
        FFTResultsMagnified = DigitalSignalProcessing::FFT_mag(FFTResultsRaw);

        // Get peaks of frequencies that might be of interest and their useful information like amplitude, frequency and phase
        peaks = DigitalSignalProcessing::peak_detection(FFTResultsRaw, FFTResultsMagnified);

        /*
        Since we are storing the length of the array in the first index, we do not start from 0 in the array when printing out. 
        Find out how to get length of a 2D array of a q31_t datatype. 
        For now we return the length of the array in the first index of 2D array, This must be solved
        this is NOT a good solution.
        */
        lengthOfPeakArray = peaks[0][0];

        /*
        TIPS: For getting phase of the peak FFTs from q31 format that we dont understand to radians in floats, use this:
        DigitalSignalProcessing::phaseQ31_to_radianFloat32(peaks[<x>][2]);
        */
        // Digital Signal Processing (STOP) ====================================================================================================

        // Check peak list we got from sampled frequency data and compare with the frequencies we are interested in finding
        // If the frequecies correspond, we exit the loop, else continue sampling for new sound data
        for (int i = 1; i < lengthOfPeakArray; i++) {
            int32_t peakFrequency = peaks[i][1];
            // Serial.println();
            // Serial.println("Found peak");
            for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
                if ((peakFrequency < frequenciesOfInterestMax[i]) && (peakFrequency > frequenciesOfInterestMin[i])) {
                    found = 1;
                }
            }
        }
        
        if (found) {
            //Serial.println("1 - SAMPLING: Frequency of interest found");
        }

        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);
        // Check if sampling has taken to long and if so exit the loop and try again later
        if (millis() - samplingStartTime > SAMPLING_TIMEOUT) {
            //Serial.println("1 - SAMPLING: !WARNING! Sampling timed out");
            break;
        }

        // Iterate into the next ring buffer and stop sampling for this round
    }

    // We make sure the last buffer that we are interested in is filled before continuing
    // This ensures we have the not only the data signal of the peak, but also what happens after the peaks in the signal frequency we are interested in
    // adc::startConversion(sample_period, adc::BLOCKING);

    for (uint8_t i = 0; i < 3; i++){ // This is how many buffers we want to send after the aktive_buffer in each ringbuffer
        while (!adc::buffer_filled[buffer_to_check]);
        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);
    }

    // Stop ADC sampling once we have every ring buffer sampled
    adc::stopConversion();
    Serial.println("1 - SAMPLING: Stoped sampling");

    // Process data from the ring buffers
    // active buffer is one further than the last filled one, which is the oldest one now thats why we iterate with one
    uint8_t bufferIndex = adc::active_buffer;
    // Saving finished processed and sampled Hyfrophone data
    uint16_t index = 0;
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        // Combine all buffers from chanels into one BIG array
        for (uint16_t u = 0; u < SAMPLE_LENGTH; u++) {
            index = (SAMPLE_LENGTH * i) + u;

            samplesRawHydrophone1[index] = (int16_t)adc::channel_buff_ptr[1][bufferIndex][u];
            samplesRawHydrophone2[index] = (int16_t)adc::channel_buff_ptr[2][bufferIndex][u];
            samplesRawHydrophone3[index] = (int16_t)adc::channel_buff_ptr[3][bufferIndex][u];
            samplesRawHydrophone4[index] = (int16_t)adc::channel_buff_ptr[4][bufferIndex][u];
            samplesRawHydrophone5[index] = (int16_t)adc::channel_buff_ptr[0][bufferIndex][u];
        }
        bufferIndex = (bufferIndex + 1) % BUFFER_PER_CHANNEL;
    }

    // Clean ring buffers
    // this is done so that next time we can add new data into the ring buffers withouth getting errors
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        adc::buffer_filled[i] = 0;
    }
    // Sampling (STOP) ====================================================================================================



    // Multilateration (START) ====================================================================================================
    // TODO: It is up to you my student finish acoustics for us T^T
    Serial.println("2 - MULTILATERATION: Started the Calculations");
    timeDifferenceOfArrival[0] = 1.0;
    timeDifferenceOfArrival[1] = 2.0;
    timeDifferenceOfArrival[2] = 3.0;
    timeDifferenceOfArrival[3] = 4.0;
    timeDifferenceOfArrival[4] = 5.0;

    soundLocation[0] = 7.0;
    soundLocation[1] = 8.0;
    soundLocation[2] = 9.0;
    Serial.println("2 - MULTILATERATION: Got the results");
    // Multilateration (STOP) ====================================================================================================



    // Send data (START) ====================================================================================================
    Serial.println("3 - DATA SEND: Start sending data");
    teensyUDP::send_hydrophone_data(samplesRawHydrophone1, SAMPLE_LENGTH * BUFFER_PER_CHANNEL, '1');
    teensyUDP::send_hydrophone_data(samplesRawHydrophone2, SAMPLE_LENGTH * BUFFER_PER_CHANNEL, '2');
    teensyUDP::send_hydrophone_data(samplesRawHydrophone3, SAMPLE_LENGTH * BUFFER_PER_CHANNEL, '3');
    teensyUDP::send_hydrophone_data(samplesRawHydrophone4, SAMPLE_LENGTH * BUFFER_PER_CHANNEL, '4');
    teensyUDP::send_hydrophone_data(samplesRawHydrophone5, SAMPLE_LENGTH * BUFFER_PER_CHANNEL, '5');
    
    teensyUDP::send_samples_filtered_data(samplesFiltered, SAMPLE_LENGTH);
    teensyUDP::send_FFT_data(FFTResultsMagnified, SAMPLE_LENGTH);
        
    teensyUDP::send_peak_data(peaks, lengthOfPeakArray);
    
    teensyUDP::send_tdoa_data(timeDifferenceOfArrival, TDOA_DATA_LENGHT);
    teensyUDP::send_location_data(soundLocation, POSITION_DATA_LENGHT);

    ethernetModule::UDP_clean_message_memory();
    Serial.println("3 - DATA SEND: Data sent sucsessfully");
    // Send data (STOP) ====================================================================================================


    Serial.println();
    Serial.println("--------------------------------------------------");
    Serial.println();


    // A small delay for debugging (Delete later)
    // delay(1000);
}

