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

#include "DSP.h"
#include "adc.h"

// Thomas del
#include "test_data.h"
#include "tdoa.h"
#include "add_timelag.h"
#include "pinger_finder.h"
#include "corelation.h"



// Variables for Sampling ==========

int16_t samplesRawHydrophone1[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];

// Variables for Multilateration ==========
#define TDOA_DATA_LENGHT 5 // TODO: Should be moved into multilateration library once that is operational
#define POSITION_DATA_LENGHT 3 // TODO: Should be moved into multilateration library once that is operational
double timeDifferenceOfArrival[TDOA_DATA_LENGHT]; // time difference for hydrophone 1, 2, 3, 4, 5 [s]
double soundLocation[POSITION_DATA_LENGHT]; // X, Y, Z [m]

int test_i = 0;

void setup() {
    // Debugging Setup (START) ====================================================================================================
    Serial.begin(9600);
    delay(5000); //  pause to giving time to enter serial monitor
    Serial.println("1 - Debugging Setup");
    Serial.println();
    // Debugging Setup (STOP) ====================================================================================================



    // Ready test data
}



void loop() {
    // Multilateration (START) ====================================================================================================
    // TODO: It is up to you my student finish acoustics for us T^T
    Serial.println("2 - MULTILATERATION: Started the Calculations");
    Serial.println();
    Serial.println("--------------------------------");
    
    timeDifferenceOfArrival[0] = 1.0;
    timeDifferenceOfArrival[1] = 2.0;
    timeDifferenceOfArrival[2] = 3.0;
    timeDifferenceOfArrival[3] = 4.0;
    timeDifferenceOfArrival[4] = 5.0;

    soundLocation[0] = 7.0;
    soundLocation[1] = 8.0;
    soundLocation[2] = 9.0;

    // Thomas sin kode
    {


        std::vector<Pos> hydrophone_array = {
            {1.0, 1.0, 1.0},
            {1.2, 0.8, 1.1},
            {0.9, 1.3, 1.2},
            {1.1, 1.4, 0.9}
        };

        Pos real_pinger_pos(-30, 60, 50);
        

        std::vector<float32_t> TDOA;
        TDOA.reserve(hydrophone_array.size());
        for (int i = 0; i < hydrophone_array.size(); i++){
            
            TDOA.push_back(calculate_tdoa(hydrophone_array.at(i), real_pinger_pos, 1500));
        }

        Serial.println("Real: ");
        real_pinger_pos.display();


        

        std::vector<float32_t> calculated_TDOA;

        for (int i = 0; i < 4; i++) {
            // Testing
            int* new_array = add_timelag_array(hydrophone0, HYDROPHONE_DATA_SIZE_TESTING, TDOA[i], SAMPLE_RATE);

            // Compute cross-correlation (convolution) for all possible lags
            int max_lag = 2 * HYDROPHONE_DATA_SIZE_TESTING - 1;  // Total number of possible shifts
            int32_t correlation_array[max_lag];
            crosscorelation(hydrophone0, new_array, HYDROPHONE_DATA_SIZE_TESTING, correlation_array);

            // Find peak of the cross corelation, thats where TDOA is located
            int peek_index = find_peak_index(correlation_array, max_lag);

            // Calculate the TDOA in seconds
            // Explain here in TDOA why subtract by HYDROPHONE_DATA_SIZE_TESTING (Because cross corelation produces lag in positive and negative +- HYDROPHONE_DATA_SIZE_TESTING), we want it in time format to be corret thus we subtract
            double tdoa = ((float)(peek_index - HYDROPHONE_DATA_SIZE_TESTING)/SAMPLE_RATE);

            // Testing
            // Serial.println(tdoa, 10);


            calculated_TDOA.push_back(tdoa);
        }

        Pos pinger = tdoa_multilateration(hydrophone_array, calculated_TDOA);
        Serial.println("Calculated: ");
        pinger.display();
    }



    Serial.println("--------------------------------");
    Serial.println();
    Serial.println("2 - MULTILATERATION: Got the results");
    // Multilateration (STOP) ====================================================================================================

    Serial.print("Round: ");Serial.print(test_i);Serial.println();test_i++;

    // A small delay for debugging (Delete later)
    delay(4000);
}

