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
* FILECERTSTOREMAPPINGS.H
* CFileCertStoreMappings class implementation
*
*/


/**
 @file 
 @internalTechnology
*/
 
#ifndef __FILECERTSTOREMAPPINGS_H__
#define __FILECERTSTOREMAPPINGS_H__

#include <s32file.h>
#include "tadditionalstoremapping.h"


class CFileCertStoreMappings : public CBase
	{
public:
	static CFileCertStoreMappings* NewL(TStreamId aStreamId, 
		CPermanentFileStore& aStore);
	void ConstructL();
	~CFileCertStoreMappings();
	TInt Count();
	void AddL(CFileCertStoreMapping* aEntry);
	/**
	 * Removes an entry from the list
	 * @param aEntry The entry to delete
	 * @return KErrNotFound if aEntry is not found in the list, KErrNone is the
	 * entry was successfully deleted.
	 */
	TInt Remove(const CCTCertInfo& aEntry);
	TInt SetTempRemove(const CCTCertInfo& aCertInfo, TBool aFlag);
	void UpdateL(const CCTCertInfo& aEntry);
	void ReplaceL();
	/**
	 * Returns the index in the store of aEntry. The function goes through
	 * the store entries and returns the index of the first entry whose label
	 * matches the label of aEntry.
	 * @param aEntry The entry we want the index of.
	 * @return The index of aEntry in the store.
	 */
	TInt Index(const CCTCertInfo& aEntry);
	CFileCertStoreMapping* Mapping(TInt aIndex);
	TStreamId StreamId() const;
	const CCTCertInfo& Entry(TInt aIndex);
	const CCTCertInfo& EntryByHandleL(TInt aHandle) const;
	TInt NextHandle() const;

protected:
	CFileCertStoreMappings(TStreamId aStreamId, CPermanentFileStore& aStore);
	void ExternalizeL(RWriteStream& aStream) const;
	RPointerArray<CFileCertStoreMapping>* iMappings;
	TStreamId iStreamId;
	CPermanentFileStore& iStore;
	};

#endif
