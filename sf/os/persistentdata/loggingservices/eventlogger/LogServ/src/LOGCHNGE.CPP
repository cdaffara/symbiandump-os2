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

#include "LOGCHNGE.H"
#include <logwraplimits.h>
#include <logcntdef.h>
#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServRecentList.h"
#include "LogServCacheConfig.h"
#include "LogServRecentListManager.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServDatabaseChangeInterface.h"
#include "LOGDUP.H"
#include "LogServCacheStrings.h"
#include "LogServSqlStrings.h"

//**********************************
// CLogChangeEvent
//**********************************

CLogChangeEvent::CLogChangeEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
: CLogActive(aPriority), iDatabase(aDatabase)
	{
	}

CLogChangeEvent::~CLogChangeEvent()
	{
	Cancel();
	//
	delete iEvent;
	delete iDuplicate;
	delete iDuplicateFilter;
	}

void CLogChangeEvent::ConstructL()
	{
	iEvent = CLogEvent::NewL();
	iDuplicate = CLogDuplicate::NewL(iDatabase, Priority());
	iDuplicateFilter = CLogFilter::NewL();
	}

CLogChangeEvent* CLogChangeEvent::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	CLogChangeEvent* self = new(ELeave)CLogChangeEvent(aDatabase, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

void CLogChangeEvent::StartL(const CLogEvent& aEvent, const CLogServRecentList* aRecentList, TRequestStatus& aStatus, const RMessage2& aMessage)
	{
	__ASSERT_ALWAYS(iState == ELogNone, Panic(ELogBadState2));
	__ASSERT_DEBUG(!IsActive(), Panic(ELogAlreadyActive3));

	iMessage = &aMessage;
	iEvent->CopyL(aEvent);
	iState = ELogChangeEvent;
	iRecentList = aRecentList; // Need to use this! (5yrs later - okay then :-)
	iOldRecentList = KLogNullRecentList;

	Queue(aStatus);
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CLogChangeEvent::DoRunL()
	{
	__ASSERT_DEBUG(iState != ELogNone, Panic(ELogBadState3));

	switch (iState)
		{
		case ELogChangeEvent:
			DoChangeEventL();
            ResetDuplicatesL();
            UpdateDuplicateEventsL();
			break;
		case ELogPurgeRecent:
			{
            RArray<TLogId> logIds;
            ::LogGetRecentEventsLC(iDatabase, iRecentList->Id(), iDatabase.DTICacheConfig().Config().iMaxRecentLogSize, logIds); 
            ::LogPurgeRecentEventsL(iDatabase, logIds);
            CleanupStack::PopAndDestroy(&logIds);
			}
            break;
 		default:
 			__ASSERT_DEBUG(ETrue, Panic(ELogBadState4));
 			break;
		}
	}

void CLogChangeEvent::DoComplete(TInt& aStatus)
	{
	if (iDatabase.DTIInTransaction())
		{
		if (aStatus == KErrNone)
			aStatus = iDatabase.DTICommitAndEnd();

		if (aStatus < KErrNone)
			iDatabase.DTIRollBack();
		}

	iState = ELogNone;
	}

void CLogChangeEvent::ResetDuplicatesL()
    {
    // Was it in a recent list? Is it now changing lists?
    if (iOldRecentList > 0 && (!iRecentList || iOldRecentList != iRecentList->Id()))
        {
        ::LogResetDuplicatesL(iDatabase, iEvent->Id());
        }
    }

void CLogChangeEvent::UpdateDuplicateEventsL()
    {
    // Detect duplicate events if we've added an event to a recent list
    if (iOldRecentList > 0 && iRecentList && iOldRecentList != iRecentList->Id())
        {
        iRecentList->GetFilter(*iEvent, *iDuplicateFilter);
        // If we get here we have added an event to a recent list, which may need to be purged
        iState = ELogPurgeRecent;
        // Passing -1 into this will ensure that events are updated so the most recent duplicate is first
        if (!iDuplicate->StartL(-1, iRecentList->Id(), *iDuplicateFilter, iStatus))
            {
            // Complete ourselves
            TRequestStatus* status = &iStatus;
            User::RequestComplete(status, KErrNone);
            }
        SetActive();
        }
    }

void CLogChangeEvent::DoChangeEventL()
    {
    RLogEventDbTable tbl;
    tbl.OpenLC(iDatabase.DTIDatabase());
    User::LeaveIfError(tbl.SetIndex(KLogNameEventIdx1));
    if(!tbl.SeekL(TDbSeekKey((TInt)iEvent->Id())))
        {
        User::Leave(KErrNotFound);
        }
    User::LeaveIfError(iDatabase.DTIBegin());
    tbl.GetL();
    iOldRecentList = tbl.ColInt8(RLogEventDbTable::iRecentColNo);
    DoChangeL(tbl, iDatabase.DTICacheStrings().GetIdL(iEvent->Direction()), iDatabase.DTICacheStrings().GetIdL(iEvent->Status()));
    CleanupStack::PopAndDestroy();//tbl
    }

void CLogChangeEvent::DoChangeL(RLogEventDbTable& aTbl, TLogStringId aDirectionId, TLogStringId aStatusId)
	{
	if(!iDatabase.DTIIsAllowed(EWriteOp, *iMessage, iEvent->EventType())) 
		{
		User::Leave(KErrPermissionDenied);
		}
	
	aTbl.UpdateL();

	if (iEvent->RemoteParty().Length() > 0)
		aTbl.SetColL(RLogEventDbTable::iRemotePartyColNo, iEvent->RemoteParty());
	else
		aTbl.SetColNullL(RLogEventDbTable::iRemotePartyColNo);

	if (iEvent->Direction().Length() > 0)
	    aTbl.SetColL(RLogEventDbTable::iDirectionColNo, (TUint32)aDirectionId);
	else
	    aTbl.SetColNullL(RLogEventDbTable::iDirectionColNo);

	aTbl.SetColL(RLogEventDbTable::iTimeColNo, iEvent->Time());
	aTbl.SetColL(RLogEventDbTable::iDurationTypeColNo, (TInt32)iEvent->DurationType());

	if (iEvent->DurationType() != KLogNullDurationType)
	    aTbl.SetColL(RLogEventDbTable::iDurationColNo, iEvent->Duration());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iDurationColNo);

	if (iEvent->Status().Length() > 0)
	    aTbl.SetColL(RLogEventDbTable::iStatusColNo, (TUint32)aStatusId);
	else
	    aTbl.SetColNullL(RLogEventDbTable::iStatusColNo);

	if (iEvent->Subject().Length() > 0)
	    aTbl.SetColL(RLogEventDbTable::iSubjectColNo, iEvent->Subject());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iSubjectColNo);

	if (iEvent->Number().Length() > 0)
	    aTbl.SetColL(RLogEventDbTable::iNumberColNo, iEvent->Number());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iNumberColNo);

	if (iEvent->Contact() != KLogNullContactId)
	    aTbl.SetColL(RLogEventDbTable::iContactColNo, iEvent->Contact());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iContactColNo);

	if (iEvent->Link() != KLogNullLink)
	    aTbl.SetColL(RLogEventDbTable::iLinkColNo, iEvent->Link());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iLinkColNo);

	if (iEvent->Data().Length() > 0)
	    aTbl.SetColL(RLogEventDbTable::iDataColNo, iEvent->Data());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iDataColNo);

	// Set the flags
	TInt bit = KLogFlagsCount;
	while(bit--)
		{
        aTbl.SetColL(RLogEventDbTable::iFlagColNo[bit], (TUint32)((iEvent->Flags() & 0x1 << bit) ? 1 : 0));
		}

	// Set the recent list
	if (iOldRecentList > 0)
		{
		// Removing an event from all recent lists?
		if (!iRecentList)
			{
			aTbl.SetColNullL(RLogEventDbTable::iRecentColNo);
			aTbl.SetColNullL(RLogEventDbTable::iDuplicateColNo);
			}
		// Changing list?
		else if (iOldRecentList != iRecentList->Id())
			{
			__ASSERT_DEBUG(iRecentList->Id() != KLogNullRecentList, Panic(ELogNullRecentList));
			aTbl.SetColL(RLogEventDbTable::iRecentColNo, (TInt32)iRecentList->Id());

			// This is set to -1 to prevent it temporarily appearing in a recent or duplicate view
			// It gets updated properly later when detecting duplicates
			aTbl.SetColL(RLogEventDbTable::iDuplicateColNo, -1);
			}
		}

