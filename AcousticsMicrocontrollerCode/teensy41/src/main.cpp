// Arduino Libraries
#include <Arduino.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

#include <stdlib.h>

// Sampling Analog to Digital Converter (ADC) Libraries
#include "adc.h"
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

    Serial.println("2 - Ethernet Setup");
    UDP_init();


    Serial.println("Waiting for client connection...");
    while (!UDP_check_if_connected())
        ;

    Serial.println("Waiting for client configuration...");
    setupTeensyCommunication();

    Serial.println("Client CONNECTED");
    // Ethernet Setup (STOP) ====================================================================================================

    // Sampling Setup (START) ====================================================================================================

    Serial.println("3 - Sampling Setup");
    adc_init();


    adc_config(ADC_reg_config);
    adc_setup();

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

    adc_start_conversion(SAMPLE_PERIOD, BLOCKING);

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
    Serial.println("2 - MULTILATERATION: Started the Calculations");

    if (find_pinger_position()) {
        Serial.println("Multilateration failed");
    }

    Serial.println("2 - MULTILATERATION: Got the results");
    // Multilateration (STOP) ====================================================================================================

    // Send data (START) ====================================================================================================
    Serial.println("3 - DATA SEND: Start sending data");
    
    transmit_data_udp();
  
    Serial.println("3 - DATA SEND: Data sent sucsessfully");
    // Send data (STOP) ====================================================================================================

    Serial.println("--------------------------------------------------");
}
