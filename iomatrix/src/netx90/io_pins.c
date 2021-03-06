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

#include "netIOL/netiol_regdef.h"
#include "app_bridge.h"
#include "netx_io_areas.h"
#include "uprintf.h"


#define MAX_NETIOL_NODES 4


typedef struct UNITCONFIGURATION_STRUCT
{
	unsigned long ulGpio;
	unsigned long ulComIo;
	unsigned long aulHifPio[2];
	unsigned long aulIolLedM[MAX_NETIOL_NODES];
	unsigned long ulMled;
	unsigned long ulMmio;
	unsigned long ulPioApp;
	unsigned long ulRdyRun;
	unsigned long ulRstOut;
	unsigned long aulXmio[2];
} UNITCONFIGURATION_T;


static void initialize_unit_configuration(UNITCONFIGURATION_T *ptUnitCfg)
{
	ptUnitCfg->ulGpio        = 0;
	ptUnitCfg->ulComIo       = 0;
	ptUnitCfg->aulHifPio[0]  = 0;
	ptUnitCfg->aulHifPio[1]  = 0;
	memset(ptUnitCfg->aulIolLedM, 0, MAX_NETIOL_NODES*sizeof(unsigned long));
	ptUnitCfg->ulMled        = 0;
	ptUnitCfg->ulMmio        = 0;
	ptUnitCfg->ulPioApp      = 0;
	ptUnitCfg->ulRdyRun      = 0;
	ptUnitCfg->ulRstOut      = 0;
	ptUnitCfg->aulXmio[0]    = 0;
	ptUnitCfg->aulXmio[1]    = 0;
}



/* Local copies of remote registers. */
static unsigned long s_ulAppPio_Oe;
static unsigned long s_ulAppPio_Out;

