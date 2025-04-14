#include "EthernetProtocol/ethernetModule.h"


void frequency_data_from_client(int32_t* frequenciesOfInterest, int32_t* frequencyVariances) {
    for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
        while (!UDP_check_if_connected())
            ;

        char* frequencyMessage = ethernetModule::UDP_read_message();
        char* token;

        token = strtok(frequencyMessage, ",");

        frequenciesOfInterest[i] = atoi(token);
        frequencyVariances[i] = atoi(strtok(NULL, ","));

        Serial.print(frequenciesOfInterest[i]);
        Serial.print(", ");
        Serial.println(frequencyVariances[i]);
    }
}


// The teensy is little endian, this means that
// LSB comes first
void send_data(void* data, uint32_t length) {
    uint8_t *data_ptr = (uint8_t*)data;
    uint32_t offset = 0;
    
    while (offset < length) {
        uint32_t chunk = (length - offset > MAX_CLIENT_CAPACITY) ? MAX_CLIENT_CAPACITY : (length - offset);
        UDP_send_message_raw(data_ptr + offset, chunk);
        offset += chunk;
    }
}


void setupTeensyCommunication(int32_t* frequenciesOfInterest, int32_t* frequencyVariances) {
    UDP_send_ready_signal(ethernetModule::get_remoteIP(), ethernetModule::get_remotePort());

    // After this, the client and teensy are connected
    frequency_data_from_client(frequenciesOfInterest, frequencyVariances);

    UDP_clean_message_memory();
}

