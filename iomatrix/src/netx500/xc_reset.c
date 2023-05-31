#include "netx_io_areas.h"


typedef enum {
  NX500_XC_TYPE_RPU  = 0,
  NX500_XC_TYPE_TPU  = 1,
  NX500_XC_TYPE_XPEC = 2,
} NX500_XC_TYPE_E;

static NX500_XMAC_AREA_T * const s_ptXmacStart[] =
{
  (NX500_XMAC_AREA_T*) Addr_NX500_xmac0,
  (NX500_XMAC_AREA_T*) Addr_NX500_xmac1,
  (NX500_XMAC_AREA_T*) Addr_NX500_xmac2,
  (NX500_XMAC_AREA_T*) Addr_NX500_xmac3
};

static NX500_XPEC_AREA_T * const s_ptXpecStart[] =
{
  (NX500_XPEC_AREA_T*) Addr_NX500_xpec0,
  (NX500_XPEC_AREA_T*) Addr_NX500_xpec1,
  (NX500_XPEC_AREA_T*) Addr_NX500_xpec2,
  (NX500_XPEC_AREA_T*) Addr_NX500_xpec3,
};

static NX500_XPEC_IRQ_REGISTERS_AREA_T* const s_ptXpecIrq =  (NX500_XPEC_IRQ_REGISTERS_AREA_T*) Addr_NX500_xpec_irq_registers;

int NX500_XC_Reset( unsigned int uPortNo);
int NX500_XC_Load( unsigned int uPortNo, NX500_XC_TYPE_E eXcType, const unsigned long *pulXcPrg);


#define NUM_XPECS 4

