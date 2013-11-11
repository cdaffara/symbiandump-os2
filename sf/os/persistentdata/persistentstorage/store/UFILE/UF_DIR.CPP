// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UF_STD.H"

EXPORT_C TUid CDirectFileStore::Layout() const 
/** Gets the UID that uniquely identifies this file store as a direct file store.

@return KDirectFileStoreLayoutUid. */
	{
	return KDirectFileStoreLayoutUid;
	}

EXPORT_C CDirectFileStore::CDirectFileStore(RFile& aFile)
//
// Constructor creating a new file store.
//
	: CFileStore(aFile)
	{}

EXPORT_C CDirectFileStore::CDirectFileStore(RFileBuf& aBuf,const TUidType& aType)
//
// Constructor opening an existing file store.
//
	: CFileStore(aBuf,aType)
	{}

EXPORT_C MStreamBuf* CDirectFileStore::DoReadL(TStreamId anId) const
//
// Open a stream for reading.
//
	{
	return HDirectStoreBuf::OpenL(Host(),anId,HDirectStoreBuf::ERead);
	}

EXPORT_C MStreamBuf* CDirectFileStore::DoCreateL(TStreamId& anId)
//
// Create a new stream and open for both reading and writing.
//
	{
	return HDirectStoreBuf::CreateL(Host(),anId,HDirectStoreBuf::ERead|HDirectStoreBuf::EWrite);
	}

EXPORT_C void CDirectFileStore::ExternalizeL(RWriteStream& aStream) const
//
// Externalize this store's structure, consisting of just the root stream id.
//
	{
	aStream<<iRoot;
	}

EXPORT_C void CDirectFileStore::InternalizeL(RReadStream& aStream)
//
// Internalize this store's structure, consisting of just the root stream id.
//
	{
	aStream>>iRoot;
	}

EXPORT_C void CDirectFileStore::DoSetRootL(TStreamId anId)
//
// Set the root stream id and flag a change to this store's structure.
//
	{
	iRoot=anId;
	ChangedL();
	}

EXPORT_C CFileStore* CDirectFileStore::DoNewL(RFile& aFile)
//
// Factory function creating a new file.
//
	{
	return new(ELeave) CDirectFileStore(aFile);
	}

