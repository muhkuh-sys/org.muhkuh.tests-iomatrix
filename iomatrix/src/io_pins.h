
#ifndef __IO_PINS_H__
#define __IO_PINS_H__


#include <sys/types.h>

typedef enum PINTYPE_ENUM
{
	PINTYPE_GPIO    = 0,
	PINTYPE_PIO     = 1,
	PINTYPE_MMIO    = 2,
	PINTYPE_HIFPIO  = 3,
	PINTYPE_RSTOUT  = 4
} PINTYPE_T;



typedef enum PINSTATUS_ENUM
{
	PINSTATUS_HIGHZ    = 0,
	PINSTATUS_INPUT    = 1,
	PINSTATUS_OUTPUT0  = 2,
	PINSTATUS_OUTPUT1  = 3
} PINSTATUS_T;


typedef enum PINFLAG_ENUM
{
	PINFLAG_I          = 1,  /* The pin can be switched to input mode. */
	PINFLAG_O          = 2,  /* The pin can be switched to output mode. */
	PINFLAG_Z          = 4,  /* The pin can be switched to high-Z mode. */
	PINFLAG_IOZ        = 7,  /* A shortcut for PINFLAG_I|PINFLAG_O|PINFLAG_Z. */
} PINFLAG_ENUM;


typedef struct PINDESCRIPTION_STRUCT
{
	const char *pcName;
	PINTYPE_T tType;
	unsigned int uiIndex;
	unsigned int uiDefaultValue;
	unsigned long ulFlags;
} PINDESCRIPTION_T;


int iopins_configure(const PINDESCRIPTION_T *ptPinDesc, size_t sizMaxPinDesc);
int iopins_set(const PINDESCRIPTION_T *ptPinDescription, PINSTATUS_T tValue);
int iopins_get(const PINDESCRIPTION_T *ptPinDescription, unsigned int *puiValue);

#endif  /* __IO_PINS_H__ */

