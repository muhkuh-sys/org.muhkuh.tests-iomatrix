
#include "main_test.h"

#include <string.h>

#include "io_pins.h"
#include "netx_test.h"
#include "rdy_run.h"
#include "systime.h"
#include "uprintf.h"
#include "version.h"

/*-----------------------------------*/

#define MAX_PINS_UNDER_TEST 128
#define MAX_NET_SIZE 4
#define MAX_NET_COUNT 64


typedef enum MATRIXERR_ENUM
{
	MATRIXERR_Loopback_but_drive0_does_not_follow           = 0x0001,
	MATRIXERR_Loopback_but_drive1_does_not_follow           = 0x0002,
	MATRIXERR_No_Loopback_but_shortcut_for_drive0           = 0x0004,
	MATRIXERR_No_Loopback_but_shortcut_for_drive1           = 0x0008,
	MATRIXERR_Not_at_default_state_0_at_pin_test_start      = 0x0010,
	MATRIXERR_Not_at_default_state_0_after_pin_test_drive0  = 0x0020,
	MATRIXERR_Not_at_default_state_0_after_pin_test_drive1  = 0x0040
} MATRIXERR_T;


static const PINDESCRIPTION_T atPinsUnderTest[MAX_PINS_UNDER_TEST] =
{
	{ "HIF_D00",    PINTYPE_HIFPIO,  0, 1, PINFLAG_IOZ },
	{ "HIF_D01",    PINTYPE_HIFPIO,  1, 1, PINFLAG_IOZ },
	{ "HIF_D02",    PINTYPE_HIFPIO,  2, 1, PINFLAG_IOZ },
	{ "HIF_D03",    PINTYPE_HIFPIO,  3, 1, PINFLAG_IOZ },
	{ "HIF_D04",    PINTYPE_HIFPIO,  4, 1, PINFLAG_IOZ },
	{ "HIF_D05",    PINTYPE_HIFPIO,  5, 1, PINFLAG_IOZ },
	{ "HIF_D06",    PINTYPE_HIFPIO,  6, 1, PINFLAG_IOZ },
	{ "HIF_D07",    PINTYPE_HIFPIO,  7, 1, PINFLAG_IOZ },
	{ "HIF_D08",    PINTYPE_HIFPIO,  8, 1, PINFLAG_IOZ },
	{ "HIF_D09",    PINTYPE_HIFPIO,  9, 1, PINFLAG_IOZ },
	{ "HIF_D10",    PINTYPE_HIFPIO, 10, 1, PINFLAG_IOZ },
	{ "HIF_D11",    PINTYPE_HIFPIO, 11, 1, PINFLAG_IOZ },
	{ "HIF_D12",    PINTYPE_HIFPIO, 12, 1, PINFLAG_IOZ },
	{ "HIF_D13",    PINTYPE_HIFPIO, 13, 1, PINFLAG_IOZ },
	{ "HIF_D14",    PINTYPE_HIFPIO, 14, 1, PINFLAG_IOZ },
	{ "HIF_D15",    PINTYPE_HIFPIO, 15, 1, PINFLAG_IOZ },
	{ "HIF_D16",    PINTYPE_HIFPIO, 16, 1, PINFLAG_IOZ },
	{ "HIF_D17",    PINTYPE_HIFPIO, 17, 1, PINFLAG_IOZ },
	{ "HIF_D18",    PINTYPE_HIFPIO, 18, 1, PINFLAG_IOZ },
	{ "HIF_D19",    PINTYPE_HIFPIO, 19, 1, PINFLAG_IOZ },
	{ "HIF_D20",    PINTYPE_HIFPIO, 20, 1, PINFLAG_IOZ },
	{ "HIF_D21",    PINTYPE_HIFPIO, 21, 1, PINFLAG_IOZ },
	{ "HIF_D22",    PINTYPE_HIFPIO, 22, 1, PINFLAG_IOZ },
	{ "HIF_D23",    PINTYPE_HIFPIO, 23, 1, PINFLAG_IOZ },
	{ "HIF_D24",    PINTYPE_HIFPIO, 24, 1, PINFLAG_IOZ },
	{ "HIF_D25",    PINTYPE_HIFPIO, 25, 1, PINFLAG_IOZ },
	{ "HIF_D26",    PINTYPE_HIFPIO, 26, 1, PINFLAG_IOZ },
	{ "HIF_D27",    PINTYPE_HIFPIO, 27, 1, PINFLAG_IOZ },
	{ "HIF_D28",    PINTYPE_HIFPIO, 28, 1, PINFLAG_IOZ },
	{ "HIF_D29",    PINTYPE_HIFPIO, 29, 1, PINFLAG_IOZ },
	{ "HIF_D30",    PINTYPE_HIFPIO, 30, 1, PINFLAG_IOZ },
	{ "HIF_D31",    PINTYPE_HIFPIO, 31, 1, PINFLAG_IOZ },

	{ "HIF_A00",    PINTYPE_HIFPIO, 32, 1, PINFLAG_IOZ },
	{ "HIF_A01",    PINTYPE_HIFPIO, 33, 1, PINFLAG_IOZ },
	{ "HIF_A02",    PINTYPE_HIFPIO, 34, 1, PINFLAG_IOZ },
	{ "HIF_A03",    PINTYPE_HIFPIO, 35, 1, PINFLAG_IOZ },
	{ "HIF_A04",    PINTYPE_HIFPIO, 36, 1, PINFLAG_IOZ },
	{ "HIF_A05",    PINTYPE_HIFPIO, 37, 1, PINFLAG_IOZ },
	{ "HIF_A06",    PINTYPE_HIFPIO, 38, 1, PINFLAG_IOZ },
	{ "HIF_A07",    PINTYPE_HIFPIO, 39, 1, PINFLAG_IOZ },
	{ "HIF_A08",    PINTYPE_HIFPIO, 40, 1, PINFLAG_IOZ },
	{ "HIF_A09",    PINTYPE_HIFPIO, 41, 1, PINFLAG_IOZ },
	{ "HIF_A10",    PINTYPE_HIFPIO, 42, 1, PINFLAG_IOZ },
	{ "HIF_A11",    PINTYPE_HIFPIO, 43, 1, PINFLAG_IOZ },
	{ "HIF_A12",    PINTYPE_HIFPIO, 44, 1, PINFLAG_IOZ },
	{ "HIF_A13",    PINTYPE_HIFPIO, 45, 1, PINFLAG_IOZ },
	{ "HIF_A14",    PINTYPE_HIFPIO, 46, 1, PINFLAG_IOZ },
	{ "HIF_A15",    PINTYPE_HIFPIO, 47, 1, PINFLAG_IOZ },

	{ "HIF_AHI0",   PINTYPE_HIFPIO, 48, 1, PINFLAG_IOZ },
	{ "HIF_AHI1",   PINTYPE_HIFPIO, 49, 1, PINFLAG_IOZ },

	{ "HIF_BHE3",   PINTYPE_HIFPIO, 50, 1, PINFLAG_IOZ },
	{ "HIF_BHE1",   PINTYPE_HIFPIO, 51, 1, PINFLAG_IOZ },

	{ "HIF_RDn",    PINTYPE_HIFPIO, 52, 1, PINFLAG_IOZ },
	{ "HIF_WRn",    PINTYPE_HIFPIO, 53, 1, PINFLAG_IOZ },
	{ "HIF_CSn",    PINTYPE_HIFPIO, 54, 1, PINFLAG_IOZ },
	{ "HIF_RDY",    PINTYPE_HIFPIO, 55, 1, PINFLAG_I | PINFLAG_Z },
	{ "HIF_DIRQ",   PINTYPE_HIFPIO, 56, 1, PINFLAG_IOZ },
	{ "HIF_SDCLK",  PINTYPE_HIFPIO, 57, 1, PINFLAG_IOZ },

	{ "RSTOUT",     PINTYPE_RSTOUT,  0, 0, PINFLAG_O | PINFLAG_Z }
};



