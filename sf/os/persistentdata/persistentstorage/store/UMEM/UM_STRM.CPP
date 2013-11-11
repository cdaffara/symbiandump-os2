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

#include "UM_STD.H"

/**
Constructs the RMemReadStream object and prepares the stream hosted
in the specified plain memory location for reading.

@param aPtr The start address for the area of memory that will be
the source of this stream.
@param aLength The length of the area of memory.

@see RMemReadStream::Open
*/
EXPORT_C RMemReadStream::RMemReadStream(const TAny* aPtr,TInt aLength)
	{
	Open(aPtr,aLength);
	}

/**
Open a stream that reads from a pointer of any type.

To close the stream and free its resources call Close()

@param aPtr The start address for the area of memory that will be
the source of this stream.
@param aLength The length of the area of memory.

@see TMemBuf::Set
@see RReadStream::Attach
*/
EXPORT_C void RMemReadStream::Open(const TAny* aPtr,TInt aLength)
	{
	iSource.Set((TUint8*)aPtr,(TUint8*)aPtr+aLength,iSource.ERead);
	RReadStream::Attach(&iSource);
	}

/**
Constructs the RMemWriteStream object and prepares a stream to be hosted
in the specified memory location (described by the TAny aPtr argument)
for writing using the Open() function.

@param aPtr The start address for the area of memory that is the sink of
this stream.
@param aMaxLength The maximum length of the area of memory.

@see RMemWriteStream::Open
*/
EXPORT_C RMemWriteStream::RMemWriteStream(TAny* aPtr,TInt aMaxLength)
	{
	Open(aPtr,aMaxLength);
	}

/**
Prepares a stream for writing.

When streaming takes place any existing data in the memory location will
be replaced. Note that the length of memory must be big enough to accommodate
the expected streamed data otherwise the subsequent streaming operation will
leave with KErrOverFlow.

To close the stream and free its resources call Close()

@param aPtr The start address for the area of memory that is the sink of
this stream.
@param aMaxLength The maximum length of the area of memory.

@see TMemBuf::Set
@see RWriteStream::Attach
*/
EXPORT_C void RMemWriteStream::Open(TAny* aPtr,TInt aMaxLength)
	{
	iSink.Set((TUint8*)aPtr,(TUint8*)aPtr+aMaxLength,iSink.EWrite);
	RWriteStream::Attach(&iSink);
	}

/**
Constructs the RDesReadStream object and prepares the stream hosted
by the specified descriptor for reading.

@param aDes The descriptor that will be the source of this stream.

@see RDesReadStream::Open
*/
EXPORT_C RDesReadStream::RDesReadStream(const TDesC8& aDes)
	{
	Open(aDes);
	}

/**
Open a stream that reads from a descriptor.

To close the stream and free its resources call Close()

@param aDes The descriptor that will be the source of this stream.

@see TDesBuf::Set
@see RReadStream::Attach
*/
EXPORT_C void RDesReadStream::Open(const TDesC8& aDes)
	{
	TUint8* ptr=(TUint8*)aDes.Ptr();
	iSource.Set(ptr,ptr+aDes.Length(),iSource.ERead);
	RReadStream::Attach(&iSource);
	}

/**
Constructs the RDesWriteStream object and prepares a stream to be
hosted by the specified 8-bit descriptor for writing using the Open()
function.


@param aDes The descriptor hosting the stream.

@see RDesWriteStream::Open
*/
EXPORT_C RDesWriteStream::RDesWriteStream(TDes8& aDes)
	{
	Open(aDes);
	}

/**
Prepares a stream for writing.

When streaming takes place, any existing data in the descriptor will
be replaced. Note that the maximum length of the descriptor must be
big enough to accommodate the expected streamed data, otherwise the
subsequent streaming operation will leave with KErrOverFlow.

A subsequent call to CommitL() sets the length of the descriptor.

To close the stream and free its resources call Close()

@param aDes The descriptor that will be the sink of this stream.

@see TDesBuf::Set
@see RWriteStream::Attach
*/
EXPORT_C void RDesWriteStream::Open(TDes8& aDes)
	{
	aDes.SetLength(0);
	iSink.Set(aDes,iSink.EWrite);
	RWriteStream::Attach(&iSink);
	}

