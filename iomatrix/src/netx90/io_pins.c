/***************************************************************************
 *   Copyright (C) 2018 by Christoph Thelen                                *
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


typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long ulComIo;
	unsigned long aulHifPio[2];
	unsigned long ulMled;
	unsigned long ulMmio;
	unsigned long ulRdyRun;
	unsigned long ulRstOut;
	unsigned long aulXmio[2];
} UNITCONFIGURATION_T;


static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->ulComIo      = 0;
	ptUnitCfg->aulHifPio[0] = 0;
	ptUnitCfg->aulHifPio[1] = 0;
	ptUnitCfg->ulMled       = 0;
	ptUnitCfg->ulMmio       = 0;
	ptUnitCfg->ulRdyRun     = 0;
	ptUnitCfg->ulRstOut     = 0;
	ptUnitCfg->aulXmio[0]   = 0;
	ptUnitCfg->aulXmio[1]   = 0;
}



static int collect_unit_configuration(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc, UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptAsicCtrlArea);
	int iResult;
	const PINDESCRIPTION_T *ptPinDescCnt;
	const PINDESCRIPTION_T *ptPinDescEnd;
	unsigned int uiIndex;
	unsigned long ulValue;


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

			uiIndex = ptPinDescCnt->uiIndex;
			switch( ptPinDescCnt->tType )
			{
			case PINTYPE_GPIO:
				/* The GPIO pins on the netX90 are only available on MMIOs.
				 * Use MMIOs instead.
				 */
				break;

			case PINTYPE_HIFPIO:
				if( uiIndex<16 )
				{
					ptUnitCfg->aulHifPio[0] |= 1U<<uiIndex;
					iResult = 0;
				}
				else if( uiIndex<34 )
				{
					ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-16U);
					iResult = 0;
				}
				else if( uiIndex<41 )
				{
					/* Index 38 is the HIF_RDY pin. It needs the DPM clock. */
					if( uiIndex==38 )
					{
						/* Check if the DPM clock can be enabled. */
						ulValue  = ptAsicCtrlArea->asClock_enable[0].ulMask;
						ulValue &= HOSTMSK(clock_enable0_mask_dpm);
						if( ulValue==0 )
						{
							uprintf("The pin %s needs the DPM clock, but it is masked in the clock_enable_mask register.\n", ptPinDescCnt->apcName);
						}
						else
						{
							ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-(34U-25U));
							iResult = 0;
						}
					}
					else
					{
						ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-(34U-25U));
						iResult = 0;
					}
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_MLED:
				if( uiIndex<8 )
				{
					ptUnitCfg->ulMled |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_MMIO:
				if( uiIndex<8 )
				{
					ptUnitCfg->ulMmio |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_PIO:
				/* Not yet... */
				break;

			case PINTYPE_RDYRUN:
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
				/* For now XM0_IO0-5 and XM1_IO0-5 are supported with the indices 0..11 . */
				if( uiIndex<6 )
				{
					ptUnitCfg->aulXmio[0] |= 1U << uiIndex;
					iResult = 0;
				}
				else if( uiIndex<12 )
				{
					ptUnitCfg->aulXmio[1] |= 1U << (uiIndex - 6U);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
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



static int configure_xm0io(unsigned long ulPins)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptXcStartStopArea);
	HOSTDEF(ptXc0Xmac0RegsArea);
	unsigned long ulValue;
	int iResult;


	/* Be optimistic. */
	iResult = 0;

	/* Can the XMAC0 clock be enabled? */
	ulValue = HOSTMSK(clock_enable0_mask_xmac0) | HOSTMSK(clock_enable0_mask_xc_misc);
	if( (ptAsicCtrlArea->asClock_enable[0].ulMask & ulValue)!=ulValue )
	{
		uprintf("ERROR: XM0_IO pins can not be used as the XMAC0 and XC_MISC clock can not be enabled.\n");
		iResult = -1;
	}
	else
	{
		/* Can the pins be activated? */
		ulValue   = ptAsicCtrlArea->asIo_config[0].ulMask;
		ulValue  &= HOSTMSK(io_config0_sel_xm0_io);
		ulValue >>= HOSTSRT(io_config0_sel_xm0_io);
		ulValue  &= ulPins;
		if( ulValue!=ulPins )
		{
			uprintf("ERROR: Not all XM0_IO pins can be enabled.\n");
			iResult = -1;
		}
		else
		{
			/* Are the clocks already enabled? */
			ulValue = HOSTMSK(clock_enable0_xmac0) | HOSTMSK(clock_enable0_xc_misc);
			if( (ptAsicCtrlArea->asClock_enable[0].ulEnable & ulValue)!=ulValue )
			{
				/* Enable the XMAC0 and XC_MISC clock. */
				ulValue  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulValue |= HOSTMSK(clock_enable0_xmac0);
				ulValue |= HOSTMSK(clock_enable0_xc_misc);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulValue;

				/* Stop the unit. */
				ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu0);
				ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu0);
				ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec0);
				ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec0);
				ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;
			}

			/* Using these pins is not a good idea if XMAC0 is running. */
			uprintf("ulXc_hold_status: 0x%08x\n", ptXcStartStopArea->ulXc_hold_status);
			ulValue = HOSTMSK(xc_hold_status_xc0_hold_tpu0) | HOSTMSK(xc_hold_status_xc0_hold_rpu0);
			if( (ptXcStartStopArea->ulXc_hold_status & ulValue)!=ulValue )
			{
				uprintf("ERROR: XM0_IO pins can not be used as the unit is running.\n");
				iResult = -1;
			}
			else
			{
				/* Set all pins to input. */
				ulValue  = HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio1_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio2_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio3_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio4_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio5_oe_wm);
				ptXc0Xmac0RegsArea->ulXmac_io_oe_shared0 = ulValue;

				/* XM_IO2-5 are shared with the PHY LEDs. Deactivate the LED function if they are in use. */
				if( (ulPins&((1U<<2)|(1U<<3)|(1U<<4)|(1U<<5)))!=0 )
				{
					ulValue  = ptXc0Xmac0RegsArea->ulXmac_config_sbu;
					ulValue &= ~HOSTMSK(xmac_config_sbu_phy_led_en);
					ptXc0Xmac0RegsArea->ulXmac_config_sbu = ulValue;
				}

				/* Activate the XMIO pins in the IO configuration. */
				ulValue  = ptAsicCtrlArea->asIo_config[0].ulConfig;
				ulValue &= ~HOSTMSK(io_config0_sel_xm0_io);
				ulValue |= (ulPins << HOSTSRT(io_config0_sel_xm0_io)) & HOSTMSK(io_config0_sel_xm0_io);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asIo_config[0].ulConfig = ulValue;
			}
		}
	}

	return iResult;
}



