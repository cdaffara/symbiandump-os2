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

// Class TStreamPos
inline TStreamPos::TStreamPos(TInt anOffset)
	: iOff(anOffset)
/** Constructs the stream position object from the specified value.

@param anOffset The position value. */
	{}
inline TBool TStreamPos::operator==(TStreamPos aPos) const
/** Tests whether the stream position is equal to the specified stream position.

@param aPos The stream position to be compared.
@return True, if this object's stream position value is equal to the specified 
stream position's value; false, otherwise. */
	{return iOff==aPos.iOff;}
inline TBool TStreamPos::operator!=(TStreamPos aPos) const
/** Tests whether the stream position is not equal to the specified stream position.

@param aPos The stream position to be compared.
@return True, if this object's stream position value is not equal to the specified 
stream position's value; false, otherwise. */
	{return iOff!=aPos.iOff;}
inline TBool TStreamPos::operator<(TStreamPos aPos) const
/** Tests whether the stream position is less than the specified stream position.

@param aPos The stream position to be compared.
@return True, if this object's stream position value is less than the specified 
stream position's value; false, otherwise. */
	{return iOff<aPos.iOff;}
inline TBool TStreamPos::operator<=(TStreamPos aPos) const
/** Tests whether the stream position is less than or equal to the specified stream 
position.

@param aPos The stream position to be compared.
@return True, if this object's stream position value is less than or equal 
to the specified stream position's value; false, otherwise. */
	{return iOff<=aPos.iOff;}
inline TBool TStreamPos::operator>(TStreamPos aPos) const
/** Tests whether the stream position is greater than the specified stream position.

@param aPos The stream position to be compared.
@return True, if this object's stream position value is greater than the specified 
stream position's value; false, otherwise. */
	{return iOff>aPos.iOff;}
inline TBool TStreamPos::operator>=(TStreamPos aPos) const
/** Tests whether the stream position is greater than or equal to the specified 
stream position.

@param aPos The stream position to be compared.
@return True, if this object's stream position value is greater than or equal 
to the specified stream position's value; false, otherwise. */
	{return iOff>=aPos.iOff;}
inline TInt TStreamPos::operator-(TStreamPos aPos) const
/** Gets the result of subtracting the specified stream position value from this 
object's stream position value.

@param aPos The stream position whose value is to be subtracted.
@return The result of the calculation. */
	{return iOff-aPos.iOff;}
inline TStreamPos TStreamPos::operator+(TInt anOffset) const
/** Gets a stream position object that holds the result of adding the specified 
value to this object's stream position value.

@param anOffset The value to be added.
@return The stream position object holding the result of the calculation. */
	{return TStreamPos(iOff+anOffset);}
inline TStreamPos TStreamPos::operator-(TInt anOffset) const
/** Gets a stream position object that holds the result of subtracting the specified 
value from this object's stream position value.

@param anOffset The value to be subtracted.
@return The stream position object holding the result of the calculation. */
	{return TStreamPos(iOff-anOffset);}
inline TStreamPos& TStreamPos::operator+=(TInt anOffset)
/** Adds the specified value to this stream position object.

@param anOffset The value to be added.
@return A reference to this stream position object. */
	{iOff+=anOffset;return *this;}
inline TStreamPos& TStreamPos::operator-=(TInt anOffset)
/** Subtracts the specified value from this stream position object.

@param anOffset The value to be subtracted.
@return A reference to this stream position object. */
	{iOff-=anOffset;return *this;}
inline TInt TStreamPos::Offset() const
/** Gets the stream position value.

@return The stream position value. */
	{return iOff;}
inline TStreamPos operator+(TInt anOffset,TStreamPos aPos)
	{return aPos+anOffset;}

// Class TStreamTransfer
inline TStreamTransfer::TStreamTransfer(TInt aMaxLength)
	: iVal(aMaxLength)
/** Constructs a stream transfer object specifying a length value.

This value represents the maximum amount of data that can be transferred between 
streams.

@param aMaxLength The maximum length of data that can be transferred. In debug 
mode, the function raises a STORE-Stream 13 panic, if this value is negative. */
	{
#if defined (_DEBUG)
	__DbgChkNonNegative(aMaxLength);
#endif
	}
inline TStreamTransfer::TStreamTransfer(TUnlimited)
	: iVal(-1)
