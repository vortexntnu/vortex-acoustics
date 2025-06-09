#include "ethernet.h"
#include "imxrt.h"

void ethernet_init(const uint8_t mac[6]) {
  CCM_CCGR1 |= CCM_CCGR1_ENET(0x3);

  IOMUXC_ENET_IPG_CLK_RMII_SELECT_INPUT = 1;

  IOMUXC_ENET_MDIO_SELECT_INPUT = 1;

  IOMUXC_ENET0_RXDATA_SELECT_INPUT = 1;
  IOMUXC_ENET1_RXDATA_SELECT_INPUT = 1;

  IOMUXC_ENET_RXEN_SELECT_INPUT = 1;
  IOMUXC_ENET_RXERR_SELECT_INPUT = 1;

  IOMUXC_ENET0_TIMER_SELECT_INPUT = 1;

  IOMUXC_ENET_TXCLK_SELECT_INPUT = 1;
  ENET_ECR |= ENET_ECR_RESET;

  while (ENET_ECR & ENET_ECR_RESET)
    ;

  ENET_TCR |= ENET_TCR_FDEN;
  ENET_RCR |= ENET_RCR_RMII_MODE;

  ENET_PALR = (mac[0] << 24) | (mac[1] << 16) | (mac[2] << 8) | (mac[3]);
  ENET_PAUR = (mac[4] << 8) | (mac[5]);

  ENET_ECR |= ENET_ECR_ETHEREN;
}

int ethernet_send_pkt(uint8_t *data, size_t len) {}