static int configure_xm1io(unsigned long ulPins)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptXcStartStopArea);
	HOSTDEF(ptXc0Xmac1RegsArea);
	unsigned long ulValue;
	int iResult;


	/* Be optimistic. */
	iResult = 0;

	/* Can the XMAC1 clock be enabled? */
	ulValue = HOSTMSK(clock_enable0_mask_xmac1) | HOSTMSK(clock_enable0_mask_xc_misc);
	if( (ptAsicCtrlArea->asClock_enable[0].ulMask & ulValue)!=ulValue )
	{
		uprintf("ERROR: XM1_IO pins can not be used as the XMAC1 and XC_MISC clock can not be enabled.\n");
		iResult = -1;
	}
	else
	{
		/* Can the pins be activated? */
		ulValue   = ptAsicCtrlArea->asIo_config[1].ulMask;
		ulValue  &= HOSTMSK(io_config1_sel_xm1_io);
		ulValue >>= HOSTSRT(io_config1_sel_xm1_io);
		ulValue  &= ulPins;
		if( ulValue!=ulPins )
		{
			uprintf("ERROR: Not all XM1_IO pins can be enabled.\n");
			iResult = -1;
		}
		else
		{
			/* Are the clocks already enabled? */
			ulValue = HOSTMSK(clock_enable0_xmac1) | HOSTMSK(clock_enable0_xc_misc);
			if( (ptAsicCtrlArea->asClock_enable[0].ulEnable & ulValue)!=ulValue )
			{
				/* Enable the XMAC0 and XC_MISC clock. */
				ulValue  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulValue |= HOSTMSK(clock_enable0_xmac1);
				ulValue |= HOSTMSK(clock_enable0_xc_misc);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulValue;

				/* Stop the unit. */
				ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu1);
				ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu1);
				ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec1);
				ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec1);
				ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;
			}

			/* Using these pins is not a good idea if XMAC1 is running. */
			uprintf("ulXc_hold_status: 0x%08x\n", ptXcStartStopArea->ulXc_hold_status);
			ulValue = HOSTMSK(xc_hold_status_xc0_hold_tpu1) | HOSTMSK(xc_hold_status_xc0_hold_rpu1);
			if( (ptXcStartStopArea->ulXc_hold_status & ulValue)!=ulValue )
			{
				uprintf("ERROR: XM0_IO pins can not be used as the unit is running.\n");
				iResult = -1;
			}
			else
			{
				/* Set all pins to input. */
				ulValue  = HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio1_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio2_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio3_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio4_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio5_oe_wm);
				ptXc0Xmac1RegsArea->ulXmac_io_oe_shared1 = ulValue;

				/* XM_IO2-5 are shared with the PHY LEDs. Deactivate the LED function if they are in use. */
				if( (ulPins&((1U<<2)|(1U<<3)|(1U<<4)|(1U<<5)))!=0 )
				{
					ulValue  = ptXc0Xmac1RegsArea->ulXmac_config_sbu;
					ulValue &= ~HOSTMSK(xmac_config_sbu_phy_led_en);
					ptXc0Xmac1RegsArea->ulXmac_config_sbu = ulValue;
				}

				/* Activate the XMIO pins in the IO configuration. */
				ulValue  = ptAsicCtrlArea->asIo_config[1].ulConfig;
				ulValue &= ~HOSTMSK(io_config1_sel_xm1_io);
				ulValue |= (ulPins << HOSTSRT(io_config1_sel_xm1_io)) & HOSTMSK(io_config1_sel_xm1_io);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asIo_config[1].ulConfig = ulValue;
			}
		}
	}

	return iResult;
}



