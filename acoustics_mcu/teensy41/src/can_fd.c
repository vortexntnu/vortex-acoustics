#include "can_fd.h"
#include "MIMXRT1062.h"
#include "PERI_CAN.h"
#include "PERI_CCM.h"

void canfd_init(void) {
  CCM->CCGR0 |= CCM_CCGR0_CG8(0x3) | CCM_CCGR0_CG7(0x3);

  // need pin config

  IOMUXC->SELECT_INPUT_1[4] |= 1;

  CAN1->MCR |= CAN_MCR_MDIS(1) | CAN_MCR_FRZ(1) | CAN_MCR_HALT(1);

  while (!(CAN1->MCR & CAN_MCR_FRZ(1)))
    ;

  CAN1->MCR |= CAN_MCR_SOFTRST(1);

  while (CAN1->MCR & CAN_MCR_SOFTRST(1))
    ;

  // nominal bit timing
  CAN1->CTRL1 = CAN_CTRL1_PRESDIV(9) | CAN_CTRL1_PROPSEG(5) |
                CAN_CTRL1_PSEG1(5) | CAN_CTRL1_PSEG2(2) | CAN_CTRL1_RJW(1);

  // Enable CANFD""
  CAN1->MCR |= CAN_MCR_FDEN(1);

  CAN1->IMASK1 = 0xFFFFFFFF;
  CAN1->IFLAG1 = 0xFFFFFFFF;

  // Reenable module
  CAN1->MCR &= ~(CAN_MCR_HALT(1) | CAN_MCR_FRZ(1));

  while (CAN1->MCR & CAN_MCR_FRZACK(1))
    ;

  CAN1->MCR &= ~CAN_MCR_MDIS(1);
}

int canfd_send() {}

int canfd_set_rxbuffer(uint8_t *buf) {}
