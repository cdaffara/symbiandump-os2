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

#include <logwrap.h>
#include "LOGPANIC.H"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS 
/**
@internalAll
*/
const TInt KLogActiveDoNotCompleteIfActive = KMaxTInt-1;

#endif

EXPORT_C CLogActive::CLogActive(TInt aPriority)
: CActive(aPriority)
	{
	CActiveScheduler::Add(this);
	}


EXPORT_C CLogActive::~CLogActive()
	{
	}

EXPORT_C void CLogActive::Queue(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(iReport==NULL, Panic(ELogAlreadyActive));

	aStatus=KRequestPending;
	iReport=&aStatus;
	}

EXPORT_C void CLogActive::DoCancel()
	{
	TInt result=KErrCancel;
	Complete(result);
	}

EXPORT_C void CLogActive::RunL()
	{
	TInt status=iStatus.Int();
	if (status>=KErrNone)
		{
		TRAPD(error,DoRunL());
		__ASSERT_DEBUG(error==KErrNone || !IsActive(), User::Invariant());
		if (IsActive())	
			return;
		status=error;
		}
	Complete(status);
	}

EXPORT_C void CLogActive::Complete(TInt aStatus)
	{
	if (iReport)
		{
		DoComplete(aStatus);
		if (IsActive() && aStatus == KLogActiveDoNotCompleteIfActive)
			return;
		User::RequestComplete(iReport, aStatus);
		}
	}

EXPORT_C void CLogActive::DoComplete(TInt&)
	{
	}
