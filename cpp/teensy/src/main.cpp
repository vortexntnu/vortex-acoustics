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
float sample_period = 2.3; // >= MIN_SAMP_PERIOD_TIMER
int16_t samplesRawHydrophone1[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone2[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone3[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone4[SAMPLE_LENGTH * 3];
int16_t samplesRawHydrophone5[SAMPLE_LENGTH * 3];
#define SAMPLING_TIMEOUT 10000 // [10 s] If sampling takes to long before finding a frequency of interest we exit the loop and later try again
#define BUFFER_TIMEOUT 100 // [0.1 s] If buffer takes to long time to fill up with values exit the buffer sample loop and later try again

// Variables for Digital Signal Processing ==========
int16_t samplesRawForDSP[SAMPLE_LENGTH];
q15_t* samplesFiltered;
q15_t* FFTResultsRaw;
q15_t* FFTResults;
std::vector<std::vector<q31_t>> peaks;
int16_t lengthOfPeakArray;
double timeDifferenceOfArrival[5]; // X, Y, Z
double soundLocation[3]; // X, Y, Z
// Variables for Peak Detection ==========
int32_t frequenciesOfInterest[FREQUENCY_LIST_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 0 Hz
int32_t frequencyVariances[FREQUENCY_LIST_LENGTH] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};   // +-0 Hz

int32_t frequenciesOfInterestMax[FREQUENCY_LIST_LENGTH];
int32_t frequenciesOfInterestMin[FREQUENCY_LIST_LENGTH];

// Variables for data transmission ==========
#define DATA_SEND_PAUSE 0
int32_t lastSendTime = 0;
uint8_t* clientIP;
uint16_t clientPort;
void setupTeensyCommunication();
void sendDataToClient();
// void communicationTeensy();

void setup() {
    Serial.begin(9600);
    delay(5000); // 1 second pause for giving time to enter serial monitor
    Serial.println("1 - Serial connected");
    Serial.println();

    // Ethernet Setup PART 1 (START) ====================================================================================================
    /*
    NOTE: This code HAS to come befor "Sampling Setup", otherwise some values are configured incorrectly
    Why? I have no Idea, some memory magic probably =_=
    */
    // Ethernet init
    Serial.print("2 - Ethernet Configuration");
    ethernetModule::UDP_init(); // This breaks everything
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
    samplesFiltered = DigitalSignalProcessing::filter_butterwort_9th_order_50kHz(samplesRawForDSP);
    FFTResultsRaw = DigitalSignalProcessing::FFT_raw(samplesFiltered);
    FFTResults = DigitalSignalProcessing::FFT_mag(FFTResultsRaw);
    peaks = DigitalSignalProcessing::peak_detection(FFTResultsRaw, FFTResults);
    lengthOfPeakArray = peaks[0][0];
    Serial.println("4 - DSP Setup Complete");
    Serial.println();
    // Digital Signal Processing Setup (STOP) ====================================================================================================

    // Ethernet Setup PART 2 (START) ====================================================================================================
    /*
    NOTE: This code HAS to come after "Digital Signal Processing Setup" 
    Otherwise when client request some data, the data we are pointing to has not been setup yet
    This will cause Teensy to look for data that doesn't exist and will crash the system O_O    
    NOTE: This code HAS to come after "Digital Signal Processing Setup" 
    Otherwise when client request some data, the data we are pointing to has not been setup yet
    This will cause Teensy to look for data that doesn't exist and will crash the system O_O    
    */
    // Wait until someone is connected and get their IP and Port address
    Serial.println("5 - Waiting for client connection...");
    while (!ethernetModule::UDP_check_if_connected());

    clientIP = ethernetModule::get_remoteIP();
    clientPort = ethernetModule::get_remotePort();
    Serial.println("5 - Waiting for client configuration...");
    setupTeensyCommunication();
    Serial.println("5 - Client CONNECTED");
    Serial.println();
    // Ethernet Setup PART 2 (STOP) ====================================================================================================

    for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
        frequenciesOfInterestMax[i] = frequenciesOfInterest[i] + frequencyVariances[i];
        frequenciesOfInterestMin[i] = frequenciesOfInterest[i] - frequencyVariances[i];
    }

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
    Serial.println("Start Sampling");

    uint8_t found = 0;
    uint8_t buffer_to_check = adc::active_buffer;
    unsigned long samplingStartTime = millis(); // For sampling timeout, in case we sample for to long, we want to break the loop
    unsigned long bufferStartTime = millis(); // For buffer timeout, in case the buffer stutters we want to make sure we can handle the error nd restart the sampling into the buffer
    // unsigned long printBeforeTimer = millis() + 2000;
    // unsigned long printAfterTimer = millis() + 500;
    while (!found) {
        Serial.println("Debuging God help me not this hsit again");
        Serial.println(buffer_to_check);
        Serial.println(adc::active_buffer);
        delay(1000);
        // Start sampling into the buffer
        // Sampling ONLY using BLOCKING parameter, others are not implemented
        adc::startConversion(sample_period, adc::BLOCKING);

        // Wait until ring buffer is filled
        // NOTE: The code is not good, so sometimes the buffer stutters and doesent recognize it is filled
        // Why? I dont know. But something in sampling code probably goes out of memory bound
        // So the solution is that we have a timout handler, usually sampling takes a few milliseconds to complete, if it goes over 100 ms it is safe to say it has crashed, so we just time it out
        while (!adc::buffer_filled[buffer_to_check]) {
            if (millis() - bufferStartTime > BUFFER_TIMEOUT) {
                Serial.print("Time: ");
                Serial.println(millis() - bufferStartTime);
                Serial.println("Buffer timed out");
                Serial.print("sample_index:   ");Serial.println(adc::sample_index);
                

                // Restart timer
                bufferStartTime = millis();
                break;
            }
        }
        
        // Stop sampling into the buffer
        adc::stopConversion();
        
        Serial.println("Debuging Thank you iyts not this");
        delay(1000);

        // Save raw sampled data
        for (uint16_t i = 0; i < SAMPLE_LENGTH; i++) {
            samplesRawForDSP[i] = (int16_t)adc::channel_buff_ptr[1][buffer_to_check][i];
        }

        // Increment buffer to check
        buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);

        // Digital Signal Processing (START) ====================================================================================================
        // Filter raw samples
        samplesFiltered = DigitalSignalProcessing::filter_butterwort_1th_order_50kHz(samplesRawForDSP);

        // Preform FFT calculations on filtered samples
        FFTResultsRaw = DigitalSignalProcessing::FFT_raw(samplesFiltered);
        FFTResults = DigitalSignalProcessing::FFT_mag(FFTResultsRaw);

        // Get peaks of frequencies that might be of interest and their useful information like amplitude, frequency and phase
        peaks = DigitalSignalProcessing::peak_detection(FFTResultsRaw, FFTResults);

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
        // TODO: David plz all frequency of interest not only 1, JA helt enig compy pasete ikke bruke for loop, det er for pyser bruker for lite flash minne
        for (int i = 1; i < lengthOfPeakArray; i++) {
            int32_t peakFrequency = peaks[i][1];
            // Serial.print("Debuging: Peaks: ");Serial.print(frequenciesOfInterestMin[0]);Serial.print(" < ");Serial.print(peakFrequency);Serial.print(" < ");Serial.print(frequenciesOfInterestMax[0]);
            // Serial.println();
            // delay(1000);
            if ((peakFrequency < frequenciesOfInterestMax[0]) && (peakFrequency > frequenciesOfInterestMin[0])) {
                found = 1;
            }
        }
        // // Increment buffer to check
        // buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);

        // // Stop sampling
        // adc::stopConversion();
        
        
        // Check if sampling has taken to long and if so exit the loop and try again later
        if (millis() - samplingStartTime > SAMPLING_TIMEOUT) {
            Serial.print("Time: ");
            Serial.println(millis() - samplingStartTime);
            Serial.println("Sampling timed out");
            break;
        }
    }

    // if (millis() - printAfterTimer > 500) {
    // Serial.println("Exited loop");
    // printAfterTimer = millis();
    // }

    // After finding peaks of interest let the last sampling sequence finish
    // dELETE THIS (sTART) -----
    Serial.println("Debuging AFTER: Started Sampling...");
    delay(1000);
    // dELETE THIS (STOP) -----

    adc::startConversion(sample_period, adc::BLOCKING);
    while (!adc::buffer_filled[buffer_to_check]) {
        if (millis() - bufferStartTime > BUFFER_TIMEOUT) {
            Serial.print("Time: ");
            Serial.println(millis() - bufferStartTime);
            Serial.println("Buffer timed out");
            Serial.print("sample_index:   ");Serial.println(adc::sample_index);
            
            // Restart timer
            bufferStartTime = millis();
            break;
        }
    }
    buffer_to_check = (buffer_to_check + 1) % (BUFFER_PER_CHANNEL);
    adc::stopConversion();
    Serial.println("Stoped sampling");

    // Process data from the ring-buffer sx
    // active buffer is one further than the last filled one, which is the oldest one now
    uint8_t bufferIndex = adc::active_buffer;
    bufferIndex = (bufferIndex + 1) % BUFFER_PER_CHANNEL;
    // Saving data into array we will use further down the line
    uint16_t index = 0;
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        // Combine all 3 buffers from chanels into one BIG array
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
    // Clean ring-buffers
    // this is done so that next time we can add new data into the ring-buffers
    for (uint8_t i = 0; i < BUFFER_PER_CHANNEL; i++) {
        adc::buffer_filled[i] = 0;
    }

    // dELETE THIS (sTART) -----
    Serial.println("Debuging AFTER: Hydrophone data:");
    for (int i = 0; i < SAMPLE_LENGTH*BUFFER_PER_CHANNEL; i++) {
        Serial.print(samplesRawHydrophone1[i]);Serial.print(", ");
    }
    Serial.println();
    delay(5000);
    // dELETE THIS (STOP) -----
    // Sampling (STOP) ====================================================================================================

    // Send data (START) ====================================================================================================
    Serial.println("Waiting for clients requests...");
    sendDataToClient();
    delay(3000);
    Serial.println("Data transfer complete");
    Serial.println();
    // Send data (STOP) ====================================================================================================
}


// Ditch the send skip stuff, just initialize with a "ready" message, then get the list of frequencies from the client
void setupTeensyCommunication() {
    ethernetModule::UDP_send_ready_signal(clientIP, clientPort);
    // After this, the client and teensy are connected

    while (!ethernetModule::UDP_check_if_connected())
        ;

    teensyUDP::frequency_data_from_client(frequenciesOfInterest, frequencyVariances);

    ethernetModule::UDP_clean_message_memory();
}

/*
    Simplify:
     - Constantly send data no matter if the client is ready or not, just use an interval so the user is not
       overwhelmed
*/

void sendDataToClient() {
    if (millis() - lastSendTime < DATA_SEND_PAUSE) { return; }
    lastSendTime = millis();
    
    teensyUDP::send_hydrophone_data(samplesRawHydrophone1, (SAMPLE_LENGTH * 3));
    teensyUDP::send_hydrophone_data(samplesRawHydrophone2, (SAMPLE_LENGTH * 3));
    teensyUDP::send_hydrophone_data(samplesRawHydrophone3, (SAMPLE_LENGTH * 3));
    teensyUDP::send_hydrophone_data(samplesRawHydrophone4, (SAMPLE_LENGTH * 3));
    teensyUDP::send_hydrophone_data(samplesRawHydrophone5, (SAMPLE_LENGTH * 3));

    teensyUDP::send_samples_raw_data(samplesRawForDSP, SAMPLE_LENGTH);
    teensyUDP::send_samples_filtered_data(samplesFiltered, SAMPLE_LENGTH);
    teensyUDP::send_FFT_data(FFTResults, SAMPLE_LENGTH);
    teensyUDP::send_peak_data(peaks, lengthOfPeakArray);
    
    teensyUDP::send_tdoa_data(timeDifferenceOfArrival);
    teensyUDP::send_location_data(soundLocation);

    ethernetModule::UDP_clean_message_memory();
}

