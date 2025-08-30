

#include "acoustics_interface_driver.h"
#include <arpa/inet.h>
#include <assert.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


int16_t samples_raw_hydrophone1[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone2[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone3[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone4[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone5[RAW_HYDROPHONE_SIZE];
int16_t samples_filtered[SAMPLE_LENGTH] = {0};
int16_t fft_magnified[SAMPLE_LENGTH] = {0};
float time_diff[5] = {0};
float position[4] = {0};

void* all_arrays[NUM_BUFFERS] = {samples_raw_hydrophone1,
                                 samples_raw_hydrophone2,
                                 samples_raw_hydrophone3,
                                 samples_raw_hydrophone4,
                                 samples_raw_hydrophone5,
                                 samples_filtered,
                                 fft_magnified,
                                 time_diff,
                                 position};

size_t array_byte_sizes[NUM_BUFFERS] = {
    RAW_HYDROPHONE_SIZE * sizeof(int16_t),  // i.e. 1024*2 = 2048 bytes
    RAW_HYDROPHONE_SIZE * sizeof(int16_t),
    RAW_HYDROPHONE_SIZE * sizeof(int16_t),
    RAW_HYDROPHONE_SIZE * sizeof(int16_t),
    RAW_HYDROPHONE_SIZE * sizeof(int16_t),

    SAMPLE_LENGTH * sizeof(int16_t),  // e.g. 512*2 = 1024 bytes
    SAMPLE_LENGTH * sizeof(int16_t),

    5 * sizeof(float),  // time_diff is a float[5], i.e. 20 bytes total
    4 * sizeof(float)   // position is a float[4], i.e. 16 bytes total
};

/**
 * @brief  Send a large buffer to the Teensy, chunking it into
 * “seq+offset+payload” packets.
 * @param  comm   An initialized TeensyCommunicationUDP (socket & teensy_addr
 * already set).
 * @param  data   Pointer to the raw data you want to ship.
 * @param  size   Total size (in bytes) of that data.
 * @return        0 on success (all chunks sent)
 *               -1 on socket error.
 */
static int send_data_udp(struct teensy_udp* comm,
                         void* data,
                         size_t size,
                         uint8_t sequence) {
    uint8_t* raw = (uint8_t*)data;
    size_t offset = 0;

    uint8_t buffer[1024];

    while (offset < size) {
        size_t chunk = size - offset;
        if (chunk > MTU_PAYLOAD_SIZE) {
            chunk = MTU_PAYLOAD_SIZE;
        }

        buffer[0] = sequence;

        memcpy(buffer + 1, raw + offset, chunk);

        ssize_t sent = sendto(
            comm->client_socket, buffer, (size_t)(chunk + 1), 0,
            (struct sockaddr*)&comm->teensy_addr, sizeof(comm->teensy_addr));
        if (sent < 0) {
            perror("sendto failed");
            return -1;
        }
        offset += chunk;
    }

    return 0;
}

char* get_local_ip() {
    struct ifaddrs *ifaddr, *ifa;
    static char ip[INET_ADDRSTRLEN] = "127.0.0.1";  // Default IP

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return ip;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            if (strcmp(ifa->ifa_name, "lo") == 0)
                continue;

            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            if (inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN) !=
                NULL) {
                break;
            }
        }
    }

    freeifaddrs(ifaddr);
    return ip;
}

int init_communication(struct teensy_udp* comm,
                       struct frequency_interest* freq,
                       int freq_count) {
    comm->client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (comm->client_socket < 0) {
        perror("socket");
        return -1;
    }

    memset(&comm->my_addr, 0, sizeof(comm->my_addr));
    comm->my_addr.sin_family = AF_INET;
    comm->my_addr.sin_port = htons(MY_PORT);
    comm->my_addr.sin_addr.s_addr = inet_addr(get_local_ip());

    if (bind(comm->client_socket, (struct sockaddr*)&comm->my_addr,
             sizeof(comm->my_addr)) < 0) {
        perror("bind");
        return -1;
    }

    memset(&comm->teensy_addr, 0, sizeof(comm->teensy_addr));
    comm->teensy_addr.sin_family = AF_INET;
    comm->teensy_addr.sin_port = htons(TEENSY_PORT);
    comm->teensy_addr.sin_addr.s_addr = inet_addr(TEENSY_IP);

    struct timeval timeout;
    timeout.tv_sec = SOCKET_TIMEOUT_SEC;
    timeout.tv_usec = 0;
    if (setsockopt(comm->client_socket, SOL_SOCKET, SO_RCVTIMEO,
                   (char*)&timeout, sizeof(timeout)) < 0) {
        perror("setsockopt");
        return -1;
    }

    send_acknowledge_signal(comm);

    time_t start_time = time(NULL);
    while (1) {
        if (check_if_ready(comm)) {
            break;
        }
        printf("READY signal not received. Waiting...\n");
        sleep(1);
        if (time(NULL) - start_time > TIMEOUT_MAX_SEC) {
            printf("Gave up on READY. Resending acknowledge.\n");
            start_time = time(NULL);
            send_acknowledge_signal(comm);
        }
    }

    send_frequencies_of_interest(comm, freq, freq_count);
    return 0;
}

void send_acknowledge_signal(struct teensy_udp* comm) {
    int sent =
        sendto(comm->client_socket, INITIALIZATION_MESSAGE,
               strlen(INITIALIZATION_MESSAGE), 0,
               (struct sockaddr*)&comm->teensy_addr, sizeof(comm->teensy_addr));
    if (sent < 0) {
        perror("sendto (acknowledge)");
    } else {
        printf("Sent acknowledgement: %s\n", INITIALIZATION_MESSAGE);
    }
}

int check_if_ready(struct teensy_udp* comm) {
    char buffer[1024] = {0};
    socklen_t addrlen = sizeof(comm->teensy_addr);
    int n = recvfrom(comm->client_socket, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr*)&comm->teensy_addr, &addrlen);
    if (n > 0) {
        buffer[n] = '\0';
        if (strcmp(buffer, "READY") == 0) {
            printf("READY signal received: %s\n", buffer);
            return 1;
        }
    }
    return 0;
}

