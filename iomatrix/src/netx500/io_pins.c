/***************************************************************************
 *   Copyright (C) 2013-2014 by Christoph Thelen                           *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "io_pins.h"

#include "netx_io_areas.h"
#include "uprintf.h"
#include "xc_reset.h"


//-------------------------------------
// some defines for the mysterious HIF registers
// taken from the "netX Program Reference Guide Rev0.3", page 16

#define MSK_sta_netx_rdy                        0x00000001U
#define SRT_sta_netx_rdy                        0
#define MSK_sta_netx_run                        0x00000002U
#define SRT_sta_netx_run                        1
#define MSK_sta_netx_netx_state                 0x0000000cU
#define SRT_sta_netx_netx_state                 2
#define MSK_sta_netx_host_state                 0x000000f0U
#define SRT_sta_netx_host_state                 4
#define MSK_sta_netx_netx_sta_code              0x0000ff00U
#define SRT_sta_netx_netx_sta_code              8
#define MSK_sta_netx_rdy_in                     0x00010000U
#define SRT_sta_netx_rdy_in                     16
#define MSK_sta_netx_run_in                     0x00020000U
#define SRT_sta_netx_run_in                     17
#define MSK_sta_netx_rdy_pol                    0x00040000U
#define SRT_sta_netx_rdy_pol                    18
#define MSK_sta_netx_run_pol                    0x00080000U
#define SRT_sta_netx_run_pol                    19
#define MSK_sta_netx_rdy_drv                    0x01000000U
#define SRT_sta_netx_rdy_drv                    24
#define MSK_sta_netx_run_drv                    0x02000000U
#define SRT_sta_netx_run_drv                    25



typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long ulGpio;
	unsigned long ulPio;
	unsigned long aulHifPio[2];
	unsigned long ulRdyRun;
	unsigned long ulRstOut;
	unsigned long aulXmio[4];
} UNITCONFIGURATION_T;


static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->ulGpio       = 0;
	ptUnitCfg->ulPio        = 0;
	ptUnitCfg->aulHifPio[0] = 0;
	ptUnitCfg->aulHifPio[1] = 0;
	ptUnitCfg->ulRdyRun     = 0;
	ptUnitCfg->ulRstOut     = 0;
	ptUnitCfg->aulXmio[0]   = 0;
	ptUnitCfg->aulXmio[1]   = 0;
	ptUnitCfg->aulXmio[2]   = 0;
	ptUnitCfg->aulXmio[3]   = 0;
}



static int collect_unit_configuration(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc, UNITCONFIGURATION_T *ptUnitCfg)
{
	int iResult;
	const PINDESCRIPTION_T *ptPinDescCnt;
	const PINDESCRIPTION_T *ptPinDescEnd;
	unsigned int uiIndex;


	iResult = 0;

	ptPinDescCnt = ptPinDesc;
	ptPinDescEnd = ptPinDesc + sizMaxPinDesc;
	while( ptPinDescCnt<ptPinDescEnd )
	{
		if( (ptPinDescCnt->apcName[0])=='\0' )
		{
			/* End of list. */
			break;
		}
		else
		{
			iResult = -1;

			switch( ptPinDescCnt->tType )
			{
			case PINTYPE_GPIO:
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<16 )
				{
					ptUnitCfg->ulGpio |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_HIFPIO:
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<32 )
				{
					ptUnitCfg->aulHifPio[0] |= 1U<<uiIndex;
					iResult = 0;
				}
				else if( uiIndex<53 )
				{
					ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-32U);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_MLED:
				/* The netX500 has no MLED pins. */
				break;

			case PINTYPE_MMIO:
				uprintf("The pin %s has the type MMIO. This is not available on the netX500/100!", ptPinDescCnt->apcName);
				break;

			case PINTYPE_PIO:
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<32 )
				{
					ptUnitCfg->ulPio |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_RDYRUN:
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<2 )
				{
					ptUnitCfg->ulRdyRun |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_RSTOUT:
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex==0 )
				{
					ptUnitCfg->ulRstOut |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_XMIO:
				/* For now XM*_IO0, XM*_IO1 and XM*_RX are supported. */
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<4 )
				{
					ptUnitCfg->aulXmio[0] |= 1U << uiIndex;
					iResult = 0;
				}
				else if( uiIndex<8 )
				{
					ptUnitCfg->aulXmio[1] |= 1U << (uiIndex - 4U);
					iResult = 0;
				}
				else if( uiIndex<12 )
				{
					ptUnitCfg->aulXmio[2] |= 1U << (uiIndex - 8U);
					iResult = 0;
				}
				else if( uiIndex<16 )
				{
					ptUnitCfg->aulXmio[3] |= 1U << (uiIndex - 12U);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_RAPGPIO:
				uprintf("The pin type RAPGPIO is not supported on this platform!\n");
				break;

			case PINTYPE_APPPIO:
				/* The netX500 has no APPPIO pins. */
				break;

			case PINTYPE_IOLLEDM:
				/* The netX500 has no IOL bridge yet. */
				break;

			case PINTYPE_SQI:
				/* The netX500 has no SQI unit... */
				break;
			}

			if( iResult!=0 )
			{
				break;
			}

			++ptPinDescCnt;
		}
	}

	return iResult;
}



