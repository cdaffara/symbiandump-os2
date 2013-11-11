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

#include "LogServViewWindowFetcher.h"
#include <s32mem.h>
#include "LOGGET.H"
#include "logservpanic.h"
#include "LogServView.h"

// Constants
const TInt KLogViewWindowTransferBufferGranularity = 500;


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewWindowFetcher (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServViewWindowFetcher::CLogServViewWindowFetcher(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
:	CLogActive(aPriority), iDatabase(aDatabase)
	{
	}

CLogServViewWindowFetcher::~CLogServViewWindowFetcher()
	{
	Cancel();
	delete iBuffer;
	delete iGetEvent;
	delete iEvent;
	}

void CLogServViewWindowFetcher::ConstructL()
	{
	iBuffer = CBufFlat::NewL(KLogViewWindowTransferBufferGranularity);
	iEvent = CLogEvent::NewL();
	iGetEvent = CLogGetEvent::NewL(iDatabase, Priority());
	}

CLogServViewWindowFetcher* CLogServViewWindowFetcher::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	CLogServViewWindowFetcher* self = new(ELeave) CLogServViewWindowFetcher(aDatabase, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewWindowFetcher::StartL(TRequestStatus& aStatus, const CLogServViewBase& aView, const TLogTransferWindow& aWindow, const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iState == EStateIdle, Panic(ELogViewWindowFetcherBadState));
	//
	Queue(aStatus);
	//
	iView = &aView;
	iWindow = aWindow;
	iMessage = &aMessage;
	iBuffer->Reset();
	iWindowIndex = 0;
	iState = EStateStarting;
	//
	CompleteRequest();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewWindowFetcher::DoRunL()
	{
	const TInt error = iStatus.Int();
	User::LeaveIfError(error);

	switch(iState)
		{
	case EStateStarting:
		GetNextEventL(iWindowIndex);
		iState = EStateContinuing;
		break;
	case EStateContinuing:
		ProcessEventL();
		break;
	default:
		__ASSERT_ALWAYS(0, Panic(ELogViewWindowFetcherBadState2));
		break;
		}
	}

void CLogServViewWindowFetcher::ProcessEventL()
	{
	// Try and write the event to the buffer
	const TInt sizeBefore = iBuffer->Size();
	RBufWriteStream stream(*iBuffer, iBuffer->Size());
	stream << *iEvent;
	const TInt sizeAfter = iBuffer->Size();
	//
	TBool moreToFetch = EFalse;

	const TInt numberToFetch = iWindow.Range();
	if	(sizeAfter > iWindow.iBufferSize)
		{
		//The client buffer size is too small. It should be increased, if the client wants
		//to get the server data. 
		//The server sets iServerDataSize data member with the minimal size which the client
		//side buffer should have - sizeAfter.
		TPtrC8 ptr(reinterpret_cast <TUint8*> (&iWindow), sizeof(iWindow));
		iWindow.iServerDataSize = sizeAfter;
		iMessage->WriteL(2, ptr);

		iBuffer->ResizeL(sizeBefore);
		iWindowIndex -= 1; // we didn't get this event
		}
	else if	(iWindowIndex+1 < numberToFetch)
		{
		GetNextEventL(iWindowIndex+1);
		moreToFetch = ETrue;
		}
	
	if	(!moreToFetch)
		{
		// Write whatever we have back to the client's address space
		TPtr8 pBuffer(iBuffer->Ptr(0));
		iMessage->WriteL(3, pBuffer);
		iState = EStateIdle;
		}
	}

void CLogServViewWindowFetcher::DoCancel()
	{
	switch(iState)
		{
	case EStateStarting:
		// Nothing to do, completed our own request status
		break;
	case EStateContinuing:
		iGetEvent->Cancel();
		break;
	default:
		__ASSERT_ALWAYS(0, Panic(ELogViewWindowFetcherBadState3));
		break;
		}
	CLogActive::DoCancel();
	}

void CLogServViewWindowFetcher::DoComplete(TInt& aCompletionCode)
	{
	// Indicates to the client side how many records we retrieved.
	if	(aCompletionCode == KErrNone)
		aCompletionCode = iWindowIndex+1;
	else
		iState = EStateIdle;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewWindowFetcher::GetNextEventL(TInt aWindowIndex)
	{
	const TInt index = iWindow.iLower + aWindowIndex;
	const TInt viewCount = iView->Count();
	if	(index < 0)
		{
		::PanicClientL(*iMessage, ELogViewBadWindow);
		}
	else if (index >= viewCount)
		{
		// View is still catching up with changes which have been made in the server?
		CLogEvent* event = CLogEvent::NewL();
		delete iEvent;
		iEvent = event;
		CompleteRequest();
		iWindowIndex = aWindowIndex;
		}
	else
		{
		const TLogId id = iView->At(index);
		iEvent->SetId(id);
		iGetEvent->StartL(*iEvent, iStatus, *iMessage);
		iWindowIndex = aWindowIndex;
		SetActive();
		}
	}

void CLogServViewWindowFetcher::CompleteRequest()
	{
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}
