#include "teensy_udp.h"
#include "stack/fnet_netbuf.h"
#include <cstdint>

uint8_t sequence = 0;


int32_t freq_interest_max[FREQUENCY_LIST_LENGTH]; // 0 Hz
int32_t freq_interest_min[FREQUENCY_LIST_LENGTH]; // 0 Hz


void frequency_data_from_client(void) {
    for (int i = 0; i < FREQUENCY_LIST_LENGTH; i++) {
        while (!UDP_check_if_connected())
            ;

        char* frequencyMessage = UDP_read_message();
        char* token;

        token = strtok(frequencyMessage, ",");
        int32_t freq_interest = atoi(token);
        int32_t freq_variance = atoi(strtok(NULL, ","));

        freq_interest_max[i] = freq_interest + freq_variance;
        freq_interest_min[i] = freq_interest - freq_variance;
        

        Serial.print(freq_interest);
        Serial.print(", ");
        Serial.println(freq_variance);
    }
}

void send_data_udp(const void* data_ptr, uint32_t len) {
    const uint8_t* p = (const uint8_t*)data_ptr;
    uint32_t offset = 0;

    while (offset < len) {
        uint32_t chunk = len - offset;
        if (chunk > MTU_PAYLOAD_SIZE)
            chunk = MTU_PAYLOAD_SIZE;

        uint8_t packet[MTU_RAW];
        packet[0] = sequence;                  // sequence ID
        memcpy(packet + 1, p + offset, chunk); // copy up to MTU_PAYLOAD_SIZE

        UDP_send_message_raw(packet, chunk + 1);

        offset += chunk;
        sequence = (uint8_t)(sequence + 1); // wrap at 255→0 automatically
    }
}

// Since peaks is variable length we use a different function
void send_peaks_udp(const void* peaks, uint32_t len) {
    const uint8_t* p = (const uint8_t*)peaks;
    uint8_t packets_sent = len / MAX_CLIENT_CAPACITY;

    uint8_t packet[MTU_RAW];
    packet[0] = sequence;
    packet[1] = packets_sent;
    uint32_t chunk = len;
    if (chunk > MTU_PAYLOAD_SIZE) {
        chunk = MTU_PAYLOAD_SIZE;
    }
    memcpy(packet + 2, p, chunk);
    UDP_send_message_raw(packet, chunk + 1);
    sequence++;
    if (chunk == MTU_PAYLOAD_SIZE) {
        return;
    }
    send_data_udp(p+chunk, len - chunk);
}

void setupTeensyCommunication(void) {
    UDP_send_ready_signal(get_remoteIP(), get_remotePort());

    // After this, the client and teensy are connected
    frequency_data_from_client();

    UDP_clean_message_memory();
}
