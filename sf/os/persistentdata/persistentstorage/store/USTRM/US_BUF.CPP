// Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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

/** Synchronises the stream buffer with the stream, returning any error.

In effect, this ensures that buffered data is delivered to the stream.

This function calls SynchL() inside a TRAPD harness and returns the leave 
code if a leave occurs.

@return KErrNone, if successful; otherwise one of the other system wide error 
codes.
@see MStreamBuf::SynchL()
@see MStreamBuf::DoSynchL() */
EXPORT_C TInt MStreamBuf::Synch()
	{
	TRAPD(r,SynchL());
	return r;
	}

/** Closes the stream buffer.

This function attempts to synchronise buffered data with the stream before 
freeing any resources. All errors are ignored.

@see MStreamBuf::Synch()
@see MStreamBuf::Release() */
EXPORT_C void MStreamBuf::Close()
	{
	Synch();
	Release();
	}

/** Puts a cleanup item for this object onto the cleanup stack.

This allows allocated resources to be cleaned up if a subsequent leave occurs. */
EXPORT_C void MStreamBuf::PushL()
	{
	CleanupReleasePushL(*this);
	}

/** Reads data, asynchronously, from the stream buffer into the specified descriptor; 
request completion is guaranteed, even if request initiation fails.

The function calls the virtual function DoReadL(TDes8&,TInt,TRequestStatus&) 
to implement this behaviour. The maximum number of bytes to be read is the 
value of the maximum length of the descriptor.

@param aDes The target descriptor for the data read from the stream buffer.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be read, as used in this request. This 
value can be different to the maximum length of the descriptor; this is dependent 
on the implementation.
@see MStreamBuf::DoReadL() */
EXPORT_C TInt MStreamBuf::Read(TDes8& aDes,TRequestStatus& aStatus)
	{
	return Read(aDes,aDes.MaxLength(),aStatus);
	}

