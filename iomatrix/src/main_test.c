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


#include "main_test.h"

#include <string.h>

#include "rdy_run.h"
#include "systime.h"
#include "uprintf.h"
#include "version.h"

/*-----------------------------------*/

unsigned int ulPinsUnderTest __attribute__ ((section (".matrix_definitions")));
PINDESCRIPTION_T atPinsUnderTest[MAX_PINS_UNDER_TEST] __attribute__ ((section (".matrix_definitions")));


static unsigned long s_ulVerbosity;

/*-------------------------------------------------------------------------*/


typedef struct PINTYPE_PRINT_STRUCT
{
	PINTYPE_T tType;
	const char *pcPrint;
} PINTYPE_PRINT_T;

static const PINTYPE_PRINT_T atPintypePrint[] =
{
	{ PINTYPE_GPIO,   "GPIO" },
	{ PINTYPE_PIO,    "PIO" },
	{ PINTYPE_MMIO,   "MMIO" },
	{ PINTYPE_HIFPIO, "HIFPIO" },
	{ PINTYPE_RDYRUN, "RDYRUN" },
	{ PINTYPE_RSTOUT, "RSTOUT" },
	{ PINTYPE_XMIO,   "XMIO" }
};



static void print_pin(unsigned long ulIndex, const PINDESCRIPTION_T *ptPinDesc)
{
	PINTYPE_T tType;
	const PINTYPE_PRINT_T *ptCnt;
	const PINTYPE_PRINT_T *ptEnd;
	const char *pcPrint;


	/* Get the print representation of the pin type. */
	pcPrint = "???";
	tType = ptPinDesc->tType;
	ptCnt = atPintypePrint;
	ptEnd = atPintypePrint + (sizeof(atPintypePrint)/sizeof(atPintypePrint[0]));
	do
	{
		if( ptCnt->tType==tType )
		{
			pcPrint = ptCnt->pcPrint;
			break;
		}
		++ptCnt;
	} while( ptCnt<ptEnd );

	uprintf("%03d: Pin '%s': %s[%d]\n", ulIndex, ptPinDesc->apcName, pcPrint, ptPinDesc->uiIndex);
}


/*-------------------------------------------------------------------------*/

typedef struct DATAPTR_STRUCT
{
	const unsigned char *pucCnt;
	const unsigned char *pucEnd;
} DATAPTR_T;


static unsigned long get_dword(DATAPTR_T *ptPtr)
{
	unsigned long ulValue;


	ulValue  =  (unsigned long)(*((ptPtr->pucCnt)++));
	ulValue |= ((unsigned long)(*((ptPtr->pucCnt)++))) <<  8U;
	ulValue |= ((unsigned long)(*((ptPtr->pucCnt)++))) << 16U;
	ulValue |= ((unsigned long)(*((ptPtr->pucCnt)++))) << 24U;

	return ulValue;
}


static int parse_pin_description(const unsigned char *pucDefinition, unsigned long ulDefinitionSize)
{
	int iResult;
	DATAPTR_T tPtr;
	PINDESCRIPTION_T *ptPinDesc;
	const unsigned char *pucNameStart;
	PINTYPE_T tPinType;
	unsigned int uiPinIndex;
	long lNameSize;
	unsigned long ulPinCnt;


	/* Be optimistic. */
	iResult = 0;

	/* Clear the pins under test. */
	memset(atPinsUnderTest, 0, sizeof(atPinsUnderTest));
	ulPinsUnderTest = 0;

	tPtr.pucCnt = pucDefinition;
	tPtr.pucEnd = pucDefinition + ulDefinitionSize;

	ulPinCnt = 0;
	/* Parse all pin descriptions. */
	while( tPtr.pucCnt<tPtr.pucEnd )
	{
		ptPinDesc = atPinsUnderTest + ulPinCnt;

		/* Get the next name. */
		pucNameStart = tPtr.pucCnt;
		while( tPtr.pucCnt<tPtr.pucEnd && *(tPtr.pucCnt)!='\0')
		{
			++(tPtr.pucCnt);
		}

		/* The name must not be empty. */
		lNameSize = tPtr.pucCnt - pucNameStart;
		if( lNameSize<=0 )
		{
			uprintf("Error: the name of pin %d is empty!\n", ulPinCnt);
			iResult = -1;
			break;
		}
		else if( lNameSize>MAX_PINDESCRIPTION_NAME )
		{
			uprintf("Error: the name of pin %d is too long!\n", ulPinCnt);
			iResult = -1;
			break;
		}
		else
		{
			/* Skip over the terminating 0. */
			++(tPtr.pucCnt);

			/* The entry needs some more bytes. */
			if( (tPtr.pucCnt + sizeof(unsigned long) + sizeof(unsigned long))>tPtr.pucEnd )
			{
				uprintf("Error: not enough data for the definition of entry %d. The description seems to be truncated!\n", ulPinCnt);
				iResult = -1;
				break;
			}
			else
			{
				/* Check the type. */
				tPinType = (PINTYPE_T)get_dword(&tPtr);
				iResult = -1;
				switch(tPinType)
				{
				case PINTYPE_GPIO:
				case PINTYPE_PIO:
				case PINTYPE_MLED:
				case PINTYPE_MMIO:
				case PINTYPE_HIFPIO:
				case PINTYPE_RDYRUN:
				case PINTYPE_RSTOUT:
				case PINTYPE_XMIO:
					iResult = 0;
					break;
				}
				if( iResult!=0 )
				{
					uprintf("Error: invalid pin type: %d.\n", tPinType);
				}
				else
				{
					/* Get the index of the pin. */
					uiPinIndex = (unsigned int)get_dword(&tPtr);

					/* Copy the name. */
					memset(ptPinDesc->apcName, 0, MAX_PINDESCRIPTION_NAME);
					strncpy(ptPinDesc->apcName, (const char*)pucNameStart, MAX_PINDESCRIPTION_NAME);

					ptPinDesc->tType = tPinType;
					ptPinDesc->uiIndex = uiPinIndex;

					if( s_ulVerbosity!=0 )
					{
						print_pin(ulPinCnt, ptPinDesc);
					}

					/* Move to the next space for the pin description. */
					++ulPinCnt;
					if( ulPinCnt>=MAX_PINS_UNDER_TEST )
					{
						uprintf("Error: too many pin descriptions!\n");
						iResult = -1;
						break;
					}
				}
			}
		}
	}

	if( iResult==0 )
	{
		/* Save the new number of pins in the definition. */
		ulPinsUnderTest = ulPinCnt;

		iResult = iopins_configure(atPinsUnderTest, MAX_PINS_UNDER_TEST);
	}

	return iResult;
}