static unsigned long s_aulIolLedM[MAX_NETIOL_NODES];


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
				/* On the COM side GPIO 8-11 are available. */
				if( uiIndex>7 && uiIndex<12 )
				{
					ptUnitCfg->ulGpio |= 1U << uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_HIFPIO:
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

			case PINTYPE_MLED:
				if( uiIndex<8 )
				{
					ptUnitCfg->ulMled |= 1U<<uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_MMIO:
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
				/* For now XM0_IO0-5 and XM1_IO0-5 are supported with the indices 0..11 . */
				if( uiIndex<6 )
				{
					ptUnitCfg->aulXmio[0] |= 1U << uiIndex;
					iResult = 0;
				}
				else if( uiIndex<12 )
				{
					ptUnitCfg->aulXmio[1] |= 1U << (uiIndex - 6U);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_RAPGPIO:
				uprintf("The pin type RAPGPIO is not supported on this platform!\n");
				break;

			case PINTYPE_APPPIO:
				/* The netX90 has 32 APP PIOs. */
				if( uiIndex<32 )
				{
					ptUnitCfg->ulPioApp |= 1U << uiIndex;
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
				}
				break;

			case PINTYPE_IOLLEDM:
				/* Each netIOL has 24 LEDs in the matrix. */
				if( uiIndex<24*MAX_NETIOL_NODES )
				{
					ptUnitCfg->aulIolLedM[uiIndex/24] |= 1U << (uiIndex%24);
					iResult = 0;
				}
				else
				{
					uprintf("The pin %s has an invalid index of %d!", ptPinDescCnt->apcName, uiIndex);
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



static NX90_XMAC_AREA_T * const aptXmacArea[2] =
{
	(NX90_XMAC_AREA_T*) Addr_NX90_xc0_xmac0_regs,
	(NX90_XMAC_AREA_T*) Addr_NX90_xc0_xmac1_regs
};



static NX90_XPEC_AREA_T * const aptRpecRegArea[2] =
{
	(NX90_XPEC_AREA_T*) Addr_NX90_xc0_rpec0_regs,
	(NX90_XPEC_AREA_T*) Addr_NX90_xc0_rpec1_regs
};



static NX90_XPEC_AREA_T * const aptTpecRegArea[2] =
{
	(NX90_XPEC_AREA_T*) Addr_NX90_xc0_tpec0_regs,
	(NX90_XPEC_AREA_T*) Addr_NX90_xc0_tpec1_regs
};


static unsigned long * const apulRpecPramArea[2] =
{
	(unsigned long*) Adr_NX90_xc0_rpec0_pram_ram_start,
	(unsigned long*) Adr_NX90_xc0_rpec1_pram_ram_start
};



static unsigned long * const apulTpecPramArea[2] =
{
	(unsigned long*) Adr_NX90_xc0_tpec0_pram_ram_start,
	(unsigned long*) Adr_NX90_xc0_tpec1_pram_ram_start
};



static const unsigned long XcCode_rpu_reset0[27] = {
  0x00000064, /* program size */
  0x00000000, /* trailing loads size */
  0xff110000, /* start address */
  0x01300001, 0xe15bde81, 0x01380001, 0xe15bde82, 0x0143fffd, 0xe15bde83, 0x0147fffd, 0xe15bde84,
  0x01480001, 0xe15bde85, 0x0143fff9, 0xe15bde86, 0x014bfffd, 0xe15bde87, 0x01440001, 0xe15bde88,
  0x0143fffd, 0xe15bde89, 0x01480001, 0xe15bde8a, 0x01380005, 0xe15bde8b, 0x01080000, 0x001fde8b,
  /* trailing loads */
};

static const unsigned long XcCode_rpu_reset1[27] = {
  0x00000064, /* program size */
  0x00000000, /* trailing loads size */
  0xff110800, /* start address */
  0x01300001, 0xe15bde81, 0x01380001, 0xe15bde82, 0x0143fffd, 0xe15bde83, 0x0147fffd, 0xe15bde84,
  0x01480001, 0xe15bde85, 0x0143fff9, 0xe15bde86, 0x014bfffd, 0xe15bde87, 0x01440001, 0xe15bde88,
  0x0143fffd, 0xe15bde89, 0x01480001, 0xe15bde8a, 0x01380005, 0xe15bde8b, 0x01080000, 0x001fde8b,
  /* trailing loads */
};
static const unsigned long XcCode_tpu_reset0[27] = {
  0x00000064, /* program size */
  0x00000000, /* trailing loads size */
  0xff110400, /* start address */
  0x014c0601, 0xe15bde81, 0x01540001, 0xe15bde82, 0x015ffffd, 0xe15bde83, 0x0163fffd, 0xe15bde84,
  0x01640001, 0xe15bde85, 0x015ffff9, 0xe15bde86, 0x0167fffd, 0xe15bde87, 0x01600001, 0xe15bde88,
  0x015ffffd, 0xe15bde89, 0x01640001, 0xe15bde8a, 0x01540005, 0xe15bde8b, 0x01080000, 0x001fde8b,
  /* trailing loads */
};

static const unsigned long XcCode_tpu_reset1[27] = {
  0x00000064, /* program size */
  0x00000000, /* trailing loads size */
  0xff110c00, /* start address */
  0x014c0601, 0xe15bde81, 0x01540001, 0xe15bde82, 0x015ffffd, 0xe15bde83, 0x0163fffd, 0xe15bde84,
  0x01640001, 0xe15bde85, 0x015ffff9, 0xe15bde86, 0x0167fffd, 0xe15bde87, 0x01600001, 0xe15bde88,
  0x015ffffd, 0xe15bde89, 0x01640001, 0xe15bde8a, 0x01540005, 0xe15bde8b, 0x01080000, 0x001fde8b,
  /* trailing loads */
};


static const unsigned long * const paulRpuResetCodes[2]=
{
	XcCode_rpu_reset0,
	XcCode_rpu_reset1
};

static const unsigned long * const paulTpuResetCodes[2]=
{
	XcCode_tpu_reset0,
	XcCode_tpu_reset1
};


static void NX90_XC_Load(const unsigned long* pulXcPrg)
{
	volatile unsigned long *pulDst, *pulDstCnt;
	const unsigned long *pulSrcStart, *pulSrcCnt, *pulSrcEnd;
	unsigned int uiElements;


	/* Get the number of code elements. */
	uiElements = pulXcPrg[0] / sizeof(unsigned long) - 1;

	/* Get the pointer in the XC area. */
	pulDst = (volatile unsigned long*) pulXcPrg[2];

	/* Get source start and end pointer. */
	pulSrcStart = pulXcPrg + 3;
	pulSrcEnd = pulSrcStart + uiElements;

	/* Copy the code to the XC RAM. */
	pulSrcCnt = pulSrcStart;
	pulDstCnt = pulDst;
	while( pulSrcCnt<pulSrcEnd )
	{
		*pulDstCnt = *pulSrcCnt;
		pulDstCnt++;
		pulSrcCnt++;
	}

	/* Get the number of trailing loads. */
	uiElements = pulXcPrg[1] / sizeof(unsigned long);

	/* Get source start and end pointer. */
	pulSrcCnt = pulXcPrg + 2 + pulXcPrg[0] / sizeof(unsigned long);
	pulSrcEnd = pulSrcCnt + uiElements;

	/* Write all trailing loads. */
	while( pulSrcCnt<pulSrcEnd )
	{
		/* Get the destination address. */
		pulDst = (volatile unsigned long*) *pulSrcCnt;
		pulSrcCnt++;

		/* write the data. */
		*pulDst = *pulSrcCnt;
		pulSrcCnt++;
	}
}



static void NX90_XC_Reset(unsigned int uiPortNr)
{
	HOSTDEF(ptXcStartStopArea);
	HOSTDEF(ptXpecIrqRegistersArea);
	NX90_XMAC_AREA_T* ptXmac;
	NX90_XPEC_AREA_T* ptRpec;
	NX90_XPEC_AREA_T* ptTpec;
	volatile unsigned long* pulRpecPram;
	volatile unsigned long* pulTpecPram;
	unsigned int uIdx;
	unsigned long ulValue;


	ptXmac      = aptXmacArea[uiPortNr];
	ptRpec      = aptRpecRegArea[uiPortNr];
	ptTpec      = aptTpecRegArea[uiPortNr];
	pulRpecPram = apulRpecPramArea[uiPortNr];
	pulTpecPram = apulTpecPramArea[uiPortNr];

	/* Stop xPEC and xMAC */
	if( uiPortNr==0 )
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu0);
	}
	else
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu1);
	}
	ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;

	/* Clear output enable of TX as soon as possible. */
	ptXmac->ulXmac_config_obu  = HOSTDFLT(xmac_config_obu);
	/* Clear output enable of io0..5 as soon as possible. */
	ptRpec->aulStatcfg[uiPortNr] = 0xffff0000;

	if( uiPortNr==0 )
	{
		ulValue  = HOSTMSK(xpec_config_reset_urx_fifo0);
		ulValue |= HOSTMSK(xpec_config_reset_utx_fifo0);
	}
	else
	{
		ulValue  = HOSTMSK(xpec_config_reset_urx_fifo1);
		ulValue |= HOSTMSK(xpec_config_reset_utx_fifo1);
	}
	ptRpec->ulXpec_config = ulValue;

	/* Load the rate multiplier reset code. */
	NX90_XC_Load(paulRpuResetCodes[uiPortNr]);
	NX90_XC_Load(paulTpuResetCodes[uiPortNr]);

	/* Reset the PC of the RPU and TPU unit to 0 */
	ptXmac->ulXmac_rpu_pc = 0;
	ptXmac->ulXmac_tpu_pc = 0;

	if( uiPortNr==0 )
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_start_rpu0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_start_tpu0);
	}
	else
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_start_rpu1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_start_tpu1);
	}
	ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;

	/* !!!! ATTENTION: There must be enough time between starting xMAC and stopping xMAC to execute reset program */

	pulRpecPram[0] = 0xC0000FFF;                                   /* Use the command wait b000000000000,b111111111111 at Address 0*/
	pulTpecPram[0] = 0xC0000FFF;                                   /* Use the command wait b000000000000,b111111111111 at Address 0*/

	ptRpec->ulXpec_pc = 0x7ff;                                     /* Reset the Program Counter to 0x7ff */
	ptTpec->ulXpec_pc = 0x7ff;                                     /* Reset the Program Counter to 0x7ff */


	/* let the XC run for at least 10 cycles */
	if( uiPortNr==0 )
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_start_rpec0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_start_tpec0);
	}
	else
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_start_rpec1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_start_tpec1);
	}
	for(uIdx=0; uIdx<10; ++uIdx)
	{
		ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;
	}

	if( uiPortNr==0 )
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec0);
	}
	else
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec1);
	}
	ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;
	ptRpec->ulXpec_pc     = 0x7ff;                                 /* Reset the Program Counter to 0x7ff */
	ptTpec->ulXpec_pc     = 0x7ff;                                 /* Reset the Program Counter to 0x7ff */

	/* reset all registers */
	ptRpec->aulXpec_r[0]   = 0;
	ptTpec->aulXpec_r[0]   = 0;
	ptRpec->aulXpec_r[1]   = 0;
	ptTpec->aulXpec_r[1]   = 0;
	ptRpec->aulXpec_r[2]   = 0;
	ptTpec->aulXpec_r[2]   = 0;
	ptRpec->aulXpec_r[3]   = 0;
	ptTpec->aulXpec_r[3]   = 0;
	ptRpec->aulXpec_r[4]   = 0;
	ptTpec->aulXpec_r[4]   = 0;
	ptRpec->aulXpec_r[5]   = 0;
	ptTpec->aulXpec_r[5]   = 0;
	ptRpec->aulXpec_r[6]   = 0;
	ptTpec->aulXpec_r[6]   = 0;
	ptRpec->aulXpec_r[7]   = 0;
	ptTpec->aulXpec_r[7]   = 0;

	/* Note regarding stat_bits_shared: this register is used for XC instance crossover protocols, protocol has to clear this register when initializing */

	ptRpec->ulRange_urtx_count = 0;
	ptTpec->ulRange_urtx_count = 0;
	ptRpec->ulRange45          = 0;
	ptTpec->ulRange45          = 0;
	ptRpec->ulRange67          = 0;
	ptTpec->ulRange67          = 0;
	ptRpec->ulUrx_count        = 0;
	ptTpec->ulUrx_count        = 0;
	ptRpec->ulUtx_count        = 0;
	ptTpec->ulUtx_count        = 0;

	/* Stop all Timers */
	ptRpec->ulTimer4       = 0;
	ptTpec->ulTimer4       = 0;
	ptRpec->ulTimer5       = 0;
	ptTpec->ulTimer5       = 0;
	ptRpec->aulTimer[0]    = 0;
	ptTpec->aulTimer[0]    = 0;
	ptRpec->aulTimer[1]    = 0;
	ptTpec->aulTimer[1]    = 0;
	ptRpec->aulTimer[2]    = 0;
	ptTpec->aulTimer[2]    = 0;
	ptRpec->aulTimer[3]    = 0;
	ptTpec->aulTimer[3]    = 0;

	ptRpec->ulIrq          = 0xFFFF0000; /* Clear XPEC side IRQ request lines */
	ptTpec->ulIrq          = 0xFFFF0000; /* Clear XPEC side IRQ request lines */

	/* Reset events */
	ptRpec->ulEc_maska     = 0x0000FFFF;
	ptTpec->ulEc_maska     = 0x0000FFFF;
	ptRpec->ulEc_maskb     = 0x0000FFFF;
	ptTpec->ulEc_maskb     = 0x0000FFFF;
	ptRpec->aulEc_mask[0]  = 0x0000FFFF;
	ptTpec->aulEc_mask[0]  = 0x0000FFFF;
	ptRpec->aulEc_mask[1]  = 0x0000FFFF;
	ptTpec->aulEc_mask[1]  = 0x0000FFFF;
	ptRpec->aulEc_mask[2]  = 0x0000FFFF;
	ptTpec->aulEc_mask[2]  = 0x0000FFFF;
	ptRpec->aulEc_mask[3]  = 0x0000FFFF;
	ptTpec->aulEc_mask[3]  = 0x0000FFFF;
	ptRpec->aulEc_mask[4]  = 0x0000FFFF;
	ptTpec->aulEc_mask[4]  = 0x0000FFFF;
	ptRpec->aulEc_mask[5]  = 0x0000FFFF;
	ptTpec->aulEc_mask[5]  = 0x0000FFFF;
	ptRpec->aulEc_mask[6]  = 0x0000FFFF;
	ptTpec->aulEc_mask[6]  = 0x0000FFFF;
	ptRpec->aulEc_mask[7]  = 0x0000FFFF;
	ptTpec->aulEc_mask[7]  = 0x0000FFFF;
	ptRpec->aulEc_mask[8]  = 0x0000FFFF;
	ptTpec->aulEc_mask[8]  = 0x0000FFFF;
	ptRpec->aulEc_mask[9]  = 0x0000FFFF;
	ptTpec->aulEc_mask[9]  = 0x0000FFFF;

	/* Reset shared registers. */
	if( uiPortNr==0 )
	{
		/* Reset SR0-3. */
		ptRpec->aulXpec_sr[0] = 0;
		ptRpec->aulXpec_sr[1] = 0;
		ptRpec->aulXpec_sr[2] = 0;
		ptRpec->aulXpec_sr[3] = 0;

		/* Reset SR8-11. */
		ptRpec->aulXpec_sr[8] = 0;
		ptRpec->aulXpec_sr[9] = 0;
		ptRpec->aulXpec_sr[10] = 0;
		ptRpec->aulXpec_sr[11] = 0;
	}
	else
	{
		/* Reset SR4-7. */
		ptRpec->aulXpec_sr[4] = 0;
		ptRpec->aulXpec_sr[5] = 0;
		ptRpec->aulXpec_sr[6] = 0;
		ptRpec->aulXpec_sr[7] = 0;

		/* Reset SR12-15. */
		ptRpec->aulXpec_sr[12] = 0;
		ptRpec->aulXpec_sr[13] = 0;
		ptRpec->aulXpec_sr[14] = 0;
		ptRpec->aulXpec_sr[15] = 0;
	}

	ptRpec->ulDatach_wr_cfg = 0;
	ptTpec->ulDatach_wr_cfg = 0;
	ptRpec->ulDatach_rd_cfg = 0;
	ptTpec->ulDatach_rd_cfg = 0;
	ptRpec->ulSysch_addr    = 0;
	ptTpec->ulSysch_addr    = 0;

	/* confirm all interrupts from xPEC -> Select the specific XC instance and get IRQ */
	ptXpecIrqRegistersArea->aulXc0_irq_xpec[uiPortNr] = 0x0000FFFF;

	/* hold xMAC */
	if( uiPortNr==0 )
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu0);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu0);
	}
	else
	{
		ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu1);
		ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu1);
	}
	ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;

	/* reset all xMAC registers to default values */
	ptXmac->ulXmac_rx_hw               = 0;
	ptXmac->ulXmac_rx_hw_count         = 0;
	ptXmac->ulXmac_tx                  = 0;
	ptXmac->ulXmac_tx_hw               = 0;
	ptXmac->ulXmac_tx_hw_count         = 0;
	ptXmac->ulXmac_tx_sent             = 0;
	ptXmac->aulXmac_wr[0]              = 0;
	ptXmac->aulXmac_wr[1]              = 0;
	ptXmac->aulXmac_wr[2]              = 0;
	ptXmac->aulXmac_wr[3]              = 0;
	ptXmac->aulXmac_wr[4]              = 0;
	ptXmac->aulXmac_wr[5]              = 0;
	ptXmac->aulXmac_wr[6]              = 0;
	ptXmac->aulXmac_wr[7]              = 0;
	ptXmac->aulXmac_wr[8]              = 0;
	ptXmac->aulXmac_wr[9]              = 0;
	ptXmac->ulXmac_config_mii          = 0;
	ptXmac->ulXmac_config_rx_nibble_fifo  = HOSTDFLT(xmac_config_rx_nibble_fifo);
	ptXmac->ulXmac_config_tx_nibble_fifo  = 0;
	ptXmac->ulXmac_rpu_count1          = 0;
	ptXmac->ulXmac_rpu_count2          = 0;
	ptXmac->ulXmac_tpu_count1          = 0;
	ptXmac->ulXmac_tpu_count2          = 0;
	ptXmac->ulXmac_rx_count            = 0;
	ptXmac->ulXmac_tx_count            = 0;
	ptXmac->ulXmac_rpm_mask0           = 0;
	ptXmac->ulXmac_rpm_val0            = 0;
	ptXmac->ulXmac_rpm_mask1           = 0;
	ptXmac->ulXmac_rpm_val1            = 0;
	ptXmac->ulXmac_tpm_mask0           = 0;
	ptXmac->ulXmac_tpm_val0            = 0;
	ptXmac->ulXmac_tpm_mask1           = 0;
	ptXmac->ulXmac_tpm_val1            = 0;

	ptXmac->ulXmac_rx_crc_polynomial_l = 0;
	ptXmac->ulXmac_rx_crc_polynomial_h = 0;
	ptXmac->ulXmac_rx_crc_l            = 0;
	ptXmac->ulXmac_rx_crc_h            = 0;
	ptXmac->ulXmac_rx_crc_cfg          = 0;
	ptXmac->ulXmac_tx_crc_polynomial_l = 0;
	ptXmac->ulXmac_tx_crc_polynomial_h = 0;
	ptXmac->ulXmac_tx_crc_l            = 0;
	ptXmac->ulXmac_tx_crc_h            = 0;
	ptXmac->ulXmac_tx_crc_cfg          = 0;

	ptXmac->ulXmac_rx_crc32_l          = 0;
	ptXmac->ulXmac_rx_crc32_h          = 0;
	ptXmac->ulXmac_rx_crc32_cfg        = 0;
	ptXmac->ulXmac_tx_crc32_l          = 0;
	ptXmac->ulXmac_tx_crc32_h          = 0;
	ptXmac->ulXmac_tx_crc32_cfg        = 0;

	ptXmac->ulXmac_config_sbu2         = HOSTDFLT(xmac_config_sbu2);
	ptXmac->ulXmac_config_obu2         = HOSTDFLT(xmac_config_obu2);

	ptXmac->ulXmac_rpu_pc              = 0;
	ptXmac->ulXmac_tpu_pc              = 0;
}