static const char * const apcNetListNames[MAX_NET_COUNT][MAX_NET_SIZE] =
{
	{ "HIF_A00",    "HIF_A08" },
	{ "HIF_A01",    "HIF_A09" },
	{ "HIF_A02",    "HIF_A10" },
	{ "HIF_A03",    "HIF_A11" },
	{ "HIF_A04",    "HIF_A12" },
	{ "HIF_A05",    "HIF_A13" },
	{ "HIF_A06",    "HIF_A14" },
	{ "HIF_A07",    "HIF_A15" },
	{ "HIF_D00",    "HIF_D08" },
	{ "HIF_D01",    "HIF_D09" },
	{ "HIF_D02",    "HIF_D10" },
	{ "HIF_D03",    "HIF_D11" },
	{ "HIF_D04",    "HIF_D12" },
	{ "HIF_D05",    "HIF_D13" },
	{ "HIF_D06",    "HIF_D14" },
	{ "HIF_D07",    "HIF_D15" },

	{ "HIF_D16",    "HIF_CSn" },
	{ "HIF_D17",    "HIF_D25" },
	{ "HIF_D18",    "HIF_BHE3" },
	{ "HIF_D19",    "HIF_D21" },
	{ "HIF_D20",    "HIF_D27",      "HIF_AHI0" },
	{ "HIF_D22",    "HIF_D29" },
	{ "HIF_D23",    "HIF_D28" },
	{ "HIF_D24",    "HIF_D30" },
	{ "HIF_D26",    "HIF_BHE1" },
	{ "HIF_SDCLK",  "HIF_WRn" },
	{ "HIF_AHI1",   "HIF_DIRQ" },

	{ "RSTOUT",     "HIF_RDY" }
};


