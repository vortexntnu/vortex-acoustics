#include "teensyUDP.h"

// Variables
// int32_t frequencyData[2];

namespace teensyUDP {
int32_t* frequency_data_from_client(int32_t *frequenciesOfInterest, int32_t* frequencyVariances) {
    // int32_t frequenciesOfInterest[10];
    // int32_t frequencyVariances[10];

    for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
        // NOTE: to self, while loop has to be at the end othewise the first element does not get read
        char* frequencyMessage = ethernetModule::UDP_read_message();
        char* token;

        token = strtok(frequencyMessage, ",");

        frequenciesOfInterest[i] = atoi(token);
        frequencyVariances[i] = atoi(strtok(NULL, ","));

        while (!ethernetModule::UDP_check_if_connected());
    }



    // int32_t frequencyOfInterest;
    // int32_t frequencyVariance;

    // // Read frequency client is interested in
    // while (!ethernetModule::UDP_check_if_connected())
    //     ;
    // frequencyOfInterest = atoi(ethernetModule::UDP_read_message());
    // ethernetModule::UDP_clean_message_memory();

    // // Read frequency variance client wants
    // while (!ethernetModule::UDP_check_if_connected())
    //     ;
    // frequencyVariance = atoi(ethernetModule::UDP_read_message());
    // ethernetModule::UDP_clean_message_memory();

    // // Save data to a array
    // frequencyData[0] = frequencyOfInterest;
    // frequencyData[1] = frequencyVariance;

    // Return frequency data
    return 0;
}

void send_data_16Bit(int16_t* data, int16_t lengthOfData) {
    // for an int16_t, max length is 5 digits plus null terminator
    int8_t int16TypeMaxLength = 6;
    char str[int16TypeMaxLength];

    // Allocate memory to data buffer to be sent later on
    char* dataBuffer = (char*)malloc(lengthOfData * int16TypeMaxLength * sizeof(char));

    // Format char data into a list of format "x,xxx,x,xxx,....,xx"
    int16_t index = 0;
    for (int16_t i = 0; i < lengthOfData; i++) {
        // convert raw samples int to a string in decimal format
        itoa(data[i], str, 10);
        // Convert string to char and save it into buffer we send later
        // Loop until string is empty
        for (int u = 0; str[u] != '\0'; u++) {
            dataBuffer[index] = str[u];
            index++;
        }
        dataBuffer[index] = ',';
        index++;
    }

    // Send data in intervals to not overwhelm Client
    int amountLeftToSend = 0;
    while (amountLeftToSend < index) {
        // Before sending the whole big interval, just double check that all of the registers are filled up, else skip
        if ((amountLeftToSend + MAX_CLIENT_CAPACITY) < index) {
            ethernetModule::UDP_send_message(dataBuffer, MAX_CLIENT_CAPACITY, amountLeftToSend);
        }
        amountLeftToSend += MAX_CLIENT_CAPACITY;
    }
    // There will be MOST of the time some data left unsent since it was to small for the big buffer, send the rest through here
    if (amountLeftToSend != index) {
        amountLeftToSend -= MAX_CLIENT_CAPACITY;
        index -= amountLeftToSend;
        ethernetModule::UDP_send_message(dataBuffer, index, amountLeftToSend);
    }
    // Send "DONE" as a signal, to signal that data transfer is finished
    char finishingData[] = "DONE";
    ethernetModule::UDP_send_message(finishingData, 4, 0);

    // Free up allocated space since we don't use it anymore
    free(dataBuffer);
}

void send_data_32Bit(int32_t* data, int32_t lengthOfData) {
    // for an int32_t, max length is 10 digits plus null terminator
    int8_t int32TypeMaxLength = 11;
    char str[int32TypeMaxLength];

    // Allocate memory to data buffer to be sent later on
    char* dataBuffer = (char*)malloc(lengthOfData * int32TypeMaxLength * sizeof(char));

    // Format char data into a list of format "x,xxx,x,xxx,....,xx"
    int32_t index = 0;
    for (int32_t i = 0; i < lengthOfData; i++) {
        // convert raw samples int to a string in decimal format
        itoa(data[i], str, 10);
        // Convert string to char and save it into buffer we send later
        // Loop until string is empty
        for (int u = 0; str[u] != '\0'; u++) {
            dataBuffer[index] = str[u];
            index++;
        }
        dataBuffer[index] = ',';
        index++;
    }

    // Send data in intervals to not overwhelm Client
    int amountLeftToSend = 0;
    while (amountLeftToSend < index) {
        // Before sending the whole big interval, just double check that all of the registers are filled up, else skip
        if ((amountLeftToSend + MAX_CLIENT_CAPACITY) < index) {
            ethernetModule::UDP_send_message(dataBuffer, MAX_CLIENT_CAPACITY, amountLeftToSend);
        }
        amountLeftToSend += MAX_CLIENT_CAPACITY;
    }
    // There will be MOST of the time some data left unsent since it was too small for the big buffer, send the rest through here
    if (amountLeftToSend != index) {
        amountLeftToSend -= MAX_CLIENT_CAPACITY;
        index -= amountLeftToSend;
        ethernetModule::UDP_send_message(dataBuffer, index, amountLeftToSend);
    }
    // Send "DONE" as a signal, to signal that data transfer is finished
    char finishingData[] = "DONE";
    ethernetModule::UDP_send_message(finishingData, 4, 0);

    // Free up allocated space since we don't use it anymore
    free(dataBuffer);
}

void send_hydrophone_data(int16_t* hydrophone, int16_t lengthOfData) { send_data_16Bit(hydrophone, lengthOfData); }

void send_samples_raw_data(int16_t* samplesRaw, int16_t lengthOfData) { send_data_16Bit(samplesRaw, lengthOfData); }

void send_samples_filtered_data(q15_t* samplesFiltered, int16_t lengthOfData) {
    // Convert to correct datatype before sending
    int16_t tempSamplesFilteredBuffer[lengthOfData];
    for (int i = 0; i < lengthOfData; i++) {
        tempSamplesFilteredBuffer[i] = (int16_t)samplesFiltered[i];
    }
    send_data_16Bit(tempSamplesFilteredBuffer, lengthOfData);
}

void send_FFT_data(q15_t* FFTdata, int16_t lengthOfData) {
    // Convert to correct datatype before sending
    int16_t tempFFTBuffer[lengthOfData];
    for (int i = 0; i < lengthOfData; i++) {
        tempFFTBuffer[i] = (int16_t)FFTdata[i];
    }
    send_data_16Bit(tempFFTBuffer, lengthOfData);
}

void send_peak_data(std::vector<std::vector<q31_t>> peakData, int16_t lengthOfPeakList) {
    // Only send peaks if there is some useful peak data
    if (lengthOfPeakList < 2) {
        int32_t peakDataProcessed[1] = {0};
        send_data_32Bit(peakDataProcessed, 1);
    } else {
        int32_t peakDataProcessed[(lengthOfPeakList - 1) * 3];
        int16_t index = 0;
        for (int i = 1; i < lengthOfPeakList; i++) {
            peakDataProcessed[index] = (int32_t)peakData[i][0]; // Add amplitude
            index++;
            peakDataProcessed[index] = (int32_t)peakData[i][1]; // Add frequency
            index++;
            peakDataProcessed[index] = (int32_t)peakData[i][2]; // Add phase
            index++;
        }
        send_data_32Bit(peakDataProcessed, index);
    }
}
} // namespace teensyUDP