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

#include <e32test.h>
#include <schtask.h>
#include "platsectaskcommon.h"

LOCAL_C void NotifyTestComplete()
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
/**
@SYMTestCaseID          SYSLIB-SCHSVR-CT-1344
@SYMTestCaseDesc	    Migration task handler test
@SYMTestPriority 	    High
@SYMTestActions  	    Attempt to create a representation of a scheduled task from a specified stream.
@SYMTestExpectedResults Test must not fail
@SYMREQ                 REQ0000
*/			
LOCAL_C void CheckFileOkL(TFileName& aFileName, RTest& aTest)
	{
	aTest.Next(_L(" @SYMTestCaseID:SYSLIB-SCHSVR-CT-1344 "));
	RFs fs;
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	
	TParse parsedName;
	User::LeaveIfError(fs.Parse(aFileName, parsedName));
	CFileStore* store = CDirectFileStore::OpenLC(fs, parsedName.FullName(), EFileRead);

	RStoreReadStream instream;
	instream.OpenLC(*store, store->Root());

	TInt count = instream.ReadInt32L();
	aTest(count==1);
	
	CScheduledTask* task = CScheduledTask::NewLC(instream);
	aTest(task->Info().iName.CompareF(KPlatSecTaskName)==0);

	CleanupStack::PopAndDestroy(4); // task, fs, instream, store
	}

LOCAL_C TInt Execute(TFileName& aFilename)
	{
	CTrapCleanup* cleanup = CTrapCleanup::New();
	if(!cleanup)
		return KErrNoMemory;
	
	_LIT(KPlatSecTestName, "MigrationTaskHandlerTest");
	RTest theTest(KPlatSecTestName);
	theTest.Start(KPlatSecTestName);
	theTest.Title();
    
	TRAPD(err, CheckFileOkL(aFilename, theTest));
	theTest(err==KErrNone);

	NotifyTestComplete();

	theTest.End();
	theTest.Close();
				
	delete cleanup;

	return err;
	}

GLDEF_C TInt E32Main()
	{
	TBuf<0x100> cmd;
    User::CommandLine(cmd);
	return Execute(cmd);
	}
