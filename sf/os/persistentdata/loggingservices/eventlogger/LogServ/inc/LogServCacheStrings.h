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

#ifndef __LOGSERVCACHESTRINGS_H__
#define __LOGSERVCACHESTRINGS_H__

#include <logwrap.h>
#include <logwraplimits.h>

// Classes referenced
class MLogServDatabaseTransactionInterface;

/**
Caches the most often used strings, such as "Incoming", "Outgoing", "Missed", etc.
Every string in the cache is identified by an ID. The cached strings come from 
event's "status" and "direction" fields. If an event is about to be added to the events database
table, instead of storing in the table often repeated "status" and "direction" string fields values,
the CLogServCacheStrings is searched for the ID of the string. If such string is already in the cache
its ID is returned, if no such string is found in the cache, then the string will be added to the cache 
and the ID of the added string is returned. The returned ID will be stored as part of the event 
properties in the events table. 

Wherever the strings cache is changed, if the database is already in transaction, the cache changes will
be committed/rolled back when the transaction is committed/rolled back.

Note: the often used LogEng strings are kept in a database table with name "String".

@see CLogAddEvent
@see CLogEvent
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogServCacheStrings : public CBase
	{
public:
	static CLogServCacheStrings* NewL(MLogServDatabaseTransactionInterface& aDatabase);
	~CLogServCacheStrings();

	const TPtrC  FindString(TLogStringId aId) const;
	TLogStringId FindId(const TDesC& aString);
	TLogStringId GetIdL(const TDesC& aString);
	void Commit();
	void Rollback();

private:
   	struct TLogServCacheStringEntry
		{
		public:
			static void CleanupEntry(TAny* aEntry);
			static inline TLogServCacheStringEntry* NewEntryL(TLogStringId aId, const TDesC& aString, TBool aDirty = EFalse)
				{
				TUint8* entry = (TUint8*)User::AllocL(sizeof(TLogServCacheStringEntry) + aString.Size());
				return new (entry) TLogServCacheStringEntry(aId, aString, aDirty);
				}
			static inline TLogServCacheStringEntry* NewEntryLC(TLogStringId aId, const TDesC& aString, TBool aDirty = EFalse)
				{
				TLogServCacheStringEntry* entry = TLogServCacheStringEntry::NewEntryL(aId, aString, aDirty);
				CleanupStack::PushL(TCleanupItem(&TLogServCacheStringEntry::CleanupEntry, entry));
				return entry;
				}
			static inline void DeleteEntry(TLogServCacheStringEntry* aEntry)
				{
				User::Free(aEntry);
				}
			inline const TPtrC String() const
				{
				return TPtrC(iString, iStringLength);
				}
		private:
			inline TLogServCacheStringEntry(TLogStringId aId, const TDesC& aString, TBool aDirty) :
				iId(aId),
				iDirty(aDirty),
				iStringLength(aString.Length())
				{
				(void)Mem::Copy(iString, aString.Ptr(), aString.Size());
				}
			TLogServCacheStringEntry();				
			TLogServCacheStringEntry(const TLogServCacheStringEntry&);
			TLogServCacheStringEntry& operator=(const TLogServCacheStringEntry&);
		public:		
			TLogStringId 	iId;
			TBool 			iDirty;//if set, this entry has been added to the cache during transaction
		private:			
			TInt 			iStringLength;
			TText 			iString[1];
		};

	CLogServCacheStrings(MLogServDatabaseTransactionInterface& aDatabase);
	void ConstructL();
	void ReadStringsFromDatabaseL();
	TLogStringId DoAddStringL(const TDesC& aString);
	void DestroyCache();
	void InitializeColNumsL(RDbRowSet& aRowSet);
	static TInt Compare1(const TDesC* aString, TLogServCacheStringEntry* const& aRight);
	static TInt Compare2(TLogServCacheStringEntry* const& aLeft, TLogServCacheStringEntry* const& aRight);

private:
	MLogServDatabaseTransactionInterface& iDatabase;
	RArray<TLogServCacheStringEntry*> iStrings;//List of cached strings
	TBool iDirty;//iDirty flag is set when strings are added to the cache during transaction
	TDbColNo iIdColNo;
	TDbColNo iStringColNo;
	
	};

#endif
