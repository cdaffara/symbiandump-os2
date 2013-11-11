// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "LOGMAIN.H"
#include "LOGREC.H"
#include "LOGCOMP.H"
#include "logservpanic.h"
#include "LOGQUERY.H"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServCacheConfig.h"
#include "LogCliServShared.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServSqlStrings.h"

CLogMaintenance::CLogMaintenance(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
: CLogActive(aPriority), iDatabase(aDatabase)
	{
	}

CLogMaintenance::~CLogMaintenance()
	{
	Cancel();
	delete iRecover;
	delete iCompact;
	}

void CLogMaintenance::ConstructL()
	{
	iRecover = new(ELeave)CLogRecover(iDatabase, Priority());
	iCompact = new(ELeave)CLogCompact(iDatabase, Priority());
	}

CLogMaintenance* CLogMaintenance::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	CLogMaintenance* self = new(ELeave)CLogMaintenance(aDatabase, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CLogMaintenance::Start(TBool aPurge, TRequestStatus& aStatus)
	{
	iState = ELogRecover;
	iPurge = aPurge;

	Queue(aStatus);
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

TBool CLogMaintenance::DoNextL()
	{
	switch(iState)
		{
		// Recover the database
		case ELogRecover:
			if (iRecover->Start(iStatus))
				{
				iState = ELogGetConfig;
				return ETrue;
				}
			// Fall Through

		// Get the log configuration
		case ELogGetConfig:
			if (iPurge)
				{
				iConfig = iDatabase.DTICacheConfig().Config();				
				TRequestStatus* status = &iStatus;
				User::RequestComplete(status, KErrNone);
				iState = iConfig.iMaxEventAge == 0 ? ELogCompact : ELogPurgeMainAge;
				return ETrue;
				}
			// Fall through

		// Purge old events from the log
		case ELogPurgeMainAge:
			if (iPurge)
				{
				// Get the max age
				TTime date;
				date.UniversalTime();
				date -= TTimeIntervalSeconds(iConfig.iMaxEventAge);

				ClearLogL(date, iStatus);
				iState = ELogCompact;
				return ETrue;
				}

		// Compact the database
		case ELogCompact:
			iCompact->Start(iStatus);
			iState = ELogComplete;
			return ETrue;

		case ELogComplete:
			break;

		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogNoSuchState7));
		};

	return EFalse;
	}

void CLogMaintenance::DoRunL()
	{
	if (DoNextL())
		SetActive();
	}

void CLogMaintenance::DoCancel()
	{
	iRecover->Cancel();
	iCompact->Cancel();

	CLogActive::DoCancel();
	}

void CLogMaintenance::DoComplete(TInt& aStatus)
	{
	// Ignore all errors
	aStatus = KErrNone;
	}

// aDate is expected to be UTC  
void CLogMaintenance::ClearLogL(const TTime& aDate, TRequestStatus& aStatus)
	{
	TBuf<KLogMaxDateLength> date;
	aDate.FormatL(date, LogUtils::DateFormatForLocale());
    // Get list of events to purge
    TheSql.Format(KLogSqlSelectOldestString, &date);
    RLogDbView view;
    view.PrepareLC(iDatabase.DTIDatabase(), TheSql);
	if(view.FirstL())
		{
        static TDbColNo idColNo = 0;
        if(idColNo == 0)
            {
            CDbColSet* cs = view.ColSetL();
            idColNo = cs->ColNo(KLogFieldIdString);
            delete cs;
            }
        iDatabase.DTIBeginWithRollBackProtectionLC();
		do
			{
			view.GetL();
			const TLogId id = view.ColInt32(idColNo);
			view.DeleteL();
			iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventDeleted, id);
			}
		while(view.NextL());
		// Commit changes
		iDatabase.DTICommitAndCancelRollbackProtectionL();
		}
	CleanupStack::PopAndDestroy(&view);
	// Complete the request
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}
