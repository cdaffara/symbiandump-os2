/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Implementation for Test Watcher object using RUndertaker object
* for trapping dead threads during test execution
* Collect details of dead threads and pass it on to TEF for logging
*
*/



/**
 @file TestWatcher.cpp
*/

#include "testwatcher.h"

TThreadPanicDetails::TThreadPanicDetails(TName aThreadName, 
	TInt aReason, TExitCategoryName aCategory, TTime aTime)
	: iThreadName(aThreadName), iReason(aReason), iCategory(aCategory), iTime(aTime)
	{
	}

CTestWatcher::CTestWatcher()
	{
	}

TInt ThreadFunc(TAny* anArgument)
	{
	CSharedData* mySharedData = reinterpret_cast<CSharedData*> (anArgument);
	
	RUndertaker u;
	u.Create();
	
	TRequestStatus status;
	TInt deadThread;
	
	TBool clientWaiting = ETrue;
	
	FOREVER
		{
		status = KRequestPending;
		u.Logon(status,deadThread);
		// wait for the next thread to die.
	
		if (clientWaiting)
			{
			// rendezvous with the client so that they know we're ready.
			// This guarantees that we will catch at least the first panic to 
			// occur (as long as we aren't closed before kernel tells us.
			RThread::Rendezvous(KErrNone);
			clientWaiting = EFalse;
			}

		User::WaitForRequest(status);
		// until we get back around to the top we are missing notifications now.
		// being high priority helps us, but still...
		// deal with this QUICKLY


		// get handle to the dead thread (this has already been marked for us in 
		// the kernel)
		RThread t;
		t.SetHandle(deadThread);
	
		if (t.ExitType() == EExitPanic)
			{
			// the other ways threads can die are uninteresting
			TTime now;
			now.UniversalTime();
			
			TThreadPanicDetails* tpd = new TThreadPanicDetails (t.Name(),
				t.ExitReason(),t.ExitCategory(),now);
			
			mySharedData->iPanicDetails.AppendL(tpd);
			}
		t.Close();
		}
	}
	
	
CTestWatcher::~CTestWatcher()
	{
	iWorker.Close();
	for (TInt i = 0; i<iSharedData->iPanicDetails.Count();i++)
		{
		delete iSharedData->iPanicDetails[i];
		}
	iSharedData->iPanicDetails.Close();
	delete iSharedData;
	}

CTestWatcher* CTestWatcher::NewL()
	{
	CTestWatcher* tw = new (ELeave) CTestWatcher();
	CleanupStack::PushL(tw);
	tw->ConstructL();
	CleanupStack::Pop(tw);
	return tw;
	}
	
void CTestWatcher::ConstructL() 
	{
	iSharedData = new CSharedData();
	_LIT(KUndertakerWatcher,"UndertakerWatcher");
	User::LeaveIfError(iWorker.Create(KUndertakerWatcher(),ThreadFunc,KDefaultStackSize,&User::Heap(),iSharedData,EOwnerProcess));
	iWorker.SetPriority(EPriorityAbsoluteForeground);
	}

void CTestWatcher::StartL()
	{
	TRequestStatus status = KRequestPending;
	iWorker.Rendezvous(status); 
	iWorker.Resume();
	User::WaitForRequest(status);// wait for it to have initialized the undertaker.
	User::LeaveIfError(status.Int());
	}
	
void CTestWatcher::Stop()
	{
	iWorker.Suspend();
	}