static const unsigned long XcCode_rpu_reset0[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00160000, 0x00f80001, 0x915fdb81, 0x00f40041, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x00f8fffd,
  0x914fe004, 0x00fbfffd, 0x915fdb85, 0x0107fffd, 0x915fdb86, 0x01080001, 0x915fdb87, 0x010bfffd,
  0x915fdb88, 0x01040001, 0x915fdb89, 0x01080001, 0x915fdb8a, 0x00f40001, 0x915fdb8b, 0x00f9fffd,
  0x915fdb8c, 0x00f8fffd, 0x915fdb8d, 0x00f87ffd, 0x915fdb8e, 0x00f83ffd, 0x915fdb8f, 0x00f81ffd,
  0x915fdb90, 0x00f80ffd, 0x915fdb91, 0x00f807fd, 0x915fdb92, 0x00f803fd, 0x915fdb93, 0x00f801fd,
  0x915fdb94, 0x00f800fd, 0x915fdb95, 0x00f8007d, 0x915fdb96, 0x00f8003d, 0x915fdb97, 0x00f8001d,
  0x915fdb98, 0x00f8000d, 0x915fdb99, 0x00f80005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_rpu_reset1[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00161000, 0x00f80001, 0x915fdb81, 0x00f40041, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x00f8fffd,
  0x914fe004, 0x00fbfffd, 0x915fdb85, 0x0107fffd, 0x915fdb86, 0x01080001, 0x915fdb87, 0x010bfffd,
  0x915fdb88, 0x01040001, 0x915fdb89, 0x01080001, 0x915fdb8a, 0x00f40001, 0x915fdb8b, 0x00f9fffd,
  0x915fdb8c, 0x00f8fffd, 0x915fdb8d, 0x00f87ffd, 0x915fdb8e, 0x00f83ffd, 0x915fdb8f, 0x00f81ffd,
  0x915fdb90, 0x00f80ffd, 0x915fdb91, 0x00f807fd, 0x915fdb92, 0x00f803fd, 0x915fdb93, 0x00f801fd,
  0x915fdb94, 0x00f800fd, 0x915fdb95, 0x00f8007d, 0x915fdb96, 0x00f8003d, 0x915fdb97, 0x00f8001d,
  0x915fdb98, 0x00f8000d, 0x915fdb99, 0x00f80005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_rpu_reset2[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00162000, 0x00f80001, 0x915fdb81, 0x00f40041, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x00f8fffd,
  0x914fe004, 0x00fbfffd, 0x915fdb85, 0x0107fffd, 0x915fdb86, 0x01080001, 0x915fdb87, 0x010bfffd,
  0x915fdb88, 0x01040001, 0x915fdb89, 0x01080001, 0x915fdb8a, 0x00f40001, 0x915fdb8b, 0x00f9fffd,
  0x915fdb8c, 0x00f8fffd, 0x915fdb8d, 0x00f87ffd, 0x915fdb8e, 0x00f83ffd, 0x915fdb8f, 0x00f81ffd,
  0x915fdb90, 0x00f80ffd, 0x915fdb91, 0x00f807fd, 0x915fdb92, 0x00f803fd, 0x915fdb93, 0x00f801fd,
  0x915fdb94, 0x00f800fd, 0x915fdb95, 0x00f8007d, 0x915fdb96, 0x00f8003d, 0x915fdb97, 0x00f8001d,
  0x915fdb98, 0x00f8000d, 0x915fdb99, 0x00f80005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_rpu_reset3[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00163000, 0x00f80001, 0x915fdb81, 0x00f40041, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x00f8fffd,
  0x914fe004, 0x00fbfffd, 0x915fdb85, 0x0107fffd, 0x915fdb86, 0x01080001, 0x915fdb87, 0x010bfffd,
  0x915fdb88, 0x01040001, 0x915fdb89, 0x01080001, 0x915fdb8a, 0x00f40001, 0x915fdb8b, 0x00f9fffd,
  0x915fdb8c, 0x00f8fffd, 0x915fdb8d, 0x00f87ffd, 0x915fdb8e, 0x00f83ffd, 0x915fdb8f, 0x00f81ffd,
  0x915fdb90, 0x00f80ffd, 0x915fdb91, 0x00f807fd, 0x915fdb92, 0x00f803fd, 0x915fdb93, 0x00f801fd,
  0x915fdb94, 0x00f800fd, 0x915fdb95, 0x00f8007d, 0x915fdb96, 0x00f8003d, 0x915fdb97, 0x00f8001d,
  0x915fdb98, 0x00f8000d, 0x915fdb99, 0x00f80005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_tpu_reset0[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00160400, 0x01100001, 0x915fdb81, 0x010c0641, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x0110fffd,
  0x914fe304, 0x0113fffd, 0x915fdb85, 0x011ffffd, 0x915fdb86, 0x01200001, 0x915fdb87, 0x0123fffd,
  0x915fdb88, 0x011c0001, 0x915fdb89, 0x01200001, 0x915fdb8a, 0x010c0601, 0x915fdb8b, 0x0111fffd,
  0x915fdb8c, 0x0110fffd, 0x915fdb8d, 0x01107ffd, 0x915fdb8e, 0x01103ffd, 0x915fdb8f, 0x01101ffd,
  0x915fdb90, 0x01100ffd, 0x915fdb91, 0x011007fd, 0x915fdb92, 0x011003fd, 0x915fdb93, 0x011001fd,
  0x915fdb94, 0x011000fd, 0x915fdb95, 0x0110007d, 0x915fdb96, 0x0110003d, 0x915fdb97, 0x0110001d,
  0x915fdb98, 0x0110000d, 0x915fdb99, 0x01100005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_tpu_reset1[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00161400, 0x01100001, 0x915fdb81, 0x010c0641, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x0110fffd,
  0x914fe304, 0x0113fffd, 0x915fdb85, 0x011ffffd, 0x915fdb86, 0x01200001, 0x915fdb87, 0x0123fffd,
  0x915fdb88, 0x011c0001, 0x915fdb89, 0x01200001, 0x915fdb8a, 0x010c0601, 0x915fdb8b, 0x0111fffd,
  0x915fdb8c, 0x0110fffd, 0x915fdb8d, 0x01107ffd, 0x915fdb8e, 0x01103ffd, 0x915fdb8f, 0x01101ffd,
  0x915fdb90, 0x01100ffd, 0x915fdb91, 0x011007fd, 0x915fdb92, 0x011003fd, 0x915fdb93, 0x011001fd,
  0x915fdb94, 0x011000fd, 0x915fdb95, 0x0110007d, 0x915fdb96, 0x0110003d, 0x915fdb97, 0x0110001d,
  0x915fdb98, 0x0110000d, 0x915fdb99, 0x01100005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_tpu_reset2[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00162400, 0x01100001, 0x915fdb81, 0x010c0641, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x0110fffd,
  0x914fe304, 0x0113fffd, 0x915fdb85, 0x011ffffd, 0x915fdb86, 0x01200001, 0x915fdb87, 0x0123fffd,
  0x915fdb88, 0x011c0001, 0x915fdb89, 0x01200001, 0x915fdb8a, 0x010c0601, 0x915fdb8b, 0x0111fffd,
  0x915fdb8c, 0x0110fffd, 0x915fdb8d, 0x01107ffd, 0x915fdb8e, 0x01103ffd, 0x915fdb8f, 0x01101ffd,
  0x915fdb90, 0x01100ffd, 0x915fdb91, 0x011007fd, 0x915fdb92, 0x011003fd, 0x915fdb93, 0x011001fd,
  0x915fdb94, 0x011000fd, 0x915fdb95, 0x0110007d, 0x915fdb96, 0x0110003d, 0x915fdb97, 0x0110001d,
  0x915fdb98, 0x0110000d, 0x915fdb99, 0x01100005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long XcCode_tpu_reset3[57] = {
  0x000000dc, // program size
  0x00000000, // trailing loads size
  0x00163400, 0x01100001, 0x915fdb81, 0x010c0641, 0x915fdb82, 0x00dc0000, 0x001fdb83, 0x0110fffd,
  0x914fe304, 0x0113fffd, 0x915fdb85, 0x011ffffd, 0x915fdb86, 0x01200001, 0x915fdb87, 0x0123fffd,
  0x915fdb88, 0x011c0001, 0x915fdb89, 0x01200001, 0x915fdb8a, 0x010c0601, 0x915fdb8b, 0x0111fffd,
  0x915fdb8c, 0x0110fffd, 0x915fdb8d, 0x01107ffd, 0x915fdb8e, 0x01103ffd, 0x915fdb8f, 0x01101ffd,
  0x915fdb90, 0x01100ffd, 0x915fdb91, 0x011007fd, 0x915fdb92, 0x011003fd, 0x915fdb93, 0x011001fd,
  0x915fdb94, 0x011000fd, 0x915fdb95, 0x0110007d, 0x915fdb96, 0x0110003d, 0x915fdb97, 0x0110001d,
  0x915fdb98, 0x0110000d, 0x915fdb99, 0x01100005, 0x915fdb9a, 0x00dc0000, 0x001fdb9a,
  // trailing loads

};

static const unsigned long* const paulxMacRpuCodes[4]=
{
  XcCode_rpu_reset0,
  XcCode_rpu_reset1,
  XcCode_rpu_reset2,
  XcCode_rpu_reset3
};

static const unsigned long* const paulxMacTpuCodes[4]=
{
  XcCode_tpu_reset0,
  XcCode_tpu_reset1,
  XcCode_tpu_reset2,
  XcCode_tpu_reset3
};

/*****************************************************************************/
/*! Reset XC Code
* \description
*   Reset XC port.
* \class
*   XC
* \params
*   uPortNo          [in]  XC Port Number
*   pvUser           [in]  User Specific Parameters
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX500_XC_Reset( unsigned int uPortNo)
{
  NX500_XMAC_AREA_T * ptXmac;
  NX500_XPEC_AREA_T * ptXpec;

  /* check instance number */
  if( uPortNo>=NUM_XPECS ) return -1;

  ptXmac = s_ptXmacStart[uPortNo];
  ptXpec = s_ptXpecStart[uPortNo];

  /* stop xMAC/xPEC */
  ptXpec->ulXpu_hold_pc = 1;                                   /* Hold xPEC */
  ptXpec->aulStatcfg[uPortNo] = 0x00000088;                    /* Reset IO.OE asap */
  ptXmac->ulXmac_tpu_hold_pc = MSK_NX500_xmac_tpu_hold_pc_tpu_hold;  /* Just set Hold-Bit */
  ptXmac->ulXmac_rpu_hold_pc = MSK_NX500_xmac_rpu_hold_pc_rpu_hold;  /* Just set Hold-Bit */
  ptXpec->aulStatcfg[uPortNo] = 0x00000088;                    /* Reset IO.OE asap */

  /* load ratemul reset code */
  if( NX500_XC_Load( uPortNo, NX500_XC_TYPE_RPU, paulxMacRpuCodes[uPortNo]) !=0 ) {
    return -1;
  }
  if( NX500_XC_Load( uPortNo, NX500_XC_TYPE_TPU, paulxMacTpuCodes[uPortNo]) !=0 ) {
    return -1;
  }
  ptXmac->ulXmac_rpu_pc      = 0; /* Reset PC to 0 */
  ptXmac->ulXmac_tpu_pc      = 0; /* Reset PC to 0 */
  ptXmac->ulXmac_tpu_hold_pc = 0; /* Clear Hold-Bit */
  ptXmac->ulXmac_rpu_hold_pc = 0; /* Clear Hold-Bit */

  /* !!!! ATTENTION: There must be enougth time between starting xMAC and stopping xMAC to execute reset programm */

  /* clear xPEC events and stop DMA */
  ptXpec->aulRam[0] = 0xC0000FFF; /* Use the command wait b000000000000,b111111111111 at Address 0 */
  ptXpec->ulXpec_pc = 0;          /* Reset the Program Counter to 0 on netX100/500 */
  ptXpec->ulXpec_statcfg = 0;     /* Reset Timer operation and DMA */
  ptXpec->ulXpu_hold_pc = 0;      /* Start the Program */
  ptXpec->aulRam[0x7FF] = 0x7F;   /* Link and Stop DMA */
  ptXpec->aulRam[0x7FE] = 0x00;   /* dummy access to let xpec run another cycle */
  ptXpec->ulXpu_hold_pc = 1;      /* Hold the Program Counter */
  ptXpec->ulXpec_pc = 0;          /* Reset the Program Counter to 0 on netX100/500 */

  /* reset all xPEC registers to default values */
  ptXpec->aulXpec_r[0] = 0;
  ptXpec->aulXpec_r[1] = 0;
  ptXpec->aulXpec_r[2] = 0;
  ptXpec->aulXpec_r[3] = 0;
  ptXpec->aulXpec_r[4] = 0;
  ptXpec->aulXpec_r[5] = 0;
  ptXpec->aulXpec_r[6] = 0;
  ptXpec->aulXpec_r[7] = 0;
  ptXpec->ulRange01 = 0;
  ptXpec->ulRange23 = 0;
  ptXpec->ulRange45 = 0;
  ptXpec->ulRange67 = 0;
  ptXpec->aulTimer[0] = 0;
  ptXpec->aulTimer[1] = 0;
  ptXpec->aulTimer[2] = 0;
  ptXpec->aulTimer[3] = 0;
  ptXpec->ulUrx_count = 0;
  ptXpec->ulUtx_count = 0;
  ptXpec->ulXpec_statcfg = 0;
  ptXpec->ulEc_maska = 0x0000ffff;
  ptXpec->ulEc_maskb = 0x0000ffff;
  ptXpec->aulEc_mask[0] = 0x0000ffff;
  ptXpec->aulEc_mask[1] = 0x0000ffff;
  ptXpec->aulEc_mask[2] = 0x0000ffff;
  ptXpec->aulEc_mask[3] = 0x0000ffff;
  ptXpec->aulEc_mask[4] = 0x0000ffff;
  ptXpec->aulEc_mask[5] = 0x0000ffff;
  ptXpec->aulEc_mask[6] = 0x0000ffff;
  ptXpec->aulEc_mask[7] = 0x0000ffff;
  ptXpec->aulEc_mask[8] = 0x0000ffff;
  ptXpec->aulEc_mask[9] = 0x0000ffff;
  ptXpec->ulTimer4 = 0;
  ptXpec->ulTimer5 = 0;
  ptXpec->ulIrq  = 0xffff0000;   /* confirm all ARM IRQs */
  ptXpec->ulXpec_adc = 0;

  /* reset SR of current port */
  ptXpec->aulXpec_sr[uPortNo*4]   = 0;
  ptXpec->aulXpec_sr[uPortNo*4+1] = 0;
  ptXpec->aulXpec_sr[uPortNo*4+2] = 0;
  ptXpec->aulXpec_sr[uPortNo*4+3] = 0;
  ptXpec->aulStatcfg[uPortNo]     = 0x00000088;

  /* Hold xMAC */
  ptXmac->ulXmac_tpu_hold_pc = MSK_NX500_xmac_tpu_hold_pc_tpu_hold;
  ptXmac->ulXmac_rpu_hold_pc = MSK_NX500_xmac_rpu_hold_pc_rpu_hold;

  /* reset urx and utx fifos */
  switch( uPortNo )
  {
    case 0:
      ptXmac->ulXmac_config_shared0 |= MSK_NX500_xmac_config_shared0_reset_tx_fifo;
      ptXmac->ulXmac_config_shared0 &= ~MSK_NX500_xmac_config_shared0_reset_tx_fifo;
      ptXmac->ulXmac_config_shared0 |= MSK_NX500_xmac_config_shared0_reset_rx_fifo;
      ptXmac->ulXmac_config_shared0 &= ~MSK_NX500_xmac_config_shared0_reset_rx_fifo;
      break;
    case 1:
      ptXmac->ulXmac_config_shared1 |= MSK_NX500_xmac_config_shared1_reset_tx_fifo;
      ptXmac->ulXmac_config_shared1 &= ~MSK_NX500_xmac_config_shared1_reset_tx_fifo;
      ptXmac->ulXmac_config_shared1 |= MSK_NX500_xmac_config_shared1_reset_rx_fifo;
      ptXmac->ulXmac_config_shared1 &= ~MSK_NX500_xmac_config_shared1_reset_rx_fifo;
      break;
    case 2:
      ptXmac->ulXmac_config_shared2 |= MSK_NX500_xmac_config_shared2_reset_tx_fifo;
      ptXmac->ulXmac_config_shared2 &= ~MSK_NX500_xmac_config_shared2_reset_tx_fifo;
      ptXmac->ulXmac_config_shared2 |= MSK_NX500_xmac_config_shared2_reset_rx_fifo;
      ptXmac->ulXmac_config_shared2 &= ~MSK_NX500_xmac_config_shared2_reset_rx_fifo;
      break;
    case 3:
      ptXmac->ulXmac_config_shared3 |= MSK_NX500_xmac_config_shared3_reset_tx_fifo;
      ptXmac->ulXmac_config_shared3 &= ~MSK_NX500_xmac_config_shared3_reset_tx_fifo;
      ptXmac->ulXmac_config_shared3 |= MSK_NX500_xmac_config_shared3_reset_rx_fifo;
      ptXmac->ulXmac_config_shared3 &= ~MSK_NX500_xmac_config_shared3_reset_rx_fifo;
      break;
  }

  /* reset IRQs from ARM side */
  s_ptXpecIrq->aulIrq_xpec[uPortNo] = 0x0000FFFF;

  /* reset all xMAC registers to default values */
  ptXmac->ulXmac_rx_hw               = 0;
  ptXmac->ulXmac_rx_hw_count         = 0;
  ptXmac->ulXmac_tx                  = 0;
  ptXmac->ulXmac_tx_hw               = 0;
  ptXmac->ulXmac_tx_hw_count         = 0;
  ptXmac->ulXmac_tx_sent             = 0;
  ptXmac->aulXmac_wr[0]              = 0;
  ptXmac->aulXmac_wr[1]              = 0;
  ptXmac->aulXmac_wr[2]              = 0;
  ptXmac->aulXmac_wr[3]              = 0;
  ptXmac->aulXmac_wr[4]              = 0;
  ptXmac->aulXmac_wr[5]              = 0;
  ptXmac->aulXmac_wr[6]              = 0;
  ptXmac->aulXmac_wr[7]              = 0;
  ptXmac->aulXmac_wr[8]              = 0;
  ptXmac->aulXmac_wr[9]              = 0;
  ptXmac->ulXmac_config_mii          = 0;
  ptXmac->ulXmac_config_nibble_fifo  = 0x00000280;
  ptXmac->ulXmac_rpu_count1          = 0;
  ptXmac->ulXmac_rpu_count2          = 0;
  ptXmac->ulXmac_tpu_count1          = 0;
  ptXmac->ulXmac_tpu_count2          = 0;
  ptXmac->ulXmac_rx_count            = 0;
  ptXmac->ulXmac_tx_count            = 0;
  ptXmac->ulXmac_rpm_mask0           = 0;
  ptXmac->ulXmac_rpm_val0            = 0;
  ptXmac->ulXmac_rpm_mask1           = 0;
  ptXmac->ulXmac_rpm_val1            = 0;
  ptXmac->ulXmac_tpm_mask0           = 0;
  ptXmac->ulXmac_tpm_val0            = 0;
  ptXmac->ulXmac_tpm_mask1           = 0;
  ptXmac->ulXmac_tpm_val1            = 0;
  ptXmac->ulXmac_rx_crc_polynomial_l = 0;
  ptXmac->ulXmac_rx_crc_polynomial_h = 0;
  ptXmac->ulXmac_rx_crc_l            = 0;
  ptXmac->ulXmac_rx_crc_h            = 0;
  ptXmac->ulXmac_rx_crc_cfg          = 0;
  ptXmac->ulXmac_tx_crc_polynomial_l = 0;
  ptXmac->ulXmac_tx_crc_polynomial_h = 0;
  ptXmac->ulXmac_tx_crc_l            = 0;
  ptXmac->ulXmac_tx_crc_h            = 0;
  ptXmac->ulXmac_tx_crc_cfg          = 0;

  /* reset encoder and PWM on ports 2 and 3 */
  switch( uPortNo )
  {
    case 2: /* fall through */
    case 3:
      ptXmac->ulXmac_pwm_config = 0;
      ptXmac->ulXmac_pwm_status = 0;
      ptXmac->ulXmac_pwm_tp = 0;
      ptXmac->ulXmac_pwm_tu = 0;
      ptXmac->ulXmac_pwm_tv = 0;
      ptXmac->ulXmac_pwm_tw = 0;
      ptXmac->ulXmac_pwm_td = 0;
      ptXmac->ulXmac_rpwm_tp = 0;
      ptXmac->ulXmac_rpwm_tr = 0;
      ptXmac->ulXmac_pos_config_encoder = 0;
      ptXmac->ulXmac_pos_config_capture = 0;
      ptXmac->ulXmac_pos_command = 0;
      ptXmac->ulXmac_pos_status = 0;
      break;
    default: break;
  }

  return 0;
}


/*****************************************************************************/
/*! Load XC Code
* \description
*   Load XC port.
* \class
*   XC
* \params
*   uPortNo              [in]  XC Port Number
*   eXcType              [in]  XC Entity To Download To (RPU, TPU, xPEC)
*   pulXcPrg             [in]  Pointer To Microcode
* \return
*   0 on success
*   -1 on erroneous                                                          */
/*****************************************************************************/
int NX500_XC_Load( unsigned int uPortNo, NX500_XC_TYPE_E eXcType, const unsigned long *pulXcPrg)
{
  volatile unsigned long *pulDst, *pulDstCnt;
  volatile unsigned long *pulRamStart, *pulRamEnd;
  unsigned long ulPhysicalStart;
  const unsigned long *pulSrcStart, *pulSrcCnt, *pulSrcEnd;
  unsigned int uiElements;

  /* check instance number */
  if( uPortNo>=NUM_XPECS ) return -1;

  /* get the start and end address of the ram area, get the physical address */
  switch( eXcType )
  {
    case NX500_XC_TYPE_XPEC:
      pulRamStart = (volatile unsigned long*) s_ptXpecStart[uPortNo];
      pulRamEnd = pulRamStart + (sizeof(NX500_XPEC_AREA_T) / sizeof(unsigned long));
      ulPhysicalStart = (unsigned long) s_ptXpecStart[uPortNo];
      break;

    case NX500_XC_TYPE_RPU:
      /* fall through */

    case NX500_XC_TYPE_TPU:
      pulRamStart = (volatile unsigned long*)s_ptXmacStart[uPortNo];
      pulRamEnd = pulRamStart + (sizeof(NX500_XMAC_AREA_T)/sizeof(unsigned long));
      ulPhysicalStart = (unsigned long) s_ptXmacStart[uPortNo];
      break;

    default: return -1; /* unknown unit type */
  }

  /* get the number of code elements */
  uiElements = pulXcPrg[0] / sizeof(unsigned long) - 1;

  /* get the pointer in the xc area */
  /* ram_virtual_start + code_physical_start - ram_physical_start */
  pulDst = pulRamStart + (pulXcPrg[2] - ulPhysicalStart) / sizeof(unsigned long);

  /* the code must fit into the ram area */
  if( (pulDst<pulRamStart) || ((pulDst+uiElements)>pulRamEnd) ) {
    /* the code exceeds the xpec ram! */
    return -1;
  }

  /* get source start and end pointer */
  pulSrcStart = pulXcPrg + 3;
  pulSrcEnd = pulSrcStart + uiElements;

  /* copy the code to xc ram */
  pulSrcCnt = pulSrcStart;
  pulDstCnt = pulDst;
  while( pulSrcCnt<pulSrcEnd ) {
    *pulDstCnt = *pulSrcCnt;
    pulDstCnt++;
    pulSrcCnt++;
  }

  /* compare the code */
  pulSrcCnt = pulSrcStart;
  pulDstCnt = pulDst;
  while( pulSrcCnt<pulSrcEnd ) {
    if( *pulDstCnt != *pulSrcCnt ) return -1;
    pulDstCnt++;
    pulSrcCnt++;
  }

  /* get the number of trailing loads */
  uiElements = pulXcPrg[1] / sizeof(unsigned long);

  /* get source start and end pointer */
  pulSrcCnt = pulXcPrg + 2 + pulXcPrg[0] / sizeof(unsigned long);
  pulSrcEnd = pulSrcCnt + uiElements;

  /* write all trailing loads */
  while( pulSrcCnt<pulSrcEnd ) {
    /* get the destination address ( ram_virtual_start + data_physical_start - ram_physical_start) */
    pulDst = pulRamStart + (*pulSrcCnt - ulPhysicalStart) / sizeof(unsigned long);
    pulSrcCnt++;

    /* check the destination address */
    if( (pulDst<pulRamStart) || (pulDst>=pulRamEnd) ) return -1;

    /* write the data */
    *pulDst = *pulSrcCnt;
    pulSrcCnt++;
  }

  return 0;
}
