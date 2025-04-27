

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

#define MTU_PAYLOAD_SIZE 1471 // for example
#define HYDRO_PKTS_PER_HYDROPHONE 6
#define NUM_HYDROPHONES 5
#define FILTERED_PKTS 2
#define SEQ_HYDRO_END (HYDRO_PKTS_PER_HYDROPHONE * NUM_HYDROPHONES) // 30
#define SEQ_FILTERED_END (SEQ_HYDRO_END + FILTERED_PKTS)
#define SEQ_FFT_END (SEQ_FILTERED_END + FILTERED_PKTS)

int16_t samples_raw_hydrophone1[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone2[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone3[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone4[RAW_HYDROPHONE_SIZE];
int16_t samples_raw_hydrophone5[RAW_HYDROPHONE_SIZE];

int16_t *samples_raw_hydrophones[5] = {
    samples_raw_hydrophone1, samples_raw_hydrophone2, samples_raw_hydrophone3,
    samples_raw_hydrophone4, samples_raw_hydrophone5};

int16_t samples_filtered[SAMPLE_LENGTH] = {0};
int16_t fft_magnified[SAMPLE_LENGTH] = {0};

typedef struct {
  size_t index;      // FFT bin index (optional, for debugging)
  int32_t amplitude; // Peak amplitude (converted to Q31)
  int32_t frequency; // Frequency (in Hz, represented in Q31 if needed)
  int16_t phase;     // Phase shift in Q15 format
} Peak;

typedef struct {
  uint8_t expected_seq;
  size_t offset;
  uint16_t peaks_bytes;
} StreamState;

Peak peaks[SAMPLE_LENGTH] = {0};

float time_diff[5] = {0};
float position[4] = {0};

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

int handle_data(StreamState *st, const uint8_t *buf, uint32_t len) {
  uint8_t seq = buf[0];
  if (seq != st->expected_seq) {
    // out of order packet
    return -1;
  }

  // Hydrophones: seq in [0 .. 29]
  if (seq < SEQ_HYDRO_END) {
    int hp = seq / HYDRO_PKTS_PER_HYDROPHONE; // 0..4
    memcpy(samples_raw_hydrophones[hp] + st->offset, buf + 1, len - 1);
    st->offset += (len - 1);
    if (st->offset >= RAW_HYDROPHONE_SIZE) {
      st->offset = 0;
    }
  }
  // Filtered: seq in [30..31]
  else if (seq < SEQ_FILTERED_END) {
    memcpy(samples_filtered + st->offset, buf + 1, len - 1);
    st->offset += (len - 1);
    if (st->offset >= SAMPLE_LENGTH) {
      st->offset = 0;
    }
  }
  // FFT: seq in [32..33]
  else if (seq < SEQ_FFT_END) {
    memcpy(samples_fft + st->offset, buf + 1, len - 1);
    st->offset += (len - 1);
    if (st->offset >= SAMPLE_LENGTH) {
      st->offset = 0;
    }
  }
  // Peak header: exactly seq == SEQ_PEAK_HDR
  else if (seq == SEQ_PEAK_HDR) {
    st->peaks_bytes = buf[1]; // total number of peak‐data packets
    st->offset = 0;
  }
  // Peak data: next `peaks_bytes` sequences
  else if (seq < SEQ_PEAK_HDR + st->peaks_bytes) {
    // payload starts at buf+2, length = len-2
    memcpy(peaks + st->offset, buf + 2, len - 2);
    st->offset += (len - 2);
  }
  // TDOA: next packet
  else if (seq == SEQ_PEAK_HDR + st->peaks_bytes) {
    memcpy(time_diff, buf + 1, len - 1);
  }
  // Position: final packet
  else if (seq == SEQ_PEAK_HDR + st->peaks_bytes + 1) {
    memcpy(position, buf + 1, len - 1);

    // Done with this frame… reset for next
    stream_init(st);
  }

  st->expected_seq++;
  return 0;
}
