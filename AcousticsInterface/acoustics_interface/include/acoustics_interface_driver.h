

#ifndef ACOUSTICS_INTERFACE_DRIVER_HPP
#define ACOUSTICS_INTERFACE_DRIVER_HPP

#ifdef __cplusplus

extern "C" {

#endif

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
    

#define SAMPLE_LENGTH 1024
#define BUFFER_PER_CHANNEL 6
#define RAW_HYDROPHONE_SIZE (SAMPLE_LENGTH * BUFFER_PER_CHANNEL)

#define MTU_PAYLOAD_SIZE 1471
#define HYDRO_PKTS_PER_HYDROPHONE 6
#define NUM_HYDROPHONES 5
#define FILTERED_PKTS 2
#define NUM_BUFFERS 9

#define TEENSY_IP "10.0.0.111"
#define TEENSY_PORT 8888
#define MY_PORT 9999
#define MAX_PACKAGE_SIZE_RECEIVED 65536
#define SOCKET_TIMEOUT_SEC 1
#define TIMEOUT_MAX_SEC 10
#define INITIALIZATION_MESSAGE "HELLO :D"
#define NUM_FREQ_INTERESTS 10

struct frequency_interest {
    int frequency;
    int variance;
};

struct teensy_udp {
    int client_socket;
    struct sockaddr_in teensy_addr;
    struct sockaddr_in my_addr;
};

extern int16_t samples_raw_hydrophone1[RAW_HYDROPHONE_SIZE];
extern int16_t samples_raw_hydrophone2[RAW_HYDROPHONE_SIZE];
extern int16_t samples_raw_hydrophone3[RAW_HYDROPHONE_SIZE];
extern int16_t samples_raw_hydrophone4[RAW_HYDROPHONE_SIZE];
extern int16_t samples_raw_hydrophone5[RAW_HYDROPHONE_SIZE];
extern int16_t samples_filtered[SAMPLE_LENGTH];
extern int16_t fft_magnified[SAMPLE_LENGTH];
extern float time_diff[5];
extern float position[4];

char* get_local_ip(void);
int init_communication(struct teensy_udp* comm,
                       struct frequency_interest* freq,
                       int freq_count);
void send_acknowledge_signal(struct teensy_udp* comm);
int check_if_ready(struct teensy_udp* comm);
void send_frequencies_of_interest(struct teensy_udp* comm,
                                  struct frequency_interest* freq,
                                  int freq_count);
void fetch_data(struct teensy_udp* comm);

#ifdef __cplusplus
}

#endif

#endif  // !ACOUSTICS_INTERFACE_DRIVER_HPP
