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

#include "LogViewWindowLockObserver.h"

// User includes
#include "logservcli.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogViewWindowLockObserver (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogViewWindowLockObserver::CLogViewWindowLockObserver(RLogSession& aSession, TLogViewId aViewId, MLogViewWindowLockObserver& aObserver, TInt aPriority)
:	CLogActive(aPriority), iSession(aSession), iViewId(aViewId), iObserver(aObserver)
	{
	}

CLogViewWindowLockObserver::~CLogViewWindowLockObserver()
	{
	Cancel();
	}

void CLogViewWindowLockObserver::ConstructL()
	{
	IssueRequest();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowLockObserver::DoRunL()
	{
	User::LeaveIfError(iStatus.Int());
	const TLogViewLockStatus state = static_cast<TLogViewLockStatus>(iStatus.Int());
	//
	IssueRequest();
	iObserver.HandleWindowLockStatusChangeL(state);
	}

void CLogViewWindowLockObserver::DoCancel()
	{
	iSession.Send(ELogViewNotifyLockStatusChangeCancel, TIpcArgs(iViewId));
	}

TInt CLogViewWindowLockObserver::RunError(TInt /*aError*/)
	{
	return KErrNone;
	}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogViewWindowLockObserver::IssueRequest()
	{
	iSession.Send(ELogViewNotifyLockStatusChange, TIpcArgs(iViewId), iStatus);
	SetActive();
	}

