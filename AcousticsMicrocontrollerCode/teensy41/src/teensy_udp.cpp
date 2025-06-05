#include "teensy_udp.h"
#include "stack/fnet_netbuf.h"
#include <cstddef>
#include <cstdint>

int32_t freq_interest_max[FREQUENCY_LIST_LENGTH]; // 0 Hz
int32_t freq_interest_min[FREQUENCY_LIST_LENGTH]; // 0 Hz
//
FrequencyInterest frequencyInterest[FREQUENCY_LIST_LENGTH];

int frequency_data_from_client(void) {
    while (!UDP_check_if_connected())
        ;
    uint8_t* data = (uint8_t*)(UDP_read_message() + 1);
    if (data[0] != 1) {
        return -1;
    }
    for (size_t i = 0; i < 10; i++) {
        size_t idx = 8 * (i) + 1;
        int freq = ((int)data[idx] << 24) | ((int)data[idx + 1] << 16) | ((int)data[idx + 2] << 8) | ((int)data[idx + 3]);
        int variance = ((int)data[idx+4] << 24) | ((int)data[idx + 5] << 16) | ((int)data[idx + 6] << 8) | ((int)data[idx + 7]);
        frequencyInterest[i].frequency = freq;
        frequencyInterest[i].variance = variance;
    }
}

static void send_data_udp(const void* data_ptr, size_t len, uint8_t sequence) {
    const uint8_t* p = (const uint8_t*)data_ptr;
    size_t offset = 0;

    while (offset < len) {
        size_t chunk = len - offset;
        if (chunk > MTU_PAYLOAD_SIZE)
            chunk = MTU_PAYLOAD_SIZE;

        uint8_t packet[MTU_RAW];
        packet[0] = sequence;
        packet[1] = (offset >> 24) & 0xFF;
        packet[2] = (offset >> 16) & 0xFF;
        packet[3] = (offset >> 8) & 0xFF;
        packet[4] = (offset >> 0) & 0xFF;
        memcpy(packet + 5, p + offset, chunk);
        UDP_send_message_raw(packet, chunk + 5);

        offset += chunk;
    }
}

void setupTeensyCommunication(void) {
    UDP_send_ready_signal(get_remoteIP(), get_remotePort());

    // After this, the client and teensy are connected
    frequency_data_from_client();

    UDP_clean_message_memory();
}

void transmit_data_udp(void) {
    for (uint8_t i = 0; i < NUM_HYDROPHONES; i++) {
        send_data_udp(samples_raw_hydrophones[i], sizeof(int16_t) * RAW_HYDROPHONE_SIZE, i);
    }
    send_data_udp(samples_filtered, sizeof(q15_t) * SAMPLE_LENGTH, 5);
    send_data_udp(fft_results_magnified, sizeof(q15_t) * SAMPLE_LENGTH, 6);
    send_data_udp(timeDifferenceOfArrival, sizeof(float32_t) * TDOA_DATA_LENGTH, 7);
    send_data_udp(soundLocation, sizeof(float32_t) * POSITION_DATA_LENGTH, 8);

    UDP_clean_message_memory();
}