static const PINDESCRIPTION_T *aptNetList[MAX_NET_COUNT][MAX_NET_SIZE];


/* This array has one entry for every pin under test.
 * The entry is a pointer to the description of the pins net.
 * NULL means that the pin is not a member of any net yet.
 */
static const PINDESCRIPTION_T **apptNetOfPin[MAX_PINS_UNDER_TEST];



static unsigned short ausPinToPinErrors[MAX_PINS_UNDER_TEST][MAX_PINS_UNDER_TEST];

/*-------------------------------------------------------------------------*/

static void print_net_desc(const char * const *ppcNetDesc)
{
	const char * const *ppcCnt;
	const char * const *ppcEnd;


	ppcCnt = ppcNetDesc;
	ppcEnd = ppcNetDesc + MAX_NET_SIZE;
	if( *ppcCnt==NULL )
	{
		uprintf("empty net");
	}
	else
	{
		do
		{
			uprintf("%s", *ppcCnt);
			++ppcCnt;
			if( *ppcCnt==NULL )
			{
				break;
			}
			else
			{
				uprintf(" - ");
			}
		} while( ppcCnt<ppcEnd );
	}
}



static void print_netlist(const PINDESCRIPTION_T **pptNetList)
{
	const PINDESCRIPTION_T **pptCnt;
	const PINDESCRIPTION_T **pptEnd;


	pptCnt = pptNetList;
	pptEnd = pptNetList + MAX_NET_SIZE;
	if( *pptCnt==NULL )
	{
		uprintf("empty net");
	}
	do
	{
		uprintf("%s", (*pptCnt)->pcName);

		++pptCnt;
		if( *pptCnt==NULL )
		{
			break;
		}
		else
		{
			uprintf(" - ");
		}
	} while( pptCnt<pptEnd );
}



static void print_all_netlists(const PINDESCRIPTION_T **pptNetList)
{
	size_t sizNetListCnt;


	/* Loop over all net list entries. */
	sizNetListCnt = 0;
	do
	{
		if( *pptNetList==NULL )
		{
			/* End of list. */
			break;
		}
		else
		{
			uprintf("Net %03d: [ ", sizNetListCnt);
			print_netlist(pptNetList);
			uprintf("]\n");
		}

		pptNetList += MAX_NET_SIZE;
		++sizNetListCnt;
	} while( sizNetListCnt<MAX_NET_COUNT );
}


/*-------------------------------------------------------------------------*/


static const PINDESCRIPTION_T *find_pin_by_name(const PINDESCRIPTION_T *ptPinsUnderTest, const char *pcPinName)
{
	const PINDESCRIPTION_T *ptPinCnt;
	const PINDESCRIPTION_T *ptPinEnd;
	const PINDESCRIPTION_T *ptPinHit;


	/* Search for the pin name in the pins under test. */
	ptPinCnt = ptPinsUnderTest;
	ptPinEnd = ptPinsUnderTest + MAX_PINS_UNDER_TEST;
	ptPinHit = NULL;

	while( ptPinCnt<ptPinEnd )
	{
		if( ptPinCnt->pcName==NULL )
		{
			/* A NULL name means: end of list. */
			break;
		}
		else if( strcmp(ptPinCnt->pcName, pcPinName)==0 )
		{
			ptPinHit = ptPinCnt;
			break;
		}
		else
		{
			++ptPinCnt;
		}
	}

	return ptPinHit;
}



