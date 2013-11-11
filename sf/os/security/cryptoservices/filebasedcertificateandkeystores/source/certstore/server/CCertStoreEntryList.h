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
* CCertStoreEntryList class implementation
*
*/


/**
 @file 
 @internalTechnology
*/

#ifndef __CCERTSTOREENTRYLIST_H__
#define __CCERTSTOREENTRYLIST_H__

#include <s32file.h>

class CCertStoreEntry;

/**
 * An in-memory list of the certs in the store.
 */
class CCertStoreEntryList : public CBase
	{
public:
	/// Create a new, empty cert list
	static CCertStoreEntryList* NewLC();
	
	/// Read the cert list from a stream
	static CCertStoreEntryList* NewL(RReadStream& aStream);

	virtual ~CCertStoreEntryList();

	/// Write the cert list to a stream
	void ExternalizeL(RWriteStream& aStream) const;

	/// Get the number of certificates in the list
	TInt Count() const;

	/// Get the index of a mapping given the cert handle
	/// @return The index of the cert or KErrNotFound if it is not present.
	TInt IndexForHandle(TInt aHandle) const;

	/// Determine whether a certificate exists with the specified label
	TBool LabelExists(const TDesC& aLabel) const;

	/// Get a mapping given the cert handle.
	/// @leave KErrNotFound If the specified cert is not present.
	const CCertStoreEntry& GetByHandleL(TInt aHandle) const;
	
	/// Get a mapping given its index.
	const CCertStoreEntry& GetByIndex(TInt aIndex) const;

	/// Get the next free handle to use when adding a cert
	TInt NextFreeHandle() const;

	/// Add a mapping and return its index, takes ownership
	TInt AppendL(CCertStoreEntry* aEntry);
	
	/// Removes and returns an entry from the list, relinquishes ownership
	CCertStoreEntry* Remove(TInt aIndex);

	/// Replace an entry and return the old one, takes/relinquishes ownership
	CCertStoreEntry* Replace(TInt aIndex, CCertStoreEntry* aEntry);	
	
 private:
	CCertStoreEntryList();
	void InternalizeL(RReadStream& aStream);

 private:
	RPointerArray<CCertStoreEntry> iEntries;
	};

#endif
