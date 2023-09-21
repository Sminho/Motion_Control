
#pragma once

/* 일부 Position Table 구현에 필요한 부분빼고 일단 삭제*/
typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned short WORD;


//------------------------------------------------------------------
//                 POSITION TABLE Defines.
//------------------------------------------------------------------

static const WORD	MAX_LOOP_COUNT	= 100;
static const WORD	MAX_WAIT_TIME	= 60000;

static const WORD	PUSH_RATIO_MIN	= 20;
static const WORD	PUSH_RATIO_MAX	= 90;

static const DWORD	PUSH_SPEED_MIN	= 1;
static const DWORD	PUSH_SPEED_MAX	= 100000;

static const DWORD	PUSH_PULSECOUNT_MIN	= 1;
static const DWORD	PUSH_PULSECOUNT_MAX	= 10000;

typedef enum
{
	CMD_ABS_LOWSPEED = 0,
	CMD_ABS_HIGHSPEED,
	CMD_ABS_HIGHSPEEDDECEL,
	CMD_ABS_NORMALMOTION,
	CMD_INC_LOWSPEED,
	CMD_INC_HIGHSPEED,
	CMD_INC_HIGHSPEEDDECEL,
	CMD_INC_NORMALMOTION,
	CMD_MOVE_ORIGIN,
	CMD_COUNTERCLEAR,
	CMD_PUSH_ABSMOTION,
	CMD_STOP,

	CMD_MAX_COUNT,

	CMD_NO_COMMAND = 0xFFFF,
} COMMAND_LIST;

#ifndef	DEFINE_ITEM_NODE
#define DEFINE_ITEM_NODE

#pragma pack(2)

typedef union
{
	WORD	wBuffer[32];		// 64 bytes

	struct
	{
		LONG	lPosition;

		DWORD	dwStartSpd;
		DWORD	dwMoveSpd;

		WORD	wAccelRate;
		WORD	wDecelRate;

		WORD	wCommand;
		WORD	wWaitTime;
		WORD	wContinuous;
		WORD	wBranch;

		WORD	wCond_branch0;
		WORD	wCond_branch1;
		WORD	wCond_branch2;
		WORD	wLoopCount;
		WORD	wBranchAfterLoop;
		WORD	wPTSet;
		WORD	wLoopCountCLR;

		WORD	bCheckInpos;		// 0 : Check Inpos, 1 : Don't Check.

		LONG	lTriggerPos;
		WORD	wTriggerOnTime;

		WORD	wPushRatio;
		DWORD	dwPushSpeed;
		LONG	lPushPosition;
		WORD	wPushMode;
	};
} ITEM_NODE, *LPITEM_NODE;

#pragma pack()

#endif


static const WORD	OFFSET_POSITION		= 0;
static const WORD	OFFSET_LOWSPEED		= 4;
static const WORD	OFFSET_HIGHSPEED	= 8;
static const WORD	OFFSET_ACCELTIME	= 12;
static const WORD	OFFSET_DECELTIME	= 14;
static const WORD	OFFSET_COMMAND		= 16;
static const WORD	OFFSET_WAITTIME		= 18;
static const WORD	OFFSET_CONTINUOUS	= 20;
static const WORD	OFFSET_JUMPTABLENO	= 22;
static const WORD	OFFSET_JUMPPT0		= 24;
static const WORD	OFFSET_JUMPPT1		= 26;
static const WORD	OFFSET_JUMPPT2		= 28;
static const WORD	OFFSET_LOOPCOUNT		= 30;
static const WORD	OFFSET_LOOPJUMPTABLENO	= 32;
static const WORD	OFFSET_PTSET			= 34;
static const WORD	OFFSET_LOOPCOUNTCLEAR	= 36;
static const WORD	OFFSET_CHECKINPOSITION	= 38;
static const WORD	OFFSET_TRIGGERPOSITION	= 40;
static const WORD	OFFSET_TRIGGERONTIME	= 44;
static const WORD	OFFSET_PUSHRATIO		= 46;
static const WORD	OFFSET_PUSHSPEED		= 48;
static const WORD	OFFSET_PUSHPOSITION		= 52;
static const WORD	OFFSET_PUSHMODE			= 56;

static const WORD	OFFSET_BLANK			= 58;