static int build_net_list(const PINDESCRIPTION_T *ptPinsUnderTest, const char * const *ppcNetListNames, const PINDESCRIPTION_T **pptNetList)
{
	int iResult;
	size_t sizNetListCnt;
	size_t sizNetPinCnt;
	const PINDESCRIPTION_T *ptPinHit;
	const char *pcPinName;
	size_t sizPinIdx;


	/* Be optimistic. */
	iResult = 0;

	/* Clear the complete net list. */
	memset(pptNetList, 0, sizeof(PINDESCRIPTION_T*)*MAX_NET_COUNT*MAX_NET_SIZE);

	/* Clear the net index. */
	memset(apptNetOfPin, 0, sizeof(PINDESCRIPTION_T**)*MAX_PINS_UNDER_TEST);

	/* Loop over all net list name entries. */
	sizNetListCnt = 0;
	do
	{
		if( *ppcNetListNames==NULL )
		{
			/* End of list. */
			break;
		}
		else
		{
			/* Loop over all pins in this net. */
			sizNetPinCnt = 0;
			do
			{
				pcPinName = ppcNetListNames[sizNetPinCnt];
				if( pcPinName==NULL )
				{
					break;
				}
				else
				{
					ptPinHit = find_pin_by_name(ptPinsUnderTest, pcPinName);
					if( ptPinHit==NULL )
					{
						uprintf("Error in net ");
						print_net_desc(ppcNetListNames);
						uprintf(": the pin name '%s' is not a member of the pins under test!\n", pcPinName);
						iResult = -1;
						break;
					}
					else
					{
						/* Is this pin already part of a network? */
						sizPinIdx = (size_t)(ptPinHit - atPinsUnderTest);
						if( apptNetOfPin[sizPinIdx]!=NULL )
						{
							/* The pin is already a member of another net. */
							uprintf("Error in net ");
							print_net_desc(ppcNetListNames);
							uprintf(": the pin '%s' is already a member of another network!\n", pcPinName);
							iResult = -1;
							break;
						}
						else
						{
							/* Add the pin to the current network. */
							pptNetList[sizNetPinCnt] = ptPinHit;

							/* Mark the pin as 'used'. */
							apptNetOfPin[sizPinIdx] = pptNetList;
						}
					}
				}

				++sizNetPinCnt;
			} while( sizNetPinCnt<MAX_NET_SIZE );
		}

		if( iResult!=0 )
		{
			break;
		}

		ppcNetListNames += MAX_NET_SIZE;
		pptNetList += MAX_NET_SIZE;
		++sizNetListCnt;
	} while( sizNetListCnt<MAX_NET_COUNT );

	return iResult;
}



/*-------------------------------------------------------------------------*/


static int record_error(const PINDESCRIPTION_T *ptDrivingPin, const PINDESCRIPTION_T *ptReceivingPin, MATRIXERR_T tError)
{
	int iResult;
	size_t sizDriverIdx;
	size_t sizReceiverIdx;
	unsigned short usError;


	/* Be pessimistic. */
	iResult = -1;

	/* The receiver must not be NULL. */
	if( ptReceivingPin==NULL )
	{
		uprintf("ERROR: calling record_error with ptReceivingPin=NULL!\n");
	}
	else
	{
		/* If no pin is driving, this is an 'at defaults' test. */
		if( ptDrivingPin==NULL )
		{
			ptDrivingPin = ptReceivingPin;
		}

		/* Get the index of both pins. */
		sizDriverIdx = (size_t)(ptDrivingPin - atPinsUnderTest);
		sizReceiverIdx = (size_t)(ptReceivingPin - atPinsUnderTest);
		if( sizDriverIdx>=MAX_PINS_UNDER_TEST )
		{
			uprintf("ERROR: calling record_error with invalid ptDrivingPin: 0x%08x\n", ptDrivingPin);
		}
		else if( sizReceiverIdx>=MAX_PINS_UNDER_TEST )
		{
			uprintf("ERROR: calling record_error with invalid ptReceivingPin: 0x%08x\n", ptReceivingPin);
		}
		else
		{
			usError = (unsigned short)tError;
			ausPinToPinErrors[sizDriverIdx][sizReceiverIdx] |= usError;
			iResult = 0;
		}
	}

	return iResult;
}


