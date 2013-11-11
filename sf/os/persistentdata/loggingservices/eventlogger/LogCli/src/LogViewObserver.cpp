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

#include "LogViewObserver.h"

// System includes
#include <s32mem.h>

// User includes
#include <logwrap.h>
#include <logcli.h>
#include "logclipanic.h"
#include "logservcli.h"
#include "LogChangeDefinition.h"
#include "LogViewChangeObserverInternal.h"

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewObserver (source)
/////////////////////////////////////////////////////////////////////////////////////////
CLogViewObserver::CLogViewObserver(CLogView& aView, CLogClient& aClient, MLogViewChangeObserverInternal& aObserver, TLogViewId aViewId, TInt aPriority)
:	CActive(aPriority), iClient(aClient), iObserver(aObserver), iViewId(aViewId), iView(aView)
	{
	CActiveScheduler::Add(this);
	}

CLogViewObserver::~CLogViewObserver()
	{
	Cancel();
	//
	delete iChanges;
	}

void CLogViewObserver::ConstructL()
	{
	iChanges = CLogChangeDefinition::NewL();
	RequestChanges();
	}

CLogViewObserver* CLogViewObserver::NewL(CLogView& aView, CLogClient& aClient, MLogViewChangeObserverInternal& aObserver, TLogViewId aViewId, TInt aPriority)
	{
	CLogViewObserver* self = new(ELeave) CLogViewObserver(aView, aClient, aObserver, aViewId, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLogViewObserver::RequestChanges()
	{
	iClient.Session().Send(ELogViewChangeNotificationsRequest, TIpcArgs(iViewId), iStatus);
	SetActive();
	}

void CLogViewObserver::RunL()
	{
	const TInt error = iStatus.Int();
	if (error == KErrServerTerminated)
		{
		// Effectively does a "delete this"
		iView.NotifyLogServerTerminatedL();
		}
	else if (error < KErrNone)
		{
		// Don't request any further changes.
		}
	else
		{
		const TInt changesSize = iStatus.Int();
		User::LeaveIfError(changesSize);
		FetchChangesL(changesSize);
		RequestChanges();
		NotifyObserverL();
		}
	}

TInt CLogViewObserver::RunError(TInt /*aError*/)
	{
	
	// This point can be reached when RequestChanges has already been 
	// called, for example when an observer leaves when it is notified of 
	// a change.
	// Check IsActive() before calling RequestChanges to avoid calling it 
	// twice in a row and causing a E32USER-CBase:42 panic.
 
	if (!IsActive())
		{
		RequestChanges();	
		}
 	 
	return KErrNone;
	}

void CLogViewObserver::DoCancel()
	{
	RequestChangesCancel();
	}

void CLogViewObserver::RequestChangesCancel()
	{
	iClient.Session().Send(ELogViewChangeNotificationsCancel, TIpcArgs(iViewId));
	}

void CLogViewObserver::FetchChangesL(TInt aBufferSizeRequired)
	{
	HBufC8* buffer = HBufC8::NewLC(aBufferSizeRequired);
	TPtr8 pBuffer(buffer->Des());
	//
	User::LeaveIfError(iClient.Session().Send(ELogViewFetchChanges, TIpcArgs(iViewId,aBufferSizeRequired,&pBuffer)));
	//
	iChanges->Reset();
	RDesReadStream stream(*buffer);
	stream >> *iChanges;
	CleanupStack::PopAndDestroy(buffer);
	}

void CLogViewObserver::NotifyObserverL()
	{
	TLogId id;
	TInt viewIndex;

	// Copy the changes
	const TInt count = iChanges->Count();
	if	(!count)
		return;
	//
	CLogChangeDefinition* changes = CLogChangeDefinition::NewL(*iChanges);
	CleanupStack::PushL(changes);
	iChanges->Reset();
	//
	for(TInt i=0; i<count; i++)
		{
		const TLogDatabaseChangeType type = changes->At(i, id, viewIndex);
		//
		switch(type)
			{
		case ELogChangeTypeEventAdded:
			iObserver.HandleLogViewChangeEventAddedL(id, viewIndex, i, count);
			break;
		case ELogChangeTypeEventChanged:
			iObserver.HandleLogViewChangeEventChangedL(id, viewIndex, i, count);
			break;
		case ELogChangeTypeEventDeleted:
			iObserver.HandleLogViewChangeEventDeletedL(id, viewIndex, i, count);
			break;
		case ELogChangeTypeLogCleared:
			iObserver.HandleLogViewChangeEventLogClearedL();
			break;
		default:
		case ELogChangeTypeUndefined:
			__ASSERT_DEBUG(EFalse, Panic(ELogUnexpectedChangeType));
			}
		}
	CleanupStack::PopAndDestroy(changes);
	}
