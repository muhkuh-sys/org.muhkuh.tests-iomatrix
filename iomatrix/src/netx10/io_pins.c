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


typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long  ulGpio;
	unsigned long aulHifPio[2];
	unsigned long  ulMmio;
	unsigned long  ulRdyRun;
	unsigned long  ulRstOut;
	unsigned long  ulXmio;
} UNITCONFIGURATION_T;


static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->ulGpio       = 0;
	ptUnitCfg->aulHifPio[0] = 0;
	ptUnitCfg->aulHifPio[1] = 0;
	ptUnitCfg->ulMmio       = 0;
	ptUnitCfg->ulRdyRun     = 0;
	ptUnitCfg->ulRstOut     = 0;
	ptUnitCfg->ulXmio       = 0;
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
				/* The pin index must be 0..15 .*/
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

			case PINTYPE_MLED:
				/* The netX10 has no MLED pins. */
				break;

			case PINTYPE_MMIO:
				/* The pin index must be 0..31 .*/
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<32 )
				{
					ptUnitCfg->ulMmio |= 1U<<uiIndex;
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
				else if( uiIndex<50 )
				{
					ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-32U);
					iResult = 0;
				}
				else if( uiIndex<58 )
				{
					ptUnitCfg->aulHifPio[1] |= 1U<<(uiIndex-26U);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_PIO:
				uprintf("The pin type PIO is not supported on this platform!\n");
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

			case PINTYPE_RDYRUN:
				/* The pin index must be 0..1 .*/
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

			case PINTYPE_XMIO:
				/* The pin index must be 0..1 .*/
				uiIndex = ptPinDescCnt->uiIndex;
				if( uiIndex<2 )
				{
					ptUnitCfg->ulXmio |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_RAPGPIO:
				/* The netX10 has no RAPGPIOs. */
				break;

			case PINTYPE_APPPIO:
				/* The netX10 has no APPPIO pins. */
				break;

			case PINTYPE_IOLLEDM:
				/* The netX10 has no IOL bridge yet. */
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


/*-------------------------------------------------------------------------*/


static int iopins_configure_hifpio(UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long ulAct0;
	unsigned long ulAct1;
	unsigned long ulValue;


	ulAct0 = ptUnitCfg->aulHifPio[0];
	ulAct1 = ptUnitCfg->aulHifPio[1];

	if( (ulAct0|ulAct1)!=0 )
	{
		/* Set the pins to input. */
		ptHifIoCtrlArea->aulHif_pio_oe[0] &= ~ulAct0;
		ptHifIoCtrlArea->aulHif_pio_oe[1] &= ~ulAct1;

		/* Collect the configuration. */
		ulValue  = 3U << HOSTSRT(hif_io_cfg_hif_mi_cfg);
		/* FIXME: only set this if the RDY pin is really used. */
		ulValue |= HOSTMSK(hif_io_cfg_en_hif_rdy_pio_mi);
		/* FIXME: only activate the according bits if A11..A23 are selected. */
		ulValue |= HOSTMSK(hif_io_cfg_en_hif_a23to11_pio_mi);
		ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
		ptHifIoCtrlArea->ulHif_io_cfg = ulValue;
	}

	return 0;
}


static int iopins_configure_mmio(UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptMmioCtrlArea);
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulAct;
	unsigned long ulValue;
	unsigned int uiCnt;


	ulAct = ptUnitCfg->ulMmio;
	if( ulAct!=0 )
	{
		/* Disable the driver for the activated pins. */
		ptMmioCtrlArea->ulMmio_pio_out_line_cfg &= ~ulAct;
		ptMmioCtrlArea->ulMmio_pio_oe_line_cfg  &= ~ulAct;

		/* Loop over all selected MMIOs and set them to PIO mode. */
		for(uiCnt=0; uiCnt<(sizeof(ptMmioCtrlArea->aulMmio_cfg)/sizeof(ptMmioCtrlArea->aulMmio_cfg[0])); ++uiCnt)
		{
			ulValue  = 1U << uiCnt;
			ulValue &= ulAct;
			if( ulValue!=0 )
			{
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptMmioCtrlArea->aulMmio_cfg[uiCnt] = HOSTMMIO(PIO);
			}
		}
	}

	return 0;
}



static int iopins_configure_rdyrun(UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulAct;
	unsigned long ulValue;


	ulAct = ptUnitCfg->ulRdyRun;
	if( ulAct!=0 )
	{
		ulValue = ptAsicCtrlArea->ulRdy_run_cfg;
		if( (ulAct&1)!=0 )
		{
			ulValue &= ~HOSTMSK(rdy_run_cfg_RDY_POL);
			ulValue &= ~HOSTMSK(rdy_run_cfg_RDY_DRV);
		}

		if( (ulAct&2)!=0 )
		{
			ulValue &= ~HOSTMSK(rdy_run_cfg_RUN_POL);
			ulValue &= ~HOSTMSK(rdy_run_cfg_RUN_DRV);
		}
		ptAsicCtrlArea->ulRdy_run_cfg = ulValue;
	}

	return 0;
}



static int iopins_configure_xmio(UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptXmac0Area);
	HOSTDEF(ptXpec0Area);
	unsigned long ulAct0;
	unsigned long ulAct1;
	unsigned long ulValue;


	ulAct0 = ptUnitCfg->ulXmio;
	if( ulAct0!=0 )
	{
		/* Stop the XC unit. */

		/* Stop the XMAC RPU. */
		ptXmac0Area->ulXmac_rpu_hold_pc = MSK_NX10_xmac_rpu_hold_pc_rpu_hold;
		/* Stop the XMAC TPU. */
		ptXmac0Area->ulXmac_tpu_hold_pc = MSK_NX10_xmac_tpu_hold_pc_tpu_hold;
		/* Stop the XPEC. */
		ptXpec0Area->ulXpu_hold_pc = 1;

		/* Init config mask value for this unit */
		ulValue = 0xffffffffU;
		/* use IO0 ? */
		if( (ulAct0&1U)!=0 )
		{
			/* yes, use IO0 -> clear bits in config mask to set pin to input */
			ulAct1 &= ~(MSK_NX10_xmac_config_shared0_gpio0_out|MSK_NX10_xmac_config_shared0_gpio0_oe);
		}
		/* use IO1 ? */
		if( (ulAct0&2)!=0 )
		{
			/* yes, use IO1 -> clear bits in config mask to set pin to input */
			ulAct1 &= ~(MSK_NX10_xmac_config_shared0_gpio1_out|MSK_NX10_xmac_config_shared0_gpio1_oe);
		}
		/* set config value */
		ptXmac0Area->ulXmac_config_shared0 &= ulValue;
	}

	return 0;
}