void send_frequencies_of_interest(struct teensy_udp* comm,
                                  struct frequency_interest* freq,
                                  int freq_count) {
    send_data_udp(comm, freq, freq_count, 1);
}

/**
 * @brief   Handle one incoming UDP “chunk.”
 * @param   buf   Pointer to the first byte of the UDP payload
 * @param   len   Length of that payload
 * @return  0 on success
 *         -1 if the packet was too small to contain “seq+offset+1 byte data”
 *         -2 if seq is out of range
 *         -3 if offset+data_len would overrun the selected buffer
 */
static int handle_data(const uint8_t* buf, uint32_t len) {
    if (len < 6) {
        return -1;
    }
    uint8_t seq = buf[0];
    if (seq >= NUM_BUFFERS) {
        return -2;
    }
    uint32_t offset = ((uint32_t)buf[1] << 24) | ((uint32_t)buf[2] << 16) |
                      ((uint32_t)buf[3] << 8) | ((uint32_t)buf[4]);

    const uint8_t* data_ptr = buf + 5;
    uint32_t data_len = len - 5;

    if (offset + data_len > array_byte_sizes[seq]) {
        return -3;
    }

    uint8_t* pDest = (uint8_t*)all_arrays[seq];

    memcpy(pDest + offset, data_ptr, data_len);

    return 0;
}

void fetch_data(struct teensy_udp* comm) {
    int attempts = 0;
    uint8_t buffer[1500];
    while (attempts < 1000) {
        socklen_t addrlen = sizeof(comm->teensy_addr);
        int n = recvfrom(comm->client_socket, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr*)&comm->teensy_addr, &addrlen);
        if (n <= 0) {
            break;
        }
        attempts++;
        handle_data(buffer, n);
    }
}
