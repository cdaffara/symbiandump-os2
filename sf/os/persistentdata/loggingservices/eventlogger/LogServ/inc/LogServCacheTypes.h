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

#ifndef __LOGSERVCACHETYPES_H__
#define __LOGSERVCACHETYPES_H__

#include <logwrap.h>
#include <logwraplimits.h>

// Classes referenced
class CLogEventType;
class TResourceReader;
class MLogServDatabaseTransactionInterface;

/**
Every element stored in the CLogServCacheTypes is of TLogServCacheTypeEntry type. 

@see CLogServCacheTypes
@see CLogEventType
@internalComponent
*/
struct TLogServCacheTypeEntry
	{
	inline TLogServCacheTypeEntry() :
		iEventTypeId(KLogNullTypeId),
		iEventType(NULL)
		{
		}
	inline TLogServCacheTypeEntry(TLogTypeId aId, CLogEventType* aEventType) :
		iEventTypeId(aId),
		iEventType(aEventType)
		{
		}

	TLogTypeId 		iEventTypeId;
	CLogEventType*	iEventType;
	};

/**
Caches the event types.
The event type is used to set the event description.

Note: the event types are kept in a database table with name "Type".

@see CLogEvent
@see CLogAddEvent 
@see TLogServCacheTypeEntry
@see CLogEventType
@see MLogServDatabaseTransactionInterface
@internalComponent
*/
class CLogServCacheTypes : public CBase
	{
public:
	static CLogServCacheTypes* NewL(MLogServDatabaseTransactionInterface& aDatabase);
	~CLogServCacheTypes();

public:
	const TLogServCacheTypeEntry&	FindById(TLogTypeId aId) const;
	const TLogServCacheTypeEntry&	FindByUid(TUid aUid) const;
	
	TLogTypeId	AddTypeL(const CLogEventType& aEventType);
	TInt 		ChangeType(const CLogEventType& aEventType);
	TInt 		DeleteType(TUid aUid);
	
	void CreateStandardTypesL(TResourceReader& aResourceSpecifiedTypes, TBool aReadOnly = EFalse);
	void InitializeL();
	void CopyStandardTypeUidsL(RArray<TUid>& aUidsCopy);

private:
	CLogServCacheTypes(MLogServDatabaseTransactionInterface& aDatabase);
	void ConstructL();
	TLogTypeId DoAddTypeL(const CLogEventType& aEventType);
	TInt DoChangeType(const CLogEventType& aEventType);
	void InitializeColNumsL(RDbRowSet& aRowSet);
	void DestroyCache();
	//
	static TInt Compare1(const TUid* aUid, const TLogServCacheTypeEntry& aRight);
	static TInt Compare2(const TLogServCacheTypeEntry& aLeft, const TLogServCacheTypeEntry& aRight);

	void __DbgTestInvariant() const;

private:
	MLogServDatabaseTransactionInterface& iDatabase;
	RArray<TLogServCacheTypeEntry> 	iTypes;
	RArray<TUid> 					iStandardTypeUids;
	TLogServCacheTypeEntry			iInvalidCacheEntry;
	//Type table - column numbers
	TInt							iIdColNo;
	TInt							iUidColNo;
	TInt							iDescriptionColNo;
	TInt							iEnabledColNo;
	};

#endif