int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc)
{
	int iResult;
	UNITCONFIGURATION_T tUnitCfg;


	initialize_unit_configuration(&tUnitCfg);
	iResult = collect_unit_configuration(ptPinDesc, sizMaxPinDesc, &tUnitCfg);
	if( iResult==0 )
	{
		/*
		 *  HIFPIO
		 */
		iResult = iopins_configure_hifpio(&tUnitCfg);
		if( iResult==0 )
		{
			/*
			 *  MMIO
			 */
			iResult = iopins_configure_mmio(&tUnitCfg);
			if( iResult==0 )
			{
				/*
				 *  RDYRUN
				 */
				iResult = iopins_configure_rdyrun(&tUnitCfg);
				if( iResult==0 )
				{
					/*
					 *  XMIO
					 */
					iResult = iopins_configure_xmio(&tUnitCfg);
				}
			}
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
	if( uiIndex<32U )
	{
		aulMsk[0] = 1U << uiIndex;
		aulMsk[1] = 0U;
		iResult = 0;
	}
	else if( uiIndex<50U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-32U);
		iResult = 0;
	}
	else if( uiIndex<58U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-26U);
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
	if( uiIndex<32U )
	{
		aulMsk[0] = 1U << uiIndex;
		aulMsk[1] = 0U;
		iResult = 0;
	}
	else if( uiIndex<50U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-32U);
		iResult = 0;
	}
	else if( uiIndex<58U )
	{
		aulMsk[0] = 0U;
		aulMsk[1] = 1U << (uiIndex-26U);
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



static int set_mmio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptMmioCtrlArea);
	int iResult;
	unsigned long ulOe;
	unsigned long ulOut;
	unsigned long ulMask;


	/* Be pessimistic. */
	iResult = -1;

	ulOut = ptMmioCtrlArea->ulMmio_pio_out_line_cfg;
	ulOe  = ptMmioCtrlArea->ulMmio_pio_oe_line_cfg;

	/* get bit for the pin */
	ulMask = 1U << uiIndex;

	switch( tValue )
	{
	case PINSTATUS_HIGHZ:
		/* Disable the output driver. */
		ulOe  &= ~ulMask;
		iResult = 0;
		break;

	case PINSTATUS_OUTPUT0:
		/* Set the output value to 0. */
		ulOut &= ~ulMask;
		ulOe  |=  ulMask;
		iResult = 0;
		break;

	case PINSTATUS_OUTPUT1:
		/* Set the output value to 1. */
		ulOut |= ulMask;
		ulOe  |= ulMask;
		iResult = 0;
		break;
	};

	if( iResult==0 )
	{
		/* Write back the modified values.
		 * NOTE: first write the output value, then the output enable.
		 */
		ptMmioCtrlArea->ulMmio_pio_out_line_cfg = ulOut;
		ptMmioCtrlArea->ulMmio_pio_oe_line_cfg  = ulOe;
	}

	return iResult;
}


