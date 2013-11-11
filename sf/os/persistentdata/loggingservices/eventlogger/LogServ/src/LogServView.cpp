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
#include "LogServView.h"
#include "logpackage.h"
#include "logservpanic.h"
#include "LogServBackupInterface.h"
#include "LogServViewChangeManager.h"
#include "LogServDatabaseChangeInterface.h"
#include "LogServCacheTypes.h"
#include "LogServSqlStrings.h"
#include "LOGFILTQ.H"

// Constants
const TInt KLogViewContentsGranuality = 20;
const TInt KLogViewLockStatusEventGranularity = 3;

TDbColNo CLogServViewBase::iIdColNo = 0;
TDbColNo CLogServViewBase::iTypeColNo = 0;
TDbColNo CLogServViewBase::iFlagColNo[] = {0, 0, 0, 0};

TDbColNo CLogServViewRecent::iIdColNo = 0;
TDbColNo CLogServViewRecent::iRecentColNo = 0;
TDbColNo CLogServViewRecent::iDuplicateColNo = 0;

//diagnostic message for the platform security
const char* KIgnoreDiagnostic = "This diagnostic message does not indicate an error, please ignore it";

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewBase (source)
/////////////////////////////////////////////////////////////////////////////////////////
CLogServViewBase::CLogServViewBase(MLogServDatabaseTransactionInterface& aDatabase, 
                                   MLogServBackupInterface& aBackupInterface, 
                                   CLogPackage& aPackage, TLogViewType aType, TLogViewId aViewId, 
                                   const RMessage2& aMessage) :
    iDatabase(aDatabase), 
    iPackage(aPackage),
    iBackupInterface(aBackupInterface), 
    iType(aType), 
    iViewId(aViewId), 
    iMessage (aMessage)
	{
	}

CLogServViewBase::~CLogServViewBase()
	{
	iDatabase.DTIChangeInterface().DCIRequestChangeNotificationsCancel(*this);
	iBackupInterface.BIObserverRemove(*this);
	//
	iViewContents.Close();
	//	
	iStandardTypeSecurityCache.Close();
	//
	delete iLockChangeObserver;
	delete iSql;
	delete iChangeManager;
	}


