#ifndef ETHERNET_H
#define ETHERNET_H


#include "imxrt.h"
#include <stdint.h>
#include <stddef.h>



const uint8_t mac_address[6] = {0x00, 0x01, 0xBE, 0xEE, 0xFE, 0xED};

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus

void ethernet_init(const uint8_t mac[6]);

#ifdef __cplusplus
}

#endif //  __cplusplus

#endif // !ETHERNET_H
