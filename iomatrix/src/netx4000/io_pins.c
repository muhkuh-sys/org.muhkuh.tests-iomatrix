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

#include <string.h>

#include "netx_io_areas.h"
#include "portcontrol.h"
#include "uprintf.h"


typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long ulComIo;
	unsigned long aulHifPio[2];
	unsigned long ulMled;
	unsigned long aulMmio[4];
	unsigned long ulRdyRun;
	unsigned long ulRstOut;
	unsigned long aulXmio[2];
	unsigned long aulRapGpio[5];
} UNITCONFIGURATION_T;



static const unsigned short ausPortControlIndex[5*32] =
{
	/* GPIO0 */
	PORTCONTROL_INDEX(10, 10), /* GPIO0_0  */
	PORTCONTROL_INDEX( 3,  1), /* GPIO0_1  */
	PORTCONTROL_INDEX( 3,  2), /* GPIO0_2  */
	PORTCONTROL_INDEX( 3,  3), /* GPIO0_3  */
	PORTCONTROL_INDEX( 3,  4), /* GPIO0_4  */
	PORTCONTROL_INDEX( 3,  5), /* GPIO0_5  */
	PORTCONTROL_INDEX( 3,  6), /* GPIO0_6  */
	PORTCONTROL_INDEX( 3,  7), /* GPIO0_7  */
	PORTCONTROL_INDEX( 3,  8), /* GPIO0_8  */
	PORTCONTROL_INDEX( 3,  9), /* GPIO0_9  */
	PORTCONTROL_INDEX( 3, 10), /* GPIO0_10 */
	PORTCONTROL_INDEX( 3, 11), /* GPIO0_11 */
	PORTCONTROL_INDEX( 3, 12), /* GPIO0_12 */
	PORTCONTROL_INDEX( 3, 13), /* GPIO0_13 */
	PORTCONTROL_INDEX( 3, 14), /* GPIO0_14 */
	PORTCONTROL_INDEX( 3, 15), /* GPIO0_15 */
	PORTCONTROL_INDEX( 7,  1), /* GPIO0_16 */
	PORTCONTROL_INDEX( 7,  2), /* GPIO0_17 */
	PORTCONTROL_INDEX( 7,  3), /* GPIO0_18 */
	PORTCONTROL_INDEX( 7,  4), /* GPIO0_19 */
	PORTCONTROL_INDEX( 7,  5), /* GPIO0_20 */
	PORTCONTROL_INDEX( 7,  6), /* GPIO0_21 */
	PORTCONTROL_INDEX( 7,  7), /* GPIO0_22 */
	PORTCONTROL_INDEX( 7,  8), /* GPIO0_23 */
	0xffffU,                   /* GPIO0_24 */
	0xffffU,                   /* GPIO0_25 */
	0xffffU,                   /* GPIO0_26 */
	0xffffU,                   /* GPIO0_27 */
	0xffffU,                   /* GPIO0_28 */
	0xffffU,                   /* GPIO0_29 */
	0xffffU,                   /* GPIO0_30 */
	0xffffU,                   /* GPIO0_31 */

	/* GPIO1 */
	PORTCONTROL_INDEX( 4,  0), /* GPIO1_0  */
	PORTCONTROL_INDEX( 4,  1), /* GPIO1_1  */
	PORTCONTROL_INDEX( 4,  2), /* GPIO1_2  */
	PORTCONTROL_INDEX( 4,  3), /* GPIO1_3  */
	PORTCONTROL_INDEX( 4,  4), /* GPIO1_4  */
	PORTCONTROL_INDEX( 4,  5), /* GPIO1_5  */
	PORTCONTROL_INDEX( 4,  6), /* GPIO1_6  */
	PORTCONTROL_INDEX( 4,  7), /* GPIO1_7  */
	PORTCONTROL_INDEX( 4,  8), /* GPIO1_8  */
	PORTCONTROL_INDEX( 4,  9), /* GPIO1_9  */
	PORTCONTROL_INDEX( 4, 10), /* GPIO1_10 */
	PORTCONTROL_INDEX( 4, 11), /* GPIO1_11 */
	PORTCONTROL_INDEX( 4, 12), /* GPIO1_12 */
	PORTCONTROL_INDEX( 4, 13), /* GPIO1_13 */
	PORTCONTROL_INDEX( 4, 14), /* GPIO1_14 */
	PORTCONTROL_INDEX( 4, 15), /* GPIO1_15 */
	PORTCONTROL_INDEX(11,  0), /* GPIO1_16 */
	PORTCONTROL_INDEX(11,  1), /* GPIO1_17 */
	PORTCONTROL_INDEX(11,  2), /* GPIO1_18 */
	PORTCONTROL_INDEX(11,  3), /* GPIO1_19 */
	PORTCONTROL_INDEX(11,  4), /* GPIO1_20 */
	PORTCONTROL_INDEX(11,  5), /* GPIO1_21 */
	PORTCONTROL_INDEX(11,  6), /* GPIO1_22 */
	PORTCONTROL_INDEX(11,  7), /* GPIO1_23 */
	PORTCONTROL_INDEX(14,  0), /* GPIO1_24 */
	PORTCONTROL_INDEX(14,  1), /* GPIO1_25 */
	0xffffU,                   /* GPIO1_26 */
	0xffffU,                   /* GPIO1_27 */
	0xffffU,                   /* GPIO1_28 */
	0xffffU,                   /* GPIO1_29 */
	0xffffU,                   /* GPIO1_30 */
	0xffffU,                   /* GPIO1_31 */

	/* GPIO2 */
	PORTCONTROL_INDEX( 5,  0), /* GPIO2_0  */
	PORTCONTROL_INDEX( 5,  1), /* GPIO2_1  */
	PORTCONTROL_INDEX( 5,  2), /* GPIO2_2  */
	PORTCONTROL_INDEX( 5,  3), /* GPIO2_3  */
	PORTCONTROL_INDEX( 5,  4), /* GPIO2_4  */
	PORTCONTROL_INDEX( 5,  5), /* GPIO2_5  */
	PORTCONTROL_INDEX( 5,  6), /* GPIO2_6  */
	PORTCONTROL_INDEX( 5,  7), /* GPIO2_7  */
	PORTCONTROL_INDEX( 5,  8), /* GPIO2_8  */
	PORTCONTROL_INDEX( 5,  9), /* GPIO2_9  */
	PORTCONTROL_INDEX( 5, 10), /* GPIO2_10 */
	PORTCONTROL_INDEX( 5, 11), /* GPIO2_11 */
	PORTCONTROL_INDEX( 5, 12), /* GPIO2_12 */
	PORTCONTROL_INDEX( 5, 13), /* GPIO2_13 */
	PORTCONTROL_INDEX( 5, 14), /* GPIO2_14 */
	PORTCONTROL_INDEX( 5, 15), /* GPIO2_15 */
	PORTCONTROL_INDEX( 6,  0), /* GPIO2_16 */
	PORTCONTROL_INDEX( 6,  1), /* GPIO2_17 */
	PORTCONTROL_INDEX( 6,  2), /* GPIO2_18 */
	PORTCONTROL_INDEX( 6,  3), /* GPIO2_19 */
	PORTCONTROL_INDEX( 6,  4), /* GPIO2_20 */
	PORTCONTROL_INDEX( 6,  5), /* GPIO2_21 */
	PORTCONTROL_INDEX( 6,  6), /* GPIO2_22 */
	PORTCONTROL_INDEX( 6,  7), /* GPIO2_23 */
	PORTCONTROL_INDEX( 6,  8), /* GPIO2_24 */
	PORTCONTROL_INDEX( 6,  9), /* GPIO2_25 */
	PORTCONTROL_INDEX( 6, 10), /* GPIO2_26 */
	PORTCONTROL_INDEX( 6, 11), /* GPIO2_27 */
	PORTCONTROL_INDEX( 6, 12), /* GPIO2_28 */
	PORTCONTROL_INDEX( 6, 13), /* GPIO2_29 */
	PORTCONTROL_INDEX( 6, 14), /* GPIO2_30 */
	PORTCONTROL_INDEX( 6, 15), /* GPIO2_31 */

	/* GPIO3 */
	PORTCONTROL_INDEX(12,  0), /* GPIO3_0  */
	PORTCONTROL_INDEX(12,  1), /* GPIO3_1  */
	PORTCONTROL_INDEX(12,  2), /* GPIO3_2  */
	PORTCONTROL_INDEX(12,  3), /* GPIO3_3  */
	PORTCONTROL_INDEX(12,  4), /* GPIO3_4  */
	PORTCONTROL_INDEX(12,  5), /* GPIO3_5  */
	PORTCONTROL_INDEX(12,  6), /* GPIO3_6  */
	PORTCONTROL_INDEX(12,  7), /* GPIO3_7  */
	PORTCONTROL_INDEX(12,  8), /* GPIO3_8  */
	PORTCONTROL_INDEX(12,  9), /* GPIO3_9  */
	PORTCONTROL_INDEX(12, 10), /* GPIO3_10 */
	PORTCONTROL_INDEX(12, 11), /* GPIO3_11 */
	PORTCONTROL_INDEX(12, 12), /* GPIO3_12 */
	PORTCONTROL_INDEX(12, 13), /* GPIO3_13 */
	PORTCONTROL_INDEX(12, 14), /* GPIO3_14 */
	PORTCONTROL_INDEX(12, 15), /* GPIO3_15 */
	PORTCONTROL_INDEX(13,  0), /* GPIO3_16 */
	PORTCONTROL_INDEX(13,  1), /* GPIO3_17 */
	PORTCONTROL_INDEX(13,  2), /* GPIO3_18 */
	PORTCONTROL_INDEX(13,  3), /* GPIO3_19 */
	PORTCONTROL_INDEX(13,  4), /* GPIO3_20 */
	PORTCONTROL_INDEX(13,  5), /* GPIO3_21 */
	PORTCONTROL_INDEX(13,  6), /* GPIO3_22 */
	PORTCONTROL_INDEX(13,  7), /* GPIO3_23 */
	PORTCONTROL_INDEX(13,  8), /* GPIO3_24 */
	PORTCONTROL_INDEX(13,  9), /* GPIO3_25 */
	PORTCONTROL_INDEX(13, 10), /* GPIO3_26 */
	PORTCONTROL_INDEX(13, 11), /* GPIO3_27 */
	PORTCONTROL_INDEX( 8, 15), /* GPIO3_28 */
	PORTCONTROL_INDEX(10,  2), /* GPIO3_29 */
	PORTCONTROL_INDEX( 9,  0), /* GPIO3_30 */
	0xffffU,                   /* GPIO3_31 */

	/* GPIO4 */
	PORTCONTROL_INDEX(15,  0), /* GPIO4_0  */
	PORTCONTROL_INDEX(15,  1), /* GPIO4_1  */
	PORTCONTROL_INDEX(15,  2), /* GPIO4_2  */
	PORTCONTROL_INDEX(15,  3), /* GPIO4_3  */
	PORTCONTROL_INDEX(15,  4), /* GPIO4_4  */
	PORTCONTROL_INDEX(15,  5), /* GPIO4_5  */
	PORTCONTROL_INDEX(15,  6), /* GPIO4_6  */
	PORTCONTROL_INDEX(15,  7), /* GPIO4_7  */
	PORTCONTROL_INDEX(15,  8), /* GPIO4_8  */
	PORTCONTROL_INDEX(15,  9), /* GPIO4_9  */
	PORTCONTROL_INDEX(15, 10), /* GPIO4_10 */
	PORTCONTROL_INDEX(15, 11), /* GPIO4_11 */
	PORTCONTROL_INDEX(15, 12), /* GPIO4_12 */
	PORTCONTROL_INDEX(15, 13), /* GPIO4_13 */
	PORTCONTROL_INDEX(16,  0), /* GPIO4_14 */
	PORTCONTROL_INDEX(16,  1), /* GPIO4_15 */
	PORTCONTROL_INDEX(16,  2), /* GPIO4_16 */
	PORTCONTROL_INDEX(16,  3), /* GPIO4_17 */
	PORTCONTROL_INDEX(16,  4), /* GPIO4_18 */
	PORTCONTROL_INDEX(16,  5), /* GPIO4_19 */
	PORTCONTROL_INDEX(16,  6), /* GPIO4_20 */
	PORTCONTROL_INDEX(16,  7), /* GPIO4_21 */
	PORTCONTROL_INDEX(16,  8), /* GPIO4_22 */
	PORTCONTROL_INDEX(16,  9), /* GPIO4_23 */
	PORTCONTROL_INDEX(16, 10), /* GPIO4_24 */
	PORTCONTROL_INDEX(16, 11), /* GPIO4_25 */
	PORTCONTROL_INDEX( 2,  0), /* GPIO4_26 */
	PORTCONTROL_INDEX( 2,  1), /* GPIO4_27 */
	0xffffU,                   /* GPIO4_28 */
	0xffffU,                   /* GPIO4_29 */
	0xffffU,                   /* GPIO4_30 */
	0xffffU                    /* GPIO4_31 */
};