static int configure_xm0io(unsigned long ulPins)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptXcStartStopArea);
	HOSTDEF(ptXc0Xmac0RegsArea);
	unsigned long ulValue;
	int iResult;


	/* Be optimistic. */
	iResult = 0;

	/* Can the XMAC0 clock be enabled? */
	ulValue = HOSTMSK(clock_enable0_mask_xmac0) | HOSTMSK(clock_enable0_mask_xc_misc);
	if( (ptAsicCtrlArea->asClock_enable[0].ulMask & ulValue)!=ulValue )
	{
		uprintf("ERROR: XM0_IO pins can not be used as the XMAC0 and XC_MISC clock can not be enabled.\n");
		iResult = -1;
	}
	else
	{
		/* Can the pins be activated? */
		ulValue   = ptAsicCtrlArea->asIo_config[0].ulMask;
		ulValue  &= HOSTMSK(io_config0_sel_xm0_io);
		ulValue >>= HOSTSRT(io_config0_sel_xm0_io);
		ulValue  &= ulPins;
		if( ulValue!=ulPins )
		{
			uprintf("ERROR: Not all XM0_IO pins can be enabled.\n");
			iResult = -1;
		}
		else
		{
			/* Are the clocks already enabled? */
			ulValue = HOSTMSK(clock_enable0_xmac0) | HOSTMSK(clock_enable0_xc_misc);
			if( (ptAsicCtrlArea->asClock_enable[0].ulEnable & ulValue)!=ulValue )
			{
				uprintf("Enabling XC0 clocks...\n");

				/* Enable the XMAC0 and XC_MISC clock. */
				ulValue  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulValue |= HOSTMSK(clock_enable0_xmac0);
				ulValue |= HOSTMSK(clock_enable0_xmac0_wm);
				ulValue |= HOSTMSK(clock_enable0_xc_misc);
				ulValue |= HOSTMSK(clock_enable0_xc_misc_wm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulValue;
			}
			else
			{
				uprintf("The XC0 clocks are already enabled.\n");
			}

			NX90_XC_Reset(0);

			/* Stop the unit. */
			ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu0);
			ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu0);
			ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec0);
			ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec0);
			ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;

			/* Using these pins is not a good idea if XMAC0 is running. */
			uprintf("ulXc_hold_status: 0x%08x\n", ptXcStartStopArea->ulXc_hold_status);
			ulValue = HOSTMSK(xc_hold_status_xc0_hold_tpu0) | HOSTMSK(xc_hold_status_xc0_hold_rpu0);
			if( (ptXcStartStopArea->ulXc_hold_status & ulValue)!=ulValue )
			{
				uprintf("ERROR: XM0_IO pins can not be used as the unit is running.\n");
				iResult = -1;
			}
			else
			{
				/* Set all pins to input. */
				ulValue  = HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio1_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio2_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio3_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio4_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio5_oe_wm);
				ptXc0Xmac0RegsArea->ulXmac_io_oe_shared0 = ulValue;

				/* XM_IO2-5 are shared with the PHY LEDs. Deactivate the LED function if they are in use. */
				if( (ulPins&((1U<<2)|(1U<<3)|(1U<<4)|(1U<<5)))!=0 )
				{
					ulValue  = ptXc0Xmac0RegsArea->ulXmac_config_sbu;
					ulValue &= ~HOSTMSK(xmac_config_sbu_phy_led_en);
					ptXc0Xmac0RegsArea->ulXmac_config_sbu = ulValue;
				}

				/* Activate the XMIO pins in the IO configuration. */
				ulValue  = ptAsicCtrlArea->asIo_config[0].ulConfig;
				ulValue &= ~HOSTMSK(io_config0_sel_xm0_io);
				ulValue |= (ulPins << HOSTSRT(io_config0_sel_xm0_io)) & HOSTMSK(io_config0_sel_xm0_io);
				ulValue |= HOSTMSK(io_config0_sel_xm0_io_wm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asIo_config[0].ulConfig = ulValue;
			}
		}
	}

	return iResult;
}



