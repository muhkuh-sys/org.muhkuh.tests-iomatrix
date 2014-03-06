
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


static const PINDESCRIPTION_T atPinsUnderTest[MAX_PINS_UNDER_TEST] =
{
	{ "HIF_D00",    PINTYPE_HIFPIO,  0, 1, 0 },
	{ "HIF_D01",    PINTYPE_HIFPIO,  1, 1, 0 },
	{ "HIF_D02",    PINTYPE_HIFPIO,  2, 1, 0 },
	{ "HIF_D03",    PINTYPE_HIFPIO,  3, 1, 0 },
	{ "HIF_D08",    PINTYPE_HIFPIO,  8, 1, 0 },
	{ "HIF_D09",    PINTYPE_HIFPIO,  9, 1, 0 },
	{ "HIF_D10",    PINTYPE_HIFPIO, 10, 1, 0 },
	{ "HIF_D11",    PINTYPE_HIFPIO, 11, 1, 0 },


#if 0
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
#endif
};



static const char * const apcNetListNames[MAX_NET_COUNT][MAX_NET_SIZE] =
{
	{ "HIF_D00", "HIF_D08" },
	{ "HIF_D01", "HIF_D09" },
	{ "HIF_D02", "HIF_D10" },
	{ "HIF_D03", "HIF_D11" }
};


static const PINDESCRIPTION_T *aptNetList[MAX_NET_COUNT][MAX_NET_SIZE];


/* This array has one entry for every pin under test.
 * The entry is a pointer to the description of the pins net.
 * NULL means that the pin is not a member of any net yet.
 */
static const PINDESCRIPTION_T **apptNetOfPin[MAX_PINS_UNDER_TEST];

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
			uprintf("Processing net ");
			print_net_desc(ppcNetListNames);
			uprintf("\n");

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
						uprintf("The pin name '%s' is not a member of the pins under test!\n", pcPinName);
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
							uprintf("The pin '%s' is already a member of another network!\n", pcPinName);
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


