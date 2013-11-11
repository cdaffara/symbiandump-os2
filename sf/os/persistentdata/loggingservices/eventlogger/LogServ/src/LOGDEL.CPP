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

#include "LOGDEL.H"
#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServResourceInterpreter.h"
#include "LogServCacheTypes.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServSqlStrings.h"

CLogDeleteEvent::CLogDeleteEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
: CLogActive(aPriority), iDatabase(aDatabase)
	{
	}

CLogDeleteEvent::~CLogDeleteEvent()
	{
	Cancel();
	}

CLogDeleteEvent* CLogDeleteEvent::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	return new (ELeave) CLogDeleteEvent(aDatabase, aPriority);
	}
	

void CLogDeleteEvent::StartL(TLogId aId, TRequestStatus& aStatus, const RMessage2& aMessage)
	{
	__ASSERT_DEBUG(!IsActive(), Panic(ELogAlreadyActive6));
	
	iMessage = const_cast<RMessage2*>(&aMessage);	// how to avoid this cast?
	iId = aId;
	
    Queue(aStatus);
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
    SetActive();
	}

void CLogDeleteEvent::DoRunL()
	{
	RLogEventDbTable tbl;
	tbl.OpenLC(iDatabase.DTIDatabase());
    User::LeaveIfError(tbl.SetIndex(KLogNameEventIdx1));
    //Find event
    if(!tbl.SeekL(TDbSeekKey((TInt)iId)))
        {
        iMessage->Complete(KErrNotFound);
        DoCancel();
        User::Leave(KErrNotFound);
        }
    tbl.GetL();
    //Platsec check
    TLogTypeId logTypeId = tbl.ColUint8(RLogEventDbTable::iTypeColNo);
    const TLogServCacheTypeEntry& entry = iDatabase.DTICacheTypes().FindById(logTypeId);
    __ASSERT_DEBUG(entry.iEventTypeId != KLogNullTypeId, Panic(ELogNoEventTypeAtId));
    if(!iDatabase.DTIIsAllowed(EWriteOp, *iMessage, entry.iEventType->Uid()))
        {
        User::Leave(KErrPermissionDenied);
        }
    User::LeaveIfError(iDatabase.DTIBegin());
    //Process duplicates 
    ::LogResetDuplicatesL(iDatabase, iId);
    //Delete the event
    tbl.DeleteL();
    iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventDeleted, iId);
    //
    CleanupStack::PopAndDestroy(&tbl);
	}


void CLogDeleteEvent::DoCancel()
	{
	CLogActive::DoCancel();
	}

void CLogDeleteEvent::DoComplete(TInt& aStatus)
	{
	// Attempt to commit the transaction
	if (iDatabase.DTIInTransaction())
		{
		if (aStatus == KErrNone)
			aStatus = iDatabase.DTICommitAndEnd();

		if (aStatus < KErrNone)
			iDatabase.DTIRollBack();
		}
	}