static int configure_xm1io(unsigned long ulPins)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptXcStartStopArea);
	HOSTDEF(ptXc0Xmac1RegsArea);
	unsigned long ulValue;
	int iResult;


	/* Be optimistic. */
	iResult = 0;

	/* Can the XMAC1 clock be enabled? */
	ulValue = HOSTMSK(clock_enable0_mask_xmac1) | HOSTMSK(clock_enable0_mask_xc_misc);
	if( (ptAsicCtrlArea->asClock_enable[0].ulMask & ulValue)!=ulValue )
	{
		uprintf("ERROR: XM1_IO pins can not be used as the XMAC1 and XC_MISC clock can not be enabled.\n");
		iResult = -1;
	}
	else
	{
		/* Can the pins be activated? */
		ulValue   = ptAsicCtrlArea->asIo_config[1].ulMask;
		ulValue  &= HOSTMSK(io_config1_sel_xm1_io);
		ulValue >>= HOSTSRT(io_config1_sel_xm1_io);
		ulValue  &= ulPins;
		if( ulValue!=ulPins )
		{
			uprintf("ERROR: Not all XM1_IO pins can be enabled.\n");
			iResult = -1;
		}
		else
		{
			/* Are the clocks already enabled? */
			ulValue = HOSTMSK(clock_enable0_xmac1) | HOSTMSK(clock_enable0_xc_misc);
			if( (ptAsicCtrlArea->asClock_enable[0].ulEnable & ulValue)!=ulValue )
			{
				uprintf("Enabling XC1 clocks...\n");

				/* Enable the XMAC0 and XC_MISC clock. */
				ulValue  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulValue |= HOSTMSK(clock_enable0_xmac1);
				ulValue |= HOSTMSK(clock_enable0_xmac1_wm);
				ulValue |= HOSTMSK(clock_enable0_xc_misc);
				ulValue |= HOSTMSK(clock_enable0_xc_misc_wm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulValue;
			}
			else
			{
				uprintf("The XC1 clocks are already enabled.\n");
			}

			NX90_XC_Reset(1);

			/* Stop the unit. */
			ulValue  = HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpu1);
			ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpu1);
			ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_rpec1);
			ulValue |= HOSTMSK(xc_start_stop_ctrl_xc0_stop_tpec1);
			ptXcStartStopArea->ulXc_start_stop_ctrl = ulValue;

			/* Using these pins is not a good idea if XMAC1 is running. */
			uprintf("ulXc_hold_status: 0x%08x\n", ptXcStartStopArea->ulXc_hold_status);
			ulValue = HOSTMSK(xc_hold_status_xc0_hold_tpu1) | HOSTMSK(xc_hold_status_xc0_hold_rpu1);
			if( (ptXcStartStopArea->ulXc_hold_status & ulValue)!=ulValue )
			{
				uprintf("ERROR: XM0_IO pins can not be used as the unit is running.\n");
				iResult = -1;
			}
			else
			{
				/* Set all pins to input. */
				ulValue  = HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio1_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio2_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio3_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio4_oe_wm);
				ulValue |= HOSTMSK(xmac_io_oe_shared0_gpio5_oe_wm);
				ptXc0Xmac1RegsArea->ulXmac_io_oe_shared1 = ulValue;

				/* XM_IO2-5 are shared with the PHY LEDs. Deactivate the LED function if they are in use. */
				if( (ulPins&((1U<<2)|(1U<<3)|(1U<<4)|(1U<<5)))!=0 )
				{
					ulValue  = ptXc0Xmac1RegsArea->ulXmac_config_sbu;
					ulValue &= ~HOSTMSK(xmac_config_sbu_phy_led_en);
					ptXc0Xmac1RegsArea->ulXmac_config_sbu = ulValue;
				}

				/* Activate the XMIO pins in the IO configuration. */
				ulValue  = ptAsicCtrlArea->asIo_config[1].ulConfig;
				ulValue &= ~HOSTMSK(io_config1_sel_xm1_io);
				ulValue |= (ulPins << HOSTSRT(io_config1_sel_xm1_io)) & HOSTMSK(io_config1_sel_xm1_io);
				ulValue |= HOSTMSK(io_config1_sel_xm1_io_wm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asIo_config[1].ulConfig = ulValue;
			}
		}
	}

	return iResult;
}