static unsigned int dump_errors(void)
{
	unsigned int uiErrorCount;
	size_t sizDriverIdx;
	size_t sizReceiverIdx;
	const PINDESCRIPTION_T *ptDrivingPin;
	const PINDESCRIPTION_T *ptReceivingPin;
	unsigned short usError;


	uiErrorCount = 0;

	uprintf("\n\nMatrix errors:\n");
	for(sizDriverIdx=0; sizDriverIdx<MAX_PINS_UNDER_TEST; ++sizDriverIdx)
	{
		for(sizReceiverIdx=0; sizReceiverIdx<MAX_PINS_UNDER_TEST; ++sizReceiverIdx)
		{
			usError = ausPinToPinErrors[sizDriverIdx][sizReceiverIdx];
			if( usError!=0 )
			{
				ptDrivingPin = atPinsUnderTest + sizDriverIdx;
				ptReceivingPin = atPinsUnderTest + sizReceiverIdx;
				uprintf("%s - %s:\n", ptDrivingPin->pcName, ptReceivingPin->pcName);
				if( (usError&((unsigned short)MATRIXERR_Loopback_but_drive0_does_not_follow))!=0 )
				{
					uprintf("\tLoopback_but_drive0_does_not_follow\n");
					++uiErrorCount;
				}
				if( (usError&((unsigned short)MATRIXERR_Loopback_but_drive1_does_not_follow))!=0 )
				{
					uprintf("\tLoopback_but_drive1_does_not_follow\n");
					++uiErrorCount;
				}
				if( (usError&((unsigned short)MATRIXERR_No_Loopback_but_shortcut_for_drive0))!=0 )
				{
					uprintf("\tNo_Loopback_but_shortcut_for_drive0\n");
					++uiErrorCount;
				}
				if( (usError&((unsigned short)MATRIXERR_No_Loopback_but_shortcut_for_drive1))!=0 )
				{
					uprintf("\tNo_Loopback_but_shortcut_for_drive1\n");
					++uiErrorCount;
				}
				if( (usError&((unsigned short)MATRIXERR_Not_at_default_state_0_at_pin_test_start))!=0 )
				{
					uprintf("\tNot_at_default_state_0_at_pin_test_start\n");
					++uiErrorCount;
				}
				if( (usError&((unsigned short)MATRIXERR_Not_at_default_state_0_after_pin_test_drive0))!=0 )
				{
					uprintf("\tNot_at_default_state_0_after_pin_test_drive0\n");
					++uiErrorCount;
				}
				if( (usError&((unsigned short)MATRIXERR_Not_at_default_state_0_after_pin_test_drive1))!=0 )
				{
					uprintf("\tNot_at_default_state_0_after_pin_test_drive1\n");
					++uiErrorCount;
				}
			}
		}
	}

	if( uiErrorCount==0 )
	{
		uprintf("  No errors, all connections OK!\n");
	}
	else
	{
		uprintf("\n  There were %d errors.\n", uiErrorCount);
	}

	return uiErrorCount;
}

/*-------------------------------------------------------------------------*/


static int set_net_to_default(const PINDESCRIPTION_T **pptNet)
{
	int iResult;
	size_t sizNetPinCnt;
	const PINDESCRIPTION_T *ptPin;
	unsigned long ulFlags;
	PINSTATUS_T tPinStatus;


	/* Be optimistic. */
	iResult = 0;

	/* Do all pins in the network support INPUT or HIGHZ mode? */
	sizNetPinCnt = 0;
	do
	{
		ptPin = pptNet[sizNetPinCnt];
		if( ptPin==NULL )
		{
			/* End of list. */
			break;
		}
		else
		{
			ulFlags  = ptPin->ulFlags;
			ulFlags &= ((unsigned long)PINFLAG_I) | ((unsigned long)PINFLAG_Z);
			if( ulFlags!=0 )
			{
				tPinStatus = PINSTATUS_HIGHZ;
			}
			else
			{
				ulFlags  = ptPin->ulFlags;
				ulFlags &= ((unsigned long)PINFLAG_O);
				if( ulFlags!=0 )
				{
					/* Drive the default value. */
					if( ptPin->uiDefaultValue==0 )
					{
						tPinStatus = PINSTATUS_OUTPUT0;
					}
					else
					{
						tPinStatus = PINSTATUS_OUTPUT1;
					}
				}
				else
				{
					uprintf("The pin '%s' is neither I, O, nor HIGHZ!", ptPin->pcName);
					iResult = -1;
				}
			}

			if( iResult==0 )
			{
				iResult = iopins_set(ptPin, tPinStatus);
			}
		}
		++sizNetPinCnt;
	} while( sizNetPinCnt<MAX_NET_SIZE);

	return iResult;
}


