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
}



static int collect_unit_configuration(const PINDESCRIPTION_T *ptPinDesc, size_t sizMaxPinDesc, UNITCONFIGURATION_T *ptUnitCfg)
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

			switch( ptPinDescCnt->tType )
			{
			case PINTYPE_GPIO:
				/* The GPIO pins on the netX90 are only available on MMIOs.
				 * Use MMIOs instead.
				 */
				break;

			case PINTYPE_HIFPIO:
				uiIndex = ptPinDescCnt->uiIndex;
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

			case PINTYPE_MMIO:
				uiIndex = ptPinDescCnt->uiIndex;
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



int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, size_t sizMaxPinDesc)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptHifIoCtrlArea);
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
				/* The RDY pin is special. The PIO funcion must
				 * be enabled with a bit in the hif_io_cfg register
				 * and the DPM clock must be enabled.
				 */
				ulValue |= HOSTMSK(hif_io_cfg_en_hif_rdy_pio_mi);

				ulClockEnable  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulClockEnable |= HOSTMSK(clock_enable0_dpm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulClockEnable;
			}

			ulValue |= 3 << HOSTSRT(hif_io_cfg_hif_mi_cfg);
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptHifIoCtrlArea->ulHif_io_cfg = ulValue;

			ulValue  = 1 << HOSTSRT(hif_pio_cfg_in_ctrl);
			ptHifIoCtrlArea->ulHif_pio_cfg = ulValue;
		}

		/*
		 *  MMIO
		 *  Configure all MMIO pins as PIO/input.
		 */
		if( tUnitCfg.ulMmio!=0 )
		{
			for(iCnt=0; iCnt<8; ++iCnt)
			{
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
				ptMmioCtrlArea->aulMmio_cfg[iCnt] = MMIO_CFG_PIO;
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
		if( tUnitCfg.ulRstOut!=0 )
		{
			/* Disable the reset out driver. */
			ulValue  = ptAsicCtrlArea->ulReset_ctrl;
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT));
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptAsicCtrlArea->ulReset_ctrl = ulValue;
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


static int get_hifpio(unsigned int uiIndex, unsigned int *puiValue)
{
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long aulMsk[2];
	unsigned long aulValue[2];
	unsigned int uiValue;
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
			uiValue = 0;
		}
		else
		{
			uiValue = 1;
		}

		*puiValue = uiValue;
	}

	return iResult;
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
		iResult = -1;
		
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Set PIO mode, clear the output enable bit. */
			ulValue = HOSTMSK(mmio0_cfg_mmio_sel);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set PIO mode, set the output enable bit and the output bit to 0. */
			ulValue = HOSTMSK(mmio0_cfg_mmio_sel) | HOSTMSK(mmio0_cfg_pio_oe);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set PIO mode, set the output enable bit and the output bit to 1. */
			ulValue = HOSTMSK(mmio0_cfg_mmio_sel) | HOSTMSK(mmio0_cfg_pio_oe) | HOSTMSK(mmio0_cfg_pio_out);
			iResult = 0;
			break;
		}

		/* reconfigure the MMIO */
		if( iResult==0 )
		{
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptMmioCtrlArea->aulMmio_cfg[uiIndex] = ulValue;
		}
	}

	return iResult;
}


static int get_mmiopio(unsigned int uiIndex, unsigned int *puiValue)
{
	int iResult;
	HOSTDEF(ptMmioCtrlArea);
	unsigned long ulValue;
	unsigned int uiValue;

	/* assume failure */
	iResult = -1;
	
	/* check the index */
	if( uiIndex<8 )
	{
		ulValue = ptMmioCtrlArea->aulMmio_cfg[uiIndex];
		ulValue &= HOSTMSK(mmio0_cfg_status_in_ro);
		if (ulValue == 0)
		{
			uiValue = 0;
		}
		else
		{
			uiValue = 1;
		}
		*puiValue = uiValue;

		iResult = 0;
	}

	return iResult;
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


static int get_rdyrun(unsigned int uiIndex, unsigned int *puiValue)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	unsigned int uiValue;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex<2 )
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
			uiValue = 0;
		}
		else
		{
			uiValue = 1;
		}
		*puiValue = uiValue;
	}

	return iResult;
}



static int set_rstout(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex==0U )
	{
		ulValue  = ptAsicCtrlArea->ulReset_ctrl;

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
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptAsicCtrlArea->ulReset_ctrl = ulValue;
		}
	}

	return iResult;
}


static int get_rstout(unsigned int uiIndex __attribute__ ((unused)), unsigned int *puiValue __attribute__ ((unused)))
{
	/* The RstOut pin is output only. */
	uprintf("The RSTOUT pin can not be used as an input!\n");
	return -1;
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
		/* Not yet... */
		break;

	case PINTYPE_HIFPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_hifpio(uiIndex, tValue);
		break;

	case PINTYPE_MMIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_mmiopio(uiIndex, tValue);
		break;

	case PINTYPE_PIO:
		/* Not yet... */
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
		/* Not yet... */
		break;
	}

	return iResult;
}


int iopins_get(const PINDESCRIPTION_T *ptPinDescription, unsigned int *puiValue)
{
	int iResult;
	unsigned int uiIndex;


	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		/* Not yet... */
		break;

	case PINTYPE_HIFPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_hifpio(uiIndex, puiValue);
		break;

	case PINTYPE_MMIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_mmiopio(uiIndex, puiValue);
		break;

	case PINTYPE_PIO:
		/* Not yet... */
		break;

	case PINTYPE_RDYRUN:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_rdyrun(uiIndex, puiValue);
		break;

	case PINTYPE_RSTOUT:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_rstout(uiIndex, puiValue);
		break;

	case PINTYPE_XMIO:
		/* Not yet... */
		break;
	}

	return iResult;
}

