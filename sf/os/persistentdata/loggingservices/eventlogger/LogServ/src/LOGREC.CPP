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

#include "LOGREC.H"

// User includes
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"


CLogRecover::CLogRecover(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
: CLogActive(aPriority), iDatabase(aDatabase)
	{
	}

TBool CLogRecover::Start(TRequestStatus& aStatus)
	{
	if (!iDatabase.DTIDatabaseIsDamaged())
		return EFalse;

	TInt error = iOperation.Recover(iDatabase.DTIDatabase(), iStep());
	if (error != KErrNone)
		{
		__ASSERT_DEBUG(ETrue, Panic(ELogRecoverError));
		return EFalse;
		}

	if (iStep() > 0)
		{
		Queue(aStatus);
		TRequestStatus* st = &iStatus;
		User::RequestComplete(st, KErrNone);
		SetActive();

		return ETrue;
		}

	return EFalse;
	}

void CLogRecover::DoRunL()
	{
	if (iStep() > 0)
		{
		iOperation.Next(iStep, iStatus);
		SetActive();
		}
	else
		__ASSERT_DEBUG(!iDatabase.DTIInTransaction(), Panic(ELogDatabaseDamaged3));
	}

void CLogRecover::DoComplete(TInt& /*aCompletionCode*/)
	{
	iOperation.Close();
	}