static int set_all_pins_to_input(const PINDESCRIPTION_T **pptNetList)
{
	int iResult;
	size_t sizNetListCnt;


	/* Be optimistic. */
	iResult = 0;

	/* Loop over all net list entries. */
	sizNetListCnt = 0;
	do
	{
		if( *pptNetList==NULL )
		{
			/* End of list. */
			break;
		}
		else
		{
			iResult = set_net_to_default(pptNetList);
			if( iResult!=0 )
			{
				break;
			}
		}

		pptNetList += MAX_NET_SIZE;
		++sizNetListCnt;
	} while( sizNetListCnt<MAX_NET_COUNT );

	return iResult;
}



static int check_all_pins_for_default(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetDoNotTest, const PINDESCRIPTION_T *ptDrivingPin, MATRIXERR_T tError)
{
	int iResult;
	size_t sizNetListCnt;
	size_t sizNetPinCnt;
	const PINDESCRIPTION_T *ptPin;
	const PINDESCRIPTION_T **pptCnt;
	const PINDESCRIPTION_T **pptEnd;
	unsigned int uiValue;
	unsigned int uiDefaultValue;
	unsigned long ulFlags;


	/* Be optimistic. */
	iResult = 0;

	/* Loop over all net list entries. */
	sizNetListCnt = 0;
	do
	{
		if( *pptNetList==NULL )
		{
			/* End of list. */
			break;
		}
		else
		{
			sizNetPinCnt = 0;
			do
			{
				ptPin = pptNetList[sizNetPinCnt];
				if( ptPin==NULL )
				{
					/* End of list. */
					break;
				}
				else
				{
					/* Is the pin in the skip list? */
					if( pptNetDoNotTest!=NULL )
					{
						pptCnt = pptNetDoNotTest;
						pptEnd = pptNetDoNotTest + MAX_NET_SIZE;
						while( pptCnt<pptEnd )
						{
							if( *pptCnt==NULL )
							{
								/* End of list. */
								break;
							}
							else if( *pptCnt==ptPin )
							{
								/* Yes, the pin is in the skip list. */
								ptPin = NULL;
							}
							++pptCnt;
						}
					}

					if( ptPin!=NULL )
					{
						/* Can this pin be used as an input? */
						ulFlags  = ptPin->ulFlags;
						ulFlags &= ((unsigned long)PINFLAG_I);
						if( ulFlags!=0 )
						{
							uiDefaultValue = ptPin->uiDefaultValue;

							iResult = iopins_get(ptPin, &uiValue);
							if( iResult==0 )
							{
								if( uiValue!=uiDefaultValue )
								{
									iResult = record_error(ptDrivingPin, ptPin, tError);
									uprintf("Pin '%s' is not at its default value of %d, but %d!\n", ptPin->pcName, uiDefaultValue, uiValue);
								}
							}
							if( iResult!=0 )
							{
								break;
							}
						}
					}
				}
				++sizNetPinCnt;
			} while( sizNetPinCnt<MAX_NET_SIZE);
		}

		if( iResult!=0 )
		{
			break;
		}

		pptNetList += MAX_NET_SIZE;
		++sizNetListCnt;
	} while( sizNetListCnt<MAX_NET_COUNT );

	return iResult;
}


/*-------------------------------------------------------------------------*/

static void delay_to_stabilize_pins(void)
{
	unsigned long ulHandle;
	int iElapsed;


	ulHandle = systime_get_ms();
	do
	{
		iElapsed = systime_elapsed(ulHandle, 2);
	} while( iElapsed==0 );
}


