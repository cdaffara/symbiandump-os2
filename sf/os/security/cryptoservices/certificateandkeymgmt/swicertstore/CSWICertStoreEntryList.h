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
* CSWICertStoreEntryList class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __CSWICERTSTOREENTRYLIST_H__
#define __CSWICERTSTOREENTRYLIST_H__

#include <s32file.h>

class MCTToken;
class TCTTokenObjectHandle;
class CSWICertStoreEntry;

/**
 * An in-memory list of the certs in the store.
 */
NONSHARABLE_CLASS(CSWICertStoreEntryList) : public CBase
	{
 public:
	/// Read the cert list from a stream
	static CSWICertStoreEntryList* NewL(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex = 0, TInt aStoreIndex = 0);

	static CSWICertStoreEntryList* NewL();
	
	virtual ~CSWICertStoreEntryList();

	/// Get the number of certificates in the list
	TInt Count() const;

	/// Determine whether a certificate exists with the specified label
	TBool LabelExists(const TDesC& aLabel) const;
	
	/// Add a mapping and return its index, takes ownership
	TInt AppendL(CSWICertStoreEntry* aEntry);
	
	/// Removes and an entry from the list
	void Remove(TInt aIndex);
	
	/// Get a mapping given its index.
	const CSWICertStoreEntry& GetByIndex(TInt aIndex) const;

	/// Get a mapping given the cert handle.
	/// @leave KErrNotFound If the specified cert is not present.
	const CSWICertStoreEntry& GetByHandleL(const TCTTokenObjectHandle& aHandle) const;
	
 private:
	CSWICertStoreEntryList();
	void InternalizeL(RReadStream& aStream, MCTToken& aToken, TInt aCertIndex, TInt aStoreIndex);

 private:
	RPointerArray<CSWICertStoreEntry> iEntries;
	};

#endif
