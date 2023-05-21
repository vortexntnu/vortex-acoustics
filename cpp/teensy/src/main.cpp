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

// Libraries for ethernet
#include "ethernetModule.h"

// ethernet variables
int16_t packetSize = 0;
char* messageToReceive;
char messageToSend[] = "AB";

void setup() {
    Serial.begin(9600);
    while (!Serial)
        ;
    Serial.println("Serial connected\r\n");

    ethernetModule::UDP_init();
}

void loop() {
    // Check if data is being transferred to 
    packetSize = ethernetModule::UDP_check_if_connected();

    if (packetSize > 0) {
        messageToReceive = ethernetModule::UDP_read_message();
        Serial.println(messageToReceive);

        ethernetModule::UDP_send_message(messageToSend);

        //Serial.println(messageToSend);
    }

    ethernetModule::UDP_clean_message_memory();
}
