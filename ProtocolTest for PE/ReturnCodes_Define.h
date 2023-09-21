
#pragma once

#ifndef FMM_RETURN_CODE
#define FMM_RETURN_CODE

//------------------------------------------------------------------
//                 Return Code Defines.
//------------------------------------------------------------------
typedef enum _FMM_ERROR
{
	FMM_OK = 0,

	FMM_NOT_OPEN,
	FMM_INVALID_PORT_NUM,
	FMM_INVALID_SLAVE_NUM,

	FMC_DISCONNECTED = 5,
	FMC_TIMEOUT_ERROR,
	FMC_CRCFAILED_ERROR,
	FMC_RECVPACKET_ERROR,	// PACKET SIZE ERROR

	FMM_POSTABLE_ERROR,

	FMP_FRAMETYPEERROR = 0x80,
	FMP_DATAERROR,
	FMP_PACKETERROR,
	FMP_RUNFAIL = 0x85,
	FMP_RESETFAIL,
	FMP_SERVOONFAIL1,
	FMP_SERVOONFAIL2,
	FMP_SERVOONFAIL3,
	FMP_SERVOOFF_FAIL,
	FMP_ROMACCESS,

	FMP_PACKETCRCERROR = 0xAA,

	FMM_UNKNOWN_ERROR = 0xFF,

} FMM_ERROR;

#endif	//FMM_RETURN_CODE
