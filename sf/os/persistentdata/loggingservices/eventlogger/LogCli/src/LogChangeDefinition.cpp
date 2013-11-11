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

#include "LogChangeDefinition.h"

// User includes
#include "logclipanic.h"


/////////////////////////////////////////////////////////////////////////////////////////
// -----> CLogChangeDefinition (source)
/////////////////////////////////////////////////////////////////////////////////////////

CLogChangeDefinition::CLogChangeDefinition(TInt aGranularity)
:	iChanges(aGranularity)
	{
	}

EXPORT_C CLogChangeDefinition::~CLogChangeDefinition()
	{
	iChanges.Close();
	}

void CLogChangeDefinition::ConstructL(const CLogChangeDefinition& aCopy)
	{
	const TInt count = aCopy.Count();
	for(TInt i=0; i<count; i++)
		{
		const TLogShdChangeDetails& item = aCopy.iChanges[i];
		User::LeaveIfError(iChanges.Append(item));
		}
	}

EXPORT_C CLogChangeDefinition* CLogChangeDefinition::NewL(TInt aGranularity)
	{
	CLogChangeDefinition* self = new(ELeave) CLogChangeDefinition(aGranularity);
	return self;
	}

EXPORT_C CLogChangeDefinition* CLogChangeDefinition::NewL(const CLogChangeDefinition& aCopy)
	{
	const TInt count = Max(1, aCopy.Count());
	CLogChangeDefinition* self = new(ELeave) CLogChangeDefinition(count);
	CleanupStack::PushL(self);
	self->ConstructL(aCopy);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CLogChangeDefinition* CLogChangeDefinition::NewL(RReadStream& aStream)
	{
	CLogChangeDefinition* self = new(ELeave) CLogChangeDefinition(KLogChangeDefinitionDefaultGranularity);
	CleanupStack::PushL(self);
	aStream >> *self;
	CleanupStack::Pop(self);
	return self;
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

EXPORT_C TInt CLogChangeDefinition::Count() const
	{
	return iChanges.Count();
	}

EXPORT_C TLogDatabaseChangeType CLogChangeDefinition::At(TInt aIndex) const
	{
	const TLogShdChangeDetails& entry = iChanges[aIndex];
	return entry.iType;
	}

EXPORT_C TLogDatabaseChangeType CLogChangeDefinition::At(TInt aIndex, TLogId& aId) const
	{
	const TLogShdChangeDetails& entry = iChanges[aIndex];
	aId = entry.iId;
	return entry.iType;
	}

EXPORT_C TLogDatabaseChangeType CLogChangeDefinition::At(TInt aIndex, TLogId& aId, TInt& aViewIndex) const
	{
	const TLogShdChangeDetails& entry = iChanges[aIndex];
	aId = entry.iId;
	aViewIndex = entry.iViewIndex;
	return entry.iType;
	}

EXPORT_C TInt CLogChangeDefinition::Find(TLogId aId) const
	{
	// Linear search
	const TLogShdChangeDetails entryToFind(aId);
	TIdentityRelation<TLogShdChangeDetails> relation(CompareEntryIds);
	const TInt position = iChanges.Find(entryToFind, relation);
	return position;
	}

EXPORT_C TInt CLogChangeDefinition::Find(TLogId aId, TLogDatabaseChangeType aType) const
	{
	// First try and find any log id regardless of change type
	TInt position = Find(aId);
	if	(position >= 0)
		{
		const TInt count = iChanges.Count();

		// Keep iterating through the changes until we 
		//
		// a) Reach the end of the changes OR
		// b) Reach a change corresponding to a different log id OR
		// c) Find the change we're interested in
		TInt i = position;
		position = KErrNotFound;
		//
		for(; i<count; i++)
			{
			const TLogShdChangeDetails& entry = iChanges[i];
			if	(entry.iId == aId)
				{
				// Now check the change type
				if	(entry.iType == aType)
					{
					// (c)
					position = i;
					break;
					}
				}
			else
				{
				// (b)
				break;
				}
			}
		// (a) is implicit
		}
	return position;
	}

EXPORT_C TInt CLogChangeDefinition::FindByViewIndex(TInt aViewIndex) const
	{
	// Cannot use binary search
	const TLogShdChangeDetails entryToFind(KLogNullId, ELogChangeTypeUndefined, aViewIndex);
	TIdentityRelation<TLogShdChangeDetails> relation(CompareViewIndicies);
	const TInt position = iChanges.Find(entryToFind, relation);
	return position;
	}

EXPORT_C void CLogChangeDefinition::AddL(TLogId aId, TLogDatabaseChangeType aType, TInt aViewIndex)
	{
	switch(aType)
		{
	case ELogChangeTypeEventChanged:
	case ELogChangeTypeEventChangedHidden:
	case ELogChangeTypeEventAdded:
	case ELogChangeTypeEventDeleted:
	case ELogChangeTypeLogCleared:
		AddToContainerL(aId, aType, aViewIndex);
		break;
	//
	default:
	case ELogChangeTypeUndefined:
		__ASSERT_ALWAYS(0, Panic(ELogUndefinedChangeType));
		break;
		}
	}

EXPORT_C void CLogChangeDefinition::Reset()
	{
	iChanges.Reset();
	iChanges.GranularCompress();
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

EXPORT_C void CLogChangeDefinition::InternalizeL(RReadStream& aStream)
	{
	TLogShdChangeDetails entry;
	//
	TCardinality count;
	aStream >> count;
	//
	for(TInt i=0; i<count; i++)
		{
		entry.iId = static_cast<TLogId>(aStream.ReadInt32L());
		entry.iType = static_cast<TLogDatabaseChangeType>(aStream.ReadUint8L());
		entry.iViewIndex = static_cast<TInt>(aStream.ReadInt32L());
		//
		User::LeaveIfError(iChanges.Append(entry));
		}
	}

EXPORT_C void CLogChangeDefinition::ExternalizeL(RWriteStream& aStream) const
	{
	TCardinality count = Count();
	aStream << count;
	//
	for(TInt i=0; i<count; i++)
		{
		const TLogShdChangeDetails& entry = iChanges[i];
		//
		aStream.WriteInt32L(entry.iId);
		aStream.WriteUint8L(entry.iType);
		aStream.WriteInt32L(entry.iViewIndex);
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////

TBool CLogChangeDefinition::CompareEntryIds(const TLogShdChangeDetails& aLeft, const TLogShdChangeDetails& aRight)
	{
	return (aLeft.iId == aRight.iId);
	}

TBool CLogChangeDefinition::CompareViewIndicies(const TLogShdChangeDetails& aLeft, const TLogShdChangeDetails& aRight)
	{
	return (aLeft.iViewIndex == aRight.iViewIndex);
	}

void CLogChangeDefinition::AddToContainerL(TLogId aId, TLogDatabaseChangeType aType, TInt aViewIndex)
	{
	const TLogShdChangeDetails entry(aId, aType, aViewIndex);
	const TInt error = iChanges.Append(entry);
	User::LeaveIfError(error);
	}


