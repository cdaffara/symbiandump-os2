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

#include "LogClientObserver.h"

// System includes
#include <s32mem.h>

// User includes
#include <logwrap.h>
#include <logcli.h>
#include "logservcli.h"
#include <logclientchangeobserver.h>

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogClientObserver (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogClientObserver::CLogClientObserver(CLogClient& aClient, MLogClientChangeObserver& aObserver, TInt aPriority)
:	CActive(aPriority), iClient(aClient), iObserver(aObserver)
	{
	CActiveScheduler::Add(this);
	}

CLogClientObserver::~CLogClientObserver()
	{
	Cancel();
	}

CLogClientObserver* CLogClientObserver::NewL(CLogClient& aClient, MLogClientChangeObserver& aObserver, TInt aPriority)
	{
	CLogClientObserver* self = new(ELeave) CLogClientObserver(aClient, aObserver, aPriority);
	self->RequestChanges();
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogClientObserver::RunL()
	{
	if	(iStatus.Int() == KErrNone)
		NotifyObserverL();
	RequestChanges();
	}

void CLogClientObserver::DoCancel()
	{
	RequestChangesCancel();
	}

TInt CLogClientObserver::RunError(TInt /*aError*/)
	{
	// Ignore any leave when notifying the client
	return KErrNone;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogClientObserver::RequestChanges()
	{
	__ASSERT_DEBUG(!IsActive(), User::Invariant());
	iClient.Session().Send(ELogNotifyExtended, TIpcArgs(&iContextBuf,&iParamBuf1,&iParamBuf2,&iParamBuf3), iStatus);
	SetActive();
	}

void CLogClientObserver::RequestChangesCancel()
	{
	iClient.Session().Send(ELogNotifyExtendedCancel, TIpcArgs());
	}

void CLogClientObserver::NotifyObserverL()
	{
	const TUid contextUid = { iContextBuf() };
	const TInt param1 = iParamBuf1();
	const TInt param2 = iParamBuf2();
	const TInt param3 = iParamBuf3();
	//
	iObserver.HandleLogClientChangeEventL(contextUid, param1, param2, param3);
	}