/** Reads data, asynchronously, from the stream buffer into the specified descriptor; 
request completion is guaranteed, even if request initiation fails.

The function calls the virtual function DoReadL(TDes8&,TInt,TRequestStatus&) 
to implement this behaviour.

@param aDes The target descriptor for the data read from the stream buffer.
@param aMaxLength The maximum number of bytes to be read.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be read, as used in this request. This 
can be different to the value supplied in aMaxLength; this is dependent on 
the implementation.
@see MStreamBuf::DoReadL() */
EXPORT_C TInt MStreamBuf::Read(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
	{
	TInt len=0;
	TRAPD(r,len=DoReadL(aDes,aMaxLength,aStatus));
	if (r!=KErrNone)
		{
		TRequestStatus* stat=&aStatus;
		User::RequestComplete(stat,r);
		}
	return len;
	}

/** Reads data, asynchronously, from the stream buffer into the specified descriptor.

The function calls the virtual function DoReadL(TDes8&,TInt,TRequestStatus&) 
to implement this behaviour. The maximum number of bytes to be read is the 
maximum length of the descriptor.

If the function leaves, then no read request will have been initiated.

@param aDes The target descriptor for the data read from the stream buffer.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be read, as used in this request. This 
value can be different to the maximum length of the descriptor; this is dependent 
on the implementation.
@see MStreamBuf::DoReadL() */
EXPORT_C TInt MStreamBuf::ReadL(TDes8& aDes,TRequestStatus& aStatus)
	{
	return DoReadL(aDes,aDes.MaxLength(),aStatus);
	}

/** Reads data from the stream buffer into the specified data sink.

The function uses the virtual function DoReadL(MStreamInput&,TStreamTransfer) 
to implement this behaviour.

@param anInput The data sink which is the target for the read operation.
@param aMaxLength The maximum amount of data available to be read.
@return The amount of data that was not consumed. */
EXPORT_C TInt MStreamBuf::ReadL(MStreamInput& anInput,TInt aMaxLength)
	{
	return aMaxLength-DoReadL(anInput,TStreamTransfer(aMaxLength)).Left();
	}

/** Writes data, asynchronously, from the specified descriptor into the stream buffer; 
request completion is guaranteed, even if request initiation fails.

The function calls the virtual function DoWriteL(const TDesC8&,TInt,TRequestStatus&) 
to implement this behaviour. The maximum number of bytes to be written is 
the value of the maximum length of the descriptor.

@param aDes The source descriptor for the data to be written into the stream 
buffer.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be written, as used in this request. 
This can be different to the value supplied in aMaxLength; this is dependent 
on the implementation.
@see MStreamBuf::DoWriteL() */
EXPORT_C TInt MStreamBuf::Write(const TDesC8& aDes,TRequestStatus& aStatus)
	{
	return Write(aDes,aDes.Length(),aStatus);
	}

//
// Write up to aMaxLength bytes with guaranteed completion.
//
EXPORT_C TInt MStreamBuf::Write(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
	{
	TInt len=0;
	TRAPD(r,len=DoWriteL(aDes,aMaxLength,aStatus));
	if (r!=KErrNone)
		{
		TRequestStatus* stat=&aStatus;
		User::RequestComplete(stat,r);
		}
	return len;
	}

/** Writes data, asynchronously, from the specified descriptor into the stream buffer.

The function calls the virtual function DoWriteL(const TDesC8&,TInt,TRequestStatus&) 
to implement this behaviour. The maximum number of bytes to be written is 
the value of the maximum length of the descriptor.

If the function leaves, then no write request will have been initiated.

@param aDes The source descriptor for the data to be written into the stream 
buffer.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be written, as used in this request. 
This can be different to the maximum length of the descriptor; this is dependent 
on the implementation.
@see MStreamBuf::DoWriteL() */
EXPORT_C TInt MStreamBuf::WriteL(const TDesC8& aDes,TRequestStatus& aStatus)
	{
	return DoWriteL(aDes,aDes.Length(),aStatus);
	}

/** Writes data into the stream buffer from the specified data source.

The function calls the virtual function DoWriteL(MStreamOutput&,TStreamTransfer) 
to implement this behaviour.

@param anOutput The data source for the write operation.
@param aMaxLength The maximum amount of data available to be written.
@return The amount of data that was not consumed. */
EXPORT_C TInt MStreamBuf::WriteL(MStreamOutput& anOutput,TInt aMaxLength)
	{
	return aMaxLength-DoWriteL(anOutput,TStreamTransfer(aMaxLength)).Left();
	}

/** Frees resources before abandoning the stream buffer.

It is called by Release().

This implementation is empty, but classes derived from MStreamBuf can provide 
their own implementation, if necessary.

@see MStreamBuf::Release() */
EXPORT_C void MStreamBuf::DoRelease()
	{	
	}

/** Synchronises the stream buffer with the stream, leaving if any error occurs.

In effect, this ensures that buffered data is delivered to the stream.

It is called by SynchL().

This implementation is empty, but classes derived from MStreamBuf can provide 
their own implementation, if necessary.

@see MStreamBuf::SynchL() */
EXPORT_C void MStreamBuf::DoSynchL()
	{
	}

#pragma BullseyeCoverage off

//
// Cannot read from this stream buffer.
//
EXPORT_C TInt MStreamBuf::DoReadL(TAny*,TInt)
	{
	Panic(EStreamCannotRead);
	return TInt();
	}

//
// Cannot write to this stream buffer.
//
EXPORT_C void MStreamBuf::DoWriteL(const TAny*,TInt)
	{
	Panic(EStreamCannotWrite);
	}

//
// This stream buffer does not support seeking.
//
EXPORT_C TStreamPos MStreamBuf::DoSeekL(TMark,TStreamLocation,TInt)
	{
	Panic(EStreamCannotSeek);
	TStreamPos streamPos(-1);
	return streamPos;
	}

#pragma BullseyeCoverage on

/** Reads data from the stream buffer into the specified descriptor.

This function is called by ReadL(TDes8&,TInt,TRequestStatus&).

This implementation deals with the request synchronously, and completes the 
request with KErrNone. Other implementations may choose to deal with this 
in a true asynchronous manner.

In addition, the read operation itself uses the DoReadL(TAny*,TInt) variant.

@param aDes The target descriptor for the data read from the stream buffer. 
On return, the length of the descriptor is set to the number of bytes read 
from the stream buffer.
@param aMaxLength The maximum number of bytes to be read. This value must not 
be greater than the maximum length of the descriptor, otherwise the function 
raises a STORE-Stream 2 panic.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be read, as used in this request. This 
implementation uses, and returns, the value supplied in aMaxLength. Other 
implementations may choose to use a different value.
@see MStreamBuf::ReadL() */
EXPORT_C TInt MStreamBuf::DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(aMaxLength<=aDes.MaxLength(),Panic(EStreamReadBeyondEnd));
	aDes.SetLength(DoReadL((TUint8*)aDes.Ptr(),aMaxLength));
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	return aMaxLength;
	}

/** Reads data from the stream into the specified data sink.

It is called by ReadL(MStreamInput&,TStreamTransfer).

This implementation calls the sink's ReadFromL() function, which performs 
the read (transfer) operation.

This implementation of DoReadL() is called for streams that do not have buffering 
capabilities, and that are derived directly from this class.

@param anInput The target data sink.
@param aTransfer A stream transfer object defining the amount of data available 
to be read.
@return A stream transfer object defining the amount of data that was not consumed.
@see MStreamInput::ReadFromL() */
EXPORT_C TStreamTransfer MStreamBuf::DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer)
	{
	return anInput.ReadFromL(*this,aTransfer);
	}

