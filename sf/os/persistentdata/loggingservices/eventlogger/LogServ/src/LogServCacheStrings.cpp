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

#include "LogServCacheStrings.h"
#include "LogServDatabaseTransactionInterface.h"
#include "logservpanic.h"
#include "LogServSqlStrings.h"

/**
Strings array granularity.
@internalComponent
*/
const TInt KLogNumberOfInitialStrings = 16;

/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogServCacheStrings (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogServCacheStrings::CLogServCacheStrings(MLogServDatabaseTransactionInterface& aDatabase) :
	iDatabase(aDatabase), 
	iStrings(KLogNumberOfInitialStrings)
	{
	}

CLogServCacheStrings::~CLogServCacheStrings()
	{
	DestroyCache();
	}

void CLogServCacheStrings::DestroyCache()
	{
	for(TInt i=iStrings.Count()-1;i>=0;--i)
		{
		TLogServCacheStringEntry::DeleteEntry(iStrings[i]);
		}
	iStrings.Close();
	}

void CLogServCacheStrings::ConstructL()
	{
	ReadStringsFromDatabaseL();
	}

CLogServCacheStrings* CLogServCacheStrings::NewL(MLogServDatabaseTransactionInterface& aDatabase)
	{
	CLogServCacheStrings* self = new(ELeave) CLogServCacheStrings(aDatabase);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

/**
Find the descriptor corresponding to a given string id. KNullDesC if the id is not found.
*/
const TPtrC CLogServCacheStrings::FindString(TLogStringId aId) const
	{
	if(aId != KLogNullStringId)
		{
		for(TInt i=iStrings.Count()-1;i>=0;--i)
			{
			if(iStrings[i]->iId == aId)
				{
				return iStrings[i]->String();	
				}
			}
		}
	return KNullDesC();
	}


/**
Find the id of a given string. KLogNullStringId if no string is found.
*/
TLogStringId CLogServCacheStrings::FindId(const TDesC& aString)
	{
	if(aString.Length() == 0)
		{
		return KLogNullStringId;	
		}
	TInt position = iStrings.FindInOrder(aString, &CLogServCacheStrings::Compare1);
	return position >= 0 ? iStrings[position]->iId : KLogNullStringId;
	}

/**
Find the id of a given string. Add the string to the cache if the string is not there.
If the aString length is 0, then do not search the cache, do not add the string, just return KLogNullStringId.
If the string has to be added - the string will be added to the cache and
a new record will be inserted into the database.
If the database is in transaction, the string in the cache will have its "dirty" flag set. During the rollback
(if a rollback occurs) all strings with "dirty" flag set will be removed from the cache.
The idea is to keep the cache content consistent with the database content.
*/
TLogStringId CLogServCacheStrings::GetIdL(const TDesC& aString)
	{
	if(aString.Length() == 0)
		{
		return KLogNullStringId;	
		}
	TLogStringId id = FindId(aString);
	if(id == KLogNullStringId)
		{
		id = DoAddStringL(aString);	
		}
	return id;
	}

/**
Clears the dirty flag of the cache entries that have been added during the last transaction.
*/
void CLogServCacheStrings::Commit()
	{
	if(iDirty)
		{
		for(TInt i=iStrings.Count()-1;i>=0;--i)
			{
			iStrings[i]->iDirty = EFalse;
			}
		iDirty = EFalse;	
		}
	}


/**
Removes any strings added to the cache of strings since the beginning of last transaction (with iDirty flag set).
*/
void CLogServCacheStrings::Rollback()
	{
	if(iDirty)
		{
		for(TInt i=iStrings.Count()-1;i>=0;--i)
			{
			if(iStrings[i]->iDirty)
				{
				TLogServCacheStringEntry::DeleteEntry(iStrings[i]);
				iStrings.Remove(i);
				}
			}
		iDirty = EFalse;	
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

void CLogServCacheStrings::ReadStringsFromDatabaseL()
	{
	DestroyCache();
	RDbTable tbl;
	CleanupClosePushL(tbl);
	User::LeaveIfError(tbl.Open(iDatabase.DTIDatabase(), KLogNameStringString, RDbRowSet::EReadOnly));
	if(tbl.FirstL())
		{
		InitializeColNumsL(tbl);
		iStrings.ReserveL(tbl.CountL());
		do
			{
			tbl.GetL();			
			const TLogStringId id = tbl.ColInt16(iIdColNo);
			const TPtrC pString(tbl.ColDes(iStringColNo));
			TLinearOrder<TLogServCacheStringEntry*> orderer(&CLogServCacheStrings::Compare2);
			TLogServCacheStringEntry* entry = TLogServCacheStringEntry::NewEntryL(id, pString);
			TInt err = iStrings.InsertInOrder(entry, orderer);
			__ASSERT_ALWAYS(err == KErrNone, Panic(ELogStringsCacheReserved));
			}
		while(tbl.NextL());		
		}
	CleanupStack::PopAndDestroy(&tbl);
	}

//Atomic "add string to cache" operation.
TLogStringId CLogServCacheStrings::DoAddStringL(const TDesC& aString)
	{
	//Reserve space for the new string in the cache
	iStrings.ReserveL(iStrings.Count() + 1);
	//Open the database table and push it on the cleanup stack.
	//If the InitializeColNumsL() operation leaves then the table will be closed automatically.
	RDbTable tbl;
	CleanupClosePushL(tbl);
	User::LeaveIfError(tbl.Open(iDatabase.DTIDatabase(), KLogNameStringString));
	InitializeColNumsL(tbl);
	//Allocate space for the new record in the table. Push the Cancel() function on the cleanup stack.
	//If some of the next calls leaves, the insert operation will be automatically cancelled.
	tbl.InsertL();
	const TLogStringId id = tbl.ColInt16(iIdColNo);
	tbl.SetColL(iStringColNo, aString);
	//Create new cache entry and push it on the cleanup stack.
	TLogServCacheStringEntry* entry = TLogServCacheStringEntry::NewEntryLC(id, aString, iDatabase.DTIInTransaction());
	//Finish the "insert record" operation. If PutL() leaves, then:
	//  - the new cache entry is deleted
	//  - the insert operation is cancelled
	//  - the table is closed
	tbl.PutL();
	//Pop the entry and the Cancel() from the cleanup stack. Close the table.
	CleanupStack::Pop();//TLogServCacheStringEntry
	CleanupStack::PopAndDestroy(&tbl);
	//The next operation is guaranteed to be a non-failing "add entry" operation.
	TLinearOrder<TLogServCacheStringEntry*> orderer(&CLogServCacheStrings::Compare2);
	TInt err = iStrings.InsertInOrder(entry, orderer);
	__ASSERT_ALWAYS(err == KErrNone, Panic(ELogStringsCacheReserved));
	//Mark the cache as dirty. Later if there was an outstanding transaction and that transaction failed, 
	//The database rollback operation will restore the original state of the table. 
	//The CLogServCacheStrings::RollbackAddStringsL() will remove from the cache all "dirty" strings.
	iDirty = ETrue;
	return id;
	}

TInt CLogServCacheStrings::Compare1(const TDesC* aString, TLogServCacheStringEntry* const& aRight)
	{
	__ASSERT_DEBUG(aString != NULL, Panic(ELogStringsCacheNullArg1));
	__ASSERT_DEBUG(aRight != NULL, Panic(ELogStringsCacheNullArg1));
	return aString->Compare(aRight->String());
	}

TInt CLogServCacheStrings::Compare2(TLogServCacheStringEntry* const& aLeft, TLogServCacheStringEntry* const& aRight)
	{
	__ASSERT_DEBUG(aLeft != NULL, Panic(ELogStringsCacheNullArg2));
	__ASSERT_DEBUG(aRight != NULL, Panic(ELogStringsCacheNullArg2));
	return aLeft->String().Compare(aRight->String());
	}

void CLogServCacheStrings::InitializeColNumsL(RDbRowSet& aRowSet)
	{
	if(iIdColNo == 0)
		{
		CDbColSet* colset = aRowSet.ColSetL();
		iIdColNo = colset->ColNo(KLogFieldStringIdString);
		iStringColNo = colset->ColNo(KLogFieldStringTextString);
		delete colset;
		}
	__ASSERT_DEBUG(iIdColNo > 0, Panic(ELogInvalidStringColNo));
	__ASSERT_DEBUG(iStringColNo > 0, Panic(ELogInvalidStringColNo));
	}

void CLogServCacheStrings::TLogServCacheStringEntry::CleanupEntry(TAny* aEntry)
	{
	TLogServCacheStringEntry* entry = reinterpret_cast <TLogServCacheStringEntry*> (aEntry);
	CLogServCacheStrings::TLogServCacheStringEntry::DeleteEntry(entry);
	}