int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptAsicCtrlComArea);
	HOSTDEF(ptHifIoCtrlArea);
	HOSTDEF(ptMledCtrlComArea);
	HOSTDEF(ptMmioCtrlArea);
	UNITCONFIGURATION_T tUnitCfg;
	unsigned long ulValue;
	unsigned long ulClockEnable;
	int iCnt;


	initialize_unit_configuration(&tUnitCfg);
	iResult = collect_unit_configuration(ptPinDesc, sizMaxPinDesc, &tUnitCfg);
	if( iResult==0 )
	{
		/*
		 *  HIFPIO
		 */
		if( (tUnitCfg.aulHifPio[0]|tUnitCfg.aulHifPio[1])!=0 )
		{
			/* Set all pins to input. */
			ptHifIoCtrlArea->aulHif_pio_oe[0] = 0;
			ptHifIoCtrlArea->aulHif_pio_oe[1] = 0;

			/* Collect the configuration. */
			ulValue = 0;
			/* Only do this if the RDY pin is really used. */
			if( (tUnitCfg.aulHifPio[1]&HOSTMSK(hif_pio_oe1_hif_rdy))!=0 )
			{
				/* The RDY pin is special. The PIO function must
				 * be enabled with a bit in the hif_io_cfg register
				 * and the DPM clock must be enabled.
				 */
				ulValue |= HOSTMSK(hif_io_cfg_en_hif_rdy_pio_mi);

				ulClockEnable  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulClockEnable |= HOSTMSK(clock_enable0_dpm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulClockEnable;
			}

			ulValue |= 3 << HOSTSRT(hif_io_cfg_hif_mi_cfg);
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptHifIoCtrlArea->ulHif_io_cfg = ulValue;

			ulValue  = 1 << HOSTSRT(hif_pio_cfg_in_ctrl);
			ptHifIoCtrlArea->ulHif_pio_cfg = ulValue;
		}

		/*
		 * MLED
		 */
		if( tUnitCfg.ulMled!=0 )
		{
			for(iCnt=0; iCnt<8; ++iCnt)
			{
				ulValue = 0;
				if( (tUnitCfg.ulMled & (1U << iCnt))!=0 )
				{
					/* Use the line register. */
					ulValue = 2;
				}
				ptMledCtrlComArea->aulMled_ctrl_output_sel[iCnt] = ulValue;
			}

			ulValue  = HOSTDFLT(mled_ctrl_cfg);
			ulValue |= HOSTMSK(mled_ctrl_cfg_enable);
			ptMledCtrlComArea->ulMled_ctrl_cfg = ulValue;
		}

		/*
		 *  MMIO
		 *  Configure all MMIO pins as PIO/input.
		 */
		if( tUnitCfg.ulMmio!=0 )
		{
			for(iCnt=0; iCnt<8; ++iCnt)
			{
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptMmioCtrlArea->aulMmio_cfg[iCnt] = MMIO_CFG_PIO;
			}
		}

		/*
		 * RdyRun
		 */
		if( tUnitCfg.ulRdyRun!=0 )
		{
			/* Disable the RDY and RUN driver. */
			ulValue  = ptAsicCtrlComArea->ulRdy_run_cfg;
			ulValue &= ~HOSTMSK(rdy_run_cfg_RDY_DRV);
			ulValue &= ~HOSTMSK(rdy_run_cfg_RUN_DRV);
			ptAsicCtrlComArea->ulRdy_run_cfg = ulValue;
		}

		/*
		 * RstOut
		 */
		if( tUnitCfg.ulRstOut!=0 )
		{
			/* Disable the reset out driver. */
			ulValue  = ptAsicCtrlComArea->ulReset_ctrl;
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT));
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptAsicCtrlComArea->ulReset_ctrl = ulValue;
		}

		/*
		 * XMIOs
		 */
		if( tUnitCfg.aulXmio[0]!=0 )
		{
			iResult = configure_xm0io(tUnitCfg.aulXmio[0]);
		}
		if( tUnitCfg.aulXmio[1]!=0 )
		{
			iResult = configure_xm1io(tUnitCfg.aulXmio[1]);
		}
	}

	return iResult;
}


