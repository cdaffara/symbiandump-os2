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

#include "US_STD.H"

EXPORT_C RShareBuf::RShareBuf()
	: iHost(NULL)
/** Constructs an empty shared stream buffer object.

Call one of the Open() functions to prepare the stream. */
	{}

EXPORT_C void RShareBuf::Open(TStreamExchange& aHost,TStreamPos aPos,TInt aMode)
/** Prepares the shared stream buffer for streaming.

The function sets the read mark and/or the write mark to the specified position 
within the host stream.

@param aHost The object that manages shared streaming.
@param aPos The position within the host stream.
@param aMode The streaming mode. This can be read and/or write, as indicated 
by the ERead and EWrite bits.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
	{
	iHost=&aHost;
	if (aMode&ERead)
		iRMark=aPos;
	else
		iRMark.Clear();
	if (aMode&EWrite)
		iWMark=aPos;
	else
		iWMark.Clear();
	}

EXPORT_C void RShareBuf::DoRelease()
//
// Withdraw this stream buffer's marks.
//
	{
	if (iHost!=NULL)
		{
		TStreamExchange& host=*iHost;
		iRMark.Withdraw(host);
		iWMark.Withdraw(host);
		iHost=NULL;
		}
	}

EXPORT_C TInt RShareBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
//
// Read up to aMaxLength bytes.
//
	{
	return iRMark.ReadL(Host(),aPtr,aMaxLength);
	}

EXPORT_C TInt RShareBuf::DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
//
// Read up to aMaxLength bytes asynchronously.
//
	{
	return iRMark.ReadL(Host(),aDes,aMaxLength,aStatus);
	}

EXPORT_C TStreamTransfer RShareBuf::DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer)
//
// Push up to aTransfer bytes into anInput.
//
	{
	return iRMark.ReadL(Host(),anInput,aTransfer);
	}

EXPORT_C void RShareBuf::DoWriteL(const TAny* aPtr,TInt aLength)
//
// Write aLength bytes.
//
	{
	iWMark.WriteL(Host(),aPtr,aLength);
	}

EXPORT_C TInt RShareBuf::DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
//
// Write up to aMaxLength bytes asynchronously.
//
	{
	return iWMark.WriteL(Host(),aDes,aMaxLength,aStatus);
	}

EXPORT_C TStreamTransfer RShareBuf::DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
//
// Pull up to aTransfer bytes from anOutput.
//
	{
	return iWMark.WriteL(Host(),anOutput,aTransfer);
	}

EXPORT_C TStreamPos RShareBuf::DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
//
// Position the mark(s) indicated by aMark at anOffset from aLocation.
//
	{
	TStreamExchange& host=Host();
	if (!aMark)
		{
		TStreamMark mark;
		return mark.SeekL(host,aLocation,anOffset);
		}
//
	__ASSERT_ALWAYS(!(aMark&~(ERead|EWrite)),Panic(EStreamMarkInvalid));
	TStreamPos pos(0);
	if (aMark&ERead)
		{
		__ASSERT_ALWAYS(aLocation!=EStreamMark||aMark==ERead,Panic(EStreamMarkInvalid));
		if (!iRMark.RelatesTo(host))
			__LEAVE(KErrNotReady);
//
		pos=iRMark.SeekL(host,aLocation,anOffset);
		}
	if (aMark&EWrite)
		{
		if (!iWMark.RelatesTo(host))
			__LEAVE(KErrNotReady);
//
		TStreamPos p=iWMark.SeekL(host,aLocation,anOffset);
		__ASSERT_DEBUG(aMark==EWrite||p==pos,User::Invariant());
		return p;
		}
//
	return pos;
	}

EXPORT_C RShareReadStream::RShareReadStream(TStreamExchange& aHost,TStreamPos aPos)
/** Constructs the shared read stream object and prepares the shared stream for 
reading.

@param aHost The object that manages shared streaming.
@param aPos The position of the stream within the host stream. Defaults to 
the beginning of the host stream, if not explicitly specified.
@see KStreamBeginning */
	{
	Open(aHost,aPos);
	}

EXPORT_C void RShareReadStream::Open(TStreamExchange& aHost,TStreamPos aPos)
/** Prepares the shared stream for reading.

@param aHost The object that manages shared streaming.
@param aPos The position of the stream within the host stream. Defaults to 
the beginning of the host stream, if not explicitly specified. */
	{
	iSource.Open(aHost,aPos,iSource.ERead);
	RReadStream::Attach(&iSource);
	}

EXPORT_C RShareWriteStream::RShareWriteStream(TStreamExchange& aHost,TStreamPos aPos)
/** Constructs the shared write stream object and prepares the shared stream for 
writing.

@param aHost The object that manages shared streaming.
@param aPos The position of the stream within the host stream. Defaults to 
the beginning of the host stream, if not explicitly specified.
@see KStreamBeginning */
	{
	Open(aHost,aPos);
	}

EXPORT_C void RShareWriteStream::Open(TStreamExchange& aHost,TStreamPos aPos)
/** Prepares the shared stream for writing.

@param aHost The object that manages shared streaming.
@param aPos The position of the stream within the host stream. Defaults to 
the beginning of the host stream, if not explicitly specified. */
	{
	iSink.Open(aHost,aPos,iSink.EWrite);
	RWriteStream::Attach(&iSink);
	}