static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->ulComIo       = 0;
	ptUnitCfg->aulHifPio[0]  = 0;
	ptUnitCfg->aulHifPio[1]  = 0;
	ptUnitCfg->ulMled        = 0;
	ptUnitCfg->aulMmio[0]    = 0;
	ptUnitCfg->aulMmio[1]    = 0;
	ptUnitCfg->aulMmio[2]    = 0;
	ptUnitCfg->aulMmio[3]    = 0;
	ptUnitCfg->ulRdyRun      = 0;
	ptUnitCfg->ulRstOut      = 0;
	ptUnitCfg->aulXmio[0]    = 0;
	ptUnitCfg->aulXmio[1]    = 0;
	ptUnitCfg->aulRapGpio[0] = 0;
	ptUnitCfg->aulRapGpio[1] = 0;
	ptUnitCfg->aulRapGpio[2] = 0;
	ptUnitCfg->aulRapGpio[3] = 0;
	ptUnitCfg->aulRapGpio[4] = 0;
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
				/* The GPIO pins on the netX4000 are only available on MMIOs.
				 * Use MMIOs instead.
				 */
				break;

			case PINTYPE_HIFPIO:
				if( uiIndex<32 )
				{
					ptUnitCfg->aulHifPio[0] |= 1U<<uiIndex;
					iResult = 0;
				}
				else if( uiIndex<50 )
				{
					ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-16U);
					iResult = 0;
				}
				else if( uiIndex>=50 && uiIndex<58 )
				{
					/* Index  is the HIF_RDY pin. It needs the DPM clock. */
					if( uiIndex==55 )
					{
						/* Check if the DPM clock can be enabled. */
						ulValue  = ptAsicCtrlArea->ulClock_enable_mask;
						ulValue &= HOSTMSK(clock_enable_mask_dpm);
						if( ulValue==0 )
						{
							uprintf("The pin %s needs the DPM clock, but it is masked in the clock_enable_mask register.\n", ptPinDescCnt->apcName);
						}
						else
						{
							ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-32U);
							iResult = 0;
						}
					}
					else
					{
						ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-32U);
						iResult = 0;
					}
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_MLED:
				/* Not yet... */
				break;

			case PINTYPE_MMIO:
				if( uiIndex<107 )
				{
					ptUnitCfg->aulMmio[uiIndex >> 5U] |= 1U << (uiIndex & 31U);
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
				/* Not yet... */
				break;

			case PINTYPE_RAPGPIO:
				/* The netX4000 has 5 RAP GPIO units. */
				if( uiIndex<5*32 )
				{
					/* Each RAP GPIO unit has 32 pins. */
					ptUnitCfg->aulRapGpio[uiIndex>>5U] |= 1U << (uiIndex & 0x1fU);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_APPPIO:
				/* The netX4000 has no APPPIO pins. */
				break;

			case PINTYPE_IOLLEDM:
				/* The netX4000 has no IOL bridge yet. */
				break;

			case PINTYPE_SQI:
				/* Not yet... */
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



static HOSTADEF(RAP_GPIO) * const atRAPGPIOUnit[5] =
{
	(NX4000_RAP_GPIO_AREA_T * const)Addr_NX4000_RAP_GPIO0,
	(NX4000_RAP_GPIO_AREA_T * const)Addr_NX4000_RAP_GPIO1,
	(NX4000_RAP_GPIO_AREA_T * const)Addr_NX4000_RAP_GPIO2,
	(NX4000_RAP_GPIO_AREA_T * const)Addr_NX4000_RAP_GPIO3,
	(NX4000_RAP_GPIO_AREA_T * const)Addr_NX4000_RAP_GPIO4
};



static int configure_rapgpio(unsigned int uiUnit, unsigned long ulPins)
{
	int iResult;
	HOSTADEF(RAP_GPIO) * ptArea;
	unsigned short usPortControlIndex;
	unsigned int uiBitCnt;
	unsigned int uiPinIndex;
	unsigned long ulValue;


	/* The netX4000 has 5 RAP GPIO units: 0 - 4. */
	if( uiUnit>4U )
	{
		/* Invalid unit. */
		iResult = -1;
	}
	else
	{
		ptArea = atRAPGPIOUnit[uiUnit];

		/* Set the input and output mask for the unit. */
		ptArea->ulRAP_GPIO_INmask = ulPins;
		ptArea->ulRAP_GPIO_OUTmask = ulPins;
		uprintf("mask: 0x%08x\n", ulPins);

		/* Set the output enable to 0. */
		ptArea->ulRAP_GPIO_OE = 0U;
		ptArea->ulRAP_GPIO_OUT = 0U;

		/* Loop over all pins. */
		for(uiBitCnt=0U; uiBitCnt<32U; ++uiBitCnt)
		{
			/* Is the pin active? */
			ulValue = ulPins & (1U << uiBitCnt);
			if( ulValue!=0U )
			{
				uiPinIndex = (uiUnit << 5U) | uiBitCnt;

				/* Get the port control index. */
				usPortControlIndex = ausPortControlIndex[uiPinIndex];

				/* Switch the pin to REEMUX1. */
				ulValue  = portcontrol_get(usPortControlIndex);
				ulValue &= ~MSK_PORTCONTROL_CONFIGURATION_SEL;
				ulValue |= 1U << SRT_PORTCONTROL_CONFIGURATION_SEL;
				portcontrol_set(usPortControlIndex, (unsigned short)ulValue);
			}
		}
		iResult = 0;
	}

	return iResult;
}



int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptHifIoCtrlArea);
	HOSTDEF(ptMmioCtrlArea);
	UNITCONFIGURATION_T tUnitCfg;
	unsigned long ulValue;
	unsigned long ulClockEnable;
	unsigned int uiCnt;


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

				ulClockEnable  = ptAsicCtrlArea->ulClock_enable;
				ulClockEnable |= HOSTMSK(clock_enable_dpm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->ulClock_enable = ulClockEnable;
			}

			ulValue |= 3 << HOSTSRT(hif_io_cfg_hif_mi_cfg);
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptHifIoCtrlArea->ulHif_io_cfg = ulValue;

			ulValue  = 1 << HOSTSRT(hif_pio_cfg_in_ctrl);
			ptHifIoCtrlArea->ulHif_pio_cfg = ulValue;
		}

		/*
		 *  MMIO
		 *  Configure all MMIO pins as PIO/input.
		 */
		for(uiCnt=0; uiCnt<107; ++uiCnt)
		{
			ulValue  = tUnitCfg.aulMmio[uiCnt >> 5U];
			ulValue &= 1U << (uiCnt & 0x1fU);
			if( ulValue!=0 )
			{
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptMmioCtrlArea->aulMmio_cfg[uiCnt] = HOSTMMIO(PIO);
			}
		}

		/*
		 * RdyRun
		 */
		if( tUnitCfg.ulRdyRun!=0 )
		{
			/* Disable the RDY and RUN driver. */
			ulValue  = ptAsicCtrlArea->ulRdy_run_cfg;
			ulValue &= ~HOSTMSK(rdy_run_cfg_RDY_DRV);
			ulValue &= ~HOSTMSK(rdy_run_cfg_RUN_DRV);
			ptAsicCtrlArea->ulRdy_run_cfg = ulValue;
		}

		/*
		 * RstOut
		 */