static int test_pin_state(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetwork, const PINDESCRIPTION_T *ptDrivingPin, PINSTATUS_T tStatus)
{
	int iResult;
	int iResult2;
	const PINDESCRIPTION_T **pptCnt;
	const PINDESCRIPTION_T **pptEnd;
	const PINDESCRIPTION_T *ptPin;
	unsigned int uiExpectedValue;
	unsigned int uiValue;
	MATRIXERR_T tError0;
	MATRIXERR_T tError1;


	if( tStatus==PINSTATUS_OUTPUT0 )
	{
		uiExpectedValue = 0;
		tError0 = MATRIXERR_No_Loopback_but_shortcut_for_drive0;
		tError1 = MATRIXERR_Loopback_but_drive0_does_not_follow;
	}
	else if( tStatus==PINSTATUS_OUTPUT1 )
	{
		uiExpectedValue = 1;
		tError0 = MATRIXERR_No_Loopback_but_shortcut_for_drive1;
		tError1 = MATRIXERR_Loopback_but_drive1_does_not_follow;
	}
	else
	{
		return -1;
	}

	/* Set the pin to output. */
	/* TODO: print this only in verbose mode. */
//	uprintf("Driving pin '%s' to %d.\n", ptDrivingPin->pcName, uiExpectedValue);
	iResult = iopins_set(ptDrivingPin, tStatus);
	if( iResult==0 )
	{
		/* Delay a while to settle the pins. */
		delay_to_stabilize_pins();

		/* Check all pins for their default value except the network under test. */
		check_all_pins_for_default(pptNetList, pptNetwork, ptDrivingPin, tError0);

		/* Check the other pins in the network for connection. */
		pptCnt = pptNetwork;
		pptEnd = pptNetwork + MAX_NET_SIZE;
		while( pptCnt<pptEnd )
		{
			ptPin = *pptCnt;
			if( ptPin==NULL )
			{
				/* End of list. */
				break;
			}
			else if( ptPin!=ptDrivingPin )
			{
				iResult = iopins_get(ptPin, &uiValue);
				if( iResult==0 )
				{
					if( uiValue!=uiExpectedValue )
					{
						/* TODO: print this only in verbose mode. */
//						uprintf("  Pin '%s' does not follow.\n", ptPin->pcName);
						iResult = record_error(ptDrivingPin, ptPin, tError1);
					}
					else
					{
						/* TODO: print this only in verbose mode. */
//						uprintf("  Pin '%s' follows.\n", ptPin->pcName);
					}
				}
			}

			if( iResult!=0 )
			{
				break;
			}

			++pptCnt;
		}

		iResult2 = iopins_set(ptDrivingPin, PINSTATUS_HIGHZ);
		if( iResult2==0 )
		{
			/* Delay a while to settle the pins. */
			delay_to_stabilize_pins();
		}
		else
		{
			iResult = -1;
		}
	}

	return iResult;
}



static int test_pin(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetwork, const PINDESCRIPTION_T *ptDrivingPin)
{
	int iResult;
	unsigned long ulFlags;
	size_t sizNetPinCnt;
	const PINDESCRIPTION_T *ptPin;
	int iAtLeastOneInput;
	const PINDESCRIPTION_T *ptOutputOnlyPin;


	/* Can this pin be used as an output? */
	ulFlags  = ptDrivingPin->ulFlags;
	ulFlags &= ((unsigned long)PINFLAG_O);
	if( ulFlags==0 )
	{
		iResult = 0;
	}
	else
	{
		/* Among the other pins must be at least one INPUT pin.
		 * All other pins must have INPUT or HIGHZ capabilities.
		 */
		iAtLeastOneInput = 0;
		ptOutputOnlyPin = NULL;
		sizNetPinCnt = 0;
		do
		{
			ptPin = pptNetwork[sizNetPinCnt];
			if( ptPin==NULL )
			{
				/* End of list. */
				break;
			}
			/* Ignore the driving pin. */
			else if( ptPin!=ptDrivingPin )
			{
				ulFlags  = ptPin->ulFlags;
				ulFlags &= ((unsigned long)PINFLAG_I);
				if( ulFlags!=0 )
				{
					iAtLeastOneInput = 1;
				}

				ulFlags  = ptPin->ulFlags;
				ulFlags &= ((unsigned long)PINFLAG_IOZ);
				if( ulFlags==((unsigned long)PINFLAG_O) )
				{
					ptOutputOnlyPin = ptPin;
				}
			}
			++sizNetPinCnt;
		} while( sizNetPinCnt<MAX_NET_SIZE);

		if( ptOutputOnlyPin!=NULL )
		{
			uprintf("Not driving pin '%s'. The network contains an output only pin: %s\n", ptDrivingPin->pcName, ptOutputOnlyPin->pcName);
			iResult = 0;
		}
		else if( iAtLeastOneInput==0 )
		{
			uprintf("Not driving pin '%s'. The network contains no other input pins.\n", ptDrivingPin->pcName);
			iResult = 0;
		}
		else
		{
			iResult = check_all_pins_for_default(pptNetList, NULL, ptDrivingPin, MATRIXERR_Not_at_default_state_0_at_pin_test_start);
			if( iResult==0 )
			{
				iResult = test_pin_state(pptNetList, pptNetwork, ptDrivingPin, PINSTATUS_OUTPUT0);
				if( iResult==0 )
				{
					iResult = check_all_pins_for_default(pptNetList, NULL, ptDrivingPin, MATRIXERR_Not_at_default_state_0_after_pin_test_drive0);
					if( iResult==0 )
					{
						iResult = test_pin_state(pptNetList, pptNetwork, ptDrivingPin, PINSTATUS_OUTPUT1);
						if( iResult==0 )
						{
							iResult = check_all_pins_for_default(pptNetList, NULL, ptDrivingPin, MATRIXERR_Not_at_default_state_0_after_pin_test_drive1);
						}
					}
				}
			}
		}
	}

	return iResult;
}



