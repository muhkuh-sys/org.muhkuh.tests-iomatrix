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
	unsigned long aulXmio[2];
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
				/* For now XM0_IO0-1 and XM1_IO0-1 are supported with the indices 0..1 . */
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<2 )
				{
					ptUnitCfg->aulXmio[0] |= 1U << uiIndex;
					iResult = 0;
				}
				else if( uiIndex<4 )
				{
					ptUnitCfg->aulXmio[1] |= 1U << (uiIndex - 2U);
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



static int configure_xmio(void)
{
	HOSTDEF(ptXpec0Area);
	HOSTDEF(ptXpec1Area);
	HOSTDEF(ptXpec2Area);
	HOSTDEF(ptXpec3Area);
	unsigned long ulValue;
	int iResult;
	unsigned long ulStatus;
	int iStatus_out;
	int iStatus_in;
	int iStatus_oe;

	/* Be optimistic. */
	iResult = 0;

	/* Stop xpu unit */
	/* 0: start XPU, 1: hold XPU  */
	/* As a precaution, the register xpu_hold_pc of Xpec1-Xpec3 are also switched off - but it is not absolutely necessary */
	ptXpec0Area->ulXpu_hold_pc = HOSTMSK(xpu_hold_pc_hold);
	ptXpec1Area->ulXpu_hold_pc = HOSTMSK(xpu_hold_pc_hold);
	ptXpec2Area->ulXpu_hold_pc = HOSTMSK(xpu_hold_pc_hold);
	ptXpec3Area->ulXpu_hold_pc = HOSTMSK(xpu_hold_pc_hold);

	/* Have the XPUs unit stopped? */
	ulStatus =  ptXpec0Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold);
	ulStatus &= ptXpec1Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold);
	ulStatus &= ptXpec2Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold);
	ulStatus &= ptXpec3Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold);

	/* DEBUG INFORMATION: */
	uprintf("STATUS: Config Xpec0 unit:%d\n",ptXpec0Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold));
	uprintf("STATUS: Config Xpec1 unit:%d\n",ptXpec1Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold));
	uprintf("STATUS: Config Xpec2 unit:%d\n",ptXpec2Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold));
	uprintf("STATUS: Config Xpec3 unit:%d\n",ptXpec3Area->ulXpu_hold_pc & HOSTMSK(xpu_hold_pc_hold));

	if (ulStatus != HOSTMSK(xpu_hold_pc_hold))
	{
		uprintf("ERROR: XMIO pins can not be used as the unit is running.\n");
		iResult = -1;
	}
	else
	{
		/* Set all pins to input and output value to zero. */
		ulValue = 0;
		ulValue |= 0U << HOSTSRT(statcfg0_gpio0_out); // set output value of gpio0/XM0_IO0 to zero
		ulValue |= 0U << HOSTSRT(statcfg0_gpio0_oe); // set gpio0/XM0_IO0 to input
		ulValue |= 0U << HOSTSRT(statcfg0_gpio1_out); // set output value of gpio1/XM0_IO1 to zero
		ulValue |= 0U << HOSTSRT(statcfg0_gpio1_oe); // set gpio1/XM0_IO1 to input
		ptXpec0Area->aulStatcfg[0] = ulValue;

		/* DEBUG INFORMATION: */
		ulStatus = ptXpec0Area->aulStatcfg[0];

		iStatus_out = (ulStatus & HOSTMSK(statcfg0_gpio0_out)) >> HOSTSRT(statcfg0_gpio0_out);
		iStatus_in = (ulStatus & HOSTMSK(statcfg0_gpio0_in)) >> HOSTSRT(statcfg0_gpio0_in);
		iStatus_oe = (ulStatus & HOSTMSK(statcfg0_gpio0_oe)) >> HOSTSRT(statcfg0_gpio0_oe);
		uprintf("STATUS: config Statcfg0 GPIO0/XM0_IO0 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);

		iStatus_out = (ulStatus & HOSTMSK(statcfg0_gpio1_out)) >> HOSTSRT(statcfg0_gpio1_out);
		iStatus_in = (ulStatus & HOSTMSK(statcfg0_gpio1_in)) >> HOSTSRT(statcfg0_gpio1_in);
		iStatus_oe = (ulStatus & HOSTMSK(statcfg0_gpio1_oe)) >> HOSTSRT(statcfg0_gpio1_oe);
		uprintf("STATUS: config Statcfg0 GPIO1/XM0_IO1 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);

		/* Set all pins to input and output value to zero. */
		ulValue = 0;
		ulValue |= 0U << HOSTSRT(statcfg1_gpio0_out); // set output value of gpio0/XM1_IO0 to zero
		ulValue |= 0U << HOSTSRT(statcfg1_gpio0_oe); // set gpio0/XM1_IO0 to input
		ulValue |= 0U << HOSTSRT(statcfg1_gpio1_out); // set output value of gpio1/XM1_IO1 to zero
		ulValue |= 0U << HOSTSRT(statcfg1_gpio1_oe); // set gpio1/XM1_IO1 to input
		ptXpec0Area->aulStatcfg[1] = ulValue;

		/* DEBUG INFORMATION: */
		ulStatus = ptXpec0Area->aulStatcfg[1];

		iStatus_out = (ulStatus & HOSTMSK(statcfg1_gpio0_out)) >> HOSTSRT(statcfg1_gpio0_out);
		iStatus_in = (ulStatus & HOSTMSK(statcfg1_gpio0_in)) >> HOSTSRT(statcfg1_gpio0_in);
		iStatus_oe = (ulStatus & HOSTMSK(statcfg1_gpio0_oe)) >> HOSTSRT(statcfg1_gpio0_oe);
		uprintf("STATUS: condig Statcfg1 GPIO0/XM1_IO0 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);

		iStatus_out = (ulStatus & HOSTMSK(statcfg1_gpio1_out)) >> HOSTSRT(statcfg1_gpio1_out);
		iStatus_in = (ulStatus & HOSTMSK(statcfg1_gpio1_in)) >> HOSTSRT(statcfg1_gpio1_in);
		iStatus_oe = (ulStatus & HOSTMSK(statcfg1_gpio1_oe)) >> HOSTSRT(statcfg1_gpio1_oe);
		uprintf("STATUS: condig Statcfg1 GPIO1/XM1_IO1 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);
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
		if( tUnitCfg.aulXmio[0]!=0 | tUnitCfg.aulXmio[1]!=0 )
		{
			iResult = configure_xmio();
		}

	}

	return iResult;
}


/*---------------------------------------------------------------------------*/



static int set_xm0io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXpec0Area);
	unsigned long ulStatcfg;
	unsigned long ulOut;
	unsigned long ulOe;
	unsigned long ulMask_out;
	unsigned long ulMask_oe;
	int iResult;
	unsigned long ulStatus;
	int iStatus_out;
	int iStatus_in;
	int iStatus_oe;

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
			/* Clear the out bit. */
			ulOe &= ~ulMask_oe;

			/* Clear the output enable bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set HIGHZ of XM0_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Clear the out bit. */
			ulOe |= ulMask_oe;

			/* Set the output enable bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT0 of XM0_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the out bit. */
			ulOe |= ulMask_oe;

			/* Set the output enable bit. */
			ulOut |= ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT1 of XM0_IO index: %d\n",uiIndex);

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXpec0Area->aulStatcfg[0] = ulOut | ulOe;

			/* DEBUG INFORMATION: */
			ulStatus = ptXpec0Area->aulStatcfg[0];
			iStatus_out = (ulStatus & HOSTMSK(statcfg0_gpio0_out)) >> HOSTSRT(statcfg0_gpio0_out);
			iStatus_in = (ulStatus & HOSTMSK(statcfg0_gpio0_in)) >> HOSTSRT(statcfg0_gpio0_in);
			iStatus_oe = (ulStatus & HOSTMSK(statcfg0_gpio0_oe)) >> HOSTSRT(statcfg0_gpio0_oe);
			uprintf("STATUS: set function Statcfg0 GPIO0/XM0_IO0 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);

			iStatus_out = (ulStatus & HOSTMSK(statcfg0_gpio1_out)) >> HOSTSRT(statcfg0_gpio1_out);
			iStatus_in = (ulStatus & HOSTMSK(statcfg0_gpio1_in)) >> HOSTSRT(statcfg0_gpio1_in);
			iStatus_oe = (ulStatus & HOSTMSK(statcfg0_gpio1_oe)) >> HOSTSRT(statcfg0_gpio1_oe);
			uprintf("STATUS: set function Statcfg0 GPIO1/XM0_IO1 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);
		}
		else
		{
			/* DEBUG INFORMATION: */
			uprintf("ERROR: set XM0_IO index: %d\n",uiIndex);
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
	unsigned long ulStatcfg;
	unsigned long ulOut;
	unsigned long ulOe;
	unsigned long ulMask_out;
	unsigned long ulMask_oe;
	int iResult;
	unsigned long ulStatus;
	int iStatus_out;
	int iStatus_in;
	int iStatus_oe;

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
			/* Clear the out bit. */
			ulOe &= ~ulMask_oe;

			/* Clear the output enable bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set HIGHZ of XM1_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Clear the out bit. */
			ulOe |= ulMask_oe;

			/* Set the output enable bit. */
			ulOut &= ~ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT0 of XM1_IO index: %d\n",uiIndex);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the out bit. */
			ulOe |= ulMask_oe;

			/* Set the output enable bit. */
			ulOut |= ulMask_out;

			/* DEBUG INFORMATION: */
			uprintf("DEBUG: set OUTPUT1 of XM1_IO index: %d\n",uiIndex);

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXpec0Area->aulStatcfg[1] = ulOut | ulOe;

			/* DEBUG INFORMATION: */
			ulStatus = ptXpec0Area->aulStatcfg[1];
			iStatus_out = (ulStatus & HOSTMSK(statcfg1_gpio0_out)) >> HOSTSRT(statcfg1_gpio0_out);
			iStatus_in = (ulStatus & HOSTMSK(statcfg1_gpio0_in)) >> HOSTSRT(statcfg1_gpio0_in);
			iStatus_oe = (ulStatus & HOSTMSK(statcfg1_gpio0_oe)) >> HOSTSRT(statcfg1_gpio0_oe);
			uprintf("STATUS: set function Statcfg1 GPIO0/XM1_IO0 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);

			iStatus_out = (ulStatus & HOSTMSK(statcfg1_gpio1_out)) >> HOSTSRT(statcfg1_gpio1_out);
			iStatus_in = (ulStatus & HOSTMSK(statcfg1_gpio1_in)) >> HOSTSRT(statcfg1_gpio1_in);
			iStatus_oe = (ulStatus & HOSTMSK(statcfg1_gpio1_oe)) >> HOSTSRT(statcfg1_gpio1_oe);
			uprintf("STATUS: set function Statcfg1 GPIO1/XM1_IO1 - out:%d - oe:%d - in:%d\n",iStatus_out,iStatus_oe,iStatus_in);
		}
		else
		{
			/* DEBUG INFORMATION: */
			uprintf("ERROR: set XM1_IO index: %d\n",uiIndex);
		}
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM1_IO: %d\n", uiIndex);
	}

	return iResult;
}



static int get_xm0io(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXpec0Area);
	unsigned long ulStatus;
	unsigned long ulValue;
	unsigned char ucData;
	int iResult;
	int iStatus_out;
	int iStatus_in;
	int iStatus_oe;

	/* Be pessimistic. */
	iResult = -1;
	if( uiIndex<2U )
	{
		ulStatus = ptXpec0Area->aulStatcfg[0];
		ulValue = ulStatus & HOSTMSK(statcfg0_gpio0_in) << uiIndex;
		if( ulStatus==0 )
		{
			ucData = 0;
		}
		else
		{
			ucData = 1;
		}

		/* DEBUG INFORMATION: */
		iStatus_out = (ulStatus & (HOSTMSK(statcfg0_gpio0_out) << (2*uiIndex))) >> (HOSTSRT(statcfg0_gpio0_out) + 2*uiIndex);
		iStatus_in = (ulStatus & HOSTMSK(statcfg0_gpio0_in) << uiIndex) >> (HOSTSRT(statcfg0_gpio0_in) + uiIndex);
		iStatus_oe = (ulStatus & (HOSTMSK(statcfg0_gpio0_oe) << (2*uiIndex))) >> (HOSTSRT(statcfg0_gpio0_oe) + 2*uiIndex);
		uprintf("STATUS: get function Statcfg0 XM0_IO:%d - out:%d - oe:%d - in:%d\n",uiIndex,iStatus_out,iStatus_oe,iStatus_in);

		*pucData = ucData;
		iResult = 0;
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM0_IO: %d\n", uiIndex);
	}

	return iResult;
}



static int get_xm1io(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXpec0Area);
	unsigned long ulStatus;
	unsigned long ulValue;
	unsigned char ucData;
	int iResult;
	int iStatus_out;
	int iStatus_in;
	int iStatus_oe;

	/* Be pessimistic. */
	iResult = -1;
	if( uiIndex<2U )
	{
		ulStatus = ptXpec0Area->aulStatcfg[1];
		ulValue = ulStatus & HOSTMSK(statcfg1_gpio0_in) << uiIndex;
		if( ulStatus==0 )
		{
			ucData = 0;
		}
		else
		{
			ucData = 1;
		}

		/* DEBUG INFORMATION: */
		iStatus_out = (ulStatus & (HOSTMSK(statcfg1_gpio0_out) << (2*uiIndex))) >> (HOSTSRT(statcfg1_gpio0_out) + 2*uiIndex);
		iStatus_in = (ulStatus & HOSTMSK(statcfg1_gpio0_in) << uiIndex) >> (HOSTSRT(statcfg1_gpio0_in) + uiIndex);
		iStatus_oe = (ulStatus & (HOSTMSK(statcfg1_gpio0_oe) << (2*uiIndex))) >> (HOSTSRT(statcfg1_gpio0_oe) + 2*uiIndex);
		uprintf("STATUS: get function Statcfg1 XM1_IO:%d - out:%d - oe:%d - in:%d\n",uiIndex,iStatus_out,iStatus_oe,iStatus_in);

		*pucData = ucData;
		iResult = 0;
	}
	else
	{
		/* DEBUG INFORMATION: */
		uprintf("ERROR: Invalid index of XM1_IO: %d\n", uiIndex);
	}

	return iResult;
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



static int get_gpio(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptGpioArea);
	unsigned long ulValue;
	unsigned char ucData;

	/* Be pessimistic... */
	iResult = -1;

	/* Check the index. */
	if( uiIndex<32 )
	{
		/* Get the input value. */
		ulValue  = ptGpioArea->ulGpio_in;
		ulValue &= 1U << uiIndex;
		if( ulValue==0 )
		{
			ucData = 0;
		}
		else
		{
			ucData = 1;
		}
		*pucData = ucData;
		iResult = 0;
	}

	return iResult;
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



static int get_pio(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptPioArea);
	unsigned long ulValue;
	unsigned char ucData;

	/* Be pessimistic... */
	iResult = -1;

	/* Check the index. */
	if( uiIndex<32 )
	{
		ulValue  = ptPioArea->ulPio_in;
		ulValue &= 1U << uiIndex;
		if (ulValue == 0)
		{
			ucData = 0;
		}
		else
		{
			ucData = 1;
		}
		*pucData = ucData;
	}

	return iResult;
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



static int get_rdyrun(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptNetxControlledGlobalRegisterBlock1Area);
	unsigned long ulMaskIn;
	unsigned long ulValue;
	unsigned char ucData;

	/* Be pessimistic... */
	iResult = -1;

	/* Check the index. */
	if( uiIndex<2 )
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
			ucData = 0;
		}
		else
		{
			ucData = 1;
		}
		*pucData = ucData;
	}

	return iResult;
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



static int get_rstout(unsigned int uiIndex __attribute__((unused)), unsigned char *pucData __attribute__((unused)))
{
	int iResult;


	/* RSTOUT has not input function. */
	iResult = -1;

	return iResult;
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
		if( uiIndex<2 )
		{
			iResult = set_xm0io(uiIndex, tValue);
		}
		else if( uiIndex<4 )
		{
			iResult = set_xm1io(uiIndex-2U, tValue);
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


int iopins_get(const PINDESCRIPTION_T *ptPinDescription, unsigned char *pucData)
{
	int iResult;
	unsigned int uiIndex;


	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_gpio(uiIndex, pucData);
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
		iResult = get_pio(uiIndex, pucData);
		break;

	case PINTYPE_RDYRUN:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_rdyrun(uiIndex, pucData);
		break;

	case PINTYPE_RSTOUT:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_rstout(uiIndex, pucData);
		break;

	case PINTYPE_XMIO:
		uiIndex = ptPinDescription->uiIndex;
		if( uiIndex<2 )
		{
			iResult = get_xm0io(uiIndex, pucData);
		}
		else if( uiIndex<4 )
		{
			iResult = get_xm1io(uiIndex-2U, pucData);
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