/**
Constructs the RBufReadStream object and opens the stream hosted by the
specified dynamic buffer for reading using the Open() method.

@param aBuf The dynamic buffer that will be the source of this stream.
@param aPos The offset within the dynamic buffer where the stream starts.

@see RBufReadStream::Open
*/
EXPORT_C RBufReadStream::RBufReadStream(const CBufBase& aBuf,TInt aPos)
	{
	Open(aBuf,aPos);
	}

/**
Prepares the stream hosted by the specified dynamic buffer for reading.

To close the stream and free its resources call Close()

@param aBuf The dynamic buffer that will be the source of this stream.
@param aPos The offset within the dynamic buffer where the stream starts.

@see TBufBuf::Set
@see RReadStream::Attach
*/
EXPORT_C void RBufReadStream::Open(const CBufBase& aBuf,TInt aPos)
	{
	iSource.Set((CBufBase&)aBuf,aPos,iSource.ERead);
	RReadStream::Attach(&iSource);
	}

/**
Constructs the RBufWriteStream object and opens a stream that writes to
the specified dynamic buffer using the Open() function.

@param aBuf The dynamic buffer hosting the stream.
@param aPos The offset within the dynamic buffer where the stream is to
start. Defaults to zero, if not explicitly specified. The value cannot
be greater than the current size of the buffer, otherwise a E32USER-CBase 5
panic will be raised when streaming starts.

@see RBufWriteStream::Open
*/
EXPORT_C RBufWriteStream::RBufWriteStream(CBufBase& aBuf,TInt aPos)
	{
	Open(aBuf,aPos);
	}

/**
Open a stream that writes to the dynamic buffer specified in the aBuf argument.

To close the stream and free its resources call Close()

@param aBuf The dynamic buffer hosting the stream.
@param aPos The offset within the dynamic buffer where the stream is to
start. Defaults to zero, if not explicitly specified. The value cannot
be greater than the current size of the buffer, otherwise a E32USER-CBase 5
panic will be raised when streaming starts.

@see TBufBuf::Set
@see RWriteStream::Attach
*/
EXPORT_C void RBufWriteStream::Open(CBufBase& aBuf,TInt aPos)
	{
	iSink.Set(aBuf,aPos,iSink.EWrite);
	RWriteStream::Attach(&iSink);
	}

/**
Open a stream that writes into the dynamic buffer specified in the aBuf argument
using truncate mode.

@param aBuf The dynamic buffer hosting the stream.
@param aPos The offset within the dynamic buffer where the stream is to
start. Defaults to zero, if not explicitly specified. The value cannot
be greater than the current size of the buffer, otherwise a E32USER-CBase 5
panic will be raised when streaming starts.

@see TBufBuf::Set
@see TBufBuf::ETruncate
@see RWriteStream::Attach
*/
EXPORT_C void RBufWriteStream::Truncate(CBufBase& aBuf,TInt aPos)
	{
	iSink.Set(aBuf,aPos,iSink.ETruncate);
	RWriteStream::Attach(&iSink);
	}

/**
Open a stream that writes into the dynamic buffer specified in the aBuf argument
using insert mode.

@param aBuf The dynamic buffer hosting the stream.
@param aPos The offset within the dynamic buffer where the stream is to
start. Defaults to zero, if not explicitly specified. The value cannot
be greater than the current size of the buffer, otherwise a E32USER-CBase 5
panic will be raised when streaming starts.

@see TBufBuf::Set
@see TBufBuf::EInsert
@see RWriteStream::Attach
*/
EXPORT_C void RBufWriteStream::Insert(CBufBase& aBuf,TInt aPos)
	{
	iSink.Set(aBuf,aPos,iSink.EInsert);
	RWriteStream::Attach(&iSink);
	}