static int configure_gpio(unsigned long ulPins)
{
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptGpioComArea);
	unsigned long ulValue;
	unsigned long ulIoConfig;
	unsigned long ulIoMask;
	int iResult;


	iResult = 0;

	/* Get the values for GPIO 8-11. */
	ulIoConfig = 0;
	ulIoMask = 0;
	if( (ulPins&(1U<<8U))!=0U )
	{
		/* GPIO8 is used. */
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio8);
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio8_wm);
		ulIoMask = HOSTMSK(io_config2_mask_sel_gpio8);

		/* Set the GPIO to input.
		 * In the GPIO_COM module, this is GPIO0.
		 */
		ptGpioComArea->aulGpio_cfg[0] = 0U;
	}
	if( (ulPins&(1U<<9U))!=0U )
	{
		/* GPIO9 is used. */
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio9);
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio9_wm);
		ulIoMask = HOSTMSK(io_config2_mask_sel_gpio9);

		/* Set the GPIO to input.
		 * In the GPIO_COM module, this is GPIO1.
		 */
		ptGpioComArea->aulGpio_cfg[1] = 0U;
	}
	if( (ulPins&(1U<<10U))!=0U )
	{
		/* GPIO10 is used. */
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio10);
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio10_wm);
		ulIoMask = HOSTMSK(io_config2_mask_sel_gpio10);

		/* Set the GPIO to input.
		 * In the GPIO_COM module, this is GPIO2.
		 */
		ptGpioComArea->aulGpio_cfg[2] = 0U;
	}
	if( (ulPins&(1U<<11U))!=0U )
	{
		/* GPIO11 is used. */
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio11);
		ulIoConfig |= HOSTMSK(io_config2_sel_gpio11_wm);
		ulIoMask = HOSTMSK(io_config2_mask_sel_gpio11);

		/* Set the GPIO to input.
		 * In the GPIO_COM module, this is GPIO3.
		 */
		ptGpioComArea->aulGpio_cfg[3] = 0U;
	}

	/* Can the pins be enabled? */
	ulValue  = ptAsicCtrlArea->asIo_config[2].ulMask;
	ulValue &= ulIoMask;
	if( ulValue!=ulIoMask )
	{
		uprintf("Not all GPIO pins can be enabled!\n");
		iResult = -1;
	}
	else
	{
		ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
		ptAsicCtrlArea->asIo_config[2].ulConfig = ulIoConfig;
	}

	return iResult;
}



static int configure_apppio(unsigned long ulPins)
{
	HOSTDEF(ptAsicCtrlArea);
	int iResult;
	unsigned long ulConfig9;
	unsigned long ulConfig10;
	unsigned long ulMasked9;
	unsigned long ulMasked10;


	s_ulAppPio_Oe = 0;
	s_ulAppPio_Out = 0;

	/* Set all pins to input. */
	iResult = app_bridge_write_register(Adr_NX90_pio_app_pio_oe, 0x00000000);
	if( iResult!=0 )
	{
		uprintf("Failed to write to the APP bridge: %d\n", iResult);
	}
	else
	{
		iResult = app_bridge_write_register(Adr_NX90_pio_app_pio_out, 0x00000000);
		if( iResult!=0 )
		{
			uprintf("Failed to write to the APP bridge: %d\n", iResult);
		}
		else
		{
			ulConfig9  =  ulPins & 0x0000ffffU;
			ulConfig10 = (ulPins & 0xffff0000U) >> 16U;

			ulMasked9  = ptAsicCtrlArea->asIo_config[9].ulMask & ulConfig9;
			ulMasked10 = ptAsicCtrlArea->asIo_config[10].ulMask & ulConfig10;

			/* Can the pins be enabled? */
			if( (ulMasked9!=ulConfig9) || (ulMasked10!=ulConfig10) )
			{
				uprintf("Not all PIOAPP pins can be enabled!\n");
				iResult = -1;
			}
			else
			{
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asIo_config[9].ulConfig = ulConfig9 | 0xffff0000U;

				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asIo_config[10].ulConfig = ulConfig10 | 0xffff0000U;
			}
		}
	}

	return iResult;
}