/** Constructs a stream transfer object specifying that there is no explicit limit 
to the amount of data that can be transferred between streams.

The amount of data to be transferred is only limited by the streams themselves.

The arithmetical operators do not change the state of an unlimited stream 
transfer object.

@param (The enumerator value is not used). */
	{}
inline TBool TStreamTransfer::operator==(TInt aLength) const
/** Tests whether the stream transfer value is equal to the specified value.

@param aLength The length to compared. In debug mode, the function raises 
a STORE-Stream 13 panic, if this value is negative.
@return True, if the stream transfer value is equal to the specified value; 
false, otherwise. */
	{
#if defined (_DEBUG)
	__DbgChkNonNegative(aLength);
#endif
	return iVal==aLength;
	}
inline TBool TStreamTransfer::operator>(TInt aLength) const
/** Tests whether the stream transfer value is greater than the specified value.

@param aLength The length to compared. In debug mode, the function raises 
a STORE-Stream 13 panic, if this value is negative.
@return True, if the stream transfer value is greater than the specified value; 
false, otherwise. */
	{
#if defined (_DEBUG)
	__DbgChkNonNegative(aLength);
#endif
	return TUint(iVal)>TUint(aLength);
	}
inline TStreamTransfer TStreamTransfer::operator-(TInt aLength) const
/** Subtracts the specified value from the stream transfer value.

If this stream transfer object was originally constructed as an unlimited 
type, i.e. using the TStreamTransfer(TUnlimited) constructor, then this operator 
does not change the state of the object, and it remains an unlimited type.

@param aLength The length to be subtracted. In debug mode, the function raises 
a STORE-Stream 13 panic, if this value is negative.
@return A stream transfer object containing the result of the subtraction. */
	{
#if defined (_DEBUG)
	__DbgChkNonNegative(aLength);
#endif
	return iVal<0?*this:TStreamTransfer(iVal-aLength);
	}
inline TInt TStreamTransfer::operator[](TInt aMaxLength) const
	{return *this>aMaxLength?aMaxLength:iVal;}
inline TStreamTransfer& TStreamTransfer::operator-=(TInt aLength)
/** Subtracts the specified value from the stream transfer value, updating this 
stream transfer object.

If this stream transfer object was originally constructed as an unlimited 
type, i.e. using the TStreamTransfer(TUnlimited) constructor, then this operator 
does not change the state of the object, and it remains an unlimited type.

If this stream transfer object was not an unlimited type, then, in debug mode, 
the function raises a STORE-Stream 13 panic, if the result of the calculation 
is negative.

@param aLength The length to be subtracted. In debug mode, the function raises 
a STORE-Stream 13 panic, if this value is negative.
@return A reference to this stream transfer object. */
	{
#if defined (_DEBUG)
	__DbgChkNonNegative(aLength);
#endif
	if (iVal>=0)
		{
		iVal-=aLength;
#if defined (_DEBUG)
		__DbgChkNonNegative(iVal);
#endif
		}
	return *this;
	}
inline TInt TStreamTransfer::Left() const
/** Gets the stream transfer value.

@return The current stream transfer value. */
	{
#if defined (_DEBUG)
	__DbgChkNonNegative(iVal);
#endif
	return iVal;
	}
inline TBool operator==(TInt aLength,TStreamTransfer aTransfer)
	{return aTransfer==aLength;}
inline TBool operator<(TInt aLength,TStreamTransfer aTransfer)
	{return aTransfer>aLength;}

// Class MStreamBuf
inline void MStreamBuf::Release()
/** Frees resources before abandoning the stream buffer.

The function calls the virtual function DoRelease() to implement this behaviour.

Release() is called by both RReadStream::Release() and RWriteStream::Release().

@see MStreamBuf::DoRelease()
@see RReadStream::Release()
@see RWriteStream::Release() */
	{DoRelease();}
inline void MStreamBuf::SynchL()
/** Synchronises the stream buffer with the stream, leaving if any error occurs.

In effect, this ensures that buffered data is delivered to the stream.

The function calls the virtual function DoSynchL() to implement this behaviour.

@see MStreamBuf::DoSynchL() */
	{DoSynchL();}
