// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "UE_STD.H"

#include <pbe.h>

#ifndef __TOOLS2__
#include "pbencryptor.h"
#endif


EXPORT_C CSecureStore* CSecureStore::NewL(CStreamStore& aHost,const CPBEncryptSet& aKey)
/**
Instantiates a secure store.

@leave KErrNoMemory
@param aHost The stream store acting as host for the secure store's streams.
@param aKey A Password Based Encryption Object used to generate decryption and encryption 
handling objects. Ownership of this object remains with the caller who should delete it when it is no longer needed.

@return A pointer to the new secure store object.
*/
	{
	return new(ELeave) CSecureStore(aHost,aKey);
	}

EXPORT_C CSecureStore* CSecureStore::NewLC(CStreamStore& aHost,const CPBEncryptSet& aKey)
/**
Instantiates a secure store and pushes a pointer to the object onto the cleanup stack.

@leave KErrNoMemory
@param aHost The stream store acting as host for the secure store's streams.
@param aKey A Password Based Encryption Object used to generate decryption and encryption 
handling objects. Ownership of this object remains with the caller who should delete it when it is no longer needed.

@return A pointer to the new secure store object.
*/
	{
	CSecureStore* store=NewL(aHost,aKey);
	CleanupStack::PushL(store);
	return store;
	}

//	Not exported
CSecureStore::CSecureStore(CStreamStore& aHost,const CPBEncryptSet& aKey)
	: iHost(&aHost),
    iKey(aKey)
	{}

//
// Open a stream for reading from the buffer at anId.
//
EXPORT_C MStreamBuf* CSecureStore::DoReadL(TStreamId anId) const
	{
	HDecryptFilter* filter=new(ELeave) HDecryptFilter;
	CleanupDeletePushL(filter);
	RStoreReadStream stream;
	stream.OpenLC(Host(),anId);
	
	CPBDecryptor* decryptor=iKey.NewDecryptLC();
	filter->SetL(stream.Source(),decryptor,filter->ERead|filter->EAttached);
   		
	CleanupStack::Pop(3, filter);	//	decryptor, stream, filter 
	return filter;
	}

//	Determine key type and create appropriate encryptor
void CSecureStore::setEncryptFilterL(HEncryptFilter& aFilter, RStoreWriteStream& aStream)
	{
	CPBEncryptor* encryptor=iKey.NewEncryptLC();
	aFilter.SetL(aStream.Sink(),encryptor,aFilter.EWrite|aFilter.EAttached);
	CleanupStack::Pop(encryptor);
	}

//
// Create a new buffer and open a stream for writing to it.
//
EXPORT_C MStreamBuf* CSecureStore::DoCreateL(TStreamId& anId)
	{
	HEncryptFilter* filter=new(ELeave) HEncryptFilter;
	CleanupDeletePushL(filter);
	RStoreWriteStream stream;
	anId=stream.CreateLC(Host());
	setEncryptFilterL(*filter, stream);
	CleanupStack::Pop(2);	//	stream, filter
	return filter;
	}

EXPORT_C TStreamId CSecureStore::DoExtendL()
	{
	return Host().ExtendL();
	}

EXPORT_C void CSecureStore::DoDeleteL(TStreamId anId)
	{
	Host().DeleteL(anId);
	}

EXPORT_C MStreamBuf* CSecureStore::DoWriteL(TStreamId anId)
	{
	HEncryptFilter* filter=new(ELeave) HEncryptFilter;
	CleanupDeletePushL(filter);
	RStoreWriteStream stream;
	stream.OpenLC(Host(), anId);
	setEncryptFilterL(*filter, stream);
	CleanupStack::Pop(2);	//	stream, filter
	return filter;	
	}

EXPORT_C MStreamBuf* CSecureStore::DoReplaceL(TStreamId anId)
	{
	HEncryptFilter* filter=new(ELeave) HEncryptFilter;
	CleanupDeletePushL(filter);
	RStoreWriteStream stream;
	stream.ReplaceLC(Host(), anId);
	setEncryptFilterL(*filter, stream);
	CleanupStack::Pop(2);	//	stream, filter
	return filter;	
	}

EXPORT_C void CSecureStore::DoCommitL()
	{
	Host().CommitL();
	}

EXPORT_C void CSecureStore::DoRevertL()
	{
	Host().RevertL();
	}