void CLogServViewBase::ConstructL()
	{
	// Handles changes for this view
	iChangeManager = CLogServViewChangeManager::NewL(iDatabase.DTIChangeInterface());

	// Register for change events
	iDatabase.DTIChangeInterface().DCIRequestChangeNotificationsL(*this);

	// Register for backup events
	iBackupInterface.BIObserverAddL(*this, MLogServBackupInterface::EObjectView);

	// Observes when the view is locked/unlocked due to a backup
	iLockChangeObserver = CLogServViewLockObserver::NewL(iBackupInterface);

	const RArray<TUid>& arrTUids = iDatabase.DTIUidsOfStandardTypes();
	TInt count = arrTUids.Count();
	iStandardTypeSecurityCache.ReserveL(count);
	for(TInt i=0; i < count; i++)
		{
		SStandardTypeSecurity securitySetting;
		securitySetting.eventType = arrTUids[i];
		securitySetting.readAccess = iDatabase.DTIIsAllowed(EReadOp, iMessage, arrTUids[i], KIgnoreDiagnostic);
		securitySetting.writeAccess = iDatabase.DTIIsAllowed(EWriteOp, iMessage, arrTUids[i], KIgnoreDiagnostic);
		TInt err = iStandardTypeSecurityCache.Append(securitySetting);	
        __ASSERT_ALWAYS(err == KErrNone, Panic(ELogArrayReserved));
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewBase::DCOHandleChangeEventsL(const CLogChangeDefinition& aChanges)
	{
	// Just return if the view isn't setup
	if	(!iSql)
		return;

	TRAPD(error, DoHandleChangeEventsL(aChanges));
	if	(error != KErrNone)
		{
		iViewContents.Close();
	    iViewContentsReady = EFalse;
		iRebuildViewContents = ETrue;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewBase::BOHandleEventL(TLogServBackupEvent aEvent)
	{
	switch(aEvent)
		{
	case EBackupStarting:
		iViewContents.Close();
	    iViewContentsReady = EFalse;
		break;
	case EBackupEnded:
		RebuildViewL();
		break;
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

#pragma BullseyeCoverage off

/**
Remove an entry from the view.
By default you can't remove an event from a view.
*/
void CLogServViewBase::RemoveL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrNotSupported);
	}

/**
By default you can't clear duplicates from a view
*/
void CLogServViewBase::ClearDuplicatesL(const RMessage2& /*aMessage*/)
	{
	User::Leave(KErrNotSupported);
	}

#pragma BullseyeCoverage on

/**
Set the flags of all the entries in the view.
*/
void CLogServViewBase::SetFlagsL(const RMessage2& aMessage)
	{
	if	(ViewIsReady())
		{
		// Flags from client
		const TLogFlags flags = static_cast<TLogFlags>(aMessage.Int2());
		RLogDbView view;
		view.PrepareLC(iDatabase.DTIDatabase(), *iSql);
		InitializeColumnsL(view);
		if(view.FirstL())
			{
            iDatabase.DTIBeginWithRollBackProtectionLC();
			// Iterate through the events
			do
				{
				// Get current event id
				view.GetL();
				const TLogId id = view.ColInt32(CLogServViewBase::iIdColNo);
				TUint8 eventTypeIndex = view.ColUint8(CLogServViewBase::iTypeColNo);
				if(IsAllowed(EWriteOp, eventTypeIndex))
					{
					// Make the change
					view.UpdateL();
		            TInt bit = KLogFlagsCount;
		            while(bit--)
		                {
                        const TBool flagIsSet = flags & (0x1 << bit) ? 1 : 0;
		                view.SetColL(CLogServViewBase::iFlagColNo[bit], flagIsSet);
		                }
					view.PutL();
					iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChanged, id);
					}
				}
			while(view.NextL());
			iDatabase.DTICommitAndCancelRollbackProtectionL();
			}
		CleanupStack::PopAndDestroy(&view);
		}
	else
		::PanicClientL(aMessage, ELogViewRecentViewNotYetReadyForFlagSetting);
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
The client has requested change notifications
*/
void CLogServViewBase::RequestChangeNotifications(const RMessage2& aMessage)
	{
	iChangeManager->RequestChangeNotifications(aMessage);
	}

/**
The client has cancelled a previous change notification request
*/
void CLogServViewBase::RequestChangeNotificationsCancel()
	{
	iChangeManager->RequestChangeNotificationsCancel();
	}

/**
The client has requested lock status change notifications
*/
void CLogServViewBase::RequestLockStatusChanges(const RMessage2& aMessage)
	{
	iLockChangeObserver->RequestLockStatusChanges(aMessage);
	}

/**
The client has cancelled a previous change notification request
*/
void CLogServViewBase::RequestLockStatusChangesCancel()
	{
	iLockChangeObserver->RequestLockStatusChangesCancel();
	}

/**
The client has requested the current list of changes
*/
void CLogServViewBase::RequestChangesL(const RMessage2& aMessage)
	{
	if	(!iSql)
		::PanicClientL(aMessage, ELogViewNotSetupForChangesFetch);
	else
		iChangeManager->DeliverChangesL(aMessage);
	}

/**
Set up the server-side view based upon the client-side filter. This
method essentially fetches a client-side filter and then dynamically
executes the associated SELECT statement on the database to build up
a view. This view is then cached in terms of the entry ids (iViewContents).
*/
void CLogServViewBase::SetupL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType)
	{
	// Get the query string
	const TPtrC pSQL(GetQueryStringL(aMessage, aFilterType));
	HBufC* buf = pSQL.AllocLC();
	// Setup the contents
	PrepareViewContentsL(pSQL);
	// Tidy up
	CleanupStack::Pop(buf);
	delete iSql;
	iSql = buf;
	}

/**
Returns the number of entries in this view
*/
TInt CLogServViewBase::Count() const
	{
	return const_cast<CLogServViewBase*>(this)->RebuildViewContentsIfNecessary() ? 0 : iViewContents.Count(); 
	}

/**
Get the log id at the specified index
*/
TLogId CLogServViewBase::At(TInt aIndex) const
	{
	return (TUint)aIndex < iViewContents.Count() ? iViewContents[aIndex] : KLogNullId;   
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewBase::DestroyList(TAny *aPtr)
	{
	CLogFilterList* filter = reinterpret_cast<CLogFilterList*>(aPtr);
	filter->ResetAndDestroy();
	delete filter;
	}

void CLogServViewBase::InitializeColumnsL(RDbRowSet& aRowSet)
    {
    if(CLogServViewBase::iIdColNo == 0)
        {
        CDbColSet* colset = aRowSet.ColSetL();
        CLogServViewBase::iIdColNo = colset->ColNo(KLogFieldIdString);
        CLogServViewBase::iTypeColNo = colset->ColNo(KLogFieldEventTypeString);
        for(TInt i=0;i<KLogFlagsCount;++i)
            {
            TDbColName colname;
            colname.Format(KLogFieldEventFlagString, i + 1);
            CLogServViewBase::iFlagColNo[i] = colset->ColNo(colname);
            __ASSERT_DEBUG(CLogServViewBase::iFlagColNo[i] > 0, User::Invariant());
            }
        delete colset;
        }
    __ASSERT_DEBUG(CLogServViewBase::iIdColNo > 0 && CLogServViewBase::iTypeColNo > 0, User::Invariant());  
    }

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServViewBase::ResetViewContentsL(RDbRowSet& aRowSet)
	{
	// Get the view contents
	RArray<TLogId> viewContents(KLogViewContentsGranuality);
	CleanupClosePushL(viewContents);
	viewContents.ReserveL(aRowSet.CountL());
	if(aRowSet.FirstL())
		{
		do
			{
			// Get the id at the current position
			aRowSet.GetL();
			const TLogId id = aRowSet.ColInt32(CLogServViewBase::iIdColNo);
			TUint8 eventTypeIndex = aRowSet.ColUint8(CLogServViewBase::iTypeColNo);
			if(IsAllowed(EReadOp, eventTypeIndex))
			    {
				TInt err = viewContents.Append(id);
		        __ASSERT_ALWAYS(err == KErrNone, Panic(ELogArrayReserved));
			    }
			}
		while(aRowSet.NextL());
		}
	// Tidy up - don't leave from below here
    CleanupStack::Pop(&viewContents);
	iViewContents.Close();
	iViewContents = viewContents;
    iViewContentsReady = ETrue;
	}

void CLogServViewBase::PrepareViewContentsL(const TDesC& aSQL)
	{
	// Generate the view
    RLogDbView view;
    view.PrepareLC(iDatabase.DTIDatabase(), aSQL);
    InitializeColumnsL(view);
	// Reset the view
	ResetViewContentsL(view);
	CleanupStack::PopAndDestroy(&view);
	}

void CLogServViewBase::RebuildViewL()
	{
	if	(iSql && iSql->Length())
		PrepareViewContentsL(*iSql);
	}

//
//  The last change notification indication was not handled correctly.
//  Attempt to re-initialise the view
//
TInt CLogServViewBase::RebuildViewContentsIfNecessary()
	{
	TInt error = KErrNone;
	//
	if	(iRebuildViewContents && iSql && iSql->Length())
		{
		TRAP(error, 

			RebuildViewL();

			// The view is okay now
			iRebuildViewContents = EFalse;
			);
		}
	return error;
	}

void CLogServViewBase::DoHandleChangeEventsL(const CLogChangeDefinition& aChanges)
	{
    if(!ViewIsReady())
        {
        return;
        }
	
	// Generate the view
    RLogDbView view;
    view.PrepareLC(iDatabase.DTIDatabase(), *iSql);
    InitializeColumnsL(view);
	_LIT(KLogIdQuery, "Id = %d");
	TBuf<15> find;
	TInt changeIndex;

	// Prepare for a change transaction
	iChangeManager->ChangeTransactionPrepare();

	const TInt count = aChanges.Count();
	for(TInt i=0; i<count; i++)
		{
		// Fetch the change details
		TLogId logId = KLogNullId;
		TLogDatabaseChangeType type = aChanges.At(i, logId);

		// Mark the insertion position as 'not found'
		changeIndex = KErrNotFound;

		// Format the find query
		find.Format(KLogIdQuery, logId);

		// Handle the various change descriptions
		switch(type)
			{
		case ELogChangeTypeEventAdded:
			{
			__ASSERT_DEBUG(iViewContents.Find(logId) == KErrNotFound, Panic(ELogEventAlreadyInView));

			// See if the event is in the view
			if(view.FirstL())
				{
				const TDbQuery dbQuery(find);
				changeIndex = view.FindL(RDbRowSet::EForwards, dbQuery);
				if	(changeIndex >= 0)
					{
					view.GetL();
					TUint8 eventTypeIndex = view.ColUint8(CLogServViewBase::iTypeColNo);
					if (IsAllowed(EReadOp, eventTypeIndex))
						User::LeaveIfError(iViewContents.Insert(logId, changeIndex));
					
					iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventAdded, changeIndex);
					}
				}
			}
			break;
		case ELogChangeTypeEventChanged:
		case ELogChangeTypeEventChangedHidden:
			{
			// See if the event is in the view
			if	(view.FirstL() && (changeIndex = view.FindL(RDbRowSet::EForwards, TDbQuery(find))) >= KErrNone)
				{
				// If the event was already in the view then it has changed otherwise it's been added
				const TInt findIndex = iViewContents.Find(logId);
				if	(findIndex >= KErrNone)
					{
					// If the item that changed also caused its position within the view to be altered
					// then we need to simulate a delete, followed by an addition
					if	(findIndex != changeIndex)
						{
						iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventDeleted, findIndex);
						iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventAdded, changeIndex);
						}
					// Only tell the view if the CLogEvent has changed
					else if (type != ELogChangeTypeEventChangedHidden)
						{
						iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventChanged, changeIndex);
						}
					}
				else
					{
					User::LeaveIfError(iViewContents.Insert(logId, changeIndex));
					iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventAdded, changeIndex);

					// Update the type to indicate that this change action
					// really resulted in an addition
					type = ELogChangeTypeEventAdded;
					}
				}
			else
				{
				changeIndex = iViewContents.Find(logId);

				// If it used to be in the view then it's been removed because it's 
				// not in there anymore
				if	(changeIndex >= KErrNone)
					{
					iViewContents.Remove(changeIndex);
					iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventDeleted, changeIndex);

					// Update the type to indicate that this change action
					// really resulted in a deletion
					type = ELogChangeTypeEventDeleted;
					}
				}
			}
			break;
		case ELogChangeTypeEventDeleted:
			{
			changeIndex = iViewContents.Find(logId);

			// If it used to be in the view then tell it about the deletion
			if	(changeIndex >= KErrNone)
				{
				iViewContents.Remove(changeIndex);
				iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeEventDeleted, changeIndex);
				}
			}
			break;
		case ELogChangeTypeLogCleared:
			{
			RebuildViewL();
			iChangeManager->ChangeTransactionSubmitL(logId, ELogChangeTypeLogCleared, 0);
			}
			break;
		default:
			__ASSERT_DEBUG(EFalse, Panic(ELogUnrecognizedChangeType));
			break;
			}
		}

	CleanupStack::PopAndDestroy(&view);

	// Commit the transaction. Will notify client if necessary
	iChangeManager->ChangeTransactionCommitL();
	}

