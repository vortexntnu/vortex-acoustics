
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

// Variables for Sampling ==========
float sample_period = 2.4;     // >= MIN_SAMP_PERIOD_BLOCKING, Recomended: 2.4
#define SAMPLING_TIMEOUT 10000 // [ms]

// Variables for Digital Signal Processing ==========

// Variables for Multilateration ==========

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
    setupTeensyCommunication();

    Serial.println("Client CONNECTED");
    // Ethernet Setup (STOP) ====================================================================================================

    // Sampling Setup (START) ====================================================================================================

    Serial.println("3 - Sampling Setup");
    adc_init();

    const uint32_t ADC_reg_config = (1 << CONFIG_WRITE_EN) | (1 << CONFIG_PD_D) | (1 << CONFIG_REFEN) | (0x3FF << CONFIG_REFDAC) | (1 << CONFIG_VREF);

    adc_config(ADC_reg_config);
    setup();

    Serial.println("Sampling Setup complete");
    // Sampling Setup (STOP) ====================================================================================================

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

    adc_start_conversion(sample_period, BLOCKING);

    while (not_found) {
        while (!(buffer_filled & (1 << buffer_to_check)))
            ;
        // Digital Signal Processing (START) ====================================================================================================

        if (dsp_find_signal(buffer_to_check)) {
            break;
        }

        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);

        if (millis() - samplingStartTime > SAMPLING_TIMEOUT) {
            break;
        }
    }

    // We make sure the last buffer that we are interested in is filled before continuing
    // This ensures we have the not only the data signal of the peak, but also what happens after the peaks in the signal frequency we are interested in
    // startConversion(sample_period, adc::BLOCKING);
    for (int i = 2; i < BUFFER_PER_CHANNEL; i++) {
        while (!(buffer_filled & (1 << buffer_to_check)))
            ;
        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);
    }

    adc_stop_conversion();
    Serial.println("1 - SAMPLING: Stoped sampling");

    buffer_filled = 0;

    // Sampling (STOP) ====================================================================================================

    // Multilateration (START) ====================================================================================================

    if (find_pinger_position()) {
        Serial.println("Multilateration failed");
    }

    Serial.println("2 - MULTILATERATION: Started the Calculations");

    Serial.println("2 - MULTILATERATION: Got the results");
    // Multilateration (STOP) ====================================================================================================

    // Send data (START) ====================================================================================================
    Serial.println("3 - DATA SEND: Start sending data");

    sequence = 0;

    for (int i = 0; i < NUM_HYDROPHONES; i++) {
        send_data_udp(samples_raw_hydrophones[i], sizeof(int16_t) * RAW_HYDROPHONE_SIZE);
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
