
#include "main_test.h"

#include "netx_test.h"
#include "rdy_run.h"
#include "systime.h"
#include "uprintf.h"
#include "version.h"

/*-----------------------------------*/

typedef enum PINTYPE_ENUM
{
	PINTYPE_GPIO    = 0,
	PINTYPE_PIO     = 1,
	PINTYPE_MMIO    = 2,
	PINTYPE_HIFPIO  = 3
} PINTYPE_T;


typedef struct PINDESCRIPTION_STRUCT
{
	const char *pcName;
	PINTYPE_T tType;
	unsigned int uiIndex;
	unsigned int uiDefaultValue;
	unsigned long ulFlags;
} PINDESCRIPTION_T;


typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long aulHifPio[2];
} UNITCONFIGURATION_T;



static const PINDESCRIPTION_T atPinsUnderTest[] =
{
	{ "HIF_D00",    PINTYPE_HIFPIO,  0, 1, 0 },
	{ "HIF_D01",    PINTYPE_HIFPIO,  1, 1, 0 },
	{ "HIF_D02",    PINTYPE_HIFPIO,  2, 1, 0 },
	{ "HIF_D03",    PINTYPE_HIFPIO,  3, 1, 0 },
	{ "HIF_D04",    PINTYPE_HIFPIO,  4, 1, 0 },
	{ "HIF_D05",    PINTYPE_HIFPIO,  5, 1, 0 },
	{ "HIF_D06",    PINTYPE_HIFPIO,  6, 1, 0 },
	{ "HIF_D07",    PINTYPE_HIFPIO,  7, 1, 0 },
	{ "HIF_D08",    PINTYPE_HIFPIO,  8, 1, 0 },
	{ "HIF_D09",    PINTYPE_HIFPIO,  9, 1, 0 },
	{ "HIF_D10",    PINTYPE_HIFPIO, 10, 1, 0 },
	{ "HIF_D11",    PINTYPE_HIFPIO, 11, 1, 0 },
	{ "HIF_D12",    PINTYPE_HIFPIO, 12, 1, 0 },
	{ "HIF_D13",    PINTYPE_HIFPIO, 13, 1, 0 },
	{ "HIF_D14",    PINTYPE_HIFPIO, 14, 1, 0 },
	{ "HIF_D15",    PINTYPE_HIFPIO, 15, 1, 0 },
	{ "HIF_D16",    PINTYPE_HIFPIO, 16, 1, 0 },
	{ "HIF_D17",    PINTYPE_HIFPIO, 17, 1, 0 },
	{ "HIF_D18",    PINTYPE_HIFPIO, 18, 1, 0 },
	{ "HIF_D19",    PINTYPE_HIFPIO, 19, 1, 0 },
	{ "HIF_D20",    PINTYPE_HIFPIO, 20, 1, 0 },
	{ "HIF_D21",    PINTYPE_HIFPIO, 21, 1, 0 },
	{ "HIF_D22",    PINTYPE_HIFPIO, 22, 1, 0 },
	{ "HIF_D23",    PINTYPE_HIFPIO, 23, 1, 0 },
	{ "HIF_D24",    PINTYPE_HIFPIO, 24, 1, 0 },
	{ "HIF_D25",    PINTYPE_HIFPIO, 25, 1, 0 },
	{ "HIF_D26",    PINTYPE_HIFPIO, 26, 1, 0 },
	{ "HIF_D27",    PINTYPE_HIFPIO, 27, 1, 0 },
	{ "HIF_D28",    PINTYPE_HIFPIO, 28, 1, 0 },
	{ "HIF_D29",    PINTYPE_HIFPIO, 29, 1, 0 },
	{ "HIF_D30",    PINTYPE_HIFPIO, 30, 1, 0 },
	{ "HIF_D31",    PINTYPE_HIFPIO, 31, 1, 0 },

	{ "HIF_A00",    PINTYPE_HIFPIO, 32, 1, 0 },
	{ "HIF_A01",    PINTYPE_HIFPIO, 33, 1, 0 },
	{ "HIF_A02",    PINTYPE_HIFPIO, 34, 1, 0 },
	{ "HIF_A03",    PINTYPE_HIFPIO, 35, 1, 0 },
	{ "HIF_A04",    PINTYPE_HIFPIO, 36, 1, 0 },
	{ "HIF_A05",    PINTYPE_HIFPIO, 37, 1, 0 },
	{ "HIF_A06",    PINTYPE_HIFPIO, 38, 1, 0 },
	{ "HIF_A07",    PINTYPE_HIFPIO, 39, 1, 0 },
	{ "HIF_A08",    PINTYPE_HIFPIO, 40, 1, 0 },
	{ "HIF_A09",    PINTYPE_HIFPIO, 41, 1, 0 },
	{ "HIF_A10",    PINTYPE_HIFPIO, 42, 1, 0 },
	{ "HIF_A11",    PINTYPE_HIFPIO, 43, 1, 0 },
	{ "HIF_A12",    PINTYPE_HIFPIO, 44, 1, 0 },
	{ "HIF_A13",    PINTYPE_HIFPIO, 45, 1, 0 },
	{ "HIF_A14",    PINTYPE_HIFPIO, 46, 1, 0 },
	{ "HIF_A15",    PINTYPE_HIFPIO, 47, 1, 0 },

	{ "HIF_AHI0",   PINTYPE_HIFPIO, 48, 1, 0 },
	{ "HIF_AHI1",   PINTYPE_HIFPIO, 49, 1, 0 },

	{ "HIF_BHE3",   PINTYPE_HIFPIO, 50, 1, 0 },
	{ "HIF_BHE1",   PINTYPE_HIFPIO, 51, 1, 0 },

	{ "HIF_RDn",    PINTYPE_HIFPIO, 52, 1, 0 },
	{ "HIF_WRn",    PINTYPE_HIFPIO, 53, 1, 0 },
	{ "HIF_CSn",    PINTYPE_HIFPIO, 54, 1, 0 },
	{ "HIF_RDY",    PINTYPE_HIFPIO, 55, 1, 0 },
	{ "HIF_DIRQ",   PINTYPE_HIFPIO, 56, 1, 0 },
	{ "HIF_SDCLK",  PINTYPE_HIFPIO, 57, 1, 0 }
};