inline TInt MStreamBuf::ReadL(TAny* aPtr,TInt aMaxLength)
/** Reads data from the stream buffer into the specified memory location.

The function calls the virtual function DoReadL(TAny*,TInt) to implement this 
behaviour.

@param aPtr A pointer to the target memory location for the data read from 
the stream buffer.
@param aMaxLength The maximum number of bytes to be read.
@return The number of bytes read.
@see MStreamBuf::DoReadL() */
	{return DoReadL(aPtr,aMaxLength);}
inline TInt MStreamBuf::ReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
/** Reads data, asynchronously, from the stream buffer into the specified descriptor.

The function calls the virtual function DoReadL(TDes8&,TInt,TRequestStatus&) 
to implement this behaviour.

If the function leaves, then no read request will have been initiated.

@param aDes The target descriptor for the data read from the stream buffer.
@param aMaxLength The maximum number of bytes to be read.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be read, as used in this request. This 
can be different to the value supplied in aMaxLength; this is dependent on 
the implementation.
@see MStreamBuf::DoReadL() */
	{return DoReadL(aDes,aMaxLength,aStatus);}
inline TStreamTransfer MStreamBuf::ReadL(MStreamInput& anInput,TStreamTransfer aTransfer)
/** Reads data from the stream buffer into the specified data sink.

The function calls the virtual function DoReadL(MStreamInput&,TStreamTransfer) 
to implement this behaviour.

@param anInput The data sink that is the target for the read operation.
@param aTransfer Defines the amount of data available to be read.
@return The amount of data that was not consumed. */
	{return DoReadL(anInput,aTransfer);}
inline void MStreamBuf::ReadL(MStreamInput& anInput)
/** Reads data from the stream buffer into the specified data sink.

The function uses the virtual function DoReadL(MStreamInput&,TStreamTransfer) 
to implement this behaviour.

No explicit limit is placed on the amount of data that can be read.

@param anInput The data sink that is the target for the read operation. */
	{DoReadL(anInput,KStreamUnlimited);}
inline void MStreamBuf::WriteL(const TAny* aPtr,TInt aLength)
/** Writes data from the specified memory location into the stream buffer.

The function calls the virtual function DoWriteL(TAny*,TInt) to implement 
this behaviour.

@param aPtr A pointer to the memory location from which data is to be written 
to the stream buffer.
@param aLength The number of bytes to be written.
@see MStreamBuf::DoWriteL() */
	{DoWriteL(aPtr,aLength);}
inline TInt MStreamBuf::WriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
/** Writes data, asynchronously, from the specified descriptor into the stream buffer.

The function calls the virtual function DoWriteL(const TDesC8&,TInt,TRequestStatus&) 
to implement this behaviour.

If the function leaves, then no write request will have been initiated.

@param aDes The source descriptor for the data to be written into the stream 
buffer.
@param aMaxLength The maximum number of bytes to be written.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be written, as used in this request. 
This can be different to the value supplied in aMaxLength; this is dependent 
on the implementation.
@see MStreamBuf::DoWriteL() */
	{return DoWriteL(aDes,aMaxLength,aStatus);}
inline TStreamTransfer MStreamBuf::WriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer)
/** Writes data into the stream buffer from the specified data source.

The function calls the virtual function DoWriteL(MStreamOutput&,TStreamTransfer) 
to implement this behaviour.

@param anOutput The data source for the write operation.
@param aTransfer Defines the amount of data to be pulled from the output stream 
object.
@return A stream transfer object defining the amount of data that was not consumed. */
	{return DoWriteL(anOutput,aTransfer);}
inline void MStreamBuf::WriteL(MStreamOutput& anOutput)
/**Writes data into the stream buffer from the specified data source.

The function calls the virtual function DoWriteL(MStreamOutput&,TStreamTransfer) to implement this behaviour.

No explicit limit is placed on the amount of data that can be written.

@param anOutput The data source for the write operation.
@param aMaxLength The maximum amount of data available to be written.
*/
	{DoWriteL(anOutput,KStreamUnlimited);}