static int configure_iolledm(unsigned long *pulPins)
{
	int iResult;
	unsigned char ucNodeCnt;
	unsigned long ulPins;
	unsigned short usValue;


	/* Be optimistic. */
	iResult = 0;

	memset(s_aulIolLedM, 0, MAX_NETIOL_NODES*sizeof(unsigned long));

	for(ucNodeCnt=0; ucNodeCnt<MAX_NETIOL_NODES; ++ucNodeCnt)
	{
		/* Get the pins for the netIOL. */
		ulPins = pulPins[ucNodeCnt];
		if( ulPins!=0 )
		{
			/* Setup the IO multiplexer. */
			usValue  =  0U << SRT_NIOL_asic_ctrl_io_config0_sel_uart_d;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config0_sel_spi;
			usValue |=  1U << SRT_NIOL_asic_ctrl_io_config0_sel_hispi;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config0_sel_jtag;
			usValue |= PW_VAL_NIOL_asic_ctrl_io_config0;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_asic_ctrl_asic_ctrl_io_config0, usValue);

			usValue  =  0U << SRT_NIOL_asic_ctrl_io_config1_sel_sync_out_p;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config1_sel_sync_in_p;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config1_sel_irq_ext_p;
			usValue |= PW_VAL_NIOL_asic_ctrl_io_config1;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_asic_ctrl_asic_ctrl_io_config1, usValue);

			usValue  =  7U << SRT_NIOL_asic_ctrl_io_config2_sel_led_c;
			usValue |= 15U << SRT_NIOL_asic_ctrl_io_config2_sel_led_r;
			usValue |= PW_VAL_NIOL_asic_ctrl_io_config2;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_asic_ctrl_asic_ctrl_io_config2, usValue);

			usValue  =  0U << SRT_NIOL_asic_ctrl_io_config3_sel_adc_gpz;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config3_sel_adc_gpo0;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config3_sel_adc_gpo1;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config3_sel_adc_gpo2;
			usValue |=  0U << SRT_NIOL_asic_ctrl_io_config3_sel_adc_gpo3;
			usValue |= PW_VAL_NIOL_asic_ctrl_io_config3;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_asic_ctrl_asic_ctrl_io_config3, usValue);

			/* Set the PAD control register. */
			usValue  = PW_VAL_NIOL_pad_ctrl_led_r0;
			usValue |= MSK_NIOL_pad_ctrl_led_r0_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_r0, usValue);

			usValue  = PW_VAL_NIOL_pad_ctrl_led_r1;
			usValue |= MSK_NIOL_pad_ctrl_led_r1_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_r1, usValue);

			usValue  = PW_VAL_NIOL_pad_ctrl_led_r2;
			usValue |= MSK_NIOL_pad_ctrl_led_r2_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_r2, usValue);

			usValue  = PW_VAL_NIOL_pad_ctrl_led_r3;
			usValue |= MSK_NIOL_pad_ctrl_led_r3_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_r3, usValue);

			usValue  = PW_VAL_NIOL_pad_ctrl_led_c0;
			usValue |= MSK_NIOL_pad_ctrl_led_c0_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_c0, usValue);

			usValue  = PW_VAL_NIOL_pad_ctrl_led_c1;
			usValue |= MSK_NIOL_pad_ctrl_led_c1_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_c1, usValue);

			usValue  = PW_VAL_NIOL_pad_ctrl_led_c2;
			usValue |= MSK_NIOL_pad_ctrl_led_c2_ie;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_pad_ctrl_pad_ctrl_led_c2, usValue);


			/* Disable the LED matrix. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_cfg, DFLT_VAL_NIOL_ledm_cfg);

			/* Set the configuration, but do not enable the module yet. */
			usValue  =  1U << SRT_NIOL_ledm_cfg_precharge_en;
			usValue |=  1U << SRT_NIOL_ledm_cfg_column_adaptive;
			usValue |=  1U << SRT_NIOL_ledm_cfg_bipolar;
			usValue |=  3U << SRT_NIOL_ledm_cfg_column_last;
			usValue |=  0U << SRT_NIOL_ledm_cfg_en;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_cfg, usValue);

			/* Set the prescaler. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_prescaler, 1);

			/* Set the LED matrix time 0. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_t0,  100);
			/* Set the LED matrix time 1. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_t1,   25);
			/* Set the LED matrix time 2. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_t2,   25);
			/* Set the LED matrix time 3. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_t3, 4000);

			/* Enable the LEDM module. */
			usValue  =  1U << SRT_NIOL_ledm_cfg_precharge_en;
			usValue |=  1U << SRT_NIOL_ledm_cfg_column_adaptive;
			usValue |=  1U << SRT_NIOL_ledm_cfg_bipolar;
			usValue |=  3U << SRT_NIOL_ledm_cfg_column_last;
			usValue |=  1U << SRT_NIOL_ledm_cfg_en;
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_cfg, usValue);

			/* Turn off all LEDs. */
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_led15_0_rld, 0x0000);
			app_bridge_module_hispi_writeRegister16(ucNodeCnt, Adr_NIOL_ledm_ledm_led23_16_rld, 0x0000);
		}
	}

	return iResult;
}



int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, unsigned int sizMaxPinDesc)
{
	int iResult;
	HOSTDEF(ptAsicCtrlArea);
	HOSTDEF(ptAsicCtrlComArea);
	HOSTDEF(ptHifIoCtrlArea);
	HOSTDEF(ptMledCtrlComArea);
	HOSTDEF(ptMmioCtrlArea);
	UNITCONFIGURATION_T tUnitCfg;
	unsigned long ulValue;
	unsigned long ulClockEnable;
	int iCnt;
	int iAppBridgeIsInitialized;
	int iHiSpiModuleIsInitialized;


	/* The APP bridge and the modules are not initialized yet. */
	iAppBridgeIsInitialized = 0;
	iHiSpiModuleIsInitialized = 0;

	initialize_unit_configuration(&tUnitCfg);
	iResult = collect_unit_configuration(ptPinDesc, sizMaxPinDesc, &tUnitCfg);
	if( iResult==0 )
	{
		/*
		 * GPIO
		 */
		if( tUnitCfg.ulGpio!=0 )
		{
			configure_gpio(tUnitCfg.ulGpio);
		}

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

				ulClockEnable  = ptAsicCtrlArea->asClock_enable[0].ulEnable;
				ulClockEnable |= HOSTMSK(clock_enable0_dpm);
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptAsicCtrlArea->asClock_enable[0].ulEnable = ulClockEnable;
			}

			ulValue |= 3 << HOSTSRT(hif_io_cfg_hif_mi_cfg);
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptHifIoCtrlArea->ulHif_io_cfg = ulValue;

			ulValue  = 1 << HOSTSRT(hif_pio_cfg_in_ctrl);
			ptHifIoCtrlArea->ulHif_pio_cfg = ulValue;
		}

		/*
		 * MLED
		 */
		if( tUnitCfg.ulMled!=0 )
		{
			for(iCnt=0; iCnt<8; ++iCnt)
			{
				ulValue = 0;
				if( (tUnitCfg.ulMled & (1U << iCnt))!=0 )
				{
					/* Use the line register. */
					ulValue = 2;
				}
				ptMledCtrlComArea->aulMled_ctrl_output_sel[iCnt] = ulValue;
			}

			ulValue  = HOSTDFLT(mled_ctrl_cfg);
			ulValue |= HOSTMSK(mled_ctrl_cfg_enable);
			ptMledCtrlComArea->ulMled_ctrl_cfg = ulValue;
		}

		/*
		 *  MMIO
		 *  Configure all MMIO pins as PIO/input.
		 */
		if( tUnitCfg.ulMmio!=0 )
		{
			for(iCnt=0; iCnt<8; ++iCnt)
			{
				ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
				ptMmioCtrlArea->aulMmio_cfg[iCnt] = HOSTMMIO(PIO);
			}
		}

		/*
		 * RdyRun
		 */
		if( tUnitCfg.ulRdyRun!=0 )
		{
			/* Disable the RDY and RUN driver. */
			ulValue  = ptAsicCtrlComArea->ulRdy_run_cfg;
			ulValue &= ~HOSTMSK(rdy_run_cfg_RDY_DRV);
			ulValue &= ~HOSTMSK(rdy_run_cfg_RUN_DRV);
			ptAsicCtrlComArea->ulRdy_run_cfg = ulValue;
		}

		/*
		 * RstOut
		 */
		if( tUnitCfg.ulRstOut!=0 )
		{
			/* Disable the reset out driver. */
			ulValue  = ptAsicCtrlComArea->ulReset_ctrl;
			ulValue &= ~(HOSTMSK(reset_ctrl_EN_RES_REQ_OUT) | HOSTMSK(reset_ctrl_RES_REQ_OUT));
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptAsicCtrlComArea->ulReset_ctrl = ulValue;
		}

		/*
		 * XMIOs
		 */
		if( tUnitCfg.aulXmio[0]!=0 )
		{
			iResult = configure_xm0io(tUnitCfg.aulXmio[0]);
		}
		if( tUnitCfg.aulXmio[1]!=0 )
		{
			iResult = configure_xm1io(tUnitCfg.aulXmio[1]);
		}

		/*
		 * APP PIOs
		 */
		if( tUnitCfg.ulPioApp!=0 )
		{
			if( iAppBridgeIsInitialized==0 )
			{
				iResult = app_bridge_init();
				if( iResult!=0 )
				{
					uprintf("Failed to initialize the APP bridge: %d\n", iResult);
				}
				else
				{
					iAppBridgeIsInitialized = 1;
				}
			}
			if( iResult==0 )
			{
				iResult = configure_apppio(tUnitCfg.ulPioApp);
			}
		}

		/*
		 * IOL LED matrix
		 */
		ulValue  = tUnitCfg.aulIolLedM[0];
		ulValue |= tUnitCfg.aulIolLedM[1];
		ulValue |= tUnitCfg.aulIolLedM[2];
		ulValue |= tUnitCfg.aulIolLedM[3];
		if( ulValue!=0 )
		{
			if( iAppBridgeIsInitialized==0 )
			{
				iResult = app_bridge_init();
				if( iResult!=0 )
				{
					uprintf("Failed to initialize the APP bridge: %d\n", iResult);
				}
				else
				{
					iAppBridgeIsInitialized = 1;
				}
			}
			if( iResult==0 && iHiSpiModuleIsInitialized==0 )
			{
				/* Get the number of netIOLs in use. */
				ulValue = 4;
				if( tUnitCfg.aulIolLedM[3]==0U )
				{
					ulValue = 3;
					if( tUnitCfg.aulIolLedM[2]==0U )
					{
						ulValue = 2;
						if( tUnitCfg.aulIolLedM[1]==0U )
						{
							ulValue = 1;
						}
					}
				}

				iResult = app_bridge_module_hispi_initialize(ulValue);
				if( iResult!=0 )
				{
					uprintf("Failed to initialize the HiSPI module: %d\n", iResult);
				}
				else
				{
					iHiSpiModuleIsInitialized = 1;
				}
			}

			if( iResult==0 )
			{
				iResult = configure_iolledm(tUnitCfg.aulIolLedM);
			}
		}
	}

	return iResult;
}


