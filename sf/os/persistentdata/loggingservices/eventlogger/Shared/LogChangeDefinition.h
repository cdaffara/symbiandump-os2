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

#ifndef __LOGCHANGEDEFINITION_H__
#define __LOGCHANGEDEFINITION_H__

// System includes
#include <e32base.h>
#include <s32strm.h>
#include <logwrap.h>
#include <logwraplimits.h>

/**
Constants
@internalComponent
*/
const TInt KLogChangeDefinitionDefaultGranularity = 10;

/**
Enumerations
@internalComponent
*/
enum TLogDatabaseChangeType
	{
	ELogChangeTypeUndefined = -1,
	ELogChangeTypeEventAdded = 0,
	ELogChangeTypeEventChanged,
	ELogChangeTypeEventChangedHidden,
	ELogChangeTypeEventDeleted,
	ELogChangeTypeLogCleared
	};


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogChangeDefinition (header)
/////////////////////////////////////////////////////////////////////////////////////////
class CLogChangeDefinition : public CBase
/**
@internalComponent
*/
	{
public: // CONSTRUCT / DESTRUCT
	IMPORT_C static CLogChangeDefinition* NewL(TInt aGranularity = KLogChangeDefinitionDefaultGranularity);
	IMPORT_C static CLogChangeDefinition* NewL(RReadStream& aStream);
	IMPORT_C static CLogChangeDefinition* NewL(const CLogChangeDefinition& aCopy);
	IMPORT_C ~CLogChangeDefinition();

private: // INTERNAL CONSTRUCT
	CLogChangeDefinition(TInt aGranularity);
	void ConstructL(const CLogChangeDefinition& aCopy);

public: // API

	/**
	 * Return the number of events in the change definition
	 */
	IMPORT_C TInt Count() const;

	/**
	 * Get a change description
	 */
	IMPORT_C TLogDatabaseChangeType At(TInt aIndex) const;
	IMPORT_C TLogDatabaseChangeType At(TInt aIndex, TLogId& aId) const;
	IMPORT_C TLogDatabaseChangeType At(TInt aIndex, TLogId& aId, TInt& aViewIndex) const;

	/**
	 * Find the position of a specific event
	 */
	IMPORT_C TInt Find(TLogId aId) const;
	IMPORT_C TInt Find(TLogId aId, TLogDatabaseChangeType aType) const;
	IMPORT_C TInt FindByViewIndex(TInt aViewIndex) const;

	/**
	 * Add a change definition
	 */
	IMPORT_C void AddL(TLogId aId, TLogDatabaseChangeType aType, TInt aViewIndex);

	/**
	 * Reset change contents
	 */
	IMPORT_C void Reset();

public: // INTERNALIZE / EXTERNALIZE
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;

private: // INTERNAL CLASSES
	class TLogShdChangeDetails
		{
	public:
		inline TLogShdChangeDetails() : iId(KLogNullId), iType(ELogChangeTypeUndefined), iViewIndex(KErrNotFound) { }
		inline TLogShdChangeDetails(TLogId aId, TLogDatabaseChangeType aType = ELogChangeTypeUndefined, TInt aViewIndex = KErrNotFound) : iId(aId), iType(aType), iViewIndex(aViewIndex) { }

	public:
		TLogId iId;
		TLogDatabaseChangeType iType;
		TInt iViewIndex;
		};

private: // INTERNAL METHODS
	static TBool CompareEntryIds(const TLogShdChangeDetails& aLeft, const TLogShdChangeDetails& aRight);
	static TBool CompareViewIndicies(const TLogShdChangeDetails& aLeft, const TLogShdChangeDetails& aRight);
	void AddToContainerL(TLogId aId, TLogDatabaseChangeType aType, TInt aViewIndex);

private: // MEMBER DATA

	/**
	 * List of changes
	 */
	RArray<TLogShdChangeDetails> iChanges;
	};




#endif