static int test_network(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetwork, size_t sizNetIndex)
{
	int iResult;
	const PINDESCRIPTION_T **pptCnt;
	const PINDESCRIPTION_T **pptEnd;
	const PINDESCRIPTION_T *ptPin;


	/* TODO: print this only in verbose mode. */
//	uprintf("Testing net %03d: [ ", sizNetIndex);
//	print_netlist(pptNetwork);
//	uprintf("]\n");

	/* Loop over all pins in the network. */
	pptCnt = pptNetwork;
	pptEnd = pptNetwork + MAX_NET_SIZE;
	while( pptCnt<pptEnd )
	{
		ptPin = *pptCnt;
		if( ptPin==NULL )
		{
			/* End of list. */
			break;
		}
		else
		{
			iResult = test_pin(pptNetList, pptNetwork, ptPin);
			if( iResult!=0 )
			{
				/* Stop at fatal errors. */
				break;
			}
		}

		++pptCnt;
	}

	return iResult;
}


static int run_matrix_test(const PINDESCRIPTION_T **pptNetList)
{
	const PINDESCRIPTION_T **pptNetListCnt;
	size_t sizNetListCnt;
	int iResult;


	iResult = set_all_pins_to_input(pptNetList);
	if( iResult==0 )
	{
		/* Delay a while to settle the pins. */
		delay_to_stabilize_pins();

		/* Loop over all net list entries. */
		sizNetListCnt = 0;
		pptNetListCnt = pptNetList;
		do
		{
			if( *pptNetListCnt==NULL )
			{
				/* End of list. */
				break;
			}
			else
			{
				iResult = test_network(pptNetList, pptNetListCnt, sizNetListCnt);
				if( iResult!=0 )
				{
					/* Stop at fatal errors. */
					break;
				}
			}

			pptNetListCnt += MAX_NET_SIZE;
			++sizNetListCnt;
		} while( sizNetListCnt<MAX_NET_COUNT );
	}

	return iResult;
}


/*-------------------------------------------------------------------------*/

TEST_RESULT_T test(TEST_PARAMETER_T *ptTestParam)
{
	TEST_RESULT_T tTestResult;
	int iResult;
	unsigned int uiErrorCount;


	systime_init();

	uprintf("\f. *** I/O matrix test by doc_bacardi@users.sourceforge.net ***\n");
	uprintf("V" VERSION_ALL "\n\n");

	/* NOTE:
	 * Do not use the SYS led in this test. The RDY and RUN pins might be
	 * part of the test matrix.
	 */

	/* Get the test parameter. */
	//ptTestParams = (CRCTEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	/* Initialize the error array. */
	memset(ausPinToPinErrors, 0, sizeof(ausPinToPinErrors));

	iResult = build_net_list(atPinsUnderTest, apcNetListNames, aptNetList);
	if( iResult==0 )
	{
		/* TODO: check if all nets contain no more than 1 pins which
		 * are OUT only. These pins can only drive and should not be
		 * connected in one net.
		 */


		/* TODO: print this only in verbose mode. */
//		print_all_netlists(aptNetList);

		iResult = iopins_configure(atPinsUnderTest, MAX_PINS_UNDER_TEST);
		if( iResult==0 )
		{
			iResult = run_matrix_test(aptNetList);
			if( iResult==0 )
			{
				uiErrorCount = dump_errors();
				if( uiErrorCount!=0 )
				{
					iResult = -1;
				}
			}
		}
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