/*---------------------------------------------------------------------------*/


static int set_gpio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptGpioComArea);
	int iResult;
	unsigned long ulValue;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex>7U && uiIndex<12U )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit for the pin. */
			ulValue = 0;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit and the output bit to 0. */
			ulValue = 4U;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit and the output bit to 1. */
			ulValue = 5U;
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptGpioComArea->aulGpio_cfg[uiIndex-8] = ulValue;
		}
	}

	return iResult;
}



static int get_gpio(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptGpioComArea);
	int iResult;
	unsigned long ulValue;
	unsigned char ucData;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex>7U && uiIndex<12U )
	{
		ulValue  = ptGpioComArea->ulGpio_in;
		ulValue &= 1U << (uiIndex-8U);
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


static int get_hifpio(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptHifIoCtrlArea);
	unsigned long aulMsk[2];
	unsigned long aulValue[2];
	unsigned char ucData;
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



static int set_mled(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptMledCtrlComArea);
	unsigned long ulMask;
	unsigned long ulValue;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<8 )
	{
		ulMask = 1U << uiIndex;

		/* Set the bit in the line register. */
		ulValue  = ptMledCtrlComArea->ulMled_ctrl_line0;
		switch(tValue)
		{
		case PINSTATUS_HIGHZ:
			/* MLED pins can not be set to Z. */
			uprintf("Trying to set an MLED pin to Z, which is not possible.\n");
			break;

		case PINSTATUS_OUTPUT0:
			ulValue &= ~ulMask;
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			ulValue |= ulMask;
			iResult = 0;
			break;
		}
		if( iResult==0 )
		{
			ptMledCtrlComArea->ulMled_ctrl_line0 = ulValue;
		}
	}
	else
	{
		uprintf("Invalid index for MLED pins: %d, but only 0-7 possible.\n");
	}

	return iResult;
}



static int get_mled(unsigned int uiIndex, unsigned char *pucData __attribute__((unused)))
{
	if( uiIndex<8 )
	{
		/* MLED pins are output only. */
		uprintf("Trying to get an MLED pin, which is read-only.\n");
	}
	else
	{
		uprintf("Invalid index for MLED pins: %d, but only 0-7 possible. And a 'get' would not work with MLED pins anyway.\n");
	}

	return -1;
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


static int get_mmiopio(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptMmioCtrlArea);
	unsigned long ulValue;
	unsigned char ucData;

	/* assume failure */
	iResult = -1;
	
	/* check the index */
	if( uiIndex<8 )
	{
		ulValue = ptMmioCtrlArea->ulMmio_in_line_status0;
		ulValue &= 1U << uiIndex;
		if (ulValue == 0)
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
	else
	{
		uprintf("Invalid MMIO index: %d\n", uiIndex);
	}

	return iResult;
}



static int set_rdyrun(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	HOSTDEF(ptAsicCtrlComArea);
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

		ulValue = ptAsicCtrlComArea->ulRdy_run_cfg;

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
			ptAsicCtrlComArea->ulRdy_run_cfg = ulValue;
		}
	}

	return iResult;
}


static int get_rdyrun(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	HOSTDEF(ptAsicCtrlComArea);
	unsigned long ulValue;
	unsigned char ucData;


	/* assume failure */
	iResult = -1;

	/* check the index */
	if( uiIndex<2 )
	{
		ulValue = ptAsicCtrlComArea->ulRdy_run_cfg;
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



static int set_rstout(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptAsicCtrlComArea);
	HOSTDEF(ptAsicCtrlArea);
	unsigned long ulValue;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex==0U )
	{
		ulValue  = ptAsicCtrlComArea->ulReset_ctrl;

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
			ptAsicCtrlArea->ulAsic_ctrl_access_key = ptAsicCtrlArea->ulAsic_ctrl_access_key;  /* @suppress("Assignment to itself") */
			ptAsicCtrlComArea->ulReset_ctrl = ulValue;
		}
	}

	return iResult;
}



static int get_rstout(unsigned int uiIndex __attribute__ ((unused)), unsigned char *pucData __attribute__ ((unused)))
{
	/* The RstOut pin is output only. */
	uprintf("The RSTOUT pin can not be used as an input!\n");
	return -1;
}



static int set_xm0io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXc0Xmac0RegsArea);
	unsigned long ulOut;
	unsigned long ulOe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<6U )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared0_gpio0_out_wm) << uiIndex;

			/* Clear the output enable bit. */
			ulOe   = 0;
			ulOe  |= HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared0_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared0_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the out bit. */
			ulOut  = HOSTMSK(xmac_config_shared0_gpio0_out) << uiIndex;
			ulOut |= HOSTMSK(xmac_config_shared0_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared0_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared0_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXc0Xmac0RegsArea->ulXmac_config_shared0 = ulOut;
			ptXc0Xmac0RegsArea->ulXmac_io_oe_shared0  = ulOe;
		}
	}
	else
	{
		uprintf("Invalid index for XM1IO: %d\n", uiIndex);
	}

	return iResult;
}



static int set_xm1io(unsigned int uiIndex, PINSTATUS_T tValue)
{
	HOSTDEF(ptXc0Xmac1RegsArea);
	unsigned long ulOut;
	unsigned long ulOe;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;

	if( uiIndex<6U )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared1_gpio0_out_wm) << uiIndex;

			/* Clear the output enable bit. */
			ulOe   = 0;
			ulOe  |= HOSTMSK(xmac_io_oe_shared1_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Clear the out bit. */
			ulOut  = 0;
			ulOut |= HOSTMSK(xmac_config_shared1_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared1_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared1_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the out bit. */
			ulOut  = HOSTMSK(xmac_config_shared1_gpio0_out) << uiIndex;
			ulOut |= HOSTMSK(xmac_config_shared1_gpio0_out_wm) << uiIndex;

			/* Set the output enable bit. */
			ulOe   = HOSTMSK(xmac_io_oe_shared1_gpio0_oe) << uiIndex;
			ulOe  |= HOSTMSK(xmac_io_oe_shared1_gpio0_oe_wm) << uiIndex;

			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ptXc0Xmac1RegsArea->ulXmac_config_shared1 = ulOut;
			ptXc0Xmac1RegsArea->ulXmac_io_oe_shared1  = ulOe;
		}
	}
	else
	{
		uprintf("Invalid index for XM1IO: %d\n", uiIndex);
	}

	return iResult;
}



