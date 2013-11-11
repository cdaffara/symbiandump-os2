/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "CCertStoreEntryList.h"
#include "CCertStoreEntry.h"
#include "fsdatatypes.h"
#include "fstokenutil.h"

/////////////////////////////////////////////////////////////////////////////////
//CCertStoreEntryList
/////////////////////////////////////////////////////////////////////////////////

CCertStoreEntryList* CCertStoreEntryList::NewLC()
	{
	CCertStoreEntryList* self = new(ELeave) CCertStoreEntryList();
	CleanupStack::PushL(self);
	return self;
	}

CCertStoreEntryList* CCertStoreEntryList::NewL(RReadStream& aStream)
	{
	CCertStoreEntryList* self = new(ELeave) CCertStoreEntryList();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	CleanupStack::Pop();
	return self;
	}

CCertStoreEntryList::CCertStoreEntryList()
	{
	}

CCertStoreEntryList::~CCertStoreEntryList()
	{
	iEntries.ResetAndDestroy();
	iEntries.Close();
	}

void CCertStoreEntryList::ExternalizeL(RWriteStream& aStream) const
	{
	TInt count = iEntries.Count();
	aStream.WriteInt32L(count);
	for (TInt i = 0 ; i < count ; ++i)
		{
		aStream << *iEntries[i];
		}
	}

void CCertStoreEntryList::InternalizeL(RReadStream& aStream)
	{
	TInt count = aStream.ReadInt32L();
	
	for (TInt i = 0 ; i < count ; ++i)
		{
		CCertStoreEntry* entry = CCertStoreEntry::NewLC(aStream);

		// Check certificate id is unique
		TInt handle = entry->CertInfo().CertificateId();
		if (IndexForHandle(handle) != KErrNotFound)
			{
			User::Leave(KErrCorrupt);
			}
		
		AppendL(entry);
		CleanupStack::Pop(entry);		
		}
	}

TInt CCertStoreEntryList::Count() const
	{
	return iEntries.Count();
	}

TInt CCertStoreEntryList::AppendL(CCertStoreEntry* aCertInfo)
	{
	assert(aCertInfo, EPanicCertStoreEntryListAppendArguments);
	User::LeaveIfError(iEntries.Append(aCertInfo));
	return iEntries.Count() - 1;
	}

CCertStoreEntry* CCertStoreEntryList::Remove(TInt aIndex)
	{
	CCertStoreEntry* entry = iEntries[aIndex];
	iEntries.Remove(aIndex);
	return entry;
	}

CCertStoreEntry* CCertStoreEntryList::Replace(TInt aIndex, CCertStoreEntry* aNewEntry)
	{
	assert(aNewEntry, EPanicCertStoreEntryListReplaceArguments);
	CCertStoreEntry* entry = iEntries[aIndex];
	iEntries[aIndex] = aNewEntry;
	return entry;
	}

TInt CCertStoreEntryList::IndexForHandle(TInt aHandle) const
	{
	for (TInt i = 0 ; i < iEntries.Count() ; ++i)
		{
		CCertStoreEntry* entry = iEntries[i];
		if (entry->CertInfo().Reference() == aHandle)
			{
			return i;
			}
		}
	
	return KErrNotFound;
	}

TBool CCertStoreEntryList::LabelExists(const TDesC& aLabel) const
	{
	TInt end = iEntries.Count() ;
	for (TInt i = 0 ; i < end ; ++i)
		{
		CCertStoreEntry* entry = iEntries[i];
		if (entry->CertInfo().Label() == aLabel)
			{
			return ETrue;
			}
		}
	
	return EFalse;
	}

const CCertStoreEntry& CCertStoreEntryList::GetByIndex(TInt aIndex) const
	{
	return *iEntries[aIndex];
	}

const CCertStoreEntry& CCertStoreEntryList::GetByHandleL(TInt aHandle) const
	{
	TInt index = IndexForHandle(aHandle);
	User::LeaveIfError(index);
	return GetByIndex(index);
	}

TInt CCertStoreEntryList::NextFreeHandle() const 
	{
	TInt count = iEntries.Count();
	TInt maxHandle = -1;
	for (TInt i = 0; i < count; i++)
		{
		CCertStoreEntry* entry = iEntries[i];
		if (entry->CertInfo().Reference() > maxHandle)
			{
			maxHandle = entry->CertInfo().Reference();
			}
		}
	
	return maxHandle + 1;
	}