#if 0
		if( tUnitCfg.ulRstOut!=0 )
		{
			/* Disable the reset out driver. */
			ulValue  = ptAsicCtrlArea->ulReset_ctrl;
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT));
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptAsicCtrlArea->ulReset_ctrl = ulValue;
		}
#endif

		/*
		 * XMIOs
		 */
#if 0
		if( tUnitCfg.aulXmio[0]!=0 )
		{
			iResult = configure_xm0io(tUnitCfg.aulXmio[0]);
		}
		if( tUnitCfg.aulXmio[1]!=0 )
		{
			iResult = configure_xm1io(tUnitCfg.aulXmio[1]);
		}
#endif

		/*
		 * RAP GPIOs
		 */
		if( tUnitCfg.aulRapGpio[0]!=0 )
		{
			iResult = configure_rapgpio(0, tUnitCfg.aulRapGpio[0]);
		}
		if( tUnitCfg.aulRapGpio[1]!=0 )
		{
			iResult = configure_rapgpio(1, tUnitCfg.aulRapGpio[1]);
		}
		if( tUnitCfg.aulRapGpio[2]!=0 )
		{
			iResult = configure_rapgpio(2, tUnitCfg.aulRapGpio[2]);
		}
		if( tUnitCfg.aulRapGpio[3]!=0 )
		{
			iResult = configure_rapgpio(3, tUnitCfg.aulRapGpio[3]);
		}
		if( tUnitCfg.aulRapGpio[4]!=0 )
		{
			iResult = configure_rapgpio(4, tUnitCfg.aulRapGpio[4]);
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


static PIN_INVALUE_T get_hifpio(unsigned int uiIndex)
{
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long aulMsk[2];
	unsigned long aulValue[2];
	PIN_INVALUE_T tResult;


	if( uiIndex>=41U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		/* Get the mask bits for the index. */
		if( uiIndex<16U )
		{
			aulMsk[0] = 1U << uiIndex;
			aulMsk[1] = 0U;
		}
		else if( uiIndex<34U )
		{
			aulMsk[0] = 0U;
			aulMsk[1] = 1U << (uiIndex-16U);
		}
		else if( uiIndex<41U )
		{
			aulMsk[0] = 0U;
			aulMsk[1] = 1U << (uiIndex-(34U-25U));
		}

		aulValue[0]  = ptHifIoCtrlArea->aulHif_pio_in[0];
		aulValue[0] &= aulMsk[0];
		aulValue[1]  = ptHifIoCtrlArea->aulHif_pio_in[1];
		aulValue[1] &= aulMsk[1];

		if( (aulValue[0]|aulValue[1])==0 )
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


static int set_mmiopio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptMmioCtrlArea);
	unsigned long ulValue;

	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex<107 )
	{
		ulValue = ((unsigned long)HOSTMMIO(PIO)) << HOSTSRT(mmio0_cfg_mmio_sel);
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


static PIN_INVALUE_T get_mmiopio(unsigned int uiIndex)
{
	HOSTDEF(ptMmioCtrlArea);
	unsigned long ulValue;
	PIN_INVALUE_T tResult;

	/* check the index */
	if( uiIndex>=107U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		ulValue = ptMmioCtrlArea->aulMmio_in_line_status[uiIndex >> 5U];
		ulValue &= 1U << (uiIndex & 0x1fU);
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
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
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

		ulValue = ptAsicCtrlArea->ulRdy_run_cfg;

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
			ptAsicCtrlArea->ulRdy_run_cfg = ulValue;
		}
	}

	return iResult;
}


static PIN_INVALUE_T get_rdyrun(unsigned int uiIndex)
{
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	PIN_INVALUE_T tResult;


	/* check the index */
	if( uiIndex>=2U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		ulValue = ptAsicCtrlArea->ulRdy_run_cfg;
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
			tResult = PIN_INVALUE_0;
		}
		else
		{
			tResult = PIN_INVALUE_1;
		}
	}

	return tResult;
}



