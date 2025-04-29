#ifndef TEENSYUDP_H
#define TEENSYUDP_H

#include "Include/arm_const_structs.h"
#include "Include/arm_math.h"
#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <cstdint>

#include "ethernet_module.h"



#define MAX_CLIENT_CAPACITY 100
#define MTU_RAW 1472
#define MTU_PAYLOAD_SIZE (MTU_RAW -1)
#define FREQUENCY_LIST_LENGTH 10


extern uint8_t sequence;


extern int32_t freq_interest_max[FREQUENCY_LIST_LENGTH]; // 0 Hz
extern int32_t freq_interest_min[FREQUENCY_LIST_LENGTH]; // 0 Hz


void frequency_data_from_client(void);

void send_data_udp(void* data, uint32_t length);

void setupTeensyCommunication(void);


#endif // !TEENSYUDP
