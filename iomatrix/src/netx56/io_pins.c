
#include "io_pins.h"

#include "netx_io_areas.h"
#include "uprintf.h"


typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long aulHifPio[2];
	unsigned long  ulRstOut;
} UNITCONFIGURATION_T;


static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->aulHifPio[0] = 0;
	ptUnitCfg->aulHifPio[1] = 0;
	ptUnitCfg->ulRstOut     = 0;
}



static int collect_unit_configuration(const PINDESCRIPTION_T *ptPinDesc, size_t sizMaxPinDesc, UNITCONFIGURATION_T *ptUnitCfg)
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
		if( (ptPinDescCnt->pcName)==NULL )
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
				/* Not yet... */
				break;

			case PINTYPE_PIO:
				/* Not yet... */
				break;

			case PINTYPE_MMIO:
				/* Not yet... */
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
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->pcName, uiIndex);
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
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->pcName, uiIndex);
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



int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, size_t sizMaxPinDesc)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptHifIoCtrlArea);
	UNITCONFIGURATION_T tUnitCfg;
	unsigned long ulValue;


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
			/* FIXME: only do this if the RDY pin is really used. */
			ulValue  = HOSTMSK(hif_io_cfg_en_hif_rdy_pio_mi);

			ulValue |= 3 << HOSTSRT(hif_io_cfg_hif_mi_cfg);
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
			ptHifIoCtrlArea->ulHif_io_cfg = ulValue;

			ulValue  = 1 << HOSTSRT(hif_pio_cfg_in_ctrl);
			ptHifIoCtrlArea->ulHif_pio_cfg = ulValue;
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


static int get_hifpio(unsigned int uiIndex, unsigned int *puiValue)
{
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long aulMsk[2];
	unsigned long aulValue[2];
	unsigned int uiValue;
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

	case PINTYPE_PIO:
		/* Not yet... */
		break;

	case PINTYPE_MMIO:
		/* Not yet... */
		break;

	case PINTYPE_HIFPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_hifpio(uiIndex, tValue);
		break;

	case PINTYPE_RSTOUT:
		uiIndex = ptPinDescription->uiIndex;
		iResult = set_rstout(uiIndex, tValue);
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

	case PINTYPE_PIO:
		/* Not yet... */
		break;

	case PINTYPE_MMIO:
		/* Not yet... */
		break;

	case PINTYPE_HIFPIO:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_hifpio(uiIndex, puiValue);
		break;

	case PINTYPE_RSTOUT:
		uiIndex = ptPinDescription->uiIndex;
		iResult = get_rstout(uiIndex, puiValue);
		break;
	}

	return iResult;
}

