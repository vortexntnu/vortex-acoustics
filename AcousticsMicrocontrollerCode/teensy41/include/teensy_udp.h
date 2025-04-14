#ifndef TEENSYUDP_H
#define TEENSYUDP_H

#include "arm_const_structs.h"
#include "arm_math.h"
#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <stdint.h>

#include "ethernetModule.h"



#define MAX_CLIENT_CAPACITY 100
#define FREQUENCY_LIST_LENGTH 10

#ifdef __cplusplus
extern "C" {

#endif // __cplusplus


void frequency_data_from_client(int32_t* frequenciesOfInterest, int32_t* frequencyVariances);

void send_data_udp(void* data, uint32_t length);

void setupTeensyCommunication(int32_t* frequenciesOfInterest, int32_t* frequencyVariances);

#ifdef __cplusplus
}

#endif // __cplusplus

#endif // !TEENSYUDP
