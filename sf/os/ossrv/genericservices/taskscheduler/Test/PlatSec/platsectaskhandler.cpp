// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <schinfo.h>
#include <schinfointernal.h>
#include <schtask.h>
#include <s32file.h>
#include <e32math.h>
#include <e32cons.h>
#include <e32test.h>

#include "platsectaskcommon.h"


// Constants
_LIT(KConsoleName, "PlatSecTaskHandler");

static void SignalTestExe()
	{
	_LIT(KSchSemaphoreName, "SCHMinimalTaskHandler");
	RSemaphore sem;
	TInt ret = sem.OpenGlobal(KSchSemaphoreName);
	if (ret == KErrNone)
		{
		sem.Signal();
		sem.Close();
		}
	}

//***********************************************************************************
LOCAL_C TInt GetRandomNumber(const TInt aLow, const TInt aHigh, TInt64& aSeed)
	{
	TReal initialRand = (Math::FRand(aSeed) * (aHigh - aLow));
	TInt32 rand;

	// Round to 0 decimal places, ie. the nearest whole numer
	Math::Round(initialRand, initialRand, 0);
	Math::Int(rand, initialRand);

	return (aLow + rand);
	}

//***********************************************************************************
LOCAL_C void DestroyArray(TAny* aArray)
	{
	static_cast<RArray<TCapability>*>(aArray)->Reset();
	}
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1345
@SYMTestCaseDesc	    Platform security Task handler test 
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt to test the contents of a task file.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/				
LOCAL_C void ConstructConsoleL(RFile& aTaskFile, RTest& aTest)
	{
	
	CConsoleBase* console=Console::NewL(KConsoleName, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(_L(" contents of task file\n"));

	RArray<TCapability> enforcedCaps;
	CleanupStack::PushL(TCleanupItem(&DestroyArray,&enforcedCaps));

	for (TInt i=0; i<ECapability_Limit; i++)
		{
		// we're checking here to see which capabilities are enforced, skipping
		// ECapabilityMultimediaDD and ECapabilityWriteDeviceData as it is
		// needed to write persistent schedules upon which this test relies.
		
		if (i == KPlatSecTestCapability)
			continue;
		else if (i == ECapabilityWriteDeviceData)
			continue;
		else if (PlatSec::IsCapabilityEnforced((TCapability)i))
			enforcedCaps.Append((TCapability)i);
		}
	
	TBool isTestCapabilityEnforced = PlatSec::IsCapabilityEnforced(KPlatSecTestCapability);
	
	CFileStore* store;
	store = CDirectFileStore::FromLC(aTaskFile);

	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	
	TInt count = instream.ReadInt32L();
	for (TInt i=0;i<count;i++)
		{
		CScheduledTask* task = CScheduledTask::NewLC(instream);
		
		aTest(task->Info().iName.CompareF(KPlatSecTaskName)==0);
		aTest(task->Data().CompareF(KPlatSecTaskData)==0);
		aTest(task->SecurityInfo().iSecureId==KPlatSecTestSid); //This is the SID of tschsvrplatsec

		// check that client has capability it should have
		if(isTestCapabilityEnforced)
			aTest(task->SecurityInfo().iCaps.HasCapability(KPlatSecTestCapability));
		
		// check that the client has ECapabilityWriteDeviceData
		aTest(task->SecurityInfo().iCaps.HasCapability(ECapabilityWriteDeviceData));
		
		// check that client doesn't have any other capabilities
		for (TInt j=enforcedCaps.Count()-1; j>=0; --j)
			{
			aTest(!task->SecurityInfo().iCaps.HasCapability(enforcedCaps[j]));
			}
			
		
		CleanupStack::PopAndDestroy(task);
		}
		
	console->Printf(_L("Pausing for a one second..."));
	User::After(1000000);
	
	CleanupStack::PopAndDestroy(4); // instream, store, enforcedCaps, console
	}


//***********************************************************************************
LOCAL_C TInt Execute()
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		_LIT(KPlatSecTestName, "PlatSectTaskHandlerTest");
		RTest theTest(KPlatSecTestName);
		theTest.Start(KPlatSecTestName);
		theTest.Title();

		RFile file;

		// Adopt the task file from the Task Scheduler
		err = file.AdoptFromCreator(TScheduledTaskFile::FsHandleIndex(),
									TScheduledTaskFile::FileHandleIndex());
		if (err != KErrNone)
			return err;
		
		// The aParam is the name of a file where the relevant CTaskExCmdLine is
		// do the executing 
		theTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1345 "));
		TRAPD(err, ConstructConsoleL(file, theTest));
		if(err == KErrNone)
			{
			// Sometimes we want to return a bogus error value, 
			// sometimes we don't.
			TTime now;
			now.HomeTime();
			TInt64 seed = now.Int64();
			err = GetRandomNumber(-50, 200, seed); //20% chance of error being returned
			}
		
		file.Close();// Close the file
		
		theTest.End();
		theTest.Close();
				
		delete cleanup;
		}
	SignalTestExe();		
	return err;
	}


//***********************************************************************************
GLDEF_C TInt E32Main()
	{
	return Execute();
	}