static int configure_xmio(UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac0Area);
	HOSTDEF(ptXmac1Area);
	HOSTDEF(ptXmac2Area);
	HOSTDEF(ptXmac3Area);
	unsigned long ulValue;
	int iResult;


	/* Be optimistic. */
	iResult = 0;

	/* Reset all XCs. netX500/100 has 4 units. */
	NX500_XC_Reset(0);
	NX500_XC_Reset(1);
	NX500_XC_Reset(2);
	NX500_XC_Reset(3);

	/* Are XM0_IO0 or XM0_IO1 in use? */
	if( (ptUnitCfg->aulXmio[0]&3)!=0 )
	{
		/* Set the XM0_IO0 and XM0_IO1 pins to input and output value to zero. */
		ulValue  = 0U << HOSTSRT(statcfg0_gpio0_out); // set output value of gpio0/XM0_IO0 to zero
		ulValue |= 0U << HOSTSRT(statcfg0_gpio0_oe); // set gpio0/XM0_IO0 to input
		ulValue |= 0U << HOSTSRT(statcfg0_gpio1_out); // set output value of gpio1/XM0_IO1 to zero
		ulValue |= 0U << HOSTSRT(statcfg0_gpio1_oe); // set gpio1/XM0_IO1 to input
		ptXpec0Area->aulStatcfg[0] = ulValue;
	}

	/* Are XM1_IO0 or XM1_IO1 in use? */
	if( (ptUnitCfg->aulXmio[1]&3)!=0 )
	{
		/* Set the XM1_IO0 and XM1_IO1 pins to input and output value to zero. */
		ulValue  = 0U << HOSTSRT(statcfg1_gpio0_out); // set output value of gpio0/XM1_IO0 to zero
		ulValue |= 0U << HOSTSRT(statcfg1_gpio0_oe); // set gpio0/XM1_IO0 to input
		ulValue |= 0U << HOSTSRT(statcfg1_gpio1_out); // set output value of gpio1/XM1_IO1 to zero
		ulValue |= 0U << HOSTSRT(statcfg1_gpio1_oe); // set gpio1/XM1_IO1 to input
		ptXpec0Area->aulStatcfg[1] = ulValue;
	}

	/* Are XM2_IO0 or XM2_IO1 in use? */
	if( (ptUnitCfg->aulXmio[2]&3)!=0 )
	{
		/* Set the XM2_IO0 and XM2_IO1 pins to input and output value to zero. */
		ulValue  = 0U << HOSTSRT(statcfg2_gpio0_out); // set output value of gpio0/XM2_IO0 to zero
		ulValue |= 0U << HOSTSRT(statcfg2_gpio0_oe);  // set gpio0/XM1_IO0 to input
		ulValue |= 0U << HOSTSRT(statcfg2_gpio1_out); // set output value of gpio1/XM2_IO1 to zero
		ulValue |= 0U << HOSTSRT(statcfg2_gpio1_oe);  // set gpio1/XM1_IO1 to input
		ptXpec0Area->aulStatcfg[2] = ulValue;
	}

	/* Are XM3_IO0 or XM3_IO1 in use? */
	if( (ptUnitCfg->aulXmio[3]&3)!=0 )
	{
		/* Set the XM3_IO0 and XM3_IO1 pins to input and output value to zero. */
		ulValue  = 0U << HOSTSRT(statcfg3_gpio0_out); // set output value of gpio0/XM3_IO0 to zero
		ulValue |= 0U << HOSTSRT(statcfg3_gpio0_oe); // set gpio0/XM1_IO0 to input
		ulValue |= 0U << HOSTSRT(statcfg3_gpio1_out); // set output value of gpio1/XM3_IO1 to zero
		ulValue |= 0U << HOSTSRT(statcfg3_gpio1_oe); // set gpio1/XM1_IO1 to input
		ptXpec0Area->aulStatcfg[3] = ulValue;
	}

	/* Is XM0_RX in use? */
	if( (ptUnitCfg->aulXmio[0]&4)!=0 )
	{
		/* Activate continuous sampling of the XM0_RX pin. */
		ptXmac0Area->ulXmac_sbu_rate_mul_add = 0x0001U;
		ptXmac0Area->ulXmac_sbu_rate_mul_start = 0xfffeU;
		ptXmac0Area->ulXmac_start_sample_pos = 0xfffeU;
		ptXmac0Area->ulXmac_stop_sample_pos = 0xffffU;
		ptXmac0Area->ulXmac_config_sbu = 1U << HOSTSRT(xmac_config_sbu_count_modulo);
	}
	/* Is XM0_TX in use? */
	if( (ptUnitCfg->aulXmio[0]&8)!=0 )
	{
		ulValue  = HOSTMSK(xmac_config_obu_count_modulo);
		ulValue |= HOSTMSK(xmac_config_obu_tx_three_state);
		ptXmac0Area->ulXmac_config_obu = ulValue;

		ptXmac0Area->ulXmac_obu_rate_mul_add = 0x0001U;
		ptXmac0Area->ulXmac_obu_rate_mul_start = 0xff00U;
		ptXmac0Area->ulXmac_start_trans_pos = 0xff00U;
		ptXmac0Area->ulXmac_stop_trans_pos = 0xffffU;
		ptXmac0Area->ulXmac_tx = 0x0000U;
	}

	/* Is XM1_RX in use? */
	if( (ptUnitCfg->aulXmio[1]&4)!=0 )
	{
		ptXmac1Area->ulXmac_sbu_rate_mul_add = 0x0001;
		ptXmac1Area->ulXmac_sbu_rate_mul_start = 0xfffeU;
		ptXmac1Area->ulXmac_start_sample_pos = 0xfffeU;
		ptXmac1Area->ulXmac_stop_sample_pos = 0xffffU;
		ptXmac1Area->ulXmac_config_sbu = 1U << HOSTSRT(xmac_config_sbu_count_modulo);
	}
	/* Is XM1_TX in use? */
	if( (ptUnitCfg->aulXmio[1]&8)!=0 )
	{
		ulValue  = HOSTMSK(xmac_config_obu_count_modulo);
		ulValue |= HOSTMSK(xmac_config_obu_tx_three_state);
		ptXmac1Area->ulXmac_config_obu = ulValue;

		ptXmac1Area->ulXmac_obu_rate_mul_add = 0x0001U;
		ptXmac1Area->ulXmac_obu_rate_mul_start = 0xff00U;
		ptXmac1Area->ulXmac_start_trans_pos = 0xff00U;
		ptXmac1Area->ulXmac_stop_trans_pos = 0xffffU;
		ptXmac1Area->ulXmac_tx = 0x0000U;
	}

	/* Is XM2_RX in use? */
	if( (ptUnitCfg->aulXmio[2]&4)!=0 )
	{
		ptXmac2Area->ulXmac_sbu_rate_mul_add = 0x0001;
		ptXmac2Area->ulXmac_sbu_rate_mul_start = 0xfffeU;
		ptXmac2Area->ulXmac_start_sample_pos = 0xfffeU;
		ptXmac2Area->ulXmac_stop_sample_pos = 0xffffU;
		ptXmac2Area->ulXmac_config_sbu = 1U << HOSTSRT(xmac_config_sbu_count_modulo);
	}
	/* Is XM2_TX in use? */
	if( (ptUnitCfg->aulXmio[2]&8)!=0 )
	{
		ulValue  = HOSTMSK(xmac_config_obu_count_modulo);
		ulValue |= HOSTMSK(xmac_config_obu_tx_three_state);
		ptXmac2Area->ulXmac_config_obu = ulValue;

		ptXmac2Area->ulXmac_obu_rate_mul_add = 0x0001U;
		ptXmac2Area->ulXmac_obu_rate_mul_start = 0xff00U;
		ptXmac2Area->ulXmac_start_trans_pos = 0xff00U;
		ptXmac2Area->ulXmac_stop_trans_pos = 0xffffU;
		ptXmac2Area->ulXmac_tx = 0x0000U;
	}

	/* Is XM3_RX in use? */
	if( (ptUnitCfg->aulXmio[3]&4)!=0 )
	{
		ptXmac3Area->ulXmac_sbu_rate_mul_add = 0x0001;
		ptXmac3Area->ulXmac_sbu_rate_mul_start = 0xfffeU;
		ptXmac3Area->ulXmac_start_sample_pos = 0xfffeU;
		ptXmac3Area->ulXmac_stop_sample_pos = 0xffffU;
		ptXmac3Area->ulXmac_config_sbu = 1U << HOSTSRT(xmac_config_sbu_count_modulo);
	}
	/* Is XM3_TX in use? */
	if( (ptUnitCfg->aulXmio[3]&8)!=0 )
	{
		ulValue  = HOSTMSK(xmac_config_obu_count_modulo);
		ulValue |= HOSTMSK(xmac_config_obu_tx_three_state);
		ptXmac3Area->ulXmac_config_obu = ulValue;

		ptXmac3Area->ulXmac_obu_rate_mul_add = 0x0001U;
		ptXmac3Area->ulXmac_obu_rate_mul_start = 0xff00U;
		ptXmac3Area->ulXmac_start_trans_pos = 0xff00U;
		ptXmac3Area->ulXmac_stop_trans_pos = 0xffffU;
		ptXmac3Area->ulXmac_tx = 0x0000U;
	}

	return iResult;
}