static int get_mmio(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptMmioCtrlArea);
	int iResult;
	unsigned long ulValue;
	unsigned char ucData;


	iResult = 0;

	/* Read GPIO pin. */
	ulValue  = ptMmioCtrlArea->ulMmio_in_line_status;
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

	return iResult;
}


static int set_rdyrun(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptAsicCtrlArea);
	int iResult;
	unsigned long ulMaskOe;
	unsigned long ulMaskOut;
	unsigned long ulValue;


	/* Be pessimistic. */
	iResult = -1;

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
		/* Disable the output driver. */
		ulValue &= ~ulMaskOe;
		iResult = 0;
		break;

	case PINSTATUS_OUTPUT0:
		/* Set the output value to 0. */
		ulValue &= ~ulMaskOut;
		ulValue |=  ulMaskOe;
		iResult = 0;
		break;

	case PINSTATUS_OUTPUT1:
		/* Set the output value to 1. */
		ulValue |= ulMaskOut;
		ulValue |= ulMaskOe;
		iResult = 0;
		break;
	};

	if( iResult==0 )
	{
		ptAsicCtrlArea->ulRdy_run_cfg = ulValue;
	}

	return iResult;
}


static int get_rdyrun(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptAsicCtrlArea);
	int iResult;
	unsigned long ulValue;
	unsigned char ucData;


	iResult = -1;
	if( uiIndex<2 )
	{
		ulValue = ptAsicCtrlArea->ulRdy_run_cfg;
		if( uiIndex==0 )
		{
			ulValue &= HOSTMSK(rdy_run_cfg_RDY);
		}
		else
		{
			ulValue &= HOSTMSK(rdy_run_cfg_RUN);
		}

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


static int set_xmio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXmac0Area);
	int iResult;
	unsigned long ulValue;
	unsigned long ulMaskOe;
	unsigned long ulMaskOut;


	if( uiIndex==0 )
	{
		ulMaskOut = MSK_NX10_xmac_config_shared0_gpio0_out;
		ulMaskOe  = MSK_NX10_xmac_config_shared0_gpio0_oe;
	}
	else
	{
		ulMaskOut = MSK_NX10_xmac_config_shared0_gpio1_out;
		ulMaskOe  = MSK_NX10_xmac_config_shared0_gpio1_oe;
	}

	/* Get initial value of the register */
	ulValue = ptXmac0Area->ulXmac_config_shared0;
	switch( tValue )
	{
	case PINSTATUS_HIGHZ:
		/* set pin to input */
		ulValue &= ~ulMaskOe;
		iResult = 0;
		break;

	case PINSTATUS_OUTPUT0:
		/* set pin to 0 and output */
		ulValue &= ~ulMaskOut;
		ulValue |=  ulMaskOe;
		iResult = 0;
		break;

	case PINSTATUS_OUTPUT1:
		/* set pin to output and 1 */
		ulValue |=  ulMaskOut;
		ulValue |=  ulMaskOe;
		iResult = 0;
		break;
	};

	if( iResult==0 )
	{
		/* write back new value of the register */
		ptXmac0Area->ulXmac_config_shared0 = ulValue;
	}

	return iResult;
}