/** Writes data from the specified descriptor into this stream buffer.

This function is called by WriteL(const TDesC8&,TInt,TRequestStatus&).

This implementation deals with the request synchronously, and completes the 
request with KErrNone. Other implementations may choose to deal with this 
in a true asynchronous manner.

In addition, the write operation itself uses the DoWriteL(TAny*,TInt) variant.

@param aDes The source descriptor for the data to be written into the stream 
buffer.
@param aMaxLength The number of bytes to be written. This value must not be 
greater than the maximum length of the descriptor, otherwise the function 
raises a STORE-Stream 6 panic.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be written, as used in this request. 
This implementation uses, and returns, the value supplied in aMaxLength. Other 
implementations may choose to use a different value.
@see MStreamBuf::WriteL() */
EXPORT_C TInt MStreamBuf::DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
	{
	__ASSERT_DEBUG(aMaxLength<=aDes.Length(),Panic(EStreamWriteBeyondEnd));
	DoWriteL(aDes.Ptr(),aMaxLength);
	TRequestStatus* stat=&aStatus;
	User::RequestComplete(stat,KErrNone);
	return aMaxLength;
	}

//
// Default implementation turning around to anOutput.
//
EXPORT_C TStreamTransfer MStreamBuf::DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
	{
	return anOutput.WriteToL(*this,aTransfer);
	}

/** Sets the pointers that mark out the read and write areas within the intermediate 
buffer to null. */
EXPORT_C TStreamBuf::TStreamBuf()
	: iRPtr(NULL),iREnd(NULL),iWPtr(NULL),iWEnd(NULL)
	{
	}

/** Sets the start and end points of the read and/or the write area within the intermediate 
buffer.

A start point is always within an area; an end point is always the first byte 
beyond the end of an area.

@param anArea The areas within the intermediate buffer for which the start 
and end points are to be set. These can be the read area and/or the write 
area, as indicated by the ERead and EWrite bits. Only these bits can be set, 
otherwise the function raises a STORE-Stream 17 panic.
@param aPtr The start point.
@param anEnd The end point.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
EXPORT_C void TStreamBuf::SetBuf(TArea anArea,TUint8* aPtr,TUint8* anEnd)
	{
	__ASSERT_ALWAYS(!(anArea&~(ERead|EWrite)),Panic(EStreamAreaInvalid));
	if (anArea&ERead)
		SetBuf(ERead,aPtr,anEnd);
	if (anArea&EWrite)
		SetBuf(EWrite,aPtr,anEnd);
	}

/** Sets the start point of the read and/or the write area within the intermediate 
buffer.

A start point is always within an area.

@param anArea The areas within the intermediate buffer for which the start 
point is to be set. These can be the read area and/or the write area, as indicated 
by the ERead and EWrite bits. Only these bits can be set, otherwise the function 
raises a STORE-Stream 17 panic.
@param aPtr The start point.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
EXPORT_C void TStreamBuf::SetPtr(TArea anArea,TUint8* aPtr)
	{
	__ASSERT_ALWAYS(!(anArea&~(ERead|EWrite)),Panic(EStreamAreaInvalid));
	if (anArea&ERead)
		SetPtr(ERead,aPtr);
	if (anArea&EWrite)
		SetPtr(EWrite,aPtr);
	}

//
// Set the end pointer for the buffer area(s) indicated by anArea.
//
EXPORT_C void TStreamBuf::SetEnd(TArea anArea,TUint8* anEnd)
	{
	__ASSERT_ALWAYS(!(anArea&~(ERead|EWrite)),Panic(EStreamAreaInvalid));
	if (anArea&ERead)
		SetEnd(ERead,anEnd);
	if (anArea&EWrite)
		SetEnd(EWrite,anEnd);
	}

/** Gets the current start point of the read or write area within the intermediate 
buffer.

@param anArea The area within the intermediate buffer for which the start 
point is to be fetched. This can be either the read area or the write area, 
as indicated by the ERead and EWrite bits. Only one of these can be set, otherwise 
the function raises a STORE-Stream 17 panic.
@return The start point.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
EXPORT_C TUint8* TStreamBuf::Ptr(TArea anArea) const
	{
	if (anArea==ERead)
		return Ptr(ERead);
//
	__ASSERT_ALWAYS(anArea==EWrite,Panic(EStreamAreaInvalid));
	return Ptr(EWrite);
	}

/** Gets the current end point of the read or write area within the intermediate 
buffer.

An end point is always the first byte beyond the end of an area.

@param anArea The area within the intermediate buffer for which the end point 
is to be fetched. This can be either the read area or the write area, as indicated 
by the ERead and EWrite bits. Only one of these can be set, otherwise the 
function raises a STORE-Stream 17 panic.
@return The end point. */
EXPORT_C TUint8* TStreamBuf::End(TArea anArea) const
	{
	if (anArea==ERead)
		return End(ERead);
//
	__ASSERT_ALWAYS(anArea==EWrite,Panic(EStreamAreaInvalid));
	return End(EWrite);
	}

