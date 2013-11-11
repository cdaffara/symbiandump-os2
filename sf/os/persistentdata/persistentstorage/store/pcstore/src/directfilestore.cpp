// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <memory>
#include <pcstore/directfilestore.h>
#include <pcstore/storewritestream.h>
#include <pcstore/storereadstream.h>
#include <pcstore/checkeduid.h>
#include <pcstore/storeexception.h>
#include "filestreambuf.h"

using namespace std;

namespace PCStore
{
const TInt KFileStoreStartOffset=sizeof(TCheckedUid);

/**
Constructs a new direct file store object with the specified file name and mode.

@param aFile The file name of the store.
@param aMode The mode in which the object is created.
*/
CDirectFileStore::CDirectFileStore(const char* aFileName, TStoreMode aMode)
	:iMode(aMode), iRoot(0), iFileStreamBuf(0)
	{
	iFileStreamBuf = new CFileStreamBuf(aFileName, aMode == EReadStore? CFileStreamBuf::EReadFile : CFileStreamBuf::EWriteFile);
	}

/**
Destructs a direct file store object.
*/
CDirectFileStore::~CDirectFileStore()
	{
	delete iFileStreamBuf;
	}

/**
Creates a write stream with the given stream id.

This private function will be called to create write streams with fixed id, i.e. 
streams for setting type and setting root id. It is also called by CreateWriteStream() 
after a stream id has been created.

@param aId The reference to the stream id with which the write stream is created.
@return The pointer to the newly created write stream.
@exception TStoreException::EStoreModeError The stream's mode is different from the store's.
@exception TStoreException::EStreamExclusionError An existing stream is still opened.
@exception TStoreException::EFileSeekError Error occurs when seeking within the file due 
to an incorrect stream id or other reason.
*/
CStoreWriteStream* CDirectFileStore::CreateIdWriteStream(TStreamId aId)
	{
	//check the store mode
	if(iMode != EWriteStore) 
		{
		throw TStoreException(TStoreException::EStoreModeError);
		}
	
	CStoreWriteStream* pCStoreWriteStream = new CStoreWriteStream(*iFileStreamBuf, aId);
	return pCStoreWriteStream;
	}

/**
Creates a write stream which starts writing from the end of the file. 

@param aId The reference to the stream id to carry the new stream Id when this function returns.
@return The pointer to the newly created write stream.
@exception TStoreException::EStoreModeError The stream's mode is different from the store's.
@exception TStoreException::EStreamExclusionError An existing stream is still opened.
@exception TStoreException::EFileSeekError Error occurs when seeking within the file due 
to an incorrect stream id or other reason.
*/
CStoreWriteStream* CDirectFileStore::CreateWriteStream(TStreamId& aId)
	{	
	aId = iFileStreamBuf->GetNewStreamId();
	return CreateIdWriteStream(aId);
	}

/**
Creates a read stream with the given stream id.

Calls the CFileStreamBuf::StreamOpen() to flag the new stream is opened. An exception 
will be thrown if an existing stream has been opened.

@param aId The reference stream id with which the read stream is created.
@return The pointer to the newly created read stream.
@exception TStoreException::EStoreModeError The stream's mode is different from the store's.
@exception TStoreException::EStreamExclusionError An existing stream is still opened.
@exception TStoreException::EFileSeekError Error occurs when seeking within the file due 
to an incorrect stream id or other reason.
*/
CStoreReadStream* CDirectFileStore::CreateReadStream(TStreamId aId)
	{
	//check the store mode
	if(iMode != EReadStore) 
		{
		throw TStoreException(TStoreException::EStoreModeError);
		}
	CStoreReadStream* pCStoreReadStream = new CStoreReadStream(*iFileStreamBuf, aId);

	return pCStoreReadStream;
	}

/**
Sets the type of the direct file store.

Converts the specified uid type into uid with check sum and externalizes it to the 
beginning of the store file. Also externalizes the root id of this file store.

@param aType The reference to the uid type to be set to the direct file store.
*/
void CDirectFileStore::SetType(const TUidType& aType)
	{
	TCheckedUid chk(aType);
	TStreamId strId(0);
	auto_ptr <CStoreWriteStream> ws (CreateIdWriteStream(strId));
	(*ws)<<chk; // output the UID
	Externalize(*ws); // output the root id
	}

/**
Checks the type of the opened direct file store.

Internalizes the checked uid type and verify it. Also restores the root id from 
the store file and sets it as the root id of this file store.
@exception TStoreException::EStoreNotSupported The store file is not a supported 
type due to the invalid type if or it has no stream.
*/
void CDirectFileStore::CheckType()
	{
	TCheckedUid chk;
	TStreamId strId(0);
	auto_ptr <CStoreReadStream> rs (CreateReadStream(strId));
	(*rs) >> chk; 
	TUidType uidType = chk.UidType();
	if ((!uidType.IsValid()) || uidType[0]!=KDirectFileStoreLayoutUid)
		{
		throw TStoreException(TStoreException::EStoreNotSupported);
		}
	// restores and checks the root stream id.
	Internalize(*rs); 

	if(iRoot.Value() == 0)
		{
		throw TStoreException(TStoreException::EStoreNotSupported);
		}
	}

/**
Sets the root id of this direct file store as the specified stream id.

Also externalizes the specified stream id to the store file.

@param aId The reference to the stream id to be set as the root id.
@exception TStoreException::EStoreModeError This function is called by a direct file store
opened for read.
*/
void CDirectFileStore::SetRoot(TStreamId aId)
	{
	//check the store mode
	if(iMode != EWriteStore) 
		{
		throw TStoreException(TStoreException::EStoreModeError);
		}
	iRoot = aId;
	TStreamId rootId(KFileStoreStartOffset);
	auto_ptr <CStoreWriteStream> ws (CreateIdWriteStream(rootId));
	Externalize(*ws);
	}

/** 
Gets the root stream id of this direct file store.

@return The root stream id of this direct file store.
*/
TStreamId CDirectFileStore::Root() const
	{
	return  iRoot;
	}

/** 
Creates a direct file store object for internalization with the specified file name.

This is the only method to create a direct file store object for internalization.

@param aFileName The name of the file to create the direct file store for internalization.
@return The pointer to the created direct file store.
@exception TStoreException::EFileOpenError Error occurs when the file is opened.
@exception TStoreException::EStoreNotSupported The store file is not a supported 
type due to the invalid type if or it has no stream.
*/
CDirectFileStore* CDirectFileStore::Open(const char* aFileName)
	{
	auto_ptr <CDirectFileStore> directFileStore (new CDirectFileStore(aFileName, EReadStore));
	directFileStore->CheckType();
	return directFileStore.release();
	}

/** 
Creates a direct file store object for externalization with the specified file name.

This method creates direct file store object for externalization.

@param aFileName The name of the file to create the direct file store for externalization.
@return The pointer to the created direct file store.
@exception TStoreException::EFileOpenError Error occurs when the file is opened.
*/
CDirectFileStore* CDirectFileStore::Replace(const char* aFileName)
	{
	auto_ptr <CDirectFileStore> directFileStore (new CDirectFileStore(aFileName, EWriteStore));
	directFileStore->SetType(KDirectFileStoreLayoutUid);
	return directFileStore.release();
	}

/** 
Creates a direct file store object for externalization with the specified file name and Uid type.

This method creates direct file store object for externalization.
This method allows user customisation of the direct file store type.

@param aFileName The name of the file to create the direct file store for externalization.
@param aType The user customized Uid type for the direct file store.
@return The pointer to the created direct file store.
@exception TStoreException::EFileOpenError Error occurs when the file is opened.
@exception TStoreException::EStoreNotSupported The Uid type passed in is invalid.
*/
CDirectFileStore* CDirectFileStore::Replace(const char* aFileName, const TUidType& aType)
	{
	auto_ptr <CDirectFileStore> directFileStore (new CDirectFileStore(aFileName, EWriteStore));
	if (aType[0] != KDirectFileStoreLayoutUid)
		{
		throw TStoreException(TStoreException::EStoreNotSupported);;
		}
	directFileStore->SetType(aType);
	return directFileStore.release();
	}

/** 
Externalizes this object to the specified write stream.

@param aWs The write stream to which the object should be externalized.
@exception TStoreException::EFileWriteError Error occurs when writing to the file.
*/
void CDirectFileStore::Externalize(CStoreWriteStream& aWs) const 
	{
	iRoot.Externalize(aWs);
	}

/** 
Internalizes this object from the specified read stream.

@param aRs The read stream from which the object is to be internalized.
@exception TStoreException::EFileReadError Error occurs when reading from the file.
*/
void CDirectFileStore::Internalize(CStoreReadStream& aRs)
	{
	iRoot.Internalize(aRs);
	}
}