int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptGpioArea);
	HOSTDEF(ptPioArea);
	HOSTDEF(ptNetxControlledGlobalRegisterBlock1Area);
	HOSTDEF(ptNetxControlledGlobalRegisterBlock2Area);
	UNITCONFIGURATION_T tUnitCfg;
	unsigned long ulValue;
	unsigned int uiCnt;


	initialize_unit_configuration(&tUnitCfg);
	iResult = collect_unit_configuration(ptPinDesc, sizMaxPinDesc, &tUnitCfg);
	if( iResult==0 )
	{
		/*
		 *  GPIO
		 *
		 */
		if( tUnitCfg.ulGpio!=0 )
		{
			/* Configure the selected GPIO pins as input. */
			for(uiCnt=0; uiCnt<32; uiCnt++)
			{
				ulValue = tUnitCfg.ulGpio & (1U<<uiCnt);
				if( ulValue!=0 )
				{
					ptGpioArea->aulGpio_cfg[uiCnt] = 0;
				}
			}
		}

		/*
		 *  HIFPIO
		 */
		if( (tUnitCfg.aulHifPio[0]|tUnitCfg.aulHifPio[1])!=0 )
		{
			/* Switch the interface to PIO. */
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptAsicCtrlArea->ulIo_config &= ~HOSTMSK(io_config_if_select_n);

			/* enable output mode */
			ptNetxControlledGlobalRegisterBlock2Area->ulIf_conf1 = (4<<28);

			ptNetxControlledGlobalRegisterBlock2Area->ulIo_reg_drv_en0 = 0;
			ptNetxControlledGlobalRegisterBlock2Area->ulIo_reg_drv_en1 = 0;

			/* Set all pins to PIO mode, it makes no sense to keep some in DPM mode. */
			ptNetxControlledGlobalRegisterBlock2Area->ulIo_reg_mode0 = 0;
			ptNetxControlledGlobalRegisterBlock2Area->ulIo_reg_mode1 = (1<<30);
		}

		/*
		 *  PIO
		 */
		if( tUnitCfg.ulPio!=0 )
		{
			/* Configure all selected PIO pins as input. */
			ulValue  = ptPioArea->ulPio_oe;
			ulValue &= tUnitCfg.ulPio ^ 0xffffffffU;
			ptPioArea->ulPio_oe = ulValue;
		}

		/*
		 *  RDY/RUN
		 */
		if( tUnitCfg.ulRdyRun!=0 )
		{
			/* Configure the selected pins as input and non-inverted polarity. */
			ulValue = ptNetxControlledGlobalRegisterBlock1Area->ulSta_netx;
			if( (tUnitCfg.ulRdyRun & 1U)!=0 )
			{
				ulValue &= ~(MSK_sta_netx_rdy_drv | MSK_sta_netx_rdy_pol);
			}
			if( (tUnitCfg.ulRdyRun & 2U)!=0 )
			{
				ulValue &= ~(MSK_sta_netx_run_drv | MSK_sta_netx_run_pol);
			}
			ptNetxControlledGlobalRegisterBlock1Area->ulSta_netx = ulValue;
		}

		/*
		 * RstOut
		 */
		if( tUnitCfg.ulRstOut!=0 )
		{
			/* Disable the reset out driver. */
			ulValue  = ptAsicCtrlArea->ulReset_ctrl;
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT));
#if 0
			/* NOTE: This is not necessary for the netX500, but "recommended to keep software compatible for later netX versions". */
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
#endif
			ptAsicCtrlArea->ulReset_ctrl = ulValue;
		}

		/*
		 * XMIOs
		 */
		if( (tUnitCfg.aulXmio[0]|tUnitCfg.aulXmio[1]|tUnitCfg.aulXmio[2]|tUnitCfg.aulXmio[3])!=0 )
		{
			iResult = configure_xmio(&tUnitCfg);
		}

	}

	return iResult;
}


