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

#include "t_cenrep_helper.h"
#include "srvPerf.h"
#include "srvreqs.h"
#include <centralrepository.h>
#include <e32property.h>
#include <e32test.h>
#include <f32file.h>
#include <e32std.h>
#include <bautils.h>
#include <hal.h>
#include "e32math.h"

LOCAL_D RTest TheTest(_L("Central Repository Rom Flash Performance Test"));

using namespace NCentralRepositoryConstants;
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
//Test macroses and functions


LOCAL_C void CheckL(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		CleanupCDriveL();
		TheTest(EFalse, aLine);
		}
	}

#define TEST(arg) ::CheckL((arg), __LINE__)

// Function DecodeEventAndData
// Extract the high 8-bit event Id and the low 24-bit data
// from the 32 bit integer.
//
// param aEntry - the 32 bit integer to decode
// param aEventId - return the EventId Id.
// param aData - return the 24-bit data value
LOCAL_C void DecodeEventAndData(TUint32 aEntry, TUint& aEventId, TUint32& aData)
	{
	aEventId = (aEntry >> KEventIdShiftBits) & KEventIdMask;
	aData = aEntry & KDataMask;
	}

//Prints aTime parameter (converted to ms)
LOCAL_C	void PrintTiming(TUint32 aTime)
	{
	TInt freq = 0;
	TEST(HAL::Get(HAL::EFastCounterFrequency, freq) == KErrNone);
	const TInt KMicroSecIn1Sec = 1000000;
	const TInt KMsIn1Sec = 1000;

	double v = ((double)aTime * KMicroSecIn1Sec) / (double)freq; TInt v2 = (TInt)v;
	RDebug::Print(_L("####Execution time: %d ms\r\n"), v2 / KMsIn1Sec);
	}

///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4016
@SYMTestCaseDesc		Time the startup impact for 30 repositories changes and updates. 
@SYMTestActions		  	Part 2 of Server startup timing test, retrieves the recorded startup time of the Server Intialisation. Part 1 sets up the modified repositories.
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTREP-CT-4017
@SYMTestCaseDesc		Time the startup impact for 1 repository change and update. 
@SYMTestActions		  	Part 2 of Server startup timing test, retrieves the recorded startup time of the Server Intialisation. Part 1 sets up the modified repositories.
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4018
@SYMTestCaseDesc		Time the startup impact for 10 repositories changes and updates. 
@SYMTestActions		  	Part 2 of Server startup timing test, retrieves the recorded startup time of the Server Intialisation. Part 1 sets up the modified repositories.
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/	
///////////////////////////////////////////////////////////////////////////////////////	
/**
@SYMTestCaseID			SYSLIB-CENTRALREPOSITORY-CT-4019
@SYMTestCaseDesc		Time the startup impact for 20 repositories changes and updates. 
@SYMTestActions		  	Part 2 of Server startup timing test, retrieves the recorded startup time of the Server Intialisation. Part 1 sets up the modified repositories.
@SYMTestExpectedResults 	The server Intialisation time is retrieved.
@SYMTestPriority    Medium
@SYMCR				CR1198
*/	
LOCAL_C void TestCenrepRomflashing013L()
	{
	const TUid KUidRomRepository = { 0x90011116 };
	const TUint32 NewSetting = 0x3010500;	
        TInt settingValue = 0;

	CRepository* repository;
	User::LeaveIfNull(repository = CRepository::NewLC(KUidRomRepository));
	TInt r = repository->Get(NewSetting, settingValue);
	TEST(r == KErrNone);

	// Setup IPC args to retrieve performance data from server.
	TUint bufSize = KCentRepPerfTestArraySize * sizeof(TUint32);
	TAny* buf = User::AllocL(bufSize);
	TPtr8 bufDesc(static_cast<TUint8*>(buf), bufSize);

	TUint numValidEntries;
	TPckg<TUint> pckg(numValidEntries);

	TInt ret = SetGetParameters(TIpcArgs(EGetPerfResults, &bufDesc, &pckg));
	if (ret != KErrNone)
		{
		User::Free(buf);
		return;
		}

	CleanupStack::PopAndDestroy(); // repository
	
	RArray<TUint32> perfData(sizeof(TUint32),static_cast<TUint32*>(buf),KCentRepPerfTestArraySize);
	CleanupClosePushL(perfData);

	TUint eventId;
	TUint32 ticksToInitialiseServer;
	TInt ii = 0;
	// This entry is elapsed ticks to service the request
	DecodeEventAndData(perfData[ii++], eventId, ticksToInitialiseServer);
	RDebug::Print(_L("Event[37]: = [%d] \r\n"), eventId);
	PrintTiming(ticksToInitialiseServer);
	perfData.Reset();
	perfData.Close();
	CleanupStack::PopAndDestroy(); // perfData
	}


LOCAL_C void FuncTestsL()
	{ 
        TheTest.Start(_L("Centralrepository -> SYSLIB-CENTREP-CT-4016-19"));	
	TestCenrepRomflashing013L();
	TheTest.End();
	}

LOCAL_C void MainL()
	{

	FuncTestsL();

	CleanupCDriveL();

	}

TInt E32Main()
	{	
	__UHEAP_MARK;
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	TheTest.Title();
	TheTest.Start(_L("FOTx Server Intialisation Timing Test"));
	
	TRAPD(err, MainL());
	if (err != KErrNone)
		User::Panic(_L("Testing failed: "), err);
	TheTest.End();
	TheTest.Close();
	delete cleanup;
	__UHEAP_MARKEND;
	return 0;
	}
