/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "CSWICertStoreEntryList.h"
#include "CSWICertStoreEntry.h"
#include <cctcertinfo.h>

CSWICertStoreEntryList* CSWICertStoreEntryList::NewL(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex, TInt aStoreIndex)
	{
	CSWICertStoreEntryList* self = new(ELeave) CSWICertStoreEntryList();
	CleanupStack::PushL(self);
	self->InternalizeL(aStream, aToken, aCertIndex, aStoreIndex);
	CleanupStack::Pop(self);
	return self;
	}

CSWICertStoreEntryList* CSWICertStoreEntryList::NewL()
	{
	CSWICertStoreEntryList* self = new(ELeave) CSWICertStoreEntryList();
	return self;
	}

CSWICertStoreEntryList::CSWICertStoreEntryList()
	{
	}

CSWICertStoreEntryList::~CSWICertStoreEntryList()
	{
	iEntries.ResetAndDestroy();
	iEntries.Close();
	}

void CSWICertStoreEntryList::InternalizeL(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex, TInt aStoreIndex)
	{
	TInt count = aStream.ReadInt32L();
	
	for (TInt i = 0 ; i < count ; ++i)
		{
		CSWICertStoreEntry* entry = CSWICertStoreEntry::NewLC(aStream, aToken, aCertIndex+i, aStoreIndex);
		iEntries.AppendL(entry);
		CleanupStack::Pop(entry);		
		}
	}

TInt CSWICertStoreEntryList::Count() const
	{
	return iEntries.Count();
	}

TBool CSWICertStoreEntryList::LabelExists(const TDesC& aLabel) const
	{
	TInt count = iEntries.Count();
	for (TInt i = 0 ; i < count ; ++i)
		{
		CSWICertStoreEntry* entry = iEntries[i];
		if (entry->CertInfo().Label() == aLabel)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

TInt CSWICertStoreEntryList::AppendL(CSWICertStoreEntry* aCertInfo)
	{
	User::LeaveIfError(iEntries.Append(aCertInfo));
	return iEntries.Count() - 1;
	}

void CSWICertStoreEntryList::Remove(TInt aIndex)
	{
	CSWICertStoreEntry* entry = iEntries[aIndex];
	iEntries.Remove(aIndex);
	delete entry;
	}

const CSWICertStoreEntry& CSWICertStoreEntryList::GetByIndex(TInt aIndex) const
	{
	return *iEntries[aIndex];
	}

const CSWICertStoreEntry& CSWICertStoreEntryList::GetByHandleL(const TCTTokenObjectHandle& aHandle) const
	{
	TInt index = KErrNotFound;
	
	for (TInt i = 0 ; i < iEntries.Count() ; ++i)
		{
		CSWICertStoreEntry* entry = iEntries[i];
		if (aHandle == entry->CertInfo().Handle())
			{
			index = i;
			break;
			}
		}

	User::LeaveIfError(index);
	return GetByIndex(index);
	}