/*---------------------------------------------------------------------------*/


static int set_hifpio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long aulOut[2];
	unsigned long aulOen[2];
	unsigned long aulMsk[2];
	int iResult;


	/* Get the current values for the output enable and the output register. */
	aulOut[0] = ptHifIoCtrlArea->aulHif_pio_out[0];
	aulOut[1] = ptHifIoCtrlArea->aulHif_pio_out[1];

	aulOen[0] = ptHifIoCtrlArea->aulHif_pio_oe[0];
	aulOen[1] = ptHifIoCtrlArea->aulHif_pio_oe[1];

	/* Get the mask bits for the index. */
	iResult = -1;
	if( uiIndex<16U )
	{
		aulMsk[0] = 1U << uiIndex;
		aulMsk[1] = 0U;
		iResult = 0;
	}
	else if( uiIndex<34U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-16U);
		iResult = 0;
	}
	else if( uiIndex<41U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-(34U-25U));
		iResult = 0;
	}


	if( iResult==0 )
	{
		/* Modify the values. */
		iResult = -1;
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit. */
			aulOen[0] &= ~(aulMsk[0]);
			aulOen[1] &= ~(aulMsk[1]);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit. */
			aulOen[0] |= aulMsk[0];
			aulOen[1] |= aulMsk[1];

			/* Clear the output bit. */
			aulOut[0] &= ~(aulMsk[0]);
			aulOut[1] &= ~(aulMsk[1]);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit. */
			aulOen[0] |= aulMsk[0];
			aulOen[1] |= aulMsk[1];

			/* Set the output bit. */
			aulOut[0] |= aulMsk[0];
			aulOut[1] |= aulMsk[1];
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptHifIoCtrlArea->aulHif_pio_out[0] = aulOut[0];
			ptHifIoCtrlArea->aulHif_pio_out[1] = aulOut[1];

			ptHifIoCtrlArea->aulHif_pio_oe[0]  = aulOen[0];
			ptHifIoCtrlArea->aulHif_pio_oe[1]  = aulOen[1];
		}
	}

	return iResult;
}


