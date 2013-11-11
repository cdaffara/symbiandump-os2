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

#include "LOGQUERY.H"
#include "logservpanic.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServSqlStrings.h"
#include "LogDynBuf.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         RLogDbTable             /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
RLogDbTable "resource acquisition" method.
Opens the specified database table with the required access mode.
If the database indexes are damaged, before the "table open" operation, an attempt will be made to recover the databasde.
If the table is opened successfully, the current RLogDbTable object will be put on the cleanup stack. The caller is
responsible for the destruction of the RLogDbTable object. 

@param aDb RDbDatabase reference
@param aTblName Table name
@param aAccess Table access mode, one of RDbRowSet::TAccess enum item values

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
void RLogDbTable::OpenLC(RDbDatabase& aDb, const TDesC& aTblName, RDbRowSet::TAccess aAccess)
    {
    if(aDb.IsDamaged())
        {
        User::LeaveIfError(aDb.Recover());
        }
    __ASSERT_DEBUG(!aDb.IsDamaged(), Panic(ELogDatabaseDamaged2));
    CleanupClosePushL(*this);
    User::LeaveIfError(RDbTable::Open(aDb, aTblName, aAccess));
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         RLogEventDbTable             ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TDbColNo RLogEventDbTable::iIdColNo = 0;
TDbColNo RLogEventDbTable::iTypeColNo = 0;
TDbColNo RLogEventDbTable::iRemotePartyColNo = 0;
TDbColNo RLogEventDbTable::iDirectionColNo = 0;
TDbColNo RLogEventDbTable::iTimeColNo = 0;
TDbColNo RLogEventDbTable::iDurationTypeColNo = 0;
TDbColNo RLogEventDbTable::iDurationColNo = 0;
TDbColNo RLogEventDbTable::iStatusColNo = 0;
TDbColNo RLogEventDbTable::iSubjectColNo = 0;
TDbColNo RLogEventDbTable::iNumberColNo = 0;
TDbColNo RLogEventDbTable::iContactColNo = 0;
TDbColNo RLogEventDbTable::iLinkColNo = 0;
TDbColNo RLogEventDbTable::iDataColNo = 0;
TDbColNo RLogEventDbTable::iFlagColNo[] = {0, 0, 0, 0};
TDbColNo RLogEventDbTable::iRecentColNo = 0;
TDbColNo RLogEventDbTable::iDuplicateColNo = 0;
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
TDbColNo RLogEventDbTable::iSimIdColNo = 0;
#endif

/**
RLogEventDbTable "resource acquisition" method.
Opens the "Event" database table with the required access mode.
If the database indexes are damaged, before the "table open" operation, an attempt will be made to recover the databasde.
If the table is opened successfully, the current RLogEventDbTable object will be put on the cleanup stack. The caller is
responsible for the destruction of the RLogEventDbTable object. 

@param aDb RDbDatabase reference
@param aAccess Table access mode, one of RDbRowSet::TAccess enum item values

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
void RLogEventDbTable::OpenLC(RDbDatabase& aDb, RDbRowSet::TAccess aAccess)
    {
    RLogDbTable::OpenLC(aDb, KLogNameEventString, aAccess);
    InitializeColumnsL();
    }

/**
Initializes the static data members ("Event" table column numbers) of the RLogEventDbTable class.
The initialization happens just once, during the construction of the first object of RLogEventDbTable type. 

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
void RLogEventDbTable::InitializeColumnsL()
    {
    if(RLogEventDbTable::iIdColNo == 0)
        {
        CDbColSet* colset = ColSetL();
        RLogEventDbTable::iIdColNo = colset->ColNo(KLogFieldIdString);
        RLogEventDbTable::iTypeColNo = colset->ColNo(KLogFieldEventTypeString);
        RLogEventDbTable::iRemotePartyColNo = colset->ColNo(KLogFieldEventRemoteString);
        RLogEventDbTable::iDirectionColNo = colset->ColNo(KLogFieldEventDirectionString);
        RLogEventDbTable::iTimeColNo = colset->ColNo(KLogFieldEventTimeString);
        RLogEventDbTable::iDurationTypeColNo = colset->ColNo(KLogFieldEventDTypeString);
        RLogEventDbTable::iDurationColNo = colset->ColNo(KLogFieldEventDurationString);
        RLogEventDbTable::iStatusColNo = colset->ColNo(KLogFieldEventStatusString);
        RLogEventDbTable::iSubjectColNo = colset->ColNo(KLogFieldEventSubjectString);
        RLogEventDbTable::iNumberColNo = colset->ColNo(KLogFieldEventNumberString);
        RLogEventDbTable::iContactColNo = colset->ColNo(KLogFieldEventContactString);
        RLogEventDbTable::iLinkColNo = colset->ColNo(KLogFieldEventLinkString);
        RLogEventDbTable::iDataColNo = colset->ColNo(KLogFieldEventDataString);
        for(TInt i=0;i<KLogFlagsCount;++i)
            {
            TDbColName colname;
            colname.Format(KLogFieldEventFlagString, i + 1);
            RLogEventDbTable::iFlagColNo[i] = colset->ColNo(colname);
            __ASSERT_DEBUG(RLogEventDbTable::iFlagColNo[i] > 0, User::Invariant());
            }
        RLogEventDbTable::iRecentColNo = colset->ColNo(KLogFieldEventRecentString);
        RLogEventDbTable::iDuplicateColNo = colset->ColNo(KLogFieldEventDuplicateString);
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
        RLogEventDbTable::iSimIdColNo = colset->ColNo(KLogFieldEventSimId);
#endif        
        delete colset;
        }
#ifdef SYMBIAN_ENABLE_EVENTLOGGER_DUALSIM	
    __ASSERT_DEBUG(RLogEventDbTable::iIdColNo > 0 && 
            RLogEventDbTable::iTypeColNo > 0 &&
            RLogEventDbTable::iRemotePartyColNo > 0 &&
            RLogEventDbTable::iDirectionColNo > 0 &&
            RLogEventDbTable::iTimeColNo > 0 &&
            RLogEventDbTable::iDurationTypeColNo > 0 &&
            RLogEventDbTable::iDurationColNo > 0 &&
            RLogEventDbTable::iStatusColNo > 0 &&
            RLogEventDbTable::iSubjectColNo > 0 &&
            RLogEventDbTable::iNumberColNo > 0 &&
            RLogEventDbTable::iContactColNo > 0 &&
            RLogEventDbTable::iLinkColNo > 0 &&
            RLogEventDbTable::iDataColNo > 0 && 
            RLogEventDbTable::iRecentColNo > 0 &&
            RLogEventDbTable::iDuplicateColNo > 0 && 
            RLogEventDbTable::iSimIdColNo > 0, User::Invariant());
#else
    __ASSERT_DEBUG(RLogEventDbTable::iIdColNo > 0 && 
            RLogEventDbTable::iTypeColNo > 0 &&
            RLogEventDbTable::iRemotePartyColNo > 0 &&
            RLogEventDbTable::iDirectionColNo > 0 &&
            RLogEventDbTable::iTimeColNo > 0 &&
            RLogEventDbTable::iDurationTypeColNo > 0 &&
            RLogEventDbTable::iDurationColNo > 0 &&
            RLogEventDbTable::iStatusColNo > 0 &&
            RLogEventDbTable::iSubjectColNo > 0 &&
            RLogEventDbTable::iNumberColNo > 0 &&
            RLogEventDbTable::iContactColNo > 0 &&
            RLogEventDbTable::iLinkColNo > 0 &&
            RLogEventDbTable::iDataColNo > 0 && 
            RLogEventDbTable::iRecentColNo > 0 &&
            RLogEventDbTable::iDuplicateColNo > 0, User::Invariant());
#endif
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         RLogConfigDbTable             ///////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TDbColNo RLogConfigDbTable::iSizeColNo = 0;
TDbColNo RLogConfigDbTable::iRecentColNo = 0;
TDbColNo RLogConfigDbTable::iAgeColNo = 0;

/**
RLogConfigDbTable "resource acquisition" method.
Opens the "Config" database table with the required access mode.
If the database indexes are damaged, before the "table open" operation, an attempt will be made to recover the databasde.
If the table is opened successfully, the current RLogConfigDbTable object will be put on the cleanup stack. The caller is
responsible for the destruction of the RLogConfigDbTable object. 

@param aDb RDbDatabase reference
@param aAccess Table access mode, one of RDbRowSet::TAccess enum item values

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
void RLogConfigDbTable::OpenLC(RDbDatabase& aDb, RDbRowSet::TAccess aAccess)
    {
    RLogDbTable::OpenLC(aDb, KLogNameConfigString, aAccess);
    InitializeColumnsL();
    }

/**
Initializes the static data members ("Config" table column numbers) of the RLogConfigDbTable class.
The initialization happens just once, during the construction of the first object of RLogConfigDbTable type. 

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
void RLogConfigDbTable::InitializeColumnsL()
    {
    if(RLogConfigDbTable::iSizeColNo == 0)
        {
        CDbColSet* colset = ColSetL();
        RLogConfigDbTable::iSizeColNo = colset->ColNo(KLogFieldConfigSizeString);
        RLogConfigDbTable::iRecentColNo = colset->ColNo(KLogFieldConfigRecentString);
        RLogConfigDbTable::iAgeColNo = colset->ColNo(KLogFieldConfigAgeString);
        delete colset;
        }
    __ASSERT_DEBUG(RLogConfigDbTable::iSizeColNo > 0 && 
            RLogConfigDbTable::iRecentColNo > 0 &&
            RLogConfigDbTable::iAgeColNo > 0, User::Invariant());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         RLogDbView             //////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
RLogDbView "resource acquisition" method.
Prepares a database view with the passed as a parameter SQL and with the required access mode.
If the database indexes are damaged, before the "prepare view" operation, an attempt will be made to recover the databasde.
If the view is prepared successfully, the current RLogDbView object will be put on the cleanup stack and all records
evaluated. The caller is responsible for the destruction of the RLogDbView object. 

@param aDb RDbDatabase reference
@param aQuery View SQL statement 
@param aAccess View access mode, one of RDbRowSet::TAccess enum item values

@leave  KErrNoMemory, an out of memory condition has occurred;
                      Note that the function may leave with database specific errors and
                      other system-wide error codes.
*/
void RLogDbView::PrepareLC(RDbDatabase& aDb, const TDesC& aQuery, RDbRowSet::TAccess aAccess)
	{
	if(aDb.IsDamaged())
		{
		User::LeaveIfError(aDb.Recover());
		}
	__ASSERT_DEBUG(!aDb.IsDamaged(), Panic(ELogDatabaseDamaged2));
	CleanupClosePushL(*this);
	User::LeaveIfError(RDbView::Prepare(aDb, TDbQuery(aQuery, EDbCompareFolded), aAccess));
	User::LeaveIfError(RDbView::EvaluateAll());
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////         Global functions             ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
Runs the KLogSqlGetRecent SQL query and puts the IDs of the retrieved events into the aEventIds output array.
This happens only if the retrieved events count is bigger than the aMaxRecentLogSize parameter. 

@param aDb A MLogServDatabaseTransactionInterface reference
@param aRecentListId Recent list Id
@param aMaxRecentLogSize Max recent list size
@param aEventIds Output parameter. The function will put it on the cleanup stack and fill it with
                 the events ids from the recent list. The caller is responsible for the destruction of
                 the aEventIds parameter.

@leave  KErrNoMemory, an out of memory condition has occurred;
               Note that the function may leave with database specific errors and
                other system-wide error codes.

@internalComponent 
*/
void LogGetRecentEventsLC(MLogServDatabaseTransactionInterface& aDb, TLogRecentList aRecentListId, 
                          TLogRecentSize aMaxRecentLogSize, RArray<TLogId>& aEventIds)
    {
    CleanupClosePushL(aEventIds);
    TheSql.Format(KLogSqlGetRecent, aRecentListId);
    RLogDbView view;                
    view.PrepareLC(aDb.DTIDatabase(), TheSql, RDbRowSet::EReadOnly);
    TInt count = view.CountL() - aMaxRecentLogSize;
    if(count > 0)
        {
        (void)view.LastL();//If "count > 0", then there is at least one record => LastL() cannot return EFalse. 
        static TDbColNo idColNo = 0;
        if(idColNo == 0)
            {
            CDbColSet* colset = view.ColSetL();
            idColNo = colset->ColNo(KLogFieldIdString);
            delete colset;
            }
        aEventIds.ReserveL(count);
        do
            {
            view.GetL();
            aEventIds.AppendL(view.ColInt32(idColNo));
            }
        while(--count && view.PreviousL());
        }
    CleanupStack::PopAndDestroy(&view);
    }

/**
The function accepts an array of event IDs as a parameter, prepares an UPDATE SQL query using those IDs and
puts the constructed query into aSqlBuf output parameter.
 
@param aEventIds Array with event Ids, used for the construction of the SQL statement
@param aSqlBuf Output parameter. A reference to RLogDynBuf object where the SQL is constructed.
 
@leave  KErrNoMemory, an out of memory condition has occurred;
 
@internalComponent 
*/
static void LogBuildPurgeRecentSqlL(const RArray<TLogId>& aEventIds, RLogDynBuf& aSqlBuf)
    {
    __ASSERT_DEBUG(aEventIds.Count() > 0, User::Invariant());
    aSqlBuf.SetLength(0);
    aSqlBuf.AppendL(KLogSqlRemoveDuplicateEvents);
    for(TInt i=0,count=aEventIds.Count();i<count;++i)
        {
        TBuf<20> num;//buf size of 20 is enough for a 32-bit number
        num.AppendNum(aEventIds[i]);
        aSqlBuf.AppendL(KIdEqStr);
        aSqlBuf.AppendL(num);
        aSqlBuf.AppendL(KLogOr);
        aSqlBuf.AppendL(KDuplicateEqStr);
        aSqlBuf.AppendL(num);
        aSqlBuf.AppendL(KLogOr);
        }
    aSqlBuf.SetLength(aSqlBuf.Length() - KLogOr().Length()); 
    }

/**
The function accepts an array of event IDs as a parameter, prepares an UPDATE SQL query and executes
the query.
The MLogServDatabaseChangeInterface interface will be used to collect the information about the IDs of the purged events.
Later that information will be used if there are any outstanding notification requests waiting for completion.  
If the count of the aEventIds elements is 0, then no query will be prepared and executed.
  
@param aDb A reference to MLogServDatabaseTransactionInterface interface
@param aEventIds Array with event Ids, used for the construction of the SQL statement
  
@leave  KErrNoMemory, an out of memory condition has occurred;
               Note that the function may leave with database specific errors and
                other system-wide error codes.
  
@internalComponent 
*/
void LogPurgeRecentEventsL(MLogServDatabaseTransactionInterface& aDb, const RArray<TLogId>& aEventIds)
    {
    TInt count = aEventIds.Count();
    if(count == 0)
        {
        return;
        }
    RLogDynBuf sqlBuf;
    sqlBuf.CreateLC(sizeof(KLogSqlRemoveDuplicateEvents) + count * 32);//32 - approx - length of "Duplicate=N OR Id=N"
    LogBuildPurgeRecentSqlL(aEventIds, sqlBuf);
    User::LeaveIfError(aDb.DTIExecuteSql(sqlBuf.DesC()));
    CleanupStack::PopAndDestroy(&sqlBuf);
    for(TInt i=0;i<count;++i)
        {
        // This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
        aDb.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, aEventIds[i]);
        }
    }

/**
If the number of the events in the "Event" table is bigger than the aMaxLogSize parameter, 
the oldest events will be deleted from the table.
The MLogServDatabaseChangeInterface interface will be used to collect the information about the IDs of the deleted events.
Later that information will be used if there are any outstanding notification requests waiting for completion.  
If the number of the events in the "Event" table is less than the aMaxLogSize parameter, then the function does nothing. 
 
@param aDb A reference to MLogServDatabaseTransactionInterface interface
@param aTbl A reference to RLogEventDbTable object
@param aMaxLogSize The max number of events allowed to exist in the "Event" table
@param aCountPlus Integer, added to aMaxLogSize during the "max log size" calculations 
  
@leave  KErrNoMemory, an out of memory condition has occurred;
               Note that the function may leave with database specific errors and
                other system-wide error codes.
 
@internalComponent 
*/
void LogPurgeMainL(MLogServDatabaseTransactionInterface& aDb, RLogEventDbTable& aTbl, 
                   TLogSize aMaxLogSize, TInt aCountPlus)
    {
    User::LeaveIfError(aTbl.SetIndex(KLogNameEventIdx1));
    TInt count = aTbl.CountL() + aCountPlus - aMaxLogSize;
    if(count > 0)
        {
        (void)aTbl.FirstL();//If "count > 0", then there is at least one record => FirstL() cannot return EFalse. 
        TBool commit = !aDb.DTIInTransaction();
        if(commit)
            {
            aDb.DTIBeginWithRollBackProtectionLC();
            }
        do
            {
            aTbl.GetL(); 
            TLogId id = aTbl.ColInt32(RLogEventDbTable::iIdColNo);
            aTbl.DeleteL();
            aDb.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventDeleted, id);
            }
        while(--count && aTbl.NextL());
        if(commit)
            {
            aDb.DTICommitAndCancelRollbackProtectionL();
            }
        }
    }

