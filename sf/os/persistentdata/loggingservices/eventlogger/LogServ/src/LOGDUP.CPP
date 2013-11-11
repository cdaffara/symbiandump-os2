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

#include "LOGDUP.H"
#include "LOGFILTQ.H"
#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServResourceInterpreter.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServSqlStrings.h"

CLogDuplicate::CLogDuplicate(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority) :
    CLogActive(aPriority), 
    iDatabase(aDatabase)
	{
	}

CLogDuplicate::~CLogDuplicate()
	{
	Cancel();
	delete iFilterList;
	}

void CLogDuplicate::ConstructL()
	{
	iFilterList = new(ELeave) CLogFilterList;
	}

CLogDuplicate* CLogDuplicate::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	CLogDuplicate* self = new(ELeave) CLogDuplicate(aDatabase, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

TBool CLogDuplicate::StartL(TLogId aId, TLogRecentList aRecentList, const CLogFilter& aFilter, TRequestStatus& aStatus)
	{
	if (aRecentList == KLogNullRecentList)
		return EFalse;

	iId = aId;
	iRecentList = aRecentList;
	iFilterList->Reset();
	iFilterList->AppendL(&aFilter);

	Queue(aStatus);
    TRequestStatus* status = &iStatus;
    User::RequestComplete(status, KErrNone);
	SetActive();
	return ETrue;
	}

void CLogDuplicate::DoRunL()
	{
    RLogDynBuf expr;
    TLogFilterExprBuilder exprBuilder(iDatabase);
    exprBuilder.BuildExprLC(expr, *iFilterList, KLogAnd);
    TheSql.Format(KLogSqlSelectDuplicateString, iRecentList, &expr.DesC(), iId);
    CleanupStack::PopAndDestroy(&expr);

    RLogDbView view;
    view.PrepareLC(iDatabase.DTIDatabase(), TheSql);
    if(view.FirstL())
        {
        // Begin a transaction
        TBool inTransaction = iDatabase.DTIInTransaction();
        if(!inTransaction)
            {
            iDatabase.DTIBeginWithRollBackProtectionLC();
            }
        // Get column ids
        static TDbColNo idColNo = 0;
        static TDbColNo duplicateColNo = 0;
        if(idColNo == 0)
            {
            CDbColSet* cs = view.ColSetL();
            idColNo = cs->ColNo(KLogFieldIdString);
            duplicateColNo = cs->ColNo(KLogFieldEventDuplicateString);
            delete cs;
            }
        // Iterate through the events
        do
            {
            // Get current event id
            view.GetL();
            const TLogId id = view.ColInt32(idColNo);
            // Set the latest recent?
            if(iId < 0)
                {
                iId = id;
                }
            // Make the change
            view.UpdateL();
            iId == id ? view.SetColNullL(duplicateColNo) : view.SetColL(duplicateColNo, iId); 
            view.PutL();
            // This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
            iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, id);
            }
        while(view.NextL());
        // Commit changes
        if(!inTransaction)
            {
            iDatabase.DTICommitAndCancelRollbackProtectionL();
            }
        }
    CleanupStack::PopAndDestroy(&view);
	}

void CLogDuplicate::DoComplete(TInt& aStatus)
	{
	// Ignoring all errors because if an error occurs whilst detecting duplicate events 
	// it should not stop us actually adding the event to the log
	aStatus = KErrNone;
	}