static int get_xm0io(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXc0Xmac0RegsArea);
	unsigned long ulValue;
	unsigned char ucData;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;
	if( uiIndex<6U )
	{
		ulValue  = ptXc0Xmac0RegsArea->ulXmac_status_shared0;
		ulValue &= HOSTMSK(xmac_status_shared0_gpio0_in) << uiIndex;
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



static int get_xm1io(unsigned int uiIndex, unsigned char *pucData)
{
	HOSTDEF(ptXc0Xmac1RegsArea);
	unsigned long ulValue;
	unsigned char ucData;
	int iResult;


	/* Be pessimistic. */
	iResult = -1;
	if( uiIndex<6U )
	{
		ulValue  = ptXc0Xmac1RegsArea->ulXmac_status_shared1;
		ulValue &= HOSTMSK(xmac_status_shared0_gpio0_in) << uiIndex;
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



static int set_apppio(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;

	/* Be pessimistic... */
	iResult = -1;

	/* check the index */
	if( uiIndex<32U )
	{
		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			/* Clear the output enable bit for the pin. */
			s_ulAppPio_Oe &= ~(1U << uiIndex);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT0:
			/* Set the output enable bit and the output bit to 0. */
			s_ulAppPio_Oe |= 1U << uiIndex;
			s_ulAppPio_Out &= ~(1U << uiIndex);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			/* Set the output enable bit and the output bit to 1. */
			s_ulAppPio_Oe |= 1U << uiIndex;
			s_ulAppPio_Out |= 1U << uiIndex;
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			/* Set all pins to input. */
			iResult = app_bridge_write_register(Adr_NX90_pio_app_pio_oe, s_ulAppPio_Oe);
			if( iResult!=0 )
			{
				uprintf("Failed to write to the APP bridge: %d\n", iResult);
			}
			else
			{
				iResult = app_bridge_write_register(Adr_NX90_pio_app_pio_out, s_ulAppPio_Out);
			}
		}
	}

	return iResult;

}



static int get_apppio(unsigned int uiIndex, unsigned char *pucData)
{
	int iResult;
	unsigned long ulValue;
	unsigned char ucData;


	/* Be pessimistic... */
	iResult = -1;

	if( uiIndex<32 )
	{
		iResult = app_bridge_read_register(Adr_NX90_pio_app_pio_in, &ulValue);
		if( iResult!=0 )
		{
			uprintf("Failed to read from the APP bridge: %d\n", iResult);
		}
		else
		{
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
		}
	}

	return iResult;
}



static int set_iolledm(unsigned int uiIndex, PINSTATUS_T tValue)
{
	int iResult;
	unsigned char ucNetIolIndex;
	unsigned int uiLedIndex;
	unsigned long ulValue;
	unsigned short usValue;


	/* Be pessimistic... */
	iResult = -1;

	/* check the index */
	if( uiIndex<24*MAX_NETIOL_NODES )
	{
		ucNetIolIndex = (unsigned char)(uiIndex / 24);
		uiLedIndex = uiIndex % 24;

		switch( tValue )
		{
		case PINSTATUS_HIGHZ:
			uprintf("Trying to set a LEDM pin to input.\n");
			break;

		case PINSTATUS_OUTPUT0:
			s_aulIolLedM[ucNetIolIndex] &= ~(1U << uiLedIndex);
			iResult = 0;
			break;

		case PINSTATUS_OUTPUT1:
			s_aulIolLedM[ucNetIolIndex] |= 1U << uiLedIndex;
			iResult = 0;
			break;
		}

		if( iResult==0 )
		{
			ulValue = s_aulIolLedM[ucNetIolIndex];
			usValue = (unsigned short)(ulValue & 0x0000ffffU);
			app_bridge_module_hispi_writeRegister16(ucNetIolIndex, Adr_NIOL_ledm_ledm_led15_0_rld, usValue);
			usValue = (unsigned short)((ulValue >> 16) & 0x0000ffffU);
			app_bridge_module_hispi_writeRegister16(ucNetIolIndex, Adr_NIOL_ledm_ledm_led23_16_rld, usValue);
		}
	}

	return iResult;

}



static int get_iolledm(unsigned int uiIndex __attribute__((unused)), unsigned char *pucData __attribute__((unused)))
{
	/* The LEDM pins can not be read. */
	uprintf("The LEDM pins can not be read.\n");
	return -1;
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
		iResult = set_gpio(uiIndex, tValue);
		break;

	case PINTYPE_HIFPIO:
		iResult = set_hifpio(uiIndex, tValue);
		break;

	case PINTYPE_MLED:
		iResult = set_mled(uiIndex, tValue);
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
		if( uiIndex<6 )
		{
			iResult = set_xm0io(uiIndex, tValue);
		}
		else if( uiIndex<12 )
		{
			iResult = set_xm1io(uiIndex-6U, tValue);
		}
		break;

	case PINTYPE_RAPGPIO:
		uprintf("The pin type RAPGPIO is not supported on this platform!\n");
		break;

	case PINTYPE_APPPIO:
		iResult = set_apppio(uiIndex, tValue);
		break;

	case PINTYPE_IOLLEDM:
		iResult = set_iolledm(uiIndex, tValue);
		break;
	}

	return iResult;
}


int iopins_get(const PINDESCRIPTION_T *ptPinDescription, unsigned char *pucData)
{
	int iResult;
	unsigned int uiIndex;


	uiIndex = ptPinDescription->uiIndex;

	iResult = -1;
	switch( ptPinDescription->tType )
	{
	case PINTYPE_GPIO:
		iResult = get_gpio(uiIndex, pucData);
		break;

	case PINTYPE_HIFPIO:
		iResult = get_hifpio(uiIndex, pucData);
		break;

	case PINTYPE_MLED:
		iResult = get_mled(uiIndex, pucData);
		break;

	case PINTYPE_MMIO:
		iResult = get_mmiopio(uiIndex, pucData);
		break;

	case PINTYPE_PIO:
		/* Not yet... */
		break;

	case PINTYPE_RDYRUN:
		iResult = get_rdyrun(uiIndex, pucData);
		break;

	case PINTYPE_RSTOUT:
		iResult = get_rstout(uiIndex, pucData);
		break;

	case PINTYPE_XMIO:
		if( uiIndex<6 )
		{
			iResult = get_xm0io(uiIndex, pucData);
		}
		else if( uiIndex<12 )
		{
			iResult = get_xm1io(uiIndex-6U, pucData);
		}
		break;

	case PINTYPE_RAPGPIO:
		uprintf("The pin type RAPGPIO is not supported on this platform!\n");
		break;

	case PINTYPE_APPPIO:
		iResult = get_apppio(uiIndex, pucData);
		break;

	case PINTYPE_IOLLEDM:
		iResult = get_iolledm(uiIndex, pucData);
		break;
	}

	return iResult;
}