/*---------------------------------------------------------------------------*/



static int set_xm0io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac0Area);
	unsigned long ulStatcfg;
	unsigned long ulOut;
	unsigned long ulOe;
	unsigned long ulMask_out;
	unsigned long ulMask_oe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<2U )
	{
		ulStatcfg = ptXpec0Area->aulStatcfg[0];
		ulOut  = ulStatcfg & (HOSTMSK(statcfg0_gpio0_out) | HOSTMSK(statcfg0_gpio1_out));
		ulOe = ulStatcfg & (HOSTMSK(statcfg0_gpio0_oe) | HOSTMSK(statcfg0_gpio1_oe));
		ulMask_out = HOSTMSK(statcfg0_gpio0_out) << (2*uiIndex);
		ulMask_oe = HOSTMSK(statcfg0_gpio0_oe) << (2*uiIndex);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit. */
			ulOe &= ~ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set HIGHZ of XM0_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT0 of XM0_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Set the out bit. */
			ulOut |= ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT1 of XM0_IO index: %d\n",uiIndex);

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXpec0Area->aulStatcfg[0] = ulOut | ulOe;
		}
		else
		{
			/* DEBUG INFORMATION: */
			uprintf("ERROR: set XM0_IO index: %d\n",uiIndex);
		}
	}
	else if( uiIndex==2U )
	{
		if( tValue==PINSTATUS_HIGHZ )
		{
			iResult = 0;
		}
		else
		{
			uprintf("ERROR: can not set XM0_RX to output, it is read only.\n");
		}
	}
	else if( uiIndex==3U )
	{
		ulOe  = HOSTMSK(xmac_config_obu_count_modulo);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			ulOe |= HOSTMSK(xmac_config_obu_tx_three_state);
			ptXmac0Area->ulXmac_config_obu = ulOe;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			ptXmac0Area->ulXmac_config_obu = ulOe;
			ptXmac0Area->ulXmac_tx = 0x0000U;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ptXmac0Area->ulXmac_config_obu = ulOe;
			ptXmac0Area->ulXmac_tx = 0xffffU;

			iResult = 0;
			break;
		}
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM0_IO: %d\n", uiIndex);
	}

	return iResult;
}



