// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LogViewWindowFetcher.h"

// System includes
#include <s32mem.h>

// User includes
#include "logservcli.h"

// Constants
const TInt KLogSizeOfEventGuess = 200;
const TInt KLogWindowFetchBufferGranularity = KLogSizeOfEventGuess * 2;
const TInt KLogWindowFetchBufferMinimumBufferSize = 4;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowFetcher (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogViewWindowFetcher::CLogViewWindowFetcher(RLogSession& aSession, TLogViewId aViewId, MLogViewWindowFetcherObserver& aObserver, TInt aPriority)
:	CActive(aPriority), iSession(aSession), iViewId(aViewId), iObserver(aObserver), iBufferPointer(NULL, 0)
	{
	CActiveScheduler::Add(this);
	iData().iOperationType = ELogOperationViewWindowFetch;
	}

CLogViewWindowFetcher::~CLogViewWindowFetcher()
	{
	Cancel();
	//
	delete iBuffer;
	}

void CLogViewWindowFetcher::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KLogWindowFetchBufferGranularity);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowFetcher::PrepareToFetchWindowL(const TLogWindowAndCursor& aRequestedWindow)
	{
	iRequestedWindow = aRequestedWindow;
	iRequestedWindow.iValid = EFalse;
	iWindowReq = iRequestedWindow; // Initialise window
	// Setup the buffer so that its the right size to receive the events
	const TInt range = Max(1, iRequestedWindow.iUpper - iRequestedWindow.iLower + 1);
	const TInt size = KLogSizeOfEventGuess * range;
	iBuffer->ResizeL(size);
	iBufferPointer.Set(iBuffer->Ptr(0));
	//
	iFetchWindowData().iBufferSize = size;
	}

void CLogViewWindowFetcher::Start(TRequestStatus& aObserver)
	{
	iObserverRequestStatus = &aObserver;
	*iObserverRequestStatus = KRequestPending;
	if	(iRequestedWindow.Range())
		Fetch(iRequestedWindow);
	else
		{
		// Nothing to fetch
		iRequestedWindow.NormalizeWindowAndCursor();
		User::RequestComplete(iObserverRequestStatus, KErrNone);
		}
	}

void CLogViewWindowFetcher::SilentCancel()
	{
	if	(iObserverRequestStatus)
		User::RequestComplete(iObserverRequestStatus, KErrNone);
	Cancel();
	}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowFetcher::RunL()
	{
	const TInt sentByServer = iStatus.Int();
	User::LeaveIfError(sentByServer);
	//The server has completed the operation without any error, but there is no returned data
	//(sentByServer == 0). Then the client has to check if iFetchWindowData().iServerDataSize
	//data member is set, which means that the server wants to send more data than the client
	//side buffer can accept. The client has to increase the buffer size and then execute 
	//the operation again.
	TInt newSize = iFetchWindowData().iServerDataSize;
	//newSize was initialized, so iFetchWindowData().iServerDataSize can be set to 0.
	iFetchWindowData().iServerDataSize = 0;
	if(sentByServer == 0 && newSize > 0)
		{
		iBuffer->ResizeL(newSize);
		iBufferPointer.Set(iBuffer->Ptr(0));
		iFetchWindowData().iBufferSize = newSize;
		//The operation will be executed again later - see "Fetch(iWindowReq)" call.
		}
	RBufReadStream stream(*iBuffer);
	for(TInt i=0; i<sentByServer; i++)
		{
		// Give the event to the observer
		CLogEvent* event = CLogEvent::NewL();
		CleanupStack::PushL(event);
		//
		stream >> *event;
		iObserver.HandleFetchedWindowItemL(i, event);
		CleanupStack::Pop(event);
		}

	// Do we need to fetch the next batch?
	iWindowReq.iLower += sentByServer; // the lower array index for next request

	if(iWindowReq.iLower <= iWindowReq.iUpper)
		{
		// Fetch some more
		Fetch(iWindowReq);
		}
	else
		{
		// All done?
		iBuffer->ResizeL(KLogWindowFetchBufferMinimumBufferSize);
		iRequestedWindow.iValid = ETrue;
		CompleteObserver(KErrNone);
		}
	}

void CLogViewWindowFetcher::DoCancel()
	{
	if	(iData().iOperationId > 0)
		{
		const TInt errorIgnored = iSession.Send(ELogOperationCancel, TIpcArgs(&iData));
		(void) errorIgnored;
		//
		iData().iOperationId = KLogNullOperationId;
		}

	CompleteObserver(KErrCancel);
	}

TInt CLogViewWindowFetcher::RunError(TInt aError)
	{
	CompleteObserver(aError);
	return KErrNone;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowFetcher::Fetch(const TLogWindow& aWindow)
	{
	iFetchWindowData().iLower = aWindow.iLower;
	iFetchWindowData().iUpper = aWindow.iUpper;
	//
	iData().iOperationId = iSession.AllocateIdOperation();
	//
	iSession.Send(ELogViewOperationInitiate, TIpcArgs(&iData,iViewId,&iFetchWindowData,&iBufferPointer), iStatus);
	SetActive();
	}

void CLogViewWindowFetcher::CompleteObserver(TInt aError)
	{
	if	(iObserverRequestStatus)
		User::RequestComplete(iObserverRequestStatus, aError);
	}