/** Gets the number of bytes available in the read or write area within the intermediate 
buffer.

@param anArea The area within the intermediate buffer for which the number 
of available bytes is to be fetched. This can be either the read area or the 
write area, as indicated by the ERead and EWrite bits. Only one of these can 
be set, otherwise the function raises a STORE-Stream 17 panic.
@return The number of bytes available. */
EXPORT_C TInt TStreamBuf::Avail(TArea anArea) const
	{
	if (anArea==ERead)
		return Avail(ERead);
//
	__ASSERT_ALWAYS(anArea==EWrite,Panic(EStreamAreaInvalid));
	return Avail(EWrite);
	}

/** Reads data from the intermediate buffer into the specified memory location.

The function calls the virtual function UnderfLowL() to give concrete implementations 
the chance to refill the intermediate buffer, and satisfy the caller's requirements.

This implementation overrides the one supplied by the base class MStreamBuf, 
and is called by, MStreamBuf::ReadL(TAny*,TInt).

@param aPtr A pointer to the target memory location for the data read from 
the intermediate buffer.
@param aMaxLength The maximum number of bytes to be read.
@return The number of bytes read. This may be less than the amount requested.
@see MStreamBuf::ReadL()
@see MStreamBuf::DoReadL() */
EXPORT_C TInt TStreamBuf::DoReadL(TAny* aPtr,TInt aMaxLength)
	{
	__ASSERT_DEBUG(aMaxLength>=0,Panic(EStreamReadLengthNegative));
	__ASSERT_DEBUG(aMaxLength>0,Panic(EStreamReadNoTransfer));
	__ASSERT_DEBUG(Ptr(ERead)!=NULL||End(ERead)==NULL,Panic(EStreamCannotRead));
	TInt left=aMaxLength;
	TInt avail=Avail(ERead);
	__ASSERT_DEBUG(avail>=0,User::Invariant());
	if (avail==0)
		goto underflow;
//
	do
		{
		__ASSERT_DEBUG(avail==Avail(ERead),Panic(EStreamUnderflowInBreach));
		__ASSERT_DEBUG(left>0&&avail>0,User::Invariant());
		{
		TInt len=Min(left,avail);
		TUint8* ptr=Ptr(ERead);
		aPtr=Mem::Copy(aPtr,ptr,len);
		SetPtr(ERead,ptr+len);
		left-=len;
		if (left==0)
			return aMaxLength; // that's it
		}
//
	underflow:
		avail=UnderflowL(left);
		} while (avail>0);
	__ASSERT_DEBUG(avail==0&&Avail(ERead)==0,Panic(EStreamUnderflowInBreach));
	return aMaxLength-left;
	}

