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
// UTrace Performance Tests UTrace Call enums
//



/**
 @file te_instrumentation_enum.h
 @internalTechnology
 @prototype
*/

#ifndef TE_UPTUTRACEENUM_H
#define TE_UPTUTRACEENUM_H

#include <opensystemtrace.h>


/**
This is a user-side utility class to enum the various available UTrace calls

@internalTechnology
@prototype
 */

enum TUPTApiUsed
	{
	UPTNone = 0,

//User side

//Normal
	//printf
	EUptPrintTDesC8_big,
	EUptPrintTDesC8_huge,

	EUptPrintTDesC16_big,
	EUptPrintTDesC16_huge,

	EUptPrintfchar,//5
	EUptPrintfTRefByValueTDesC8,
	EUptPrintfTRefByValueTDesC16,
	//trace
	EUptTrace,
	EUptTraceTUint32,
	EUptTraceTUint32TUint32,//10
	EUptTraceTAnySmallStruct,
	EUptTraceTAnyBigStruct,
	EUptTraceTAnyHugeStruct,
	EUptTraceTemplatedSmallStruct,
	EUptTraceTemplatedBigStruct,//15
	EUptTraceTemplatedHugeStruct,


	// trace descriptor
	EUptTraceTDesC8_big,
	EUptTraceTDesC16_big,
	EUptTraceTDesC8_huge,
	EUptTraceTDesC16_huge, //20

    // ost traces
    EOstTraceDef0,
    EOstTraceDef1,
    EOstTraceDefData,
    EOstTraceDefExt1,
    EOstTraceDefExt2,//25
    EOstTraceDefExt3,
    EOstTraceDefExt4,
    EOstTraceDefExt5,
    EOstTrace0,
    EOstTrace1,//30
    EOstTraceData,
    EOstTraceExt1,
    EOstTraceExt2,
    EOstTraceExt3,
    EOstTraceExt4,//35
    EOstTraceExt5,
    EOstTraceFunctionEntry0,
    EOstTraceFunctionEntry1,
    EOstTraceFunctionEntryExt,
    EOstTraceFunctionEntryExtCtorInitList,//40
    EOstTraceFunctionEntryExtCtorNoInitList,
    EOstTraceFunctionExit0,
    EOstTraceFunctionExit1,
    EOstTraceFunctionExitExt,
    EOstTraceEventStart0,//45
    EOstTraceEventStart1,
    EOstTraceState0,
    EOstTraceState1,
    EOstTraceEventStop,
    EOstTraceData96,//50
    EOstTraceData1000,
    EUptIsTraceActive,

//kernel side
	//printf
	EUptKernelPrintfchar,
	EUptKernelPrintfTRefByValueTDesC8, 
	EUptKernelPrintfDesc8_big,//55
	EUptKernelPrintfDesc8_huge,
	//trace
	EUptKernelTrace,
	EUptKernelTraceTUint32,
	EUptKernelTraceTUint32TUint32,
	EUptKernelTraceTAnySmallStruct,//60
	EUptKernelTraceTAnyBigStruct,
	EUptKernelTraceTAnyHugeStruct,
	EUptKernelTraceTemplatedSmallStruct,
	EUptKernelTraceTemplatedBigStruct,
	EUptKernelTraceTemplatedHugeStruct,//65
	EUptKernelIsTraceActive,
	EUptKernelTraceDesc8_big,
	EUptKernelTraceDesc8_huge, 


//test
	/**
	 * Used to verify that the figures we get out are ok.
	 * Minimum execution (cached) time roughly: 2938ns
	 * This time should be equivalent to the number produced by
	 * Typical execution (non-cached) time roghly: 2773ns
	 * Stackbase: 400 byte
	 */
	UPTTraceTest,
	UPTKernelTraceTest,//70
	UPTKernelNegativeFunctionalityTest,
	UPTKernelMultipleFunctionalityTest,
//btrace
	BTraceFiltered4,//equivalent of EUptTrace
	BTraceFiltered8,// equivalent of EUptTraceTUint32
	BTraceFiltered12,//75 // equivalent of EUptTraceTUint32TUint32
	BTraceFilteredBig,//equivalent of EUptTraceTemplatedHugeStruct
	BTrace0,
	BTrace4,
	BTrace8,
	BTrace12,//80

//baseline
	UPTBaseline,
	UPTKernelBaseline,

//foo definitions
	UPTFooUserTime,
	UPTFooKernelTime, 
	};

static TInt KUPTMaxNumberOfApis = 48;
static TInt KUPTMaxNormalApi = 20;

#endif