static int get_xmio(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXmac0Area);
	unsigned long ulValue;
	unsigned long ulMask;
	unsigned char ucData;

  
	if( uiIndex==0 )
	{
		ulMask = HOSTMSK(xmac_status_shared0_gpio0_in);
	}
	else
	{
		ulMask = HOSTMSK(xmac_status_shared0_gpio1_in);
	}

	ulValue  = ptXmac0Area->ulXmac_config_shared0;
	ulValue &= ulMask;
	if( ulValue==0 )
	{
		ucData = 0;
	}
	else
	{
		ucData = 1;
	}
	*pucData = ucData;

	return 0;
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
		uprintf("The pin type GPIO is not supported on this platform!\n");
		iResult = -1;
		break;

	case PINTYPE_PIO:
		uprintf("The pin type PIO is not supported on this platform!\n");
		iResult = -1;
		break;

	case PINTYPE_MLED:
		/* The netX10 has no MLED pins. */
		break;

	case PINTYPE_MMIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_mmio(uiIndex, tValue);
		break;

	case PINTYPE_HIFPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_hifpio(uiIndex, tValue);
		break;

	case PINTYPE_RDYRUN:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_rdyrun(uiIndex, tValue);
		break;

	case PINTYPE_RSTOUT:
		uprintf("The pin type RSTOUT is not supported on this platform!\n");
		iResult = -1;
		break;

	case PINTYPE_XMIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_xmio(uiIndex, tValue);
		break;

	case PINTYPE_RAPGPIO:
		uprintf("The pin type RAPGPIO is not supported on this platform!\n");
		break;

	case PINTYPE_APPPIO:
		/* The netX10 has no APPPIO pins. */
		break;

	case PINTYPE_IOLLEDM:
		/* The netX10 has no IOL bridge yet. */
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
		uprintf("The pin type GPIO is not supported on this platform!\n");
		iResult = -1;
		break;

	case PINTYPE_PIO:
		uprintf("The pin type PIO is not supported on this platform!\n");
		iResult = -1;
		break;

	case PINTYPE_MLED:
		/* The netX10 has no MLED pins. */
		break;

	case PINTYPE_MMIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_mmio(uiIndex, pucData);
		break;

	case PINTYPE_HIFPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_hifpio(uiIndex, pucData);
		break;

	case PINTYPE_RDYRUN:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_rdyrun(uiIndex, pucData);
		break;

	case PINTYPE_RSTOUT:
		uprintf("The pin type RSTOUT is not supported on this platform!\n");
		iResult = -1;
		break;

	case PINTYPE_XMIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_xmio(uiIndex, pucData);
		break;

	case PINTYPE_RAPGPIO:
		uprintf("The pin type RAPGPIO is not supported on this platform!\n");
		break;

	case PINTYPE_APPPIO:
		/* The netX10 has no APPPIO pins. */
		break;

	case PINTYPE_IOLLEDM:
		/* The netX10 has no IOL bridge yet. */
		break;
	}

	return iResult;
}

