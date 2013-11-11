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

#include "LOGCOMP.H"
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"

// Constants
const TInt KLogMaxWastedSpacePercentage = 25;
const TInt KLogMaxWastedSpace = 128 * 1024;
const TInt KLogMinWastedSpace = 32 * 1024;


CLogCompact::CLogCompact(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
: CLogActive(aPriority), iDatabase(aDatabase)
	{
	}

void CLogCompact::Start(TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(ELogAlreadyActive5));
	
	TInt error = iOperation.UpdateStats(iDatabase.DTIDatabase(), iStep());
	iCompact = EFalse;

	Queue(aStatus);
	TRequestStatus* st = &iStatus;
	User::RequestComplete(st, error);
	SetActive();
	}

void CLogCompact::DoRunL()
	{
	if (iStep() > 0)
		{
		iOperation.Next(iStep, iStatus);
		SetActive();
		}
	else if (!iCompact && CompactRequired())
		{
		iOperation.Close();

		iCompact = ETrue;
		User::LeaveIfError(iOperation.Compact(iDatabase.DTIDatabase(), iStep()));

		if (iStep() > 0)
			{
			TRequestStatus* st = &iStatus;
			User::RequestComplete(st, KErrNone);

			SetActive();
			}
		}
	}

TBool CLogCompact::CompactRequired() const
	{
	RDbDatabase::TSize size = iDatabase.DTIDatabase().Size();
	if (size.iUsage < 0)
		return EFalse;

	const TInt wastedSpace = (size.iSize * (100 - size.iUsage)) / 100;
	return (wastedSpace > KLogMinWastedSpace && (100 - size.iUsage) > KLogMaxWastedSpacePercentage) || wastedSpace > KLogMaxWastedSpace;
	}

void CLogCompact::DoComplete(TInt&)
	{
	iOperation.Close();
	}
