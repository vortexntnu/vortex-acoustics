#ifndef TEENSYUDP_H
#define TEENSYUDP_H

#include "arm_math.h"
#include <Arduino.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <cstdint>

#include "ethernet_module.h"
#include "dsp.h"
#include "adc.h"
#include "multilateration.h"



typedef struct {
  int frequency;
  int variance;
} FrequencyInterest;


#define MAX_CLIENT_CAPACITY 100
#define MTU_RAW 1472
#define MTU_PAYLOAD_SIZE (MTU_RAW -1)
#define FREQUENCY_LIST_LENGTH 10




extern int32_t freq_interest_max[FREQUENCY_LIST_LENGTH]; // 0 Hz
extern int32_t freq_interest_min[FREQUENCY_LIST_LENGTH]; // 0 Hz


int frequency_data_from_client(void);

void setup_communication(void);

/**
* @brief transmit all relevant data over udp
*/
void transmit_data_udp(void);


#endif // !TEENSYUDP
