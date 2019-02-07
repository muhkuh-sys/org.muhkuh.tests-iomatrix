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

#ifndef __IO_PINS_H__
#define __IO_PINS_H__


#include <sys/types.h>


#define MAX_PINDESCRIPTION_NAME 15

typedef enum PINTYPE_ENUM
{
	PINTYPE_GPIO    = 0,
	PINTYPE_PIO     = 1,
	PINTYPE_MLED    = 2,
	PINTYPE_MMIO    = 3,
	PINTYPE_HIFPIO  = 4,
	PINTYPE_RDYRUN  = 5,
	PINTYPE_RSTOUT  = 6,
	PINTYPE_XMIO    = 7,
	PINTYPE_RAPGPIO = 8
} PINTYPE_T;



typedef enum PINSTATUS_ENUM
{
	PINSTATUS_HIGHZ    = 0,
	PINSTATUS_OUTPUT0  = 1,
	PINSTATUS_OUTPUT1  = 2
} PINSTATUS_T;


typedef struct PINDESCRIPTION_STRUCT
{
	char apcName[MAX_PINDESCRIPTION_NAME+1];
	PINTYPE_T tType;
	unsigned int uiIndex;
} PINDESCRIPTION_T;


int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc);
int iopins_set(const PINDESCRIPTION_T *ptPinDescription, PINSTATUS_T tValue);
int iopins_get(const PINDESCRIPTION_T *ptPinDescription, unsigned char *pucValue);

#endif  /* __IO_PINS_H__ */