static int get_hifpio(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long aulMsk[2];
	unsigned long aulValue[2];
	unsigned char ucData;
	int iResult;


	/* Get the mask bits for the index. */
	iResult = -1;
	if( uiIndex<16U )
	{
		aulMsk[0] = 1U << uiIndex;
		aulMsk[1] = 0U;
		iResult = 0;
	}
	else if( uiIndex<34U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-16U);
		iResult = 0;
	}
	else if( uiIndex<41U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-(34U-25U));
		iResult = 0;
	}


	if( iResult==0 )
	{
		aulValue[0]  = ptHifIoCtrlArea->aulHif_pio_in[0];
		aulValue[0] &= aulMsk[0];
		aulValue[1]  = ptHifIoCtrlArea->aulHif_pio_in[1];
		aulValue[1] &= aulMsk[1];

		if( (aulValue[0]|aulValue[1])==0 )
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



static int set_mled(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptMledCtrlComArea);
	unsigned long ulMask;
	unsigned long ulValue;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<8 )
	{
		ulMask = 1U << uiIndex;

		/* Set the bit in the line register. */
		ulValue  = ptMledCtrlComArea->ulMled_ctrl_line0;
		switch(tValue)
		{
		case PINSTATUS_HIGHZ:
			/* MLED pins can not be set to Z. */
			uprintf("Trying to set an MLED pin to Z, which is not possible.\n");
			break;

		case PINSTATUS_OUTPUT0:
			ulValue &= ~ulMask;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ulValue |= ulMask;
			iResult = 0;
			break;
		}
		if( iResult==0 )
		{
			ptMledCtrlComArea->ulMled_ctrl_line0 = ulValue;
		}
	}
	else
	{
		uprintf("Invalid index for MLED pins: %d, but only 0-7 possible.\n");
	}

	return iResult;
}



static int get_mled(unsigned int uiIndex, unsigned char *pucData __attribute__((unused)))
{
	if( uiIndex<8 )
	{
		/* MLED pins are output only. */
		uprintf("Trying to get an MLED pin, which is read-only.\n");
	}
	else
	{
		uprintf("Invalid index for MLED pins: %d, but only 0-7 possible. And a 'get' would not work with MLED pins anyway.\n");
	}

	return -1;
}



static int set_mmiopio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptMmioCtrlArea);
	unsigned long ulValue;
	
	/* assume failure */
	iResult = -1;
	
	/* check the index */
	if( uiIndex<8 )
	{
		ulValue = ((unsigned long)MMIO_CFG_PIO) << HOSTSRT(mmio0_cfg_mmio_sel);
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Set PIO mode, clear the output enable bit. */
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set PIO mode, set the output enable bit and the output bit to 0. */
			ulValue |= 0U << HOSTSRT(mmio0_cfg_pio_out);
			ulValue |= HOSTMSK(mmio0_cfg_pio_oe);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set PIO mode, set the output enable bit and the output bit to 1. */
			ulValue |= 1U << HOSTSRT(mmio0_cfg_pio_out);
			ulValue |= HOSTMSK(mmio0_cfg_pio_oe);
			iResult = 0;
			break;
		}

		/* reconfigure the MMIO */
		if( iResult==0 )
		{
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptMmioCtrlArea->aulMmio_cfg[uiIndex] = ulValue;
		}
		else
		{
			uprintf("Invalid pin status: %d\n", tValue);
		}
	}
	else
	{
		uprintf("Invalid MMIO index: %d\n", uiIndex);
	}

	return iResult;
}