/*-------------------------------------------------------------------------*/


static int set_pin(const IOMATRIX_PARAMETER_SET_PIN_T *ptParameter)
{
	unsigned long ulPinIndex;
	int iResult;
	const PINDESCRIPTION_T *ptPinDescription;


	/* Check if the index of the pin is in the allowed range. */
	ulPinIndex = ptParameter->ulPinIndex;
	if( ulPinIndex>=ulPinsUnderTest )
	{
		/* No, the index exceeds the array. */
		uprintf("Error: the pin index is invalid: %d\n", ulPinIndex);
		iResult = -1;
	}
	else
	{
		/* Get the pointer to the pin description. */
		ptPinDescription  = atPinsUnderTest;
		ptPinDescription += ulPinIndex;
		/* Set the pin to the requested state. */
		iResult = iopins_set(ptPinDescription, ptParameter->tStatus);
	}

	return iResult;
}



static int get_pin(IOMATRIX_PARAMETER_GET_PIN_T *ptParameter)
{
	unsigned long ulPinIndex;
	int iResult;
	const PINDESCRIPTION_T *ptPinDescription;
	unsigned char ucData;


	/* Check if the index of the pin is in the allowed range. */
	ulPinIndex = ptParameter->ulPinIndex;
	if( ulPinIndex>=ulPinsUnderTest )
	{
		/* No, the index exceeds the array. */
		uprintf("Error: the pin index is invalid: %d\n", ulPinIndex);
		iResult = -1;
	}
	else
	{
		/* Get the pointer to the pin description. */
		ptPinDescription  = atPinsUnderTest;
		ptPinDescription += ulPinIndex;

		/* Set the pin to the requested state. */
		iResult = iopins_get(ptPinDescription, &ucData);
		if( iResult==0 )
		{
			ptParameter->ucValue = ucData;
		}
	}

	return iResult;
}



static int set_all_pins(const IOMATRIX_PARAMETER_SET_ALL_PINS_T *ptParameter)
{
	unsigned long ulPinCnt;
	unsigned long ulPinMax;
	int iResult;
	const PINDESCRIPTION_T *ptPinDescription;
	PINSTATUS_T tStatus;


	/* Be optimistic. */
	iResult = 0;

	ulPinCnt = 0;
	ulPinMax = ulPinsUnderTest;
	while( ulPinCnt<ulPinMax )
	{
		/* Get the pointer to the pin description. */
		ptPinDescription  = atPinsUnderTest;
		ptPinDescription += ulPinCnt;

		/* Get the status for the pin. */
		iResult = -1;
		tStatus = ptParameter->aucStatus[ulPinCnt];
		switch(tStatus)
		{
		case PINSTATUS_HIGHZ:
		case PINSTATUS_OUTPUT0:
		case PINSTATUS_OUTPUT1:
			/* Set the pin to the requested state. */
			iResult = 0;
			break;
		}
		if( iResult!=0 )
		{
			uprintf("The pin has an invalid status of 0x%08x: ", tStatus);
			print_pin(ulPinCnt, ptPinDescription);
		}
		else
		{
			iResult = iopins_set(ptPinDescription, tStatus);
			if( iResult!=0 )
			{
				uprintf("Failed to set the pin: ");
				print_pin(ulPinCnt, ptPinDescription);
			}
		}

		if( iResult!=0 )
		{
			break;
		}

		++ulPinCnt;
	}

	return iResult;
}



