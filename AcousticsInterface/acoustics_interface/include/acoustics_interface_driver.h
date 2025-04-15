

#ifndef ACOUSTICS_INTERFACE_DRIVER_HPP
#define ACOUSTICS_INTERFACE_DRIVER_HPP


#ifdef __cplusplus

extern "C"{



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



#define TEENSY_IP "10.0.0.111"
#define TEENSY_PORT 8888
#define MY_PORT 9999
#define MAX_PACKAGE_SIZE_RECEIVED 65536
#define SOCKET_TIMEOUT_SEC 1
#define TIMEOUT_MAX_SEC 10
#define INITIALIZATION_MESSAGE "HELLO :D"

typedef struct {
  int frequency;
  int variance;
} FrequencyInterest;

typedef struct {
  int client_socket;
  struct sockaddr_in teensy_addr;
  struct sockaddr_in my_addr;
  char data_string[1024];
  char data_target[64];
} TeensyCommunicationUDP;

/* Function prototypes */
char *get_local_ip(void);
int init_communication(TeensyCommunicationUDP *comm, FrequencyInterest freq[],
                       int freq_count);
void send_acknowledge_signal(TeensyCommunicationUDP *comm);
int check_if_ready(TeensyCommunicationUDP *comm);
void send_frequencies_of_interest(TeensyCommunicationUDP *comm,
                                  FrequencyInterest freq[], int freq_count);
void fetch_data(TeensyCommunicationUDP *comm);


#ifdef __cplusplus

}

#endif


#endif // !ACOUSTICS_INTERFACE_DRIVER_HPP