inline void MStreamBuf::SeekL(TMark aMark,TStreamPos aPos)
/**Moves the position of the read or write mark in the stream.

The new position is calculated by adding the specified value to the position of the beginning of the stream.

The function calls virtual function DoSeekL(TMark,TStreamLocation,TInt) to implement this behaviour.
Notes:
As there are two current positions, one for the read mark and one for the write mark, it is not valid, in general, to use a single call to SeekL() to move both the read and write marks. 
Not all streams are seekable. 

@param aMark The type of mark, i.e. read or write.
@param aLocation A stream position value on which the calculation of the new position is based.
*/
	{DoSeekL(aMark,EStreamBeginning,aPos.Offset());}
inline TStreamPos MStreamBuf::SeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset)
/** Moves the position of the read mark or the write mark in the stream.

The new position is calculated by adding the specified offset to one of:

the position of the beginning of the stream

the position of the end of the stream

the position of the current read mark or write mark.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

As there are two current positions, one for the read mark and one for the 
write mark, it is not valid, in general, to use a single call to SeekL() to 
move both the read and write marks.

Not all streams are seekable.

@param aMark The type of mark, i.e read or write.
@param aLocation The location in the stream on which the calculation of the 
new position is based.
@param anOffset The offset value.
@return The new stream position of the read or write mark. */
	{return DoSeekL(aMark,aLocation,anOffset);}
inline TStreamPos MStreamBuf::SeekL(TRead,TStreamLocation aLocation,TInt anOffset)
/** Moves the position of the read mark in the stream.

The new position is calculated by adding the specified offset to one of:

the position of the beginning of the stream

the position of the end of the stream

the position of the current read mark.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

Not all streams are seekable.

@param (The enumerator value is not used)
@param aLocation The location in the stream on which the calculation of the 
new position is based.
@param anOffset The offset value.
@return The new stream position of the read mark. */
	{return DoSeekL(ERead,aLocation,anOffset);}
inline TStreamPos MStreamBuf::SeekL(TWrite,TStreamLocation aLocation,TInt anOffset)
/** Moves the position of the write mark in the stream.

The new position is calculated by adding the specified offset to one of:

the position of the beginning of the stream

the position of the end of the stream

the position of the current write mark.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

Not all streams are seekable.

@param (The enumerator value is not used)
@param aLocation The location in the stream on which the calculation of the 
new position is based.
@param anOffset The offset value.
@return The new stream position of the write mark. */
	{return DoSeekL(EWrite,aLocation,anOffset);}
inline TStreamPos MStreamBuf::SeekL(TRead,TInt anOffset)
/** Moves the position of the read mark in the stream by the specified offset.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

Not all streams are seekable.

@param (The enumerator value is not used)
@param anOffset The amount by which the position of the read mark is to be 
moved relative to the existing position of the read mark.
@return The new stream position of the read mark. */
	{return DoSeekL(ERead,EStreamMark,anOffset);}
inline TStreamPos MStreamBuf::SeekL(TWrite,TInt anOffset)
/** Moves the position of the write mark in the stream by the specified offset.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

Not all streams are seekable.

@param (The enumerator value is not used)
@param anOffset The amount by which the position of the write mark is to be 
moved relative to the existing position of the write mark.
@return The new stream position of the write mark. */
	{return DoSeekL(EWrite,EStreamMark,anOffset);}
inline TStreamPos MStreamBuf::TellL(TRead) const
/** Gets the position of the read mark within the stream.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

@param (The enumerator value is not used).
@return The stream position. */
	{return CONST_CAST(MStreamBuf*,this)->DoSeekL(ERead,EStreamMark,0);}
inline TStreamPos MStreamBuf::TellL(TWrite) const
/** Gets the position of the write mark within the stream.

The function calls the virtual function DoSeekL(TMark,TStreamLocation,TInt) 
to implement this behaviour.

@param (The enumerator value is not used).
@return The stream position. */
	{return CONST_CAST(MStreamBuf*,this)->DoSeekL(EWrite,EStreamMark,0);}
inline TInt MStreamBuf::SizeL() const
/** Gets the size of the stream.

@return The size of the stream, in bytes. */
	{return CONST_CAST(MStreamBuf*,this)->DoSeekL(0,EStreamEnd,0).Offset();}

// Class TStreamBuf
inline void TStreamBuf::SetBuf(TRead,TUint8* aPtr,TUint8* anEnd)
/** Sets the start and end points of the read area within the intermediate buffer.

A start point is always within an area; an end point is always the first byte 
beyond the end of an area.

@param (The enumerator is not used).
@param aPtr The start point.
@param anEnd The end point.
@see MStreamBuf::TRead */
	{iRPtr=aPtr;iREnd=anEnd;}
