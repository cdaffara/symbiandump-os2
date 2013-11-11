// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// Constants
_LIT(KMinimalTaskConsoleName, "MinimalTaskExecutor");

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
LOCAL_D TInt GetRandomNumber(const TInt aLow, const TInt aHigh, TInt64& aSeed)
	{
	TReal initialRand = (Math::FRand(aSeed) * (aHigh - aLow));
	TInt32 rand;

	// Round to 0 decimal places, ie. the nearest whole numer
	Math::Round(initialRand, initialRand, 0);
	Math::Int(rand, initialRand);

	return (aLow + rand);
	}

//***********************************************************************************
LOCAL_D void ConstructConsoleL(RFile& aTaskFile)
	{
	CConsoleBase* console=Console::NewL(KMinimalTaskConsoleName, TSize(KConsFullScreen, KConsFullScreen));
	CleanupStack::PushL(console);
	console->Printf(_L(" contents of task file\n"));
	
	CFileStore* store;
	// Open the filestore
	store = CDirectFileStore::FromLC(aTaskFile);//pushes store
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());//pushes instream

	// Get task count
	TInt count = instream.ReadInt32L();
	for (TInt i=0;i<count;i++)
		{
		CScheduledTask* task = CScheduledTask::NewLC(instream);
		
		TBuf<150> buf;
		buf.Format(_L("Running task \"%S\""), &task->Info().iName);
		User::LeaveIfError(User::InfoPrint(buf));
		
		console->Printf(task->Info().iName);
		console->Printf(_L("\n"));
		HBufC* data = const_cast<HBufC*>(&(task->Data()));
		console->Printf(*data);
		console->Printf(_L("\n"));
		console->Printf(_L("%d \n"),task->Info().iTaskId);
		TTsTime tstime = task->ValidUntil();
		const TTime time = tstime.GetLocalTime();
		TBuf<30> dateString;
		time.FormatL(dateString,(_L("%H%:1%T%*E%*D%X%*N%Y %1 %2 %3")));
		console->Printf(_L(":%S\n"), &dateString);
		CleanupStack::PopAndDestroy(task);
		}
	console->Printf(_L("Pausing for a one second..."));
	User::After(1000000);
	CleanupStack::PopAndDestroy(3); //console, store, instream
	}


//***********************************************************************************
LOCAL_D TInt Execute()
	{
	TInt err = KErrNoMemory;
	CTrapCleanup* cleanup=CTrapCleanup::New();	//can fail
	if (cleanup)
		{
		RFile file;
		
		// Adopt the task file from the Task Scheduler
		err = file.AdoptFromCreator(TScheduledTaskFile::FsHandleIndex(),
									TScheduledTaskFile::FileHandleIndex());
		if (err != KErrNone)
			return err;
		
		// The aParam is the name of a file where the relevant CTaskExCmdLine is
		// do the executing 
		TRAPD(err, ConstructConsoleL(file));
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