static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->aulHifPio[0] = 0;
	ptUnitCfg->aulHifPio[1] = 0;
}


static int collect_unit_configuration(const PINDESCRIPTION_T *ptPinDesc, size_t sizPinDesc, UNITCONFIGURATION_T *ptUnitCfg)
{
	int iResult;
	const PINDESCRIPTION_T *ptPinDescCnt;
	const PINDESCRIPTION_T *ptPinDescEnd;
	unsigned int uiIndex;


	iResult = 0;
	
	ptPinDescCnt = ptPinDesc;
	ptPinDescEnd = ptPinDesc + sizPinDesc;
	while( ptPinDescCnt<ptPinDescEnd )
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
//				uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->pcName, uiIndex);
			}
			break;
		}
		
		if( iResult!=0 )
		{
			break;
		}
		
		++ptPinDescCnt;
	}
	
	return iResult;
}



static void configure_units(UNITCONFIGURATION_T *ptUnitCfg)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long ulHifPio0;
	unsigned long ulHifPio1;
	unsigned long ulValue;


	ulHifPio0 = ptUnitCfg->aulHifPio[0];
	ulHifPio1 = ptUnitCfg->aulHifPio[1];
	if( (ulHifPio0|ulHifPio1)!=0 )
	{
		/* Set all pins to input. */
		ptHifIoCtrlArea->aulHif_pio_oe[0] = 0;
		ptHifIoCtrlArea->aulHif_pio_oe[1] = 0;
		
		/* Collect the configuration. */
		ulValue  = HOSTMSK(hif_io_cfg_en_hif_rdy_pio_mi);
		ulValue |= 3 << HOSTSRT(hif_io_cfg_hif_mi_cfg);
		ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;
		ptHifIoCtrlArea->ulHif_io_cfg = ulValue;
		
		ulValue  = 1 << HOSTSRT(hif_pio_cfg_in_ctrl);
		ptHifIoCtrlArea->ulHif_pio_cfg = ulValue;
	}
}