/** Reads data from the intermediate buffer and, if necessary, any remaining data 
from the stream to the specified target stream input object.

It is called by ReadL(MStreamInput&,TStreamTransfer).

The intermediate buffer is emptied first by calling the target stream input's 
PushL() function, which performs the read from intermediate buffer operation. 
Any remaining data is then read from the stream by calling the target stream 
object's ReadFromL() function, which performs the read from stream operation.

This implementation is called for streams that have buffering capabilities 
and are derived from this class.

@param anInput The target stream input object. 
@param aTransfer A stream transfer object defining the amount of data available 
to be written.
@return The amount of data that was not consumed.
@see MStreamInput::ReadFromL()
@see MStreamInput::PushL() */
EXPORT_C TStreamTransfer TStreamBuf::DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer)
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamReadNoTransfer));
	__ASSERT_DEBUG(Ptr(ERead)!=NULL||End(ERead)==NULL,Panic(EStreamCannotRead));
	__ASSERT_DEBUG(Avail(ERead)>=0,User::Invariant());
	TInt len=aTransfer[Avail(ERead)];
	if (len>0)
		{
		__DEBUG(TInt avail=Avail(ERead)); // may be pushing into this streambuf
		TUint8* ptr=Ptr(ERead);
		len=anInput.PushL(ptr,len);
		__ASSERT_DEBUG(len>=0&&len<=aTransfer[avail]&&Ptr(ERead)==ptr&&Avail(ERead)>=avail,Panic(EStreamPushInBreach));
		SetPtr(ERead,ptr+len);
		aTransfer-=len;
		}
	if (aTransfer>0)
		aTransfer=anInput.ReadFromL(*this,aTransfer);
	return aTransfer;
	}

/** Writes data from the specified memory location into the intermediate buffer.

The function calls the virtual function OverfLowL() to give concrete implementations 
the chance to forward the intermediate buffer content to its destination.

This implementation overrides the one supplied by the base class MStreamBuf, 
and is called by MStreamBuf::WriteL(const TAny*,TInt).

@param aPtr A pointer to the source memory location for the data to be written 
to the intermediate buffer.
@param aLength The number of bytes to be written.
@return The number of bytes written.
@see MStreamBuf::WriteL()
@see MStreamBuf::DoWriteL() */
EXPORT_C void TStreamBuf::DoWriteL(const TAny* aPtr,TInt aLength)
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	__ASSERT_DEBUG(aLength>0,Panic(EStreamWriteNoTransfer));
	__ASSERT_DEBUG(Ptr(EWrite)!=NULL||End(EWrite)==NULL,Panic(EStreamCannotWrite));
	TInt avail=Avail(EWrite);
	__ASSERT_DEBUG(avail>=0,User::Invariant());
	if (avail==0)
		goto overflow;
//
	for(;;)
		{
		__ASSERT_DEBUG(avail>0,Panic(EStreamOverflowInBreach));
		__ASSERT_DEBUG(aLength>0,User::Invariant());
		{
		TInt len=Min(aLength,avail);
		SetPtr(EWrite,Mem::Copy(Ptr(EWrite),aPtr,len));
		aLength-=len;
		if (aLength==0)
			return; // done
//
		aPtr=(TUint8*)aPtr+len;
		}
//
	overflow:
		OverflowL();
		avail=Avail(EWrite);
		};
	}

//
// Default implementation filling the buffer before turning around to anOutput.
//
EXPORT_C TStreamTransfer TStreamBuf::DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
	{
	__ASSERT_DEBUG(aTransfer>0,Panic(EStreamWriteNoTransfer));
	__ASSERT_DEBUG(Ptr(EWrite)!=NULL||End(EWrite)==NULL,Panic(EStreamCannotWrite));
	__ASSERT_DEBUG(Avail(EWrite)>=0,User::Invariant());
	TInt len=aTransfer[Avail(EWrite)];
	if (len>0)
		{
		__DEBUG(TInt avail=Avail(EWrite)); // may be pulling from this streambuf
		TUint8* ptr=Ptr(EWrite);
		len=anOutput.PullL(ptr,len);
		__ASSERT_DEBUG(len>=0&&len<=aTransfer[avail]&&Ptr(EWrite)==ptr&&Avail(EWrite)>=avail,Panic(EStreamPullInBreach));
		SetPtr(EWrite,ptr+len);
		aTransfer-=len;
		}
	if (aTransfer>0)
		aTransfer=anOutput.WriteToL(*this,aTransfer);
	return aTransfer;
	}

