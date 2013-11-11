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

#include <logcli.h>
#include <logwraplimits.h>
#include "logcntdef.h"
#include "LOGGET.H"
#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServCacheStrings.h"
#include "LogServCacheTypes.h"
#include "LogServResourceInterpreter.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServSqlStrings.h"

CLogGetEvent::CLogGetEvent(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority) :
    CLogActive(aPriority),
    iDatabase(aDatabase)
	{
	}

CLogGetEvent::~CLogGetEvent()
	{
	Cancel();
	}

CLogGetEvent* CLogGetEvent::NewL(MLogServDatabaseTransactionInterface& aDatabase, TInt aPriority)
	{
	return new (ELeave) CLogGetEvent(aDatabase, aPriority);
	}

void CLogGetEvent::StartL(CLogEvent& aEvent, TRequestStatus& aStatus, const RMessage2& aMessage)
	{
	LOGTEXT2("CLogGetEvent::StartL() - Id: %d", aEvent.Id());
	if(aEvent.Id() == KLogNullId)
		{
		::PanicClientL(aMessage, ELogNotValid);
		}
	
    RLogEventDbTable tbl;
    tbl.OpenLC(iDatabase.DTIDatabase());
    User::LeaveIfError(tbl.SetIndex(KLogNameEventIdx1));

    if(!tbl.SeekL(TDbSeekKey((TInt)aEvent.Id())))
        {
        User::Leave(KErrNotFound);
        }
    
    tbl.GetL();
    
    TLogTypeId logTypeId = tbl.ColInt16(RLogEventDbTable::iTypeColNo);
    const TLogServCacheTypeEntry& entry = iDatabase.DTICacheTypes().FindById(logTypeId);
    if(!iDatabase.DTIIsAllowed(EReadOp, aMessage, entry.iEventType->Uid()))
        {
        User::Leave(KErrPermissionDenied);
        }
    aEvent.SetEventType(entry.iEventType->Uid());
    aEvent.SetDescription(entry.iEventType->Description());
    
    TLogStringId directionId = tbl.IsColNull(RLogEventDbTable::iDirectionColNo) ? KLogNullStringId : tbl.ColInt16(RLogEventDbTable::iDirectionColNo);
    aEvent.SetDirection(iDatabase.DTICacheStrings().FindString(directionId));
    
    TLogStringId statusId = tbl.IsColNull(RLogEventDbTable::iStatusColNo) ? KLogNullStringId : tbl.ColInt16(RLogEventDbTable::iStatusColNo);
    aEvent.SetStatus(iDatabase.DTICacheStrings().FindString(statusId));
	
    aEvent.SetRemoteParty(tbl.ColDes(RLogEventDbTable::iRemotePartyColNo));
    aEvent.SetTime(tbl.ColTime(RLogEventDbTable::iTimeColNo));
    aEvent.SetDurationType(tbl.IsColNull(RLogEventDbTable::iDurationTypeColNo) ? KLogNullDurationType : tbl.ColInt8(RLogEventDbTable::iDurationTypeColNo));
    aEvent.SetDuration(tbl.ColUint32(RLogEventDbTable::iDurationColNo));
    aEvent.SetSubject(tbl.ColDes(RLogEventDbTable::iSubjectColNo));
    aEvent.SetNumber(tbl.ColDes(RLogEventDbTable::iNumberColNo));
    aEvent.SetContact(tbl.IsColNull(RLogEventDbTable::iContactColNo) ? KLogNullContactId : tbl.ColInt32(RLogEventDbTable::iContactColNo));
    aEvent.SetLink(tbl.ColUint32(RLogEventDbTable::iLinkColNo));
    if(tbl.IsColNull(RLogEventDbTable::iDataColNo))
        {
        aEvent.SetDataL(KNullDesC8);
        }
    else
        {
        RDbColReadStream stream;
        stream.OpenLC(tbl, RLogEventDbTable::iDataColNo);
        aEvent.SetDataL(stream, tbl.ColLength(RLogEventDbTable::iDataColNo));
        CleanupStack::PopAndDestroy(&stream);
        }
    
    aEvent.ClearFlags(KLogFlagsMask);
    for(TInt i=0;i<KLogFlagsCount;++i)
        {
        aEvent.SetFlags((TLogFlags)(tbl.ColUint8(RLogEventDbTable::iFlagColNo[i]) ? 0x1 << i : 0));
        }
    
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
    aEvent.SetSimId(tbl.ColUint32(RLogEventDbTable::iSimIdColNo));
#endif    
    
    CleanupStack::PopAndDestroy(&tbl);
    
    TRequestStatus* st = &aStatus;
    User::RequestComplete(st, KErrNone);
	LOGTEXT("CLogGetEvent::StartL() - end");
	}

//CLogGetEvent::StartL() does all job
#pragma BullseyeCoverage off

void CLogGetEvent::DoRunL()
	{
	LOGTEXT("CLogGetEvent::DoRunL() - begin");
	__ASSERT_DEBUG(0, User::Invariant());
	LOGTEXT("CLogGetEvent::DoRunL() - end");
	}

#pragma BullseyeCoverage on