static int get_mmiopio(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptMmioCtrlArea);
	unsigned long ulValue;
	unsigned char ucData;

	/* assume failure */
	iResult = -1;
	
	/* check the index */
	if( uiIndex<8 )
	{
		ulValue = ptMmioCtrlArea->ulMmio_in_line_status0;
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
		iResult = 0;
	}
	else
	{
		uprintf("Invalid MMIO index: %d\n", uiIndex);
	}

	return iResult;
}



static int set_rdyrun(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptAsicCtrlComArea);
	unsigned long ulValue;
	unsigned long ulMaskOe;
	unsigned long ulMaskOut;
	
	/* assume failure */
	iResult = -1;
	
	/* check the index */
	if( uiIndex<2 )
	{
		if( uiIndex==0 )
		{
			ulMaskOe  = HOSTMSK(rdy_run_cfg_RDY_DRV);
			ulMaskOut = HOSTMSK(rdy_run_cfg_RDY);
		}
		else
		{
			ulMaskOe  = HOSTMSK(rdy_run_cfg_RUN_DRV);
			ulMaskOut = HOSTMSK(rdy_run_cfg_RUN);
		}

		ulValue = ptAsicCtrlComArea->ulRdy_run_cfg;

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit for the pin. */
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
			ptAsicCtrlComArea->ulRdy_run_cfg = ulValue;
		}
	}

	return iResult;
}


static int get_rdyrun(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptAsicCtrlComArea);
	unsigned long ulValue;
	unsigned char ucData;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex<2 )
	{
		ulValue = ptAsicCtrlComArea->ulRdy_run_cfg;
		if( uiIndex==0 )
		{
			ulValue &= HOSTMSK(rdy_run_cfg_RDY_IN);
		}
		else
		{
			ulValue &= HOSTMSK(rdy_run_cfg_RUN_IN);
		}

		if (ulValue == 0)
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



static int set_rstout(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptAsicCtrlComArea);
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex==0U )
	{
		ulValue  = ptAsicCtrlComArea->ulReset_ctrl;

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Disable the reset out driver. */
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT));
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit. */
			ulValue |= HOSTMSK(reset_ctrl_EN_RES_REQ_OUT);

			/* The output bit is inverted: set the output bit to get a 0. */
			ulValue |= HOSTMSK(reset_ctrl_RES_REQ_OUT);

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit. */
			ulValue |= HOSTMSK(reset_ctrl_EN_RES_REQ_OUT);

			/* The output bit is inverted: clear the output bit to get a 1. */
			ulValue &= ~(HOSTMSK(reset_ctrl_RES_REQ_OUT));

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptAsicCtrlComArea->ulReset_ctrl = ulValue;
		}
	}

	return iResult;
}



static int get_rstout(unsigned int uiIndex __attribute__ ((unused)), unsigned char *pucData __attribute__ ((unused)))
{
	/* The RstOut pin is output only. */
	uprintf("The RSTOUT pin can not be used as an input!\n");
	return -1;
}



static int set_xm0io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXc0Xmac0RegsArea);
	unsigned long ulOut;
	unsigned long ulOe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<6U )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared0_gpio0_out_wm) << uiIndex;

			/* Clear the output enable bit. */
			ulOe   = 0;
			ulOe  |= HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared0_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared0_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the out bit. */
			ulOut  = HOSTMSK(xmac_config_shared0_gpio0_out) << uiIndex;
			ulOut |= HOSTMSK(xmac_config_shared0_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared0_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXc0Xmac0RegsArea->ulXmac_config_shared0 = ulOut;
			ptXc0Xmac0RegsArea->ulXmac_io_oe_shared0  = ulOe;
		}
	}
	else
	{
		uprintf("Invalid index for XM1IO: %d\n", uiIndex);
	}

	return iResult;
}



