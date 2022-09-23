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


#ifndef __MAIN_TEST_H__
#define __MAIN_TEST_H__


#define MAX_PINS_UNDER_TEST 128


typedef enum IOMATRIX_COMMAND_ENUM
{
	IOMATRIX_COMMAND_Parse_Pin_Description        = 0,
	IOMATRIX_COMMAND_Set_Pin                      = 1,
	IOMATRIX_COMMAND_Get_Pin                      = 2,
	IOMATRIX_COMMAND_Set_All_Pins                 = 3,
	IOMATRIX_COMMAND_Get_All_Pins                 = 4,
	IOMATRIX_COMMAND_Get_Continuous_Status_Match  = 5,
	IOMATRIX_COMMAND_Get_Continuous_Changes       = 6
} IOMATRIX_COMMAND_T;



typedef struct IOMATRIX_PARAMETER_PARSE_PIN_DESCRIPTION_STRUCT
{
	const unsigned char *pucPinDefinitionStart;      /* The start of the pin definition. */
	unsigned long ulPinDefinitionSize;               /* The size of the pin definition in bytes. */
	void *pvPinDescription;                          /* Here the pointer to the created pin description is returned. Pass this to all other commands. */
} IOMATRIX_PARAMETER_PARSE_PIN_DESCRIPTION_T;



typedef struct IOMATRIX_PARAMETER_SET_PIN_STRUCT
{
	void *pvPinDescription;                          /* A handle of the pin description. */
	unsigned long ulPinIndex;                        /* The 0-based index of the pin. */
	PINSTATUS_T tStatus;                             /* The new status for the pin. */
} IOMATRIX_PARAMETER_SET_PIN_T;



typedef struct IOMATRIX_PARAMETER_GET_PIN_STRUCT
{
	void *pvPinDescription;                          /* A handle of the pin description. */
	unsigned long ulPinIndex;                        /* The 0-based index of the pin. */
	unsigned char ucValue;                           /* The value of the pin. */
} IOMATRIX_PARAMETER_GET_PIN_T;



typedef struct IOMATRIX_PARAMETER_SET_ALL_PINS_STRUCT
{
	void *pvPinDescription;                          /* A handle of the pin description. */
	unsigned char aucStatus[MAX_PINS_UNDER_TEST];    /* The new status for all pins. */
} IOMATRIX_PARAMETER_SET_ALL_PINS_T;



typedef struct IOMATRIX_PARAMETER_GET_ALL_PINS_STRUCT
{
	void *pvPinDescription;                          /* A handle of the pin description. */
	unsigned char aucValue[MAX_PINS_UNDER_TEST];     /* The status of all pins. */
} IOMATRIX_PARAMETER_GET_ALL_PINS_T;

typedef struct IOMATRIX_PARAMETER_CONTINUOUS_PIN_STATUS_T
{
	void *pvPinDescription;							/* A handle of the pin description. */
	unsigned long ulNumberOfPatternsToTest;			/* Number of patterns in test list */
	unsigned char aucList[MAX_PINS_UNDER_TEST];		/* The list with test pattern. */
}IOMATRIX_PARAMETER_GET_CONTINUOUS_STATUS_MATCH_T;

typedef struct IOMATRIX_PARAMETER_GET_CONTINUOUS_CHANGES_STRUCT
{
	void *pvPinDescription;                         /* A handle of the pin description. */
	unsigned long ulNumberOfStates;
	unsigned long aulStates[MAX_PINS_UNDER_TEST/4];
} IOMATRIX_PARAMETER_GET_CONTINUOUS_CHANGES_T;

typedef struct IOMATRIX_PARAMETER_STRUCT
{
	unsigned long ulVerbose;
	IOMATRIX_COMMAND_T tCommand;
	union IOMATRIX_PARAMETERS_UNION
	{
		IOMATRIX_PARAMETER_PARSE_PIN_DESCRIPTION_T tParsePinDescription;
		IOMATRIX_PARAMETER_SET_PIN_T tSetPin;
		IOMATRIX_PARAMETER_GET_PIN_T tGetPin;
		IOMATRIX_PARAMETER_SET_ALL_PINS_T tSetAllPins;
		IOMATRIX_PARAMETER_GET_ALL_PINS_T tGetAllPins;
		IOMATRIX_PARAMETER_GET_CONTINUOUS_STATUS_MATCH_T tGetContinuousStatusMatch;
		IOMATRIX_PARAMETER_GET_CONTINUOUS_CHANGES_T tGetContinuousChanges;
	} uParameter;
} IOMATRIX_PARAMETER_T;


typedef enum TEST_RESULT_ENUM
{
	TEST_RESULT_OK = 0,
	TEST_RESULT_ERROR = 1
} TEST_RESULT_T;


TEST_RESULT_T test(IOMATRIX_PARAMETER_T *ptTestParams);


#endif  /* __MAIN_TEST_H__ */