static int set_xm1io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac1Area);
	unsigned long ulStatcfg;
	unsigned long ulOut;
	unsigned long ulOe;
	unsigned long ulMask_out;
	unsigned long ulMask_oe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<2U )
	{
		ulStatcfg = ptXpec0Area->aulStatcfg[1];
		ulOut  = ulStatcfg & (HOSTMSK(statcfg1_gpio0_out) | HOSTMSK(statcfg1_gpio1_out));
		ulOe = ulStatcfg & (HOSTMSK(statcfg1_gpio0_oe) | HOSTMSK(statcfg1_gpio1_oe));
		ulMask_out = HOSTMSK(statcfg1_gpio0_out) << (2*uiIndex);
		ulMask_oe = HOSTMSK(statcfg1_gpio0_oe) << (2*uiIndex);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit. */
			ulOe &= ~ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set HIGHZ of XM1_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT0 of XM1_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Set the out bit. */
			ulOut |= ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT1 of XM1_IO index: %d\n",uiIndex);

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXpec0Area->aulStatcfg[1] = ulOut | ulOe;
		}
		else
		{
			/* DEBUG INFORMATION: */
			uprintf("ERROR: set XM1_IO index: %d\n",uiIndex);
		}
	}
	else if( uiIndex==2U )
	{
		if( tValue==PINSTATUS_HIGHZ )
		{
			iResult = 0;
		}
		else
		{
			uprintf("ERROR: can not set XM1_RX to output, it is read only.\n");
		}
	}
	else if( uiIndex==3U )
	{
		ulOe  = HOSTMSK(xmac_config_obu_count_modulo);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			ulOe |= HOSTMSK(xmac_config_obu_tx_three_state);
			ptXmac1Area->ulXmac_config_obu = ulOe;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			ptXmac1Area->ulXmac_config_obu = ulOe;
			ptXmac1Area->ulXmac_tx = 0x0000U;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ptXmac1Area->ulXmac_config_obu = ulOe;
			ptXmac1Area->ulXmac_tx = 0xffffU;

			iResult = 0;
			break;
		}
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM1_IO: %d\n", uiIndex);
	}

	return iResult;
}



static int set_xm2io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac2Area);
	unsigned long ulStatcfg;
	unsigned long ulOut;
	unsigned long ulOe;
	unsigned long ulMask_out;
	unsigned long ulMask_oe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<2U )
	{
		ulStatcfg = ptXpec0Area->aulStatcfg[2];
		ulOut  = ulStatcfg & (HOSTMSK(statcfg2_gpio0_out) | HOSTMSK(statcfg2_gpio1_out));
		ulOe = ulStatcfg & (HOSTMSK(statcfg2_gpio0_oe) | HOSTMSK(statcfg2_gpio1_oe));
		ulMask_out = HOSTMSK(statcfg2_gpio0_out) << (2*uiIndex);
		ulMask_oe = HOSTMSK(statcfg2_gpio0_oe) << (2*uiIndex);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit. */
			ulOe &= ~ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set HIGHZ of XM2_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT0 of XM2_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Set the out bit. */
			ulOut |= ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT1 of XM2_IO index: %d\n",uiIndex);

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXpec0Area->aulStatcfg[2] = ulOut | ulOe;
		}
		else
		{
			/* DEBUG INFORMATION: */
			uprintf("ERROR: set XM2_IO index: %d\n",uiIndex);
		}
	}
	else if( uiIndex==2U )
	{
		if( tValue==PINSTATUS_HIGHZ )
		{
			iResult = 0;
		}
		else
		{
			uprintf("ERROR: can not set XM2_RX to output, it is read only.\n");
		}
	}
	else if( uiIndex==3U )
	{
		ulOe  = HOSTMSK(xmac_config_obu_count_modulo);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			ulOe |= HOSTMSK(xmac_config_obu_tx_three_state);
			ptXmac2Area->ulXmac_config_obu = ulOe;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			ptXmac2Area->ulXmac_config_obu = ulOe;
			ptXmac2Area->ulXmac_tx = 0x0000U;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ptXmac2Area->ulXmac_config_obu = ulOe;
			ptXmac2Area->ulXmac_tx = 0xffffU;

			iResult = 0;
			break;
		}
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM2_IO: %d\n", uiIndex);
	}

	return iResult;
}



static int set_xm3io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac3Area);
	unsigned long ulStatcfg;
	unsigned long ulOut;
	unsigned long ulOe;
	unsigned long ulMask_out;
	unsigned long ulMask_oe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<2U )
	{
		ulStatcfg = ptXpec0Area->aulStatcfg[3];
		ulOut  = ulStatcfg & (HOSTMSK(statcfg3_gpio0_out) | HOSTMSK(statcfg3_gpio1_out));
		ulOe = ulStatcfg & (HOSTMSK(statcfg3_gpio0_oe) | HOSTMSK(statcfg3_gpio1_oe));
		ulMask_out = HOSTMSK(statcfg3_gpio0_out) << (2*uiIndex);
		ulMask_oe = HOSTMSK(statcfg3_gpio0_oe) << (2*uiIndex);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit. */
			ulOe &= ~ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set HIGHZ of XM3_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Clear the out bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT0 of XM3_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit. */
			ulOe |= ulMask_oe;

			/* Set the out bit. */
			ulOut |= ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT1 of XM3_IO index: %d\n",uiIndex);

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXpec0Area->aulStatcfg[3] = ulOut | ulOe;
		}
		else
		{
			/* DEBUG INFORMATION: */
			uprintf("ERROR: set XM3_IO index: %d\n",uiIndex);
		}
	}
	else if( uiIndex==2U )
	{
		if( tValue==PINSTATUS_HIGHZ )
		{
			iResult = 0;
		}
		else
		{
			uprintf("ERROR: can not set XM3_RX to output, it is read only.\n");
		}
	}
	else if( uiIndex==3U )
	{
		ulOe  = HOSTMSK(xmac_config_obu_count_modulo);

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			ulOe |= HOSTMSK(xmac_config_obu_tx_three_state);
			ptXmac3Area->ulXmac_config_obu = ulOe;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			ptXmac3Area->ulXmac_config_obu = ulOe;
			ptXmac3Area->ulXmac_tx = 0x0000U;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ptXmac3Area->ulXmac_config_obu = ulOe;
			ptXmac3Area->ulXmac_tx = 0xffffU;

			iResult = 0;
			break;
		}
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM3_IO: %d\n", uiIndex);
	}

	return iResult;
}



