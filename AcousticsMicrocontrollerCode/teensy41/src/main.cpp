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
#include "corelation.h"



// Variables for Sampling ==========

int16_t samplesRawHydrophone1[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH * BUFFER_PER_CHANNEL];

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
    
    // timeDifferenceOfArrival[0] = 1.0;
    // timeDifferenceOfArrival[1] = 2.0;
    // timeDifferenceOfArrival[2] = 3.0;
    // timeDifferenceOfArrival[3] = 4.0;

    // soundLocation[0] = 7.0;
    // soundLocation[1] = 8.0;
    // soundLocation[2] = 9.0;

    // Thomas sin kode
    {



        Multilateration::Pos real_pinger_pos(-30, 60, 100);
        Serial.println("Real: ");
        real_pinger_pos.display();
        

        std::vector<float32_t> TDOA;

        // Testing
        for (int i = 0; i < TDOA_DATA_LENGHT; i++){
            TDOA.push_back(Multilateration::calculate_tdoa(Multilateration::hydrophonePositions[i+1], real_pinger_pos, 1500));
        }


        
        
        int hydrophone_data_array[NUM_HYDROPHONES][HYDROPHONE_DATA_SIZE] = {0};

        for (int i = 0; i < HYDROPHONE_DATA_SIZE; i++){
            hydrophone_data_array[0][i]=hydrophone0[i];
        }
        

        for (int i = 0; i < NUM_HYDROPHONES-1; i++) {
            int timelaged_array[HYDROPHONE_DATA_SIZE];
            add_timelag_array(hydrophone0, HYDROPHONE_DATA_SIZE, TDOA[i], SAMPLE_RATE, timelaged_array);
            
            for (int j = 0; j < HYDROPHONE_DATA_SIZE; j++){
                hydrophone_data_array[i+1][j] = timelaged_array[j];
            }
        }


        

        Multilateration::Pos pinger = Multilateration::find_pinger(hydrophone_data_array, TDOA_VALUES, SAMPLE_RATE);
        Serial.println("Calculated: ");
        pinger.display();
    }



    Serial.println("--------------------------------");
    Serial.println();
    Serial.println("2 - MULTILATERATION: Got the results");
    // Multilateration (STOP) ====================================================================================================

    Serial.print("Round: ");Serial.print(test_i);Serial.println();test_i++;

    // A small delay for debugging (Delete later)
    // delay(4000);
}