static int set_xm1io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXc0Xmac1RegsArea);
	unsigned long ulOut;
	unsigned long ulOe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<6U )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared1_gpio0_out_wm) << uiIndex;

			/* Clear the output enable bit. */
			ulOe   = 0;
			ulOe  |= HOSTMSK(xmac_io_oe_shared1_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared1_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared1_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared1_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the out bit. */
			ulOut  = HOSTMSK(xmac_config_shared1_gpio0_out) << uiIndex;
			ulOut |= HOSTMSK(xmac_config_shared1_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared1_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared1_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXc0Xmac1RegsArea->ulXmac_config_shared1 = ulOut;
			ptXc0Xmac1RegsArea->ulXmac_io_oe_shared1  = ulOe;
		}
	}
	else
	{
		uprintf("Invalid index for XM1IO: %d\n", uiIndex);
	}

	return iResult;
}



static int get_xm0io(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXc0Xmac0RegsArea);
	unsigned long ulValue;
	unsigned char ucData;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;
	if( uiIndex<6U )
	{
		ulValue  = ptXc0Xmac0RegsArea->ulXmac_status_shared0;
		ulValue &= HOSTMSK(xmac_status_shared0_gpio0_in) << uiIndex;
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



static int get_xm1io(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXc0Xmac1RegsArea);
	unsigned long ulValue;
	unsigned char ucData;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;
	if( uiIndex<6U )
	{
		ulValue  = ptXc0Xmac1RegsArea->ulXmac_status_shared1;
		ulValue &= HOSTMSK(xmac_status_shared0_gpio0_in) << uiIndex;
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


/*---------------------------------------------------------------------------*/


int iopins_set(const PINDESCRIPTION_T *ptPinDescription, PINSTATUS_T tValue)
{
	int iResult;
	unsigned int uiIndex;


	uiIndex = ptPinDescription->uiIndex;

	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		/* Not yet... */
		break;

	case PINTYPE_HIFPIO:
		iResult = set_hifpio(uiIndex, tValue);
		break;

	case PINTYPE_MLED:
		iResult = set_mled(uiIndex, tValue);
		break;

	case PINTYPE_MMIO:
		iResult = set_mmiopio(uiIndex, tValue);
		break;

	case PINTYPE_PIO:
		/* Not yet... */
		break;

	case PINTYPE_RDYRUN:
		iResult = set_rdyrun(uiIndex, tValue);
		break;

	case PINTYPE_RSTOUT:
		iResult = set_rstout(uiIndex, tValue);
		break;

	case PINTYPE_XMIO:
		if( uiIndex<6 )
		{
			iResult = set_xm0io(uiIndex, tValue);
		}
		else if( uiIndex<12 )
		{
			iResult = set_xm1io(uiIndex-6U, tValue);
		}
		break;
	}

	return iResult;
}


int iopins_get(const PINDESCRIPTION_T *ptPinDescription, unsigned char *pucData)
{
	int iResult;
	unsigned int uiIndex;


	uiIndex = ptPinDescription->uiIndex;

	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		/* Not yet... */
		break;

	case PINTYPE_HIFPIO:
		iResult = get_hifpio(uiIndex, pucData);
		break;

	case PINTYPE_MLED:
		iResult = get_mled(uiIndex, pucData);
		break;

	case PINTYPE_MMIO:
		iResult = get_mmiopio(uiIndex, pucData);
		break;

	case PINTYPE_PIO:
		/* Not yet... */
		break;

	case PINTYPE_RDYRUN:
		iResult = get_rdyrun(uiIndex, pucData);
		break;

	case PINTYPE_RSTOUT:
		iResult = get_rstout(uiIndex, pucData);
		break;

	case PINTYPE_XMIO:
		if( uiIndex<6 )
		{
			iResult = get_xm0io(uiIndex, pucData);
		}
		else if( uiIndex<12 )
		{
			iResult = get_xm1io(uiIndex-6U, pucData);
		}
		break;
	}

	return iResult;
}

