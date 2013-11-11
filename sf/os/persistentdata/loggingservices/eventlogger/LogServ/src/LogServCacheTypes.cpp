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

#include "logservpanic.h"
#include "LogServCacheTypes.h"
#include "LogServDatabaseTransactionInterface.h"
#include "LogServResourceInterpreter.h"
#include "LogServSqlStrings.h"

// Constants
const TInt KLogNumberOfInitialTypes = 8;
const TBool KDefaultLoggingSetting = EFalse;

CLogServCacheTypes::CLogServCacheTypes(MLogServDatabaseTransactionInterface& aDatabase) :
	iDatabase(aDatabase), 
	iTypes(KLogNumberOfInitialTypes)
	{
	}

CLogServCacheTypes::~CLogServCacheTypes()
	{
	delete iInvalidCacheEntry.iEventType;
	DestroyCache();
	iStandardTypeUids.Close();
	}

CLogServCacheTypes* CLogServCacheTypes::NewL(MLogServDatabaseTransactionInterface& aDatabase)
	{
	CLogServCacheTypes* self = new (ELeave) CLogServCacheTypes(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CLogServCacheTypes::ConstructL()
	{
	CLogEventType* invEventType = CLogEventType::NewL();
	invEventType->SetDescription(KNullDesC);
	invEventType->SetUid(KNullUid);
	invEventType->SetLoggingEnabled(KDefaultLoggingSetting);
	iInvalidCacheEntry.iEventTypeId = KLogNullTypeId;
	iInvalidCacheEntry.iEventType = invEventType;
	}

/**
*/
const TLogServCacheTypeEntry& CLogServCacheTypes::FindById(TLogTypeId aId) const
	{
	__TEST_INVARIANT;
	for(TInt i=iTypes.Count()-1;i>=0;--i)
		{
		if(iTypes[i].iEventTypeId == aId)
			{
			return iTypes[i];
			}
		}
	return iInvalidCacheEntry;
	}

/**
*/
const TLogServCacheTypeEntry& CLogServCacheTypes::FindByUid(TUid aUid) const
	{
	__TEST_INVARIANT;
	TInt position = iTypes.FindInOrder(aUid, &CLogServCacheTypes::Compare1);
	return position >= 0 ? iTypes[position] : iInvalidCacheEntry;
	}

/**
Add a new type to the database
*/
TLogTypeId CLogServCacheTypes::AddTypeL(const CLogEventType& aEventType)
	{
	__TEST_INVARIANT;
	TInt position = iTypes.FindInOrder(aEventType.Uid(), &CLogServCacheTypes::Compare1);
	if(position >= 0)
		{
		User::Leave(KErrAlreadyExists);
		}
	return DoAddTypeL(aEventType);
	}

/**
Change an existing type
*/
TInt CLogServCacheTypes::ChangeType(const CLogEventType& aEventType)
	{
	__TEST_INVARIANT;
	return DoChangeType(aEventType);
	}

/**
Delete an existing type
*/
TInt CLogServCacheTypes::DeleteType(TUid aUid)
	{
	__TEST_INVARIANT;
	if(iStandardTypeUids.Find(aUid) >= 0)
		{
		return KErrPermissionDenied;
		}
	TInt position = iTypes.FindInOrder(aUid, &CLogServCacheTypes::Compare1);	
	TheSql.Format(KLogSqlDeleteTypeString, aUid);
	TInt err = iDatabase.DTIExecuteSql(TheSql);
	if(err >= 0 && position >= 0)
		{
		delete iTypes[position].iEventType;
		iTypes.Remove(position);
		}
	return err;	
	}

/**
Creates the standard (resource file-defined) log types. Should
be called from within a transaction only when the database is
created.
This method is implicitly called within a transaction by the database marshall
*/
void CLogServCacheTypes::CreateStandardTypesL(TResourceReader& aResourceSpecifiedTypes, TBool aReadOnly /*=EFalse*/)
	{
	const TInt count = aResourceSpecifiedTypes.ReadInt16();
	iStandardTypeUids.Reset();
	iStandardTypeUids.ReserveL(count);

	RDbTable tbl;
	CleanupClosePushL(tbl);
	User::LeaveIfError(tbl.Open(iDatabase.DTIDatabase(), KLogNameTypeString));
	InitializeColNumsL(tbl);
	
	TBool commit = EFalse;
	if(!iDatabase.DTIInTransaction())
		{
		iDatabase.DTIBeginWithRollBackProtectionLC();
		commit = ETrue;
		}

	for(TInt i=0;i<count;++i)
		{
		TUint uId = aResourceSpecifiedTypes.ReadUint32();
		TPtrC des = aResourceSpecifiedTypes.ReadTPtrC();
		TUint enabled = aResourceSpecifiedTypes.ReadUint8();
		
		if(!aReadOnly)
			{
			tbl.InsertL();
			tbl.SetColL(iUidColNo, uId);
			tbl.SetColL(iDescriptionColNo, des);
			tbl.SetColL(iEnabledColNo, enabled);
			tbl.PutL();
			}

		TInt err = iStandardTypeUids.Append(TUid::Uid(uId));
		__ASSERT_ALWAYS(err == KErrNone, Panic(ELogTypesCacheReserved));
		}
		
	if(commit)
		{
		iDatabase.DTICommitAndCancelRollbackProtectionL();
		}
	
	CleanupStack::PopAndDestroy(&tbl);
	}

/**
Initialize the cache based upon the database contents. Called whenever
the database is opened.
*/
void CLogServCacheTypes::InitializeL()
	{
	__TEST_INVARIANT;

	TLinearOrder<TLogServCacheTypeEntry> orderer(&CLogServCacheTypes::Compare2);
	DestroyCache();
	
	RDbTable tbl;
	CleanupClosePushL(tbl);
	User::LeaveIfError(tbl.Open(iDatabase.DTIDatabase(), KLogNameTypeString, RDbRowSet::EReadOnly));
	
	if(tbl.FirstL())
		{
		InitializeColNumsL(tbl);
		const TInt count = tbl.CountL();
		iTypes.ReserveL(count);
		do
			{
			tbl.GetL();	
			
			CLogEventType* eventType = CLogEventType::NewL();
			eventType->SetUid(TUid::Uid(tbl.ColUint32(iUidColNo)));
			eventType->SetDescription(tbl.ColDes(iDescriptionColNo));
			eventType->SetLoggingEnabled(tbl.IsColNull(iEnabledColNo) ? EFalse : tbl.ColUint8(iEnabledColNo));
			TLogServCacheTypeEntry entry(tbl.ColInt16(iIdColNo), eventType);
			
			TInt err = iTypes.InsertInOrder(entry, orderer);
			__ASSERT_ALWAYS(err == KErrNone, Panic(ELogTypesCacheReserved));
			}
		while(tbl.NextL());
		}
	
	CleanupStack::PopAndDestroy(&tbl);
	}

void CLogServCacheTypes::CopyStandardTypeUidsL(RArray<TUid>& aUidsCopy)
	{
	__TEST_INVARIANT;
	TInt count = iStandardTypeUids.Count();
	aUidsCopy.ReserveL(aUidsCopy.Count() + count);
	for(TInt i=0;i<count;++i)
		{
		TInt err = aUidsCopy.Append(iStandardTypeUids[i]);	
		__ASSERT_ALWAYS(err == KErrNone, Panic(ELogTypesCacheReserved));
		}
	}


/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TLogTypeId CLogServCacheTypes::DoAddTypeL(const CLogEventType& aEventType)
	{
	iTypes.ReserveL(iTypes.Count() + 1);
	
	CLogEventType* eventType = CLogEventType::NewL();
	CleanupStack::PushL(eventType);
	eventType->Copy(aEventType);
	
	RDbTable tbl;
	CleanupClosePushL(tbl);
	User::LeaveIfError(tbl.Open(iDatabase.DTIDatabase(), KLogNameTypeString));
	InitializeColNumsL(tbl);
	
	tbl.InsertL();
	const TLogTypeId id = tbl.ColInt16(iIdColNo);
	tbl.SetColL(iUidColNo, eventType->Uid().iUid);
	tbl.SetColL(iDescriptionColNo, eventType->Description());
	tbl.SetColL(iEnabledColNo, eventType->LoggingEnabled() ? 1 : 0);
	tbl.PutL();

	TLinearOrder<TLogServCacheTypeEntry> orderer(&CLogServCacheTypes::Compare2);
	TLogServCacheTypeEntry entry(id, eventType);
	TInt err = iTypes.InsertInOrder(entry, orderer);
	__ASSERT_ALWAYS(err == KErrNone, Panic(ELogTypesCacheReserved));
	
	CleanupStack::PopAndDestroy(&tbl);
	CleanupStack::Pop(eventType);
	return id;
	}

/**
If the EventType is standard - i.e. it was defined in LogWrap.rss, then no changes
are allowed.  Otherwise we can only change the logging status or the description.
*/
TInt CLogServCacheTypes::DoChangeType(const CLogEventType& aEventType)
	{
	TInt position = iTypes.FindInOrder(aEventType.Uid(), &CLogServCacheTypes::Compare1);
	if(position < 0)
		{
		return position;	
		}

	if(iStandardTypeUids.Find(aEventType.Uid()) >= 0)	
		{
		return KErrPermissionDenied;		// standard built-in type - no changes allowed !
		}
	
	// First update the database (event type table)
	TheSql.Format(KLogSqlUpdateTypeString, &aEventType.Description(), aEventType.LoggingEnabled(), aEventType.Uid());
	TInt err = iDatabase.DTIExecuteSql(TheSql);
	if(err < 0)
		{
		return err;	
		}
	// Then update the cached entry
	iTypes[position].iEventType->Copy(aEventType);
	return KErrNone;
	}

void CLogServCacheTypes::InitializeColNumsL(RDbRowSet& aRowSet)
	{
	if(iIdColNo == 0)
		{
		CDbColSet* colset = aRowSet.ColSetL();
		iIdColNo = colset->ColNo(KLogFieldTypeIdString);
		iUidColNo = colset->ColNo(KLogFieldTypeUidString);
		iDescriptionColNo = colset->ColNo(KLogFieldTypeDescriptionString);
		iEnabledColNo = colset->ColNo(KLogFieldTypeEnabledString);
		delete colset;
		}
	__ASSERT_DEBUG(iIdColNo > 0, Panic(ELogInvalidTypeColNo));
	__ASSERT_DEBUG(iUidColNo > 0, Panic(ELogInvalidTypeColNo));
	__ASSERT_DEBUG(iDescriptionColNo > 0, Panic(ELogInvalidTypeColNo));
	__ASSERT_DEBUG(iEnabledColNo > 0, Panic(ELogInvalidTypeColNo));
	}

void CLogServCacheTypes::DestroyCache()
	{
	for(TInt i=iTypes.Count()-1;i>=0;--i)
		{
		delete iTypes[i].iEventType;	
		}
	iTypes.Close();		
	}

TInt CLogServCacheTypes::Compare1(const TUid* aUid, const TLogServCacheTypeEntry& aRight)
	{
	__ASSERT_DEBUG(aUid != NULL, Panic(ELogTypeCacheNullArg));
	if(*aUid == aRight.iEventType->Uid())
		{
		return 0;
		}
	else if(aUid->iUid < aRight.iEventType->Uid().iUid)
		{
		return -1;
		}
	return 1;
	}

TBool CLogServCacheTypes::Compare2(const TLogServCacheTypeEntry& aLeft, const TLogServCacheTypeEntry& aRight)
	{
	TUid typeUid = aLeft.iEventType->Uid();
	return CLogServCacheTypes::Compare1(&typeUid, aRight);
	}

void CLogServCacheTypes::__DbgTestInvariant() const
	{
#ifdef _DEBUG
	const TInt count = iTypes.Count();
	for(TInt i=0; i<count;  ++i)
		{
		const TLogServCacheTypeEntry& event1 = iTypes[i];
		__ASSERT_DEBUG(event1.iEventTypeId != KLogNullTypeId, User::Invariant());
		__ASSERT_DEBUG(event1.iEventType != NULL, User::Invariant());
		for(TInt j=0; j<count; ++j)
			{
			if(j==i)
				{
				continue;
				}
			const TLogServCacheTypeEntry& event2 = iTypes[j];
			__ASSERT_DEBUG(event2.iEventTypeId != KLogNullTypeId, User::Invariant());
			__ASSERT_DEBUG(event2.iEventType != NULL, User::Invariant());
			
			__ASSERT_DEBUG(event2.iEventTypeId != event1.iEventTypeId, User::Invariant());
			__ASSERT_DEBUG(event2.iEventType->Uid() != event1.iEventType->Uid(), User::Invariant());
			}
		}
#endif
	}
