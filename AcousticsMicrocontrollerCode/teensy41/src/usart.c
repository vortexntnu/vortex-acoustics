#include "usart.h"
#include "MIMXRT1062.h"
#include "MIMXRT1062_COMMON.h"
#include "PERI_LPUART.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define UART1_RX_BUF_SIZE 128
#define UART1_TX_BUF_SIZE 128
#define LPUART_CLOCK 600000

uint8_t rx_buf[UART1_RX_BUF_SIZE];
uint8_t tx_buf[UART1_TX_BUF_SIZE];
uint16_t rx_tail, rx_head;
uint16_t tx_tail, tx_head;

void uart1_init(uint32_t baud) {

  CCM->CCGR5 |= CCM_CCGR5_CG12(0x3);
  /* UART1_TX on pin 1 = GPIO_AD_B0_12 (ALT3) */
  // needs pin config



  LPUART1->CTRL &= ~(LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);

  uint32_t osr = 15; // 16x oversampling
  uint32_t sbr = LPUART_CLOCK / ((osr + 1) * baud);
  uint32_t remainder = LPUART_CLOCK - (baud * (osr + 1) * sbr);

  LPUART1->BAUD =
      LPUART_BAUD_OSR(osr) | LPUART_BAUD_SBR(sbr);
  LPUART1->BAUD |= (43 << 0)     /* SBR[12:0] */
                   | (13 << 16); /* BRFA[4:0] */
  LPUART1->CTRL |= LPUART_CTRL_RE(1) | LPUART_CTRL_TE(1) | LPUART_CTRL_RIE(1) |
                   LPUART_CTRL_TIE(1);

  NVIC_SetPriority(LPUART1_IRQn, 2);
  NVIC_EnableIRQ(LPUART1_IRQn);
}

static void enqueue_tx(const char *buf, int len) {
  for (int i = 0; i < len; i++) {
    uint16_t next = (tx_head + 1) % UART1_TX_BUF_SIZE;
    // while (next == tx_tail) {
    // }
    tx_buf[tx_head] = buf[i];
    tx_head = next;
  }
  LPUART1->CTRL |= LPUART_CTRL_TIE(1);
}

void uart1_printf(const char *fmt, ...) {
  char buf[128];

  va_list args;
  va_start(args, fmt);

  int len = vsnprintf(buf, sizeof(buf), fmt, args);

  enqueue_tx(buf, len);
}

void uart1_send_byte(uint8_t c) {
  uint16_t next = (tx_head + 1) % UART1_TX_BUF_SIZE;
  while (next == tx_tail) { /* buffer full: spin or block */
  }
  tx_buf[tx_head] = c;
  tx_head = next;
  LPUART1->CTRL |= LPUART_CTRL_TIE(1);
}

/* Called by application to get a received byte (or -1 if none) */
int uart1_read_byte(void) {
  if (rx_head == rx_tail)
    return -1;
  uint8_t c = rx_buf[rx_tail];
  rx_tail = (rx_tail + 1) % UART1_RX_BUF_SIZE;
  return c;
}

void __attribute__((used)) LPUART1_IRQHandler(void) {
  uint32_t status = LPUART1->STAT;

  if (status & LPUART_STAT_RDRF(1)) {
    uint8_t c = (uint8_t)LPUART1->DATA;
    uint16_t next = (rx_head + 1) % UART1_RX_BUF_SIZE;
    if (next != rx_tail) {
      rx_buf[rx_head] = c;
      rx_head = next;
    }
  }

  if (status & LPUART_STAT_TDRE(1)) {
    if (tx_tail != tx_head) {
      LPUART1->DATA = tx_buf[tx_tail];
      tx_tail = (tx_tail + 1) % UART1_TX_BUF_SIZE;
    } else {
      LPUART1->CTRL &= ~(LPUART_CTRL_TIE(1));
    }
  }
}