static PIN_INVALUE_T get_xm0io(unsigned int uiIndex)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac0Area);
	unsigned long ulStatus;
	unsigned long ulValue;
	unsigned int uiRetries;
	PIN_INVALUE_T tResult;


	if( uiIndex>=4U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		if( uiIndex<2U )
		{
			ulStatus = ptXpec0Area->aulStatcfg[0];
			ulValue = ulStatus & HOSTMSK(statcfg0_gpio0_in) << uiIndex;
			if( ulValue==0 )
			{
				tResult = PIN_INVALUE_0;
			}
			else
			{
				tResult = PIN_INVALUE_1;
			}
		}
		else if( uiIndex==2U )
		{
			uiRetries = 16U;
			tResult = PIN_INVALUE_FailedToRead;
			while( uiRetries!=0 )
			{
				ulValue = ptXmac0Area->ulXmac_rx;
				if( ulValue==0x0000U )
				{
					tResult = PIN_INVALUE_0;
					break;
				}
				else if( ulValue==0xffffU )
				{
					tResult = PIN_INVALUE_1;
					break;
				}
				else
				{
					--uiRetries;
				}
			}
		}
		else if( uiIndex==3U )
		{
			tResult = PIN_INVALUE_InputNotAvailable;
		}
	}

	return tResult;
}



static PIN_INVALUE_T get_xm1io(unsigned int uiIndex)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac1Area);
	unsigned long ulStatus;
	unsigned long ulValue;
	unsigned int uiRetries;
	PIN_INVALUE_T tResult;


	if( uiIndex>=4U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		if( uiIndex<2U )
		{
			ulStatus = ptXpec0Area->aulStatcfg[1];
			ulValue = ulStatus & HOSTMSK(statcfg1_gpio0_in) << uiIndex;
			if( ulValue==0 )
			{
				tResult = PIN_INVALUE_0;
			}
			else
			{
				tResult = PIN_INVALUE_1;
			}
		}
		else if( uiIndex==2U )
		{
			uiRetries = 16U;
			tResult = PIN_INVALUE_FailedToRead;
			while( uiRetries!=0 )
			{
				ulValue = ptXmac1Area->ulXmac_rx;
				if( ulValue==0x0000U )
				{
					tResult = PIN_INVALUE_0;
					break;
				}
				else if( ulValue==0xffffU )
				{
					tResult = PIN_INVALUE_1;
					break;
				}
				else
				{
					--uiRetries;
				}
			}
		}
		else if( uiIndex==3U )
		{
			tResult = PIN_INVALUE_InputNotAvailable;
		}
	}

	return tResult;
}



static PIN_INVALUE_T get_xm2io(unsigned int uiIndex)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac2Area);
	unsigned long ulStatus;
	unsigned long ulValue;
	unsigned int uiRetries;
	PIN_INVALUE_T tResult;


	if( uiIndex>=4U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		if( uiIndex<2U )
		{
			ulStatus = ptXpec0Area->aulStatcfg[2];
			ulValue = ulStatus & HOSTMSK(statcfg2_gpio0_in) << uiIndex;
			if( ulValue==0 )
			{
				tResult = PIN_INVALUE_0;
			}
			else
			{
				tResult = PIN_INVALUE_1;
			}
		}
		else if( uiIndex==2U )
		{
			uiRetries = 16U;
			tResult = PIN_INVALUE_FailedToRead;
			while( uiRetries!=0 )
			{
				ulValue = ptXmac2Area->ulXmac_rx;
				uprintf("XM2_RX=0x%04x\n", ulValue);
				if( ulValue==0x0000U )
				{
					tResult = PIN_INVALUE_0;
					break;
				}
				else if( ulValue==0xffffU )
				{
					tResult = PIN_INVALUE_1;
					break;
				}
				else
				{
					--uiRetries;
				}
			}
		}
		else if( uiIndex==3U )
		{
			tResult = PIN_INVALUE_InputNotAvailable;
		}
	}

	return tResult;
}



static PIN_INVALUE_T get_xm3io(unsigned int uiIndex)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXmac3Area);
	unsigned long ulStatus;
	unsigned long ulValue;
	unsigned int uiRetries;
	PIN_INVALUE_T tResult;


	if( uiIndex>=4U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		if( uiIndex<2U )
		{
			ulStatus = ptXpec0Area->aulStatcfg[3];
			ulValue = ulStatus & HOSTMSK(statcfg3_gpio0_in) << uiIndex;
			if( ulValue==0 )
			{
				tResult = PIN_INVALUE_0;
			}
			else
			{
				tResult = PIN_INVALUE_1;
			}
		}
		else if( uiIndex==2U )
		{
			uiRetries = 16U;
			tResult = PIN_INVALUE_FailedToRead;
			while( uiRetries!=0 )
			{
				ulValue = ptXmac3Area->ulXmac_rx;
				uprintf("XM3_RX=0x%04x\n", ulValue);
				if( ulValue==0x0000U )
				{
					tResult = PIN_INVALUE_0;
					break;
				}
				else if( ulValue==0xffffU )
				{
					tResult = PIN_INVALUE_1;
					break;
				}
				else
				{
					--uiRetries;
				}
			}
		}
		else if( uiIndex==3U )
		{
			tResult = PIN_INVALUE_InputNotAvailable;
		}
	}

	return tResult;
}



