// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



/**
 @file te_traceglobals.h
 @internalTechnology
*/
#ifndef TE_TRACEGLOBALS_H_
#define TE_TRACEGLOBALS_H_
#include "utracemodutracetest.h"

struct TTemplated4ByteAligned
	{
	TUint8	iData1; //1 byte
	TUint8	iData2; //1 byte
	TInt16	iData3; //2 byte
	TUint32	iData4; //4 byte
	TReal	iData5; //4 byte
	};

static const TInt 		KInfoTraceNotOutput = 1;
static const TInt16 	KMaxSizeOfData = 128;//0x100
static const TInt 		KMaxNumberOfParams = 2;
static const TUint32 	KTimerDuration = 1000000;//in microseconds
static const TUint32 	KMicroToNanoSecondConversion = 1000;
static const TInt 		KNumberOfTraceCallIterations = 1;

static const TGroupId	KGroupId = 187;
static const THasThreadIdentification		KContextId = EAddThreadIdentification;
static const THasProgramCounter	KPc= ENoProgramCounter;
static const TTraceId			KTraceId = 0;

static const TComponentId 		KComponentIdDefaulted = 6666;
//static const TComponentId 		KComponentId = TE_MODULE_UID;
static const TComponentId 		KComponentId = 0;

//different payloads
static const TUint32 	KData1 = 4444;
static const TUint32 	KData2 = 5555;
static const TUint16 	KAny1 = 6666;
static const char 		KCharString[KMaxSizeOfData] = "A char string";
static const TInt 		KCharStringSize = sizeof("A char string");
const static TTemplated4ByteAligned KStructForTemplated = {111,222,333,444,555};

static const TUint32 	KRawData[] =            
{
0x50,0x4b,0x03,0x04,0x14,0x00,0x02,0x00,0x08,0x00,0x36,0xaa,0x50,0x33,0xaf,0x61,0xac,0x5a,0x7a,0x03,0x00,0x00,0xd0,0x03,0x00,0x00,0x06,0x00,0x00,0x00,0x78,0x78,0x2e,0x74,0x69,0x66,0xf3,0xf4,0xd4,0x62,0xf8,0xc3,0xc4,0xc0,0xa0,0xb6,0xa0,0x22,0x53,0xf0,0x44,0x1f,0xbf,0x42,0x45,0x1a,0x1f,0x87,0x40,0x9f,0xf2,0x04,0x1b,0xa3,0xe3,0x9a,0xa7,0xe4,0x05,0x2e,0x3a,0x3f,0xe1,0x67,0x66,0x9e,0x62,0xe5,0xd3,0x27,0x6d,0x37,0x41,0xc1,0x79,0x42,0x4c,0xd1,0x9c,0x85,0x27,0xf4,0x2c,0x9d,0x6f,0x6c
};

_LIT8(KDefault8, "Default8");
_LIT8(KAny8, "TAny8 string");
_LIT8(KDesCFormatted8,"A _LIT8 string with integer parameter %i !");
#ifndef __KERNEL_MODE__
_LIT(KDefault16, "Default16");
_LIT(KAny16, "TAny16 string");
_LIT(KDesCFormatted16,"_LIT16 parameter %i !");
#endif //__KERNEL_MODE__


#endif /*TE_TRACEGLOBALS_H_*/