/**
Updates the event records with "Duplicate" column value equal to aEventId. 
The MLogServDatabaseChangeInterface interface will be used to collect the information about the IDs of the modified events.
Later that information will be used if there are any outstanding notification requests waiting for completion.
If no duplicates of the passed as a parameter event ID exist, then the function does nothing.  
 
@param aDb A reference to MLogServDatabaseTransactionInterface interface
@param aEventId Duplicated event id 

@leave  KErrNoMemory, an out of memory condition has occurred;
               Note that the function may leave with database specific errors and
                other system-wide error codes.
 
@internalComponent 
*/
void LogResetDuplicatesL(MLogServDatabaseTransactionInterface& aDb, TLogId aEventId)
    {
    TheSql.Format(KLogSqlDuplicateViewString, aEventId, &KNullDesC);
    RLogDbView view;
    view .PrepareLC(aDb.DTIDatabase(), TheSql);
    // Are there any duplicates?
    if(view.FirstL())
        {
        static TDbColNo idColNo = 0;
        static TDbColNo duplicateColNo = 0;
        if(idColNo == 0)
            {
            CDbColSet* colset = view.ColSetL();
            idColNo = colset->ColNo(KLogFieldIdString);
            duplicateColNo = colset->ColNo(KLogFieldEventDuplicateString);
            delete colset;
            }
        TBool commit = !aDb.DTIInTransaction();
        if(commit)
            {
            aDb.DTIBeginWithRollBackProtectionLC();
            }
        // Get the id of the latest event
        view.GetL();
        const TLogId idLatest = view.ColInt32(idColNo);
        // Mark the event as the latest duplicate
        view.UpdateL();
        view.SetColNullL(duplicateColNo);
        view.PutL();
        // This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
        aDb.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, idLatest);
        // Reset the duplicate id's of the other duplicates
        while(view.NextL())
            {
            view.UpdateL();
            const TLogId id = view.ColInt32(idColNo);
            view.SetColL(duplicateColNo, idLatest);
            view.PutL();
            // This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
            aDb.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, id);
            }
        if(commit)
            {
            aDb.DTICommitAndCancelRollbackProtectionL();
            }
        }
    CleanupStack::PopAndDestroy(&view);
    }
