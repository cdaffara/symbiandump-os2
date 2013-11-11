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

#include "UT_STD.H"

EXPORT_C void RStoreReadStream::OpenL(const CStreamStore& aStore,TStreamId anId)
/** Opens and prepares an existing stream for reading. The function leaves if it 
cannot complete successfully.

@param aStore A reference to the store containing the stream with the specified 
stream id.
@param anId The stream id of the stream to be read. */
	{
	if (anId==KNullStreamId)
		__LEAVE(KErrNotFound);
	RReadStream::Attach(aStore.DoReadL(anId));
	}

EXPORT_C void RStoreReadStream::OpenLC(const CStreamStore& aStore,TStreamId anId)
/** Opens and prepares an existing stream for reading, leaving a cleanup item on 
the cleanup stack. The function leaves if it cannot complete successfully.

Placing a cleanup item for the RStoreReadStream onto the cleanup stack allows 
allocated resources to be cleaned up if a subsequent leave occurs.

@param aStore A reference to the store containing the stream with the specified 
id.
@param anId The stream id of the stream to be read. */
	{
	OpenL(aStore,anId);
	PushL();
	}

EXPORT_C TStreamId RStoreWriteStream::CreateL(CStreamStore& aStore)
/** Creates a new stream.

The function creates a new stream in the specified store and prepares the 
stream for writing. The function returns the new stream id, and leaves if 
it cannot complete successfully.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store which is to contain the new stream.
@return The stream id of the newly created stream.
@see RWriteStream::Release()
@see RWriteStream::CommitL() */
	{
	TStreamId id;
	RWriteStream::Attach(aStore.DoCreateL(id));
	return id;
	}

EXPORT_C TStreamId RStoreWriteStream::CreateLC(CStreamStore& aStore)
/** Creates a new stream, putting a cleanup item onto the cleanup stack.

The function creates a new stream in the specified store and prepares the 
stream for writing. The function returns the new stream id, and leaves if 
it cannot complete successfully.

Putting a cleanup item onto the cleanup stack allows allocated resources to 
be cleaned up if a subsequent leave occurs.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store which is to contain the new stream.
@return The stream id of the newly created stream.
@see RWriteStream::Release()
@see RWriteStream::CommitL() */
	{
	TStreamId id=CreateL(aStore);
	PushL();
	return id;
	}

EXPORT_C void RStoreWriteStream::OpenL(CStreamStore& aStore,TStreamId anId)
/** Opens an existing stream and prepares it for overwriting.

The function leaves if cannot complete successfully.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store containing the stream.
@param anId The id of the stream to be overwritten.
@see RWriteStream::Release()
@see RWriteStream::CommitL()
@see CDirectFileStore */
	{
	if (anId==KNullStreamId)
		__LEAVE(KErrNotFound);
	RWriteStream::Attach(aStore.DoWriteL(anId));
	}

EXPORT_C void RStoreWriteStream::OpenLC(CStreamStore& aStore,TStreamId anId)
/** Opens an existing stream, prepares it for overwriting, and puts a cleanup item 
onto the cleanup stack.

The function leaves if cannot complete successfully.

Putting a cleanup item onto the cleanup stack allows allocated resources to 
be cleaned up if a subsequent leave occurs.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store containing the stream.
@param anId The id of the stream to be overwritten.
@see RWriteStream::Release()
@see RWriteStream::CommitL()
@see CDirectFileStore */
	{
	OpenL(aStore,anId);
	PushL();
	}

EXPORT_C void RStoreWriteStream::ReplaceL(CStreamStore& aStore,TStreamId anId)
/** Opens an existing stream and prepares it for replacement.

The function leaves if it cannot complete successfully.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store containing the stream.
@param anId The id of the stream to be replaced.
@see RWriteStream::Release()
@see RWriteStream::CommitL()
@see CDirectFileStore */
	{
	if (anId==KNullStreamId)
		__LEAVE(KErrNotFound);
	RWriteStream::Attach(aStore.DoReplaceL(anId));
	}

EXPORT_C void RStoreWriteStream::ReplaceLC(CStreamStore& aStore,TStreamId anId)
/** Opens an existing stream, prepares it for replacement and puts a cleanup item 
onto the cleanup stack.

The function leaves if it cannot complete successfully.

Placing a cleanup item onto the cleanup stack allows allocated resources to 
be cleaned up if a subsequent leave occurs.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store containing the stream.
@param anId The id of the stream to be replaced.
@see RWriteStream::Release()
@see RWriteStream::CommitL()
@see CDirectFileStore */
	{
	ReplaceL(aStore,anId);
	PushL();
	}

EXPORT_C void RStoreWriteStream::AppendL(CStreamStore& aStore,TStreamId anId)
/** Opens an existing stream and prepares it for appending.

The function leaves if it cannot complete successfully.

Note that a call to this function must be matched by a call to CommitL() before 
this object is disposed of.

@param aStore A reference to the store containing the stream.
@param anId The id of the stream to be appended.
@see RWriteStream::Release()
@see RWriteStream::CommitL()
@see CDirectFileStore
@see CPermanentFileStore */
	{
	AppendLC(aStore,anId);
	CleanupStack::Pop();
	}

EXPORT_C void RStoreWriteStream::AppendLC(CStreamStore& aStore,TStreamId anId)
/** Opens an existing stream, prepares it for appending, and puts a cleanup item 
onto the cleanup stack. 

The function leaves if it cannot complete successfully.

Putting a cleanup item onto the cleanup stack allows allocated resources to 
be cleaned up if a subsequent leave occurs.

Note that call to this function must be matched by a call to CommitL() before 
the RStoreWriteStream object is disposed of.

@param aStore A reference to the store containing the stream.
@param anId The id of the stream to be appended.
@see RWriteStream::Release()
@see RWriteStream::CommitL()
@see CDirectFileStore
@see CPermanentFileStore */
	{
	OpenLC(aStore,anId);
	Sink()->SeekL(MStreamBuf::EWrite,EStreamEnd);
	}