static int set_rstout(unsigned int uiIndex __attribute__((unused)), PINSTATUS_T tValue __attribute__((unused)))
{
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	return iResult;
}



static PIN_INVALUE_T get_rstout(unsigned int uiIndex)
{
	PIN_INVALUE_T tResult;


	if( uiIndex>0U )
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



static int set_rapgpio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	unsigned long ulUnit;
	unsigned long ulPin;
	unsigned long ulOE;
	unsigned long ulOUT;
	HOSTADEF(RAP_GPIO) * ptArea;


	/* Be pessimistic. */
	iResult = -1;

	ulUnit = uiIndex >> 5U;
	ulPin = uiIndex & 0X1fU;

	if( ulUnit<5U )
	{
		ptArea = atRAPGPIOUnit[ulUnit];
		ulOE = ptArea->ulRAP_GPIO_OE;
		ulOUT = ptArea->ulRAP_GPIO_OUT;

		switch(tValue)
		{
		case PINSTATUS_HIGHZ:
			ulOE &= ~(1U << ulPin);
			ulOUT &= ~(1U << ulPin);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			ulOE |= 1U << ulPin;
			ulOUT &= ~(1U << ulPin);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ulOE |= 1U << ulPin;
			ulOUT |= 1U << ulPin;
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptArea->ulRAP_GPIO_OE = ulOE;
			ptArea->ulRAP_GPIO_OUT = ulOUT;
		}
	}

	return iResult;
}