#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
	if(iEvent->SimId() != KLogNullSimId)
	    aTbl.SetColL(RLogEventDbTable::iSimIdColNo, iEvent->SimId());
	else
	    aTbl.SetColNullL(RLogEventDbTable::iSimIdColNo);
#endif	
	
	aTbl.PutL();

	iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChanged, iEvent->Id());
	}

//**********************************
// CLogChangeConfig
//**********************************

CLogChangeConfig::CLogChangeConfig(MLogServDatabaseTransactionInterface& aDatabase, const CLogServRecentListManager& aRecentSetup, TInt aPriority)
: CLogActive(aPriority), iDatabase(aDatabase), iRecentSetup(aRecentSetup)
	{
	}

CLogChangeConfig::~CLogChangeConfig()
	{
	Cancel();
	}

CLogChangeConfig* CLogChangeConfig::NewL(MLogServDatabaseTransactionInterface& aDatabase, const CLogServRecentListManager& aRecentSetup, TInt aPriority)
	{
	CLogChangeConfig* self = new(ELeave) CLogChangeConfig(aDatabase, aRecentSetup, aPriority);
	return self;
	}

void CLogChangeConfig::StartL(const TLogConfig& aConfig, TRequestStatus& aStatus)
	{
	__ASSERT_ALWAYS(iState == ELogNone, Panic(ELogBadState5));
	__ASSERT_DEBUG(!IsActive(), Panic(ELogAlreadyActive4));

	iState = ELogChange;
	iConfig = &aConfig;

	Queue(aStatus);
	TRequestStatus* status = &iStatus;
	User::RequestComplete(status, KErrNone);
	SetActive();
	}

