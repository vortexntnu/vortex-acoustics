

#include "acoustics_interface_driver.h"
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
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

#define SAMPLE_LENGTH 1024
#define BUFFER_PER_CHANNEL 6
#define RAW_HYDROPHONE_SIZE (SAMPLE_LENGTH * BUFFER_PER_CHANNEL)

#define MTU_PAYLOAD_SIZE 1471
#define HYDRO_PKTS_PER_HYDROPHONE 6
#define NUM_HYDROPHONES 5
#define FILTERED_PKTS 2
#define NUM_BUFFERS 9

int16_t samples_raw_hydrophone1[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone2[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone3[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone4[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone5[RAW_HYDROPHONE_SIZE];
int16_t samples_filtered[SAMPLE_LENGTH] = {0};
int16_t fft_magnified[SAMPLE_LENGTH] = {0};
float time_diff[5] = {0};
float position[4] = {0};

void *all_arrays[NUM_BUFFERS] = {samples_raw_hydrophone1,
                                 samples_raw_hydrophone2,
                                 samples_raw_hydrophone3,
                                 samples_raw_hydrophone4,
                                 samples_raw_hydrophone5,
                                 samples_filtered,
                                 fft_magnified,
                                 time_diff,
                                 position};

uint16_t array_sizes[NUM_BUFFERS] = {RAW_HYDROPHONE_SIZE,
                                     RAW_HYDROPHONE_SIZE,
                                     RAW_HYDROPHONE_SIZE,
                                     RAW_HYDROPHONE_SIZE,
                                     RAW_HYDROPHONE_SIZE,
                                     SAMPLE_LENGTH,
                                     SAMPLE_LENGTH,
                                     5,
                                     4};

typedef struct {
  uint8_t expected_seq;
  size_t offset;
  uint16_t peaks_bytes;
} StreamState;

void stream_init(StreamState *st) {
  st->expected_seq = 0;
  st->offset = 0;
  st->peaks_bytes = 0;
}

char *get_local_ip() {
  struct ifaddrs *ifaddr, *ifa;
  static char ip[INET_ADDRSTRLEN] = "127.0.0.1"; // Default IP

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

      struct sockaddr_in *addr = (struct sockaddr_in *)ifa->ifa_addr;
      if (inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN) != NULL) {
        break;
      }
    }
  }

  freeifaddrs(ifaddr);
  return ip;
}

int init_communication(TeensyCommunicationUDP *comm, FrequencyInterest freq[],
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

  if (bind(comm->client_socket, (struct sockaddr *)&comm->my_addr,
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
  if (setsockopt(comm->client_socket, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                 sizeof(timeout)) < 0) {
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

void send_acknowledge_signal(TeensyCommunicationUDP *comm) {
  int sent =
      sendto(comm->client_socket, INITIALIZATION_MESSAGE,
             strlen(INITIALIZATION_MESSAGE), 0,
             (struct sockaddr *)&comm->teensy_addr, sizeof(comm->teensy_addr));
  if (sent < 0) {
    perror("sendto (acknowledge)");
  } else {
    printf("Sent acknowledgement: %s\n", INITIALIZATION_MESSAGE);
  }
}

int check_if_ready(TeensyCommunicationUDP *comm) {
  char buffer[1024] = {0};
  socklen_t addrlen = sizeof(comm->teensy_addr);
  int n = recvfrom(comm->client_socket, buffer, sizeof(buffer) - 1, 0,
                   (struct sockaddr *)&comm->teensy_addr, &addrlen);
  if (n > 0) {
    buffer[n] = '\0';
    if (strcmp(buffer, "READY") == 0) {
      printf("READY signal received: %s\n", buffer);
      return 1;
    }
  }
  return 0;
}

void send_frequencies_of_interest(TeensyCommunicationUDP *comm,
                                  FrequencyInterest freq[], int freq_count) {
  assert(freq_count == 10);
  char msg[64];
  for (int i = 0; i < freq_count; i++) {
    snprintf(msg, sizeof(msg), "%d,%d,", freq[i].frequency, freq[i].variance);
    int sent = sendto(comm->client_socket, msg, strlen(msg), 0,
                      (struct sockaddr *)&comm->teensy_addr,
                      sizeof(comm->teensy_addr));
    if (sent < 0) {
      perror("sendto (frequencies)");
    } else {
      printf("Sent frequency message: %s\n", msg);
    }
  }
}

void fetch_data(TeensyCommunicationUDP *comm) {
  int attempts = 0;
  while (attempts < 1000) {
    uint8_t buffer[1500] = {0};
    socklen_t addrlen = sizeof(comm->teensy_addr);
    int n = recvfrom(comm->client_socket, buffer, sizeof(buffer) - 1, 0,
                     (struct sockaddr *)&comm->teensy_addr, &addrlen);
    if (n <= 0) {
      break;
    }
    buffer[n] = '\0';
    attempts++;
  }
}


/**
*@brief handles incoming udp data
*@param message buffer
*@param message length
*@return 0 on success
* -1 on message to sall
* -2 invalid sequence header
* -3 invalid offset
*/
int handle_data(const uint8_t *buf, uint32_t len) {
  if (len < 6) {
    return -1;
  }
  uint8_t seq = buf[0];
  if (seq >= NUM_BUFFERS) {
    return -2;
  }
  uint32_t offset = ((uint32_t)buf[1] << 24) | ((uint32_t)buf[2] << 16) |
                    ((uint32_t)buf[3] << 8) | ((uint32_t)buf[4]);

  const uint8_t *data_ptr = buf + 5;
  uint32_t data_len = len - 5;

  if (offset + data_len > array_sizes[seq]) {
    return -3;
  }

  memcpy(all_arrays[seq] + offset, data_ptr, data_len);

  return 0; 
}
