#ifndef ETHERNET_H
#define ETHERNET_H


#include "imxrt.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus

void ethernet_init(const uint8_t mac[6]);

#ifdef __cplusplus
}

#endif //  __cplusplus

#endif // !ETHERNET_H