static int set_gpio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptGpioArea);

	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex<16 )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Set input mode. */
			ptGpioArea->aulGpio_cfg[uiIndex] = 0;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set PIO mode, set the output enable bit and the output bit to 0. */
			ptGpioArea->aulGpio_cfg[uiIndex] = 1;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set PIO mode, set the output enable bit and the output bit to 1. */
			ptGpioArea->aulGpio_cfg[uiIndex] = 9;
			iResult = 0;
			break;
		}
	}

	return iResult;
}



static PIN_INVALUE_T get_gpio(unsigned int uiIndex)
{
	PIN_INVALUE_T tResult;
	HOSTDEF(ptGpioArea);
	unsigned long ulValue;


	/* Check the index. */
	if( uiIndex>=32U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		/* Get the input value. */
		ulValue  = ptGpioArea->ulGpio_in;
		ulValue &= 1U << uiIndex;
		if( ulValue==0 )
		{
			tResult = PIN_INVALUE_0;
		}
		else
		{
			tResult = PIN_INVALUE_1;
		}
	}

	return tResult;
}



static int set_pio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptPioArea);
	unsigned long ulOe;
	unsigned long ulOut;
	unsigned long ulMask;


	/* Be pessimistic... */
	iResult = -1;

	/* Check the index. */
	if( uiIndex<32 )
	{
		ulOe  = ptPioArea->ulPio_oe;
		ulOut = ptPioArea->ulPio_out;
		ulMask = 1U << uiIndex;

		/* Set the value. */
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit. */
			ulOe  &= ~ulMask;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit and the output bit to 0. */
			ulOe  |=  ulMask;
			ulOut &= ~ulMask;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit and the output bit to 1. */
			ulOe  |=  ulMask;
			ulOut |=  ulMask;
			iResult = 0;
			break;
		}

		/* Reconfigure the PIO pin. */
		if( iResult==0 )
		{
			ptPioArea->ulPio_out = ulOut;
			ptPioArea->ulPio_oe  = ulOe;
		}
	}

	return iResult;
}



static PIN_INVALUE_T get_pio(unsigned int uiIndex)
{
	PIN_INVALUE_T tResult;
	HOSTDEF(ptPioArea);
	unsigned long ulValue;

	/* Check the index. */
	if( uiIndex>=32U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		ulValue  = ptPioArea->ulPio_in;
		ulValue &= 1U << uiIndex;
		if (ulValue == 0)
		{
			tResult = PIN_INVALUE_0;
		}
		else
		{
			tResult = PIN_INVALUE_1;
		}
	}

	return tResult;
}



static int set_rdyrun(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptNetxControlledGlobalRegisterBlock1Area);
	int iResult;
	unsigned long ulMaskOe;
	unsigned long ulMaskOut;
	unsigned long ulValue;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex<2 )
	{
		if( uiIndex==0 )
		{
			ulMaskOe  = MSK_sta_netx_rdy_drv;
			ulMaskOut = MSK_sta_netx_rdy;
		}
		else
		{
			ulMaskOe  = MSK_sta_netx_run_drv;
			ulMaskOut = MSK_sta_netx_run;
		}

		ulValue = ptNetxControlledGlobalRegisterBlock1Area->ulSta_netx;
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Set input mode. */
			ulValue &= ~ulMaskOe;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit and the output bit to 0. */
			ulValue |=  ulMaskOe;
			ulValue &= ~ulMaskOut;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit and the output bit to 1. */
			ulValue |=  ulMaskOe;
			ulValue |=  ulMaskOut;
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptNetxControlledGlobalRegisterBlock1Area->ulSta_netx = ulValue;
		}
	}

	return iResult;
}



static PIN_INVALUE_T get_rdyrun(unsigned int uiIndex)
{
	PIN_INVALUE_T tResult;
	HOSTDEF(ptNetxControlledGlobalRegisterBlock1Area);
	unsigned long ulMaskIn;
	unsigned long ulValue;

	/* Check the index. */
	if( uiIndex>=2U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		if( uiIndex==0 )
		{
			ulMaskIn  = MSK_sta_netx_rdy_in;
		}
		else
		{
			ulMaskIn  = MSK_sta_netx_run_in;
		}

		/* Get the input value. */
		ulValue  = ptNetxControlledGlobalRegisterBlock1Area->ulSta_netx;
		ulValue &= ulMaskIn;
		if( ulValue==0 )
		{
			tResult = PIN_INVALUE_0;
		}
		else
		{
			tResult = PIN_INVALUE_1;
		}
	}

	return tResult;
}



static int set_rstout(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptAsicCtrlArea);
	int iResult;
	unsigned long ulValue;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex==0 )
	{
		ulValue = ptAsicCtrlArea->ulReset_ctrl;
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Set input mode. */
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT));
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit and the output bit to 0. */
			ulValue |=  HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit and the output bit to 1. */
			ulValue |=  HOSTMSK(reset_ctrl_EN_RES_REQ_OUT);
			ulValue &= ~HOSTMSK(reset_ctrl_RES_REQ_OUT);
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
#if 0
			/* NOTE: This is not necessary for the netX500, but "recommended to keep software compatible for later netX versions". */
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
#endif
			ptAsicCtrlArea->ulReset_ctrl = ulValue;
		}
	}

	return iResult;
}



static PIN_INVALUE_T get_rstout(unsigned int uiIndex)
{
	PIN_INVALUE_T tResult;


	if( uiIndex>=1U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		/* The RST_OUT pin is output only. */
		tResult = PIN_INVALUE_InputNotAvailable;
	}

	return tResult;
}