void CLogChangeConfig::DoRunL()
	{
	switch (iState)
		{
		case ELogChange:
			{
			// Start a transaction
			User::LeaveIfError(iDatabase.DTIBegin());
			// Change the config, using the existing transaction
			iDatabase.DTICacheConfig().UpdateL(*iConfig);
			//Purge old events
            RLogEventDbTable tbl;
            tbl.OpenLC(iDatabase.DTIDatabase());
            ::LogPurgeMainL(iDatabase, tbl, iConfig->iMaxLogSize, 0);
            CleanupStack::PopAndDestroy(&tbl);
            iRecent = 0;
            //
			iState = ELogPurgeRecent;
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			SetActive();
			break;
			}
		case ELogPurgeRecent:
			{
			// Are there any more recent lists to check
			if(iRecent >= iRecentSetup.Count())
			    {
				break;
			    }
            RArray<TLogId> logIds;
            ::LogGetRecentEventsLC(iDatabase, iRecentSetup.List(iRecent).Id(), iConfig->iMaxRecentLogSize, logIds); 
            ::LogPurgeRecentEventsL(iDatabase, logIds);
            CleanupStack::PopAndDestroy(&logIds);
            // Purge next recent list?
            if (++iRecent < iRecentSetup.Count())
                {
                iState = ELogPurgeRecent;
                TRequestStatus* status = &iStatus;
                User::RequestComplete(status, KErrNone);
                SetActive();
                }
            break;
			}
		default:
			__ASSERT_DEBUG(ETrue, Panic(ELogBadState6));
			break;
		}
	}

void CLogChangeConfig::DoComplete(TInt& aStatus)
	{
	// Try and commit the transaction
	if (iDatabase.DTIInTransaction())
		{
		if	(aStatus == KErrNone)
			aStatus = iDatabase.DTICommitAndEnd();

		if	(aStatus < KErrNone)
			{
			iDatabase.DTICacheConfig().Rollback();
			iDatabase.DTIRollBack();
			}
		else
		    {
		    iDatabase.DTICacheConfig().Commit();
		    }
		}
    __ASSERT_DEBUG(!iDatabase.DTICacheConfig().InTransaction(), Panic(ELogChangeConfigLogicError));
	iState = ELogNone;
	}