inline void TStreamBuf::SetBuf(TWrite,TUint8* aPtr,TUint8* anEnd)
/** Sets the start and end points of the write area within the intermediate buffer.

A start point is always within an area; an end point is always the first byte 
beyond the end of an area.

@param (The enumerator is not used).
@param aPtr The start point.
@param anEnd The end point.
@see MStreamBuf::TWrite */
	{iWPtr=aPtr;iWEnd=anEnd;}
inline void TStreamBuf::SetPtr(TRead,TUint8* aPtr)
/** Sets the start point of the write area within the intermediate buffer.

A start point is always within an area.

@param (The enumerator is not used).
@param aPtr The start point.
@see MStreamBuf::TWrite */
	{iRPtr=aPtr;}
inline void TStreamBuf::SetPtr(TWrite,TUint8* aPtr)
/** Sets the start point of the write area within the intermediate buffer.

A start point is always within an area.

@param (The enumerator is not used).
@param aPtr The start point.
@see MStreamBuf::TWrite */
	{iWPtr=aPtr;}
inline void TStreamBuf::SetEnd(TRead,TUint8* anEnd)
	{iREnd=anEnd;}
inline void TStreamBuf::SetEnd(TWrite,TUint8* anEnd)
	{iWEnd=anEnd;}
inline TUint8* TStreamBuf::Ptr(TRead) const
/** Gets the current start point of the read area within the intermediate buffer.

@param (The enumerator is not used).
@return The start point.
@see MStreamBuf::TRead */
	{return iRPtr;}
inline TUint8* TStreamBuf::Ptr(TWrite) const
/** Gets the current start point of the write area within the intermediate buffer.

@param (The enumerator is not used).
@return The start point.
@see MStreamBuf::TWrite */
	{return iWPtr;}
inline TUint8* TStreamBuf::End(TRead) const
/** Gets the current end point of the read area within the intermediate buffer.

An end point is always the first byte beyond the end of an area.

@param (The enumerator is not used).
@return The end point.
@see MStreamBuf::TRead */
	{return iREnd;}
inline TUint8* TStreamBuf::End(TWrite) const
/** Gets the current end point of the write area within the intermediate buffer.

An end point is always the first byte beyond the end of an area.

@param (The enumerator is not used).
@return The end point.
@see MStreamBuf::TWrite */
	{return iWEnd;}
inline TInt TStreamBuf::Avail(TRead) const
/** Gets the number of bytes available in the read area within the intermediate 
buffer.

@param (The enumerator is not used).
@return The number of bytes available.
@see MStreamBuf::TRead */
	{return iREnd-iRPtr;}
inline TInt TStreamBuf::Avail(TWrite) const
/** Gets the number of bytes available in the write area within the intermediate 
buffer.

@param (The enumerator is not used).
@return The number of bytes available.
@see MStreamBuf::TWrite */
	{return iWEnd-iWPtr;}

// Class TStreamFilter
inline void TStreamFilter::Set(MStreamBuf* aHost,TInt aMode)
/** Sets up the filter to use the specified host for streamed data.

Taking ownership of the host stream buffer means that calls to SynchL() propagate 
to the host buffer after the filter has flushed its data, and that when the 
filter is released it also releases the host buffer. 

@param aHost The host for the streamed data - a stream buffer.
@param aMode The mode in which the stream buffer is to be used. It can be used 
in either read or write modes, represented by ERead and EWrite, but not both 
at the same time. In debug mode, setting both raises a STORE-Stream 18 panic. 
In addition, specify EAttached to indicate that the filter should take ownership 
of the host stream buffer.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
	{
#if defined (_DEBUG)
	__DbgChkMode(aMode);
#endif
	iHost=aHost;iMode=aMode;
	}
inline void TStreamFilter::Committed()
/** Flags the streamed data as committed. */
	{iMode&=~EWrite;}
inline TBool TStreamFilter::IsCommitted() const
/** Tests whether the streamed data is committed.

@return True, if streamed data is committed; false, otherwise. */
	{return iHost==NULL||!(iMode&EWrite);}