/*---------------------------------------------------------------------------*/


int iopins_set(const PINDESCRIPTION_T *ptPinDescription, PINSTATUS_T tValue)
{
	int iResult;
	unsigned int uiIndex;


	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_gpio(uiIndex, tValue);
		break;

	case PINTYPE_HIFPIO:
		/* Not yet... */
		break;

	case PINTYPE_MLED:
		/* The netX500 has no MLED pins. */
		break;

	case PINTYPE_MMIO:
		/* Not available! */
		break;

	case PINTYPE_PIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_pio(uiIndex, tValue);
		break;

	case PINTYPE_RDYRUN:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_rdyrun(uiIndex, tValue);
		break;

	case PINTYPE_RSTOUT:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_rstout(uiIndex, tValue);
		break;

	case PINTYPE_XMIO:
		uiIndex = ptPinDescription->uiIndex;
		if( uiIndex<4 )
		{
			iResult = set_xm0io(uiIndex, tValue);
		}
		else if( uiIndex<8 )
		{
			iResult = set_xm1io(uiIndex-4U, tValue);
		}
		else if( uiIndex<12 )
		{
			iResult = set_xm2io(uiIndex-8U, tValue);
		}
		else if( uiIndex<16 )
		{
			iResult = set_xm3io(uiIndex-12U, tValue);
		}
		break;

	case PINTYPE_RAPGPIO:
		uprintf("The pin type RAPGPIO is not supported on this platform!\n");
		break;

	case PINTYPE_APPPIO:
		/* The netX500 has no APP PIOs. */
		break;

	case PINTYPE_IOLLEDM:
		/* The netX500 has no IOL bridge yet. */
		break;

	case PINTYPE_SQI:
		/* Not yet... */
		break;
	}

	return iResult;
}


PIN_INVALUE_T iopins_get(const PINDESCRIPTION_T *ptPinDescription)
{
	PIN_INVALUE_T tResult;
	unsigned int uiIndex;


	uiIndex = ptPinDescription->uiIndex;

	tResult = PIN_INVALUE_InvalidPinType;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		tResult = get_gpio(uiIndex);
		break;

	case PINTYPE_HIFPIO:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_MLED:
		tResult = PIN_INVALUE_PintypeNotAvailable;
		break;

	case PINTYPE_MMIO:
		tResult = PIN_INVALUE_PintypeNotAvailable;
		break;

	case PINTYPE_PIO:
		tResult = get_pio(uiIndex);
		break;

	case PINTYPE_RDYRUN:
		tResult = get_rdyrun(uiIndex);
		break;

	case PINTYPE_RSTOUT:
		tResult = get_rstout(uiIndex);
		break;

	case PINTYPE_XMIO:
		if( uiIndex<4 )
		{
			tResult = get_xm0io(uiIndex);
		}
		else if( uiIndex<8 )
		{
			tResult = get_xm1io(uiIndex-4U);
		}
		else if( uiIndex<12 )
		{
			tResult = get_xm2io(uiIndex-8U);
		}
		else if( uiIndex<16 )
		{
			tResult = get_xm3io(uiIndex-12U);
		}
		else
		{
			tResult = PIN_INVALUE_InvalidPinIndex;
		}
		break;

	case PINTYPE_RAPGPIO:
		tResult = PIN_INVALUE_PintypeNotAvailable;
		break;

	case PINTYPE_APPPIO:
		tResult = PIN_INVALUE_PintypeNotAvailable;
		break;

	case PINTYPE_IOLLEDM:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_SQI:
		tResult = PIN_INVALUE_PintypeNotAvailable;
		break;
	}

	return tResult;
}


int iopins_get_initial(const PINDESCRIPTION_T *ptPinDescription, PINSTATUS_T *ptValue)
{
	int iResult;
	PINSTATUS_T tValue;


	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		/* GPIO pins are initially switched to input. */
		tValue = PINSTATUS_HIGHZ;
		iResult = 0;
		break;

	case PINTYPE_HIFPIO:
		/* Not supported yet. */
		break;

	case PINTYPE_MLED:
		/* Not available. */
		break;

	case PINTYPE_MMIO:
		/* Not available. */
		break;

	case PINTYPE_PIO:
		/* PIO pins are initially switched to input. */
		tValue = PINSTATUS_HIGHZ;
		iResult = 0;
		break;

	case PINTYPE_RDYRUN:
		/* RDY/RUN pins are initially switched to input. */
		tValue = PINSTATUS_HIGHZ;
		iResult = 0;
		break;

	case PINTYPE_RSTOUT:
		/* The RSTOUT pin is initially switched to input. */
		tValue = PINSTATUS_HIGHZ;
		iResult = 0;
		break;

	case PINTYPE_XMIO:
		/* The netX500 has 4 units with 4 pins each.
		 * All pins of each unit are initially input.
		 */
		/* The RSTOUT pin is initially switched to input. */
		tValue = PINSTATUS_HIGHZ;
		iResult = 0;
		break;

	case PINTYPE_RAPGPIO:
		/* Not available. */
		break;

	case PINTYPE_APPPIO:
		/* Not available. */
		break;

	case PINTYPE_IOLLEDM:
		/* Not supported yet. */
		break;

	case PINTYPE_SQI:
		/* Not available. */
		break;
	}

	if( iResult==0 )
	{
		*ptValue = tValue;
	}

	return iResult;
}