static PIN_INVALUE_T get_rapgpio(unsigned int uiIndex)
{
	PIN_INVALUE_T tResult;
	unsigned long ulUnit;
	unsigned long ulPin;
	unsigned long ulValue;
	HOSTADEF(RAP_GPIO) * ptArea;


	if( uiIndex>=160U )
	{
		tResult = PIN_INVALUE_InvalidPinIndex;
	}
	else
	{
		ulUnit = uiIndex >> 5U;
		ulPin = uiIndex & 0X1fU;

		ptArea = atRAPGPIOUnit[ulUnit];
		ulValue   = ptArea->ulRAP_GPIO_IN;
		ulValue  &= 1U << ulPin;
		ulValue >>= ulPin;

		if (ulValue == 0U)
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
		/* Not yet... */
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
		/* Not yet... */
		break;

	case PINTYPE_RAPGPIO:
		iResult = set_rapgpio(uiIndex, tValue);
		break;

	case PINTYPE_APPPIO:
		/* The netX4000 has no APP PIOs. */
		break;

	case PINTYPE_IOLLEDM:
		/* The netX4000 has no IOL bridge yet. */
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
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_HIFPIO:
		tResult = get_hifpio(uiIndex);
		break;

	case PINTYPE_MLED:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_MMIO:
		tResult = get_mmiopio(uiIndex);
		break;

	case PINTYPE_PIO:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_RDYRUN:
		tResult = get_rdyrun(uiIndex);
		break;

	case PINTYPE_RSTOUT:
		tResult = get_rstout(uiIndex);
		break;

	case PINTYPE_XMIO:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_RAPGPIO:
		tResult = get_rapgpio(uiIndex);
		break;

	case PINTYPE_APPPIO:
		tResult = PIN_INVALUE_PintypeNotAvailable;
		break;

	case PINTYPE_IOLLEDM:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;

	case PINTYPE_SQI:
		tResult = PIN_INVALUE_PintypeNotSupportedYet;
		break;
	}

	return tResult;
}