static int get_all_pins(IOMATRIX_PARAMETER_GET_ALL_PINS_T *ptParameter)
{
	unsigned long ulPinCnt;
	unsigned long ulPinMax;
	int iResult;
	const PINDESCRIPTION_T *ptPinDescription;
	unsigned char *pucValue;


	/* Be optimistic. */
	iResult = 0;

	ulPinCnt = 0;
	ulPinMax = ulPinsUnderTest;
	while( ulPinCnt<ulPinMax )
	{
		/* Get the pointer to the pin description. */
		ptPinDescription  = atPinsUnderTest;
		ptPinDescription += ulPinCnt;

		pucValue = ptParameter->aucValue + ulPinCnt;

		/* Set the pin to the requested state. */
		iResult = iopins_get(ptPinDescription, pucValue);
		if( iResult!=0 )
		{
			uprintf("Failed to get the pin: ");
			print_pin(ulPinCnt, ptPinDescription);
			break;
		}

		++ulPinCnt;
	}

	return iResult;
}



/*-------------------------------------------------------------------------*/

TEST_RESULT_T test(IOMATRIX_PARAMETER_T *ptTestParams)
{
	TEST_RESULT_T tTestResult;
	int iResult;


	systime_init();


	/* Set the verbose mode. */
	s_ulVerbosity = ptTestParams->ulVerbose;
	if( s_ulVerbosity!=0 )
	{
		uprintf("\f. *** I/O matrix test by doc_bacardi@users.sourceforge.net ***\n");
		uprintf("V" VERSION_ALL "\n\n");

		/* NOTE:
		 * Do not use the SYS led in this test. The RDY and RUN pins might be
		 * part of the test matrix.
		 */

		/* Get the test parameter. */
		uprintf(". Parameters: 0x%08x\n", (unsigned long)ptTestParams);
		uprintf(".   Verbose: 0x%08x\n", ptTestParams->ulVerbose);
		uprintf(".   Command: 0x%08x\n", ptTestParams->tCommand);
	}

	/* Be pessimistic. */
	iResult = -1;

	switch( ptTestParams->tCommand )
	{
	case IOMATRIX_COMMAND_Parse_Pin_Description:
		if( s_ulVerbosity!=0 )
		{
			uprintf("Mode: Parse Pin Description\n");
		}

		ptTestParams->uParameter.tParsePinDescription.pvPinDescription = NULL;

		iResult = parse_pin_description(ptTestParams->uParameter.tParsePinDescription.pucPinDefinitionStart, ptTestParams->uParameter.tParsePinDescription.ulPinDefinitionSize);
		if( iResult==0 )
		{
			ptTestParams->uParameter.tParsePinDescription.pvPinDescription = (void*)atPinsUnderTest;
		}
		break;

	case IOMATRIX_COMMAND_Set_Pin:
		if( s_ulVerbosity!=0 )
		{
			uprintf("Mode: Set Pin\n");
		}

		if( ptTestParams->uParameter.tSetPin.pvPinDescription != (void*)atPinsUnderTest )
		{
			uprintf("Error: the pin description handle is invalid!\n");
		}
		else
		{
			iResult = set_pin(&(ptTestParams->uParameter.tSetPin));
		}
		break;

	case IOMATRIX_COMMAND_Get_Pin:
		if( s_ulVerbosity!=0 )
		{
			uprintf("Mode: Get Pin\n");
		}

		if( ptTestParams->uParameter.tGetPin.pvPinDescription != (void*)atPinsUnderTest )
		{
			uprintf("Error: the pin description handle is invalid!\n");
		}
		else
		{
			iResult = get_pin(&(ptTestParams->uParameter.tGetPin));
		}
		break;

	case IOMATRIX_COMMAND_Set_All_Pins:
		if( s_ulVerbosity!=0 )
		{
			uprintf("Mode: Set All Pins\n");
		}

		if( ptTestParams->uParameter.tSetAllPins.pvPinDescription != (void*)atPinsUnderTest )
		{
			uprintf("Error: the pin description handle is invalid!\n");
		}
		else
		{
			iResult = set_all_pins(&(ptTestParams->uParameter.tSetAllPins));
		}
		break;

	case IOMATRIX_COMMAND_Get_All_Pins:
		if( s_ulVerbosity!=0 )
		{
			uprintf("Mode: Get All Pins\n");
		}

		if( ptTestParams->uParameter.tGetAllPins.pvPinDescription != (void*)atPinsUnderTest )
		{
			uprintf("Error: the pin description handle is invalid!\n");
		}
		else
		{
			iResult = get_all_pins(&(ptTestParams->uParameter.tGetAllPins));
		}
		break;
	}


	if( iResult==0 )
	{
		tTestResult = TEST_RESULT_OK;
	}
	else
	{
		tTestResult = TEST_RESULT_ERROR;
	}

	return tTestResult;
}

/*-----------------------------------*/

