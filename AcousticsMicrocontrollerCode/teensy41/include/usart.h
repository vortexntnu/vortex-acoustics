#ifndef USART_H
#define USART_H

#include "imxrt.h"
#include <stdint.h>

#ifdef __cplusplus

extern "C" {

#endif // __cplusplus

void uart1_init(void);

void uart1_prinft(const char *fmt, ...);

void uart1_send_byte(uint8_t c);

int read_byte(void);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !USART_H