static int set_all_pins_to_input(const PINDESCRIPTION_T **pptNetList)
{
	int iResult;
	size_t sizNetListCnt;
	size_t sizNetPinCnt;
	const PINDESCRIPTION_T *ptPin;


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
					iResult = iopins_set(ptPin, PINSTATUS_INPUT);
					if( iResult!=0 )
					{
						break;
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



static int check_all_pins_for_default(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetDoNotTest)
{
	int iResult;
	int iErrors;
	size_t sizNetListCnt;
	size_t sizNetPinCnt;
	const PINDESCRIPTION_T *ptPin;
	const PINDESCRIPTION_T **pptCnt;
	const PINDESCRIPTION_T **pptEnd;
	unsigned int uiValue;
	unsigned int uiDefaultValue;


	/* Be optimistic. */
	iResult = 0;
	iErrors = 0;

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
						uiDefaultValue = ptPin->uiDefaultValue;

						iResult = iopins_get(ptPin, &uiValue);
						if( iResult!=0 )
						{
							break;
						}
						else
						{
							if( uiValue!=uiDefaultValue )
							{
								uprintf("Pin '%s' is not at its default value of %d, but %d!\n", ptPin->pcName, uiDefaultValue, uiValue);
								iErrors = -1;
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

	if( iErrors!=0 )
	{
		iResult = -1;
	}

	return iResult;
}


/*-------------------------------------------------------------------------*/

static void delay_to_stabilize_pins(void)
{
	uprintf("Deeeeeeeeeeeeeeeeeeeeeeeeeeeeeeelay!\n");
}


static int test_pin_state(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetwork, const PINDESCRIPTION_T *ptDrivingPin, PINSTATUS_T tStatus)
{
	int iResult;
	int iResult2;
	int iErrors;
	const PINDESCRIPTION_T **pptCnt;
	const PINDESCRIPTION_T **pptEnd;
	const PINDESCRIPTION_T *ptPin;
	unsigned int uiExpectedValue;
	unsigned int uiValue;


	if( tStatus==PINSTATUS_OUTPUT0 )
	{
		uiExpectedValue = 0;
	}
	else if( tStatus==PINSTATUS_OUTPUT1 )
	{
		uiExpectedValue = 1;
	}
	else
	{
		return -1;
	}

	iErrors = 0;

	/* Set the pin to output. */
	uprintf("Driving pin '%s' to %d.\n", ptDrivingPin->pcName, uiExpectedValue);
	iResult = iopins_set(ptDrivingPin, tStatus);
	if( iResult==0 )
	{
		/* Delay a while to settle the pins. */
		delay_to_stabilize_pins();

		/* Check all pins for their default value except the network under test. */
		check_all_pins_for_default(pptNetList, pptNetwork);

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
						uprintf("Pin '%s' has not the expected value of %d, but %d!\n", ptPin->pcName, uiExpectedValue, uiValue);
						iErrors = -1;
					}
				}
			}

			if( iResult!=0 )
			{
				break;
			}

			++pptCnt;
		}

		iResult2 = iopins_set(ptDrivingPin, PINSTATUS_INPUT);
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

	if( iErrors!=0 )
	{
		iResult = -1;
	}

	return iResult;
}



static int test_pin(const PINDESCRIPTION_T **pptNetList, const PINDESCRIPTION_T **pptNetwork, const PINDESCRIPTION_T *ptDrivingPin)
{
	int iResult;


	iResult = check_all_pins_for_default(pptNetList, NULL);
	if( iResult==0 )
	{
		iResult = test_pin_state(pptNetList, pptNetwork, ptDrivingPin, PINSTATUS_OUTPUT0);
		if( iResult==0 )
		{
			iResult = check_all_pins_for_default(pptNetList, NULL);
			if( iResult==0 )
			{
				iResult = test_pin_state(pptNetList, pptNetwork, ptDrivingPin, PINSTATUS_OUTPUT1);
				if( iResult==0 )
				{
					iResult = check_all_pins_for_default(pptNetList, NULL);
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


	uprintf("Testing net %03d: [ ", sizNetIndex);
	print_netlist(pptNetwork);
	uprintf("]\n");

	iResult = set_all_pins_to_input(pptNetList);
	if( iResult==0 )
	{
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
					break;
				}
			}

			++pptCnt;
		}

		if( iResult==0 )
		{
			uprintf("Net OK!\n");
		}
		else
		{
			uprintf("Net ERROR!\n");
		}
	}

	return iResult;
}


static int run_matrix_test(const PINDESCRIPTION_T **pptNetList)
{
	const PINDESCRIPTION_T **pptNetListCnt;
	size_t sizNetListCnt;
	int iResult;


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
		}

		if( iResult!=0 )
		{
			break;
		}

		pptNetListCnt += MAX_NET_SIZE;
		++sizNetListCnt;
	} while( sizNetListCnt<MAX_NET_COUNT );

	return -1;
}


/*-------------------------------------------------------------------------*/

TEST_RESULT_T test(TEST_PARAMETER_T *ptTestParam)
{
	TEST_RESULT_T tTestResult;
	int iResult;


	systime_init();

//	uprintf("\f. *** I/O matrix test by doc_bacardi@users.sourceforge.net ***\n");
//	uprintf("V" VERSION_ALL "\n\n");

	/* Switch off SYS led. */
	rdy_run_setLEDs(RDYRUN_OFF);

	/* Get the test parameter. */
	//ptTestParams = (CRCTEST_PARAMETER_T*)(ptTestParam->pvInitParams);

	iResult = build_net_list(atPinsUnderTest, apcNetListNames, aptNetList);
	if( iResult==0 )
	{
		print_all_netlists(aptNetList);

		iResult = iopins_configure(atPinsUnderTest, MAX_PINS_UNDER_TEST);
		if( iResult==0 )
		{
			iResult = run_matrix_test(aptNetList);
		}
	}

	rdy_run_setLEDs(RDYRUN_GREEN);
	tTestResult = TEST_RESULT_OK;

	return tTestResult;
}

/*-----------------------------------*/