TBool CLogServViewBase::IsAllowed(TEventOp aEventOp, TUint8 aEventTypeIndex)
	{
	TBool result = ETrue;

	const TLogServCacheTypeEntry& entry = iDatabase.DTICacheTypes().FindById(aEventTypeIndex);
	TUid eventTypeUid = entry.iEventType->Uid();
	TInt count = iStandardTypeSecurityCache.Count();
		
	for(TInt i=0;i<count;++i)
		{
		if (eventTypeUid == iStandardTypeSecurityCache[i].eventType)
			{
			result = (aEventOp == EWriteOp) ? iStandardTypeSecurityCache[i].writeAccess : iStandardTypeSecurityCache[i].readAccess;
			break;
			}
		}
	
	return result;
	}

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewLockObserver (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServViewLockObserver::CLogServViewLockObserver(MLogServBackupInterface& aBackupInterface)
:	iBackupInterface(aBackupInterface), iLockEvents(KLogViewLockStatusEventGranularity)
	{
	}

CLogServViewLockObserver::~CLogServViewLockObserver()
	{
	iBackupInterface.BIObserverRemove(*this);
	iLockEvents.Close();
	}

void CLogServViewLockObserver::ConstructL()
	{
	// Register for backup events
	iBackupInterface.BIObserverAddL(*this, MLogServBackupInterface::EObjectViewLock);
	}

CLogServViewLockObserver* CLogServViewLockObserver::NewL(MLogServBackupInterface& aBackupInterface)
	{
	CLogServViewLockObserver* self = new(ELeave) CLogServViewLockObserver(aBackupInterface);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLogServViewLockObserver::BOHandleEventL(TLogServBackupEvent aEvent)
	{
	// Map event
	TLogViewLockStatus status = ELogViewWindowOpen;
	if	(aEvent == MLogServBackupObserver::EBackupStarting)
		status = ELogViewWindowLocked;

	// Cache or complete immediately
	if	(!HaveLockStatusChangePointer())
		User::LeaveIfError(iLockEvents.Append(status));
	else
		CompleteLockStatusChangeMessage(status);
	}

/**
The client has requested lock status change notifications
*/
void CLogServViewLockObserver::RequestLockStatusChanges(const RMessage2& aMessage)
	{
	if	(!HaveLockStatusChangePointer())
		{
		iLockStatusChangeMessage = aMessage;
		
		// Already have one cached event
		if	(iLockEvents.Count())
			{
			CompleteLockStatusChangeMessage(iLockEvents[0]);
			iLockEvents.Remove(0);
			}
		}
	else
		PanicClient(aMessage, ELogViewLockStatusChangeRequestAlreadyIssued);
	}

/**
The client has cancelled a previous change notification request
*/
void CLogServViewLockObserver::RequestLockStatusChangesCancel()
	{
	if	(HaveLockStatusChangePointer())
		CompleteLockStatusChangeMessage(KErrCancel);
	}

void  CLogServViewLockObserver::CompleteLockStatusChangeMessage(TInt aCompletionCode)
	{
	__ASSERT_ALWAYS(HaveLockStatusChangePointer(), Panic(ELogViewNoLockStatusChangeMessage));
	iLockStatusChangeMessage.Complete(aCompletionCode);
	}

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewEvent (source)
/////////////////////////////////////////////////////////////////////////////////////////
CLogServViewEvent::CLogServViewEvent(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage)
:	CLogServViewBase(aDatabase, aBackupInterface, aPackage, ELogViewTypeEvent, aViewId, aMessage)
	{
	}

CLogServViewEvent* CLogServViewEvent::NewL(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage)
	{
	CLogServViewEvent* self = new(ELeave) CLogServViewEvent(aDatabase, aBackupInterface, aPackage, aViewId, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Setup the view
Fetch the client-side SQL query string which this view uses to obtain a data-set.
*/
TPtrC CLogServViewEvent::GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType) 
	{
	// Read stuff from the client
	iPackage.ResizeL(aMessage.GetDesLengthL(2));
	aMessage.ReadL(2, iPackage.Ptr());

	// Decode the parameters we've read from the client
	CLogFilterList* filter = new(ELeave)CLogFilterList;
	CleanupStack::PushL(TCleanupItem(DestroyList, filter));
	iPackage.GetLogFilterListL(*filter);
	RLogDynBuf expr;
    TLogFilterExprBuilder exprBuilder(iDatabase);
    exprBuilder.BuildExprLC(expr, *filter, KLogWhere, aFilterType);
    // Generate the query string that will be used
    TheSql.Format(KLogSqlEventViewString, &KLogViewSelectColList, &expr.DesC());
    CleanupStack::PopAndDestroy(2, filter);
	return TheSql;
	}

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewRecent (source)
/////////////////////////////////////////////////////////////////////////////////////////
CLogServViewRecent::CLogServViewRecent(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage)
:	CLogServViewBase(aDatabase, aBackupInterface, aPackage, ELogViewTypeRecent, aViewId, aMessage)
	{
	}

CLogServViewRecent* CLogServViewRecent::NewL(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage)
	{
	CLogServViewRecent* self = new(ELeave) CLogServViewRecent(aDatabase, aBackupInterface, aPackage, aViewId, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Setup the view
Fetch the client-side SQL query string which this view uses to obtain a data-set.
*/
TPtrC CLogServViewRecent::GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType)
	{
	// Read stuff from the client
	iPackage.ResizeL(aMessage.GetDesLengthL(2));
	aMessage.ReadL(2, iPackage.Ptr());

	// Decode the parameters we've read from the client
	CLogFilterList* filter = new(ELeave)CLogFilterList;
	CleanupStack::PushL(TCleanupItem(DestroyList, filter));
	iPackage.GetLogFilterListL(*filter);
	RLogDynBuf expr;
	TLogFilterExprBuilder exprBuilder(iDatabase);
	exprBuilder.BuildExprLC(expr, *filter, KLogAnd, aFilterType);
    iRecentList = static_cast<TLogRecentList>(aMessage.Int3());
    // Generate the view that will be used
    if (iRecentList == KLogNullRecentList)
        {
        TheSql.Format(KLogSqlAllRecentViewString, &KLogViewSelectColList, &expr.DesC());
        }
    else
        {
        TheSql.Format(KLogSqlRecentViewString, &KLogViewSelectColList, iRecentList, &expr.DesC());
        }
    CleanupStack::PopAndDestroy(2, filter);
	return TheSql;
	}

/**
Remove an entry from the view.
Note the client can call this even if the view isn't valid.
*/
void CLogServViewRecent::RemoveL(const RMessage2& aMessage)
	{
	if(! aMessage.HasCapability(ECapabilityWriteDeviceData))
		User::Leave(KErrPermissionDenied);
		
	const TLogId id = static_cast<TLogId>(aMessage.Int2());
    TBuf<20> num;
    num.AppendNum(id);

	iDatabase.DTIBeginWithRollBackProtectionLC();
	
    TheSql.Copy(KLogSqlRemoveDuplicateEvents);
    TheSql.Append(KIdEqStr);
    TheSql.Append(num);
    TheSql.Append(KLogOr);
    TheSql.Append(KDuplicateEqStr);
    TheSql.Append(num);
    User::LeaveIfError(iDatabase.DTIExecuteSql(TheSql));

	// This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
	iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, id);
	iDatabase.DTICommitAndCancelRollbackProtectionL();
	}

/**
Clear all duplicate events associated with this recent list.
*/
void CLogServViewRecent::ClearDuplicatesL(const RMessage2& aMessage)
	{
	if(! aMessage.HasCapability(ECapabilityWriteDeviceData))
		User::Leave(KErrPermissionDenied);
	
	if	(iRecentList > 0)
		{
		// Get list of duplicates
		TheSql.Format(KLogSqlSelectAllDuplicatesString, iRecentList);
        RLogDbView view;
        view.PrepareLC(iDatabase.DTIDatabase(), TheSql);
        InitializeColumns2L(view);
		if(view.FirstL())
			{
            iDatabase.DTIBeginWithRollBackProtectionLC();
			// Iterate through the events
			do
				{
				// Get current event id
				view.GetL();
				const TLogId id = view.ColInt32(CLogServViewRecent::iIdColNo);
				// Make the change
				view.UpdateL();
				view.SetColNullL(CLogServViewRecent::iRecentColNo);
				view.SetColNullL(CLogServViewRecent::iDuplicateColNo);
				view.PutL();
				// This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
				iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, id);
				}
			while(view.NextL());
			iDatabase.DTICommitAndCancelRollbackProtectionL();
			}
		CleanupStack::PopAndDestroy(&view);
		}
	else
		::PanicClientL(aMessage, ELogInvalidRecentView);
	}

void CLogServViewRecent::InitializeColumns2L(RDbRowSet& aRowSet)
    {
    if(CLogServViewRecent::iIdColNo == 0)
        {
        CDbColSet* colset = aRowSet.ColSetL();
        CLogServViewRecent::iIdColNo = colset->ColNo(KLogFieldIdString);
        CLogServViewRecent::iRecentColNo = colset->ColNo(KLogFieldEventRecentString);
        CLogServViewRecent::iDuplicateColNo = colset->ColNo(KLogFieldEventDuplicateString);
        delete colset;
        }
    __ASSERT_DEBUG(CLogServViewRecent::iIdColNo > 0 && 
            CLogServViewRecent::iRecentColNo > 0 && 
            CLogServViewRecent::iDuplicateColNo > 0, User::Invariant());  
        
    }

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServViewDuplicate (source)
/////////////////////////////////////////////////////////////////////////////////////////
CLogServViewDuplicate::CLogServViewDuplicate(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage)
:	CLogServViewBase(aDatabase, aBackupInterface, aPackage, ELogViewTypeDuplicate, aViewId, aMessage)
//
//	Duplicate view
//
	{
	}

CLogServViewDuplicate* CLogServViewDuplicate::NewL(MLogServDatabaseTransactionInterface& aDatabase, MLogServBackupInterface& aBackupInterface, CLogPackage& aPackage, TLogViewId aViewId, const RMessage2& aMessage)
	{
	CLogServViewDuplicate* self = new(ELeave) CLogServViewDuplicate(aDatabase, aBackupInterface, aPackage, aViewId, aMessage);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Setup the view
Fetch the client-side SQL query string which this view uses to obtain a data-set.
*/
TPtrC CLogServViewDuplicate::GetQueryStringL(const RMessage2& aMessage, TLogFilterConstructionType aFilterType)
	{
	// Read stuff from the client
	iPackage.ResizeL(aMessage.GetDesLengthL(2));
	aMessage.ReadL(2, iPackage.Ptr());

	// Decode the parameters we've read from the client
	CLogFilterList* filter = new(ELeave)CLogFilterList;
	CleanupStack::PushL(TCleanupItem(DestroyList, filter));
	iPackage.GetLogFilterListL(*filter);
	RLogDynBuf expr;
	TLogFilterExprBuilder exprBuilder(iDatabase);
	exprBuilder.BuildExprLC(expr, *filter, KLogAnd, aFilterType);
	iSourceId = (TInt)aMessage.Ptr3();
    // Generate the view that will be used
    TheSql.Format(KLogSqlDuplicateViewString2, &KLogViewSelectColList, iSourceId, &expr.DesC());
    CleanupStack::PopAndDestroy(2, filter);
	return TheSql;
	}

/**
Remove an entry from the view.
*/
void CLogServViewDuplicate::RemoveL(const RMessage2& aMessage)
	{
	if(! aMessage.HasCapability(ECapabilityWriteDeviceData))
		User::Leave(KErrPermissionDenied);

	const TLogId id = static_cast<TLogId>(aMessage.Int2());

	iDatabase.DTIBeginWithRollBackProtectionLC();

	// Do the actual work
	TheSql.Format(KLogSqlRemoveDuplicateString, id, iSourceId);
	const TInt error = iDatabase.DTIExecuteSql(TheSql);
	User::LeaveIfError(error);
	//
	// This is a "hidden" change. It may affect the contents of a view, but the actual event hasn't changed
	iDatabase.DTIChangeInterface().DCISubmitChangedEventContextL(ELogChangeTypeEventChangedHidden, id);
	iDatabase.DTICommitAndCancelRollbackProtectionL();
	}