static int pin_set_input(const PINDESCRIPTION_T *ptPinDescription)
{
	HOSTDEF(ptHifIoCtrlArea);
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
		if( uiIndex<32 )
		{
			ptHifIoCtrlArea->aulHif_pio_oe[0] &= ~(1U<<uiIndex);
			iResult = 0;
		}
		else if( uiIndex<50 )
		{
			ptHifIoCtrlArea->aulHif_pio_oe[1] &= ~(1U<<(uiIndex-32U));
			iResult = 0;
		}
		else if( uiIndex<58 )
		{
			ptHifIoCtrlArea->aulHif_pio_oe[1] &= ~(1U<<(uiIndex-26U));
			iResult = 0;
		}
		else
		{
//			uprintf("The pin %s has an invalid index of %d!", ptPinDescription->pcName, uiIndex);
		}
		break;
	}
	
	return iResult;
}



static int pin_set_output(const PINDESCRIPTION_T *ptPinDescription, unsigned int uiValue)
{
	HOSTDEF(ptHifIoCtrlArea);
	int iResult;
	unsigned int uiIndex;
	unsigned long ulData;
	unsigned long ulValue;
	
	
	
	if( uiValue==0 )
	{
		ulData = 0;
	}
	else
	{
		ulData = 1;
	}
	
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
		if( uiIndex<32 )
		{
			ulValue  = ptHifIoCtrlArea->aulHif_pio_out[0];
			ulValue &= ~(1U<<uiIndex);
			ulValue |= ulData << uiIndex;
			ptHifIoCtrlArea->aulHif_pio_out[0] = ulValue;
			ptHifIoCtrlArea->aulHif_pio_oe[0] |= 1U<<uiIndex;
			iResult = 0;
		}
		else if( uiIndex<50 )
		{
			ulValue  = ptHifIoCtrlArea->aulHif_pio_out[1];
			ulValue &= ~(1U<<(uiIndex-32U));
			ulValue |= ulData << (uiIndex-32U);
			ptHifIoCtrlArea->aulHif_pio_out[1] = ulValue;
			ptHifIoCtrlArea->aulHif_pio_oe[1] |= 1U<<(uiIndex-32U);
			iResult = 0;
		}
		else if( uiIndex<58 )
		{
			ulValue  = ptHifIoCtrlArea->aulHif_pio_out[1];
			ulValue &= ~(1U<<(uiIndex-26U));
			ulValue |= ulData << (uiIndex-26U);
			ptHifIoCtrlArea->aulHif_pio_out[1] = ulValue;
			ptHifIoCtrlArea->aulHif_pio_oe[1] |= 1U<<(uiIndex-26U);
			iResult = 0;
		}
		else
		{
//			uprintf("The pin %s has an invalid index of %d!", ptPinDescription->pcName, uiIndex);
		}
		break;
	}
	
	return iResult;
}


/*-----------------------------------*/

TEST_RESULT_T test(TEST_PARAMETER_T *ptTestParam)
{
	TEST_RESULT_T tTestResult;
	UNITCONFIGURATION_T tUnitCfg;
	int iResult;
	unsigned long ulSystimeHandle;
	unsigned int uiValue;
	const PINDESCRIPTION_T *ptPinDescCnt;
	const PINDESCRIPTION_T *ptPinDescEnd;


	systime_init();

//	uprintf("\f. *** I/O matrix test by doc_bacardi@users.sourceforge.net ***\n");
//	uprintf("V" VERSION_ALL "\n\n");

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Get the test parameter. */
	//ptTestParams = (CRCTEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	initialize_unit_configuration(&tUnitCfg);
	iResult = collect_unit_configuration(atPinsUnderTest, (sizeof(atPinsUnderTest)/sizeof(atPinsUnderTest[0])), &tUnitCfg);
	if( iResult==0 )
	{
		configure_units(&tUnitCfg);
	
		uiValue = 0;
		while(1)
		{
			ulSystimeHandle = systime_get_ms();
			while( systime_elapsed(ulSystimeHandle, 1000)==0 ) {};
			
			ptPinDescCnt = atPinsUnderTest;
			ptPinDescEnd = atPinsUnderTest + (sizeof(atPinsUnderTest)/sizeof(atPinsUnderTest[0]));
			while( ptPinDescCnt<ptPinDescEnd )
			{
				iResult = pin_set_output(ptPinDescCnt, uiValue);
				if( iResult!=0 )
				{
					break;
				}
				++ptPinDescCnt;
			}
			
			uiValue ^= 1;
			
			if( iResult!=0 )
			{
				break;
			}
		}
	}

	rdy_run_setLEDs(RDYRUN_GREEN);
	tTestResult = TEST_RESULT_OK;

	return tTestResult;
}

/*-----------------------------------*/

