#include "usart.h"
#include "imxrt.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#define UART1_RX_BUF_SIZE 128
#define UART1_TX_BUF_SIZE 128

uint8_t rx_buf[UART1_RX_BUF_SIZE];
uint8_t tx_buf[UART1_TX_BUF_SIZE];
uint16_t rx_tail, rx_head;
uint16_t tx_tail, tx_head;

void uart1_init(void) {

  CCM_CCGR5 |= CCM_CCGR5_LPUART1(1);

  /* UART1_TX on pin 1 = GPIO_AD_B0_12 (ALT3) */
  IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_12 = 3;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_12 = 0x10B0;

  /* UART1_RX on pin 0 = GPIO_AD_B0_13 (ALT3) */
  IOMUXC_SW_MUX_CTL_PAD_GPIO_AD_B0_13 = 3;
  IOMUXC_SW_PAD_CTL_PAD_GPIO_AD_B0_13 = 0x10B0;

  LPUART1_BAUD = (43 << 0)     /* SBR[12:0] */
                 | (13 << 16); /* BRFA[4:0] */

  LPUART1_CTRL |=
      LPUART_CTRL_RE | LPUART_CTRL_TE | LPUART_CTRL_RIE | LPUART_CTRL_TIE;
}

void uart1_printf(const char *fmt, ...) {
  char buf[128];

  va_list args;
  va_start(args, fmt);

  int len = vsnprintf(buf, sizeof(buf), fmt, args);

  for (int i = 0; i < len; i++) {
    while (!(LPUART1_STAT & LPUART_STAT_TDRE))
      ;
    LPUART1_DATA = buf[i];
  }
}

void uart1_send_byte(uint8_t c) {
  uint16_t next = (tx_head + 1) % UART1_TX_BUF_SIZE;
  while (next == tx_tail) { /* buffer full: spin or block */
  }
  tx_buf[tx_head] = c;
  tx_head = next;
  LPUART1_CTRL |= LPUART_CTRL_TIE;
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
  uint32_t status = LPUART1_STAT;

  if (status & LPUART_STAT_RDRF) { 
    uint8_t c = (uint8_t)LPUART1_DATA;
    uint16_t next = (rx_head + 1) % UART1_RX_BUF_SIZE;
    if (next != rx_tail) {
      rx_buf[rx_head] = c;
      rx_head = next;
    }
    // else overflow: drop byte
  }

  if (status & LPUART_STAT_TDRE) { // TDRE = bit 23
    if (tx_tail != tx_head) {
      LPUART1_DATA = tx_buf[tx_tail];
      tx_tail = (tx_tail + 1) % UART1_TX_BUF_SIZE;
    } else {
      LPUART1_CTRL &= ~(1 << 7); // clear TIE
    }
  }
}
