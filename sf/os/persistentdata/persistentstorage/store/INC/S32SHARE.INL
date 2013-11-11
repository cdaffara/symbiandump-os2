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

// Class TStreamMark
inline TStreamMark::TStreamMark()
	: iPos(KStreamBeginning-1)
/** Constructs a default shared stream mark object.

The position for the mark is uninitialised.

An uninitialised mark means that a call to IsEmpty() returns true.

@see IsEmpty() */
	{}
inline TStreamMark::TStreamMark(TStreamPos aPos)
	: iPos(aPos)
/** Constructs the shared stream mark object, setting the mark to the specified 
stream position.

An initialised mark means that a call to IsEmpty() returns false.

@param aPos The stream position
@see IsEmpty() */
	{
#if defined (_DEBUG)
	__DbgChkPos(aPos);
#endif
	}
inline TStreamMark& TStreamMark::operator=(TStreamPos aPos)
/** Assigns the specified stream position value to this shared stream mark object.

@param aPos The stream position value to be assigned.
@return A reference to this shared stream mark object. */
	{
#if defined (_DEBUG)
	__DbgChkPos(aPos);
#endif
	iPos=aPos;
	return *this;
	}
inline TStreamMark::operator TStreamMark*()
	{return this;}
inline TStreamMark::operator const TStreamMark*() const
	{return this;}
inline TBool TStreamMark::operator==(const TStreamMark& aMark) const
/** Tests whether this object and the specified referenced shared stream mark object 
are the same object.

@param aMark A reference to a shared stream mark object.
@return True, if the two objects are the same object; false, otherwise. */
	{return this==&aMark;}
inline TBool TStreamMark::operator==(const TStreamMark* aPtr) const
	{return this==aPtr;}
inline TBool TStreamMark::operator!=(const TStreamMark& aMark) const
/** Tests whether this object and the specified shared stream mark object are different 
objects.

@param aMark A pointer to a shared stream mark object.
@return True, if the two objects are different objects; false, otherwise. */
	{return this!=&aMark;}
inline TBool TStreamMark::operator!=(const TStreamMark* aPtr) const
	{return this!=aPtr;}
inline TBool TStreamMark::IsEmpty() const
/** Tests whether this mark object is uninitialised.

@return True, if this mark object is uninitialised; false, otherwise. */
	{return iPos<KStreamBeginning;}
inline void TStreamMark::Clear()
/** Resets the mark object to its default state.

On return from this function, the mark is uninitialised and a call to IsEmpty() 
returns true.

Empties the object/makes it empty so that IsEmpty() returns false */
	{iPos=KStreamBeginning-1;}
inline TStreamPos TStreamMark::Position() const
/** Gets the position of the mark.

@return The stream position. */
	{
#if defined (_DEBUG)
	__DbgChkPos(iPos);
#endif
	return iPos;
	}
inline TBool TStreamMark::IsWith(TStreamExchange& aHost) const
/** Tests whether the specified shared streaming manager currently refers to this 
mark object.

@param aHost The object that manages shared streaming.
@return True, if the shared stream manager refers to this mark; false, otherwise. */
	{return aHost.RefersTo(*this);}
inline TBool TStreamMark::RelatesTo(TStreamExchange& aHost) const
/** Tests whether the specified shared streaming manager currently refers to this 
mark object OR whether this mark object is initialised.

@param aHost The object that manages shared streaming.
@return True, if the shared stream manager refers to this mark OR if this mark 
object is initialised; false, otherwise.
@see IsWith()
@see TStreamExchange::IsActive() */
	{return iPos>=KStreamBeginning||aHost.RefersTo(*this);}
inline void TStreamMark::Withdraw(TStreamExchange& aHost)
/** Instructs the shared streaming manager to remove any reference it has to this 
mark object.

@param aHost The object that manages shared streaming. */
	{aHost.Drop(*this);}
inline void TStreamMark::ExtractL(TStreamExchange& aHost)
/** Refreshes this mark from the mark in the host stream buffer and tells the shared 
streaming manager to drop any reference it has to to this mark object.

@param aHost The object that manages shared streaming. */
	{aHost.GetL(*this);}
inline TInt TStreamMark::ReadL(TStreamExchange& aHost,TAny* aPtr,TInt aMaxLength)
/** Reads data from the shared stream into the specified memory location.

@param aHost The object that manages shared streaming.
@param aPtr A pointer to the target memory location for the data read from 
the shared stream.
@param aMaxLength The maximum number of bytes to be read.
@return The number of bytes read. */
	{return aHost.DoReadL(aPtr,aMaxLength,*this);}
inline TInt TStreamMark::ReadL(TStreamExchange& aHost,TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
/** Reads data, asynchronously, from the shared stream into the specified descriptor.

If the function leaves, then no read request will have been initiated.

@param aHost The object that manages shared streaming.
@param aDes The target descriptor for the data read from the shared stream.
@param aMaxLength The maximum number of bytes to be read.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be read, as used in this request. This 
can be different to the value supplied in aMaxLength; this is dependent on 
the implementation of the underlying stream. */
	{return aHost.DoReadL(aDes,aMaxLength,aStatus,*this);}
inline TStreamTransfer TStreamMark::ReadL(TStreamExchange& aHost,MStreamInput& anInput,TStreamTransfer aTransfer)
/** Reads data from the shared stream into the specified data sink.

@param aHost The object that manages shared streaming.
@param anInput The sink which is the target for the read operation.
@param aTransfer Defines the amount of data available to be read from the shared 
stream.
@return The amount of data that was not consumed. */
	{return aHost.DoReadL(anInput,aTransfer,*this);}
inline TInt TStreamMark::ReadL(TStreamExchange& aHost,MStreamInput& anInput,TInt aMaxLength)
/** Reads data from the shared stream into the specified data sink.

@param aHost The object that manages shared streaming.
@param anInput The sink which is the target for the read operation.
@param aMaxLength The maximum amount of data available to be read from the 
shared stream.
@return The amount of data that was not consumed. */
	{return aMaxLength-aHost.DoReadL(anInput,TStreamTransfer(aMaxLength),*this).Left();}
inline void TStreamMark::ReadL(TStreamExchange& aHost,MStreamInput& anInput)
/** Reads data from the shared stream into the specified data sink.

No explicit limit is placed on the amount of data that can be read.

@param aHost The object that manages shared streaming.
@param anInput The sink which is the target for the read operation. */
	{aHost.DoReadL(anInput,KStreamUnlimited,*this);}
inline void TStreamMark::WriteL(TStreamExchange& aHost,const TAny* aPtr,TInt aLength)
/** Writes data from the specified memory location into the shared stream.

@param aHost The object that manages shared streaming.
@param aPtr A pointer to the memory location from which data is to be written 
to the shared stream.
@param aLength The number of bytes to be written. */
	{aHost.DoWriteL(aPtr,aLength,*this);}
inline TInt TStreamMark::WriteL(TStreamExchange& aHost,const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus)
/** Writes data, asynchronously, from the specified descriptor into the shared 
stream.

If the function leaves, then no write request will have been initiated.

@param aHost The object that manages shared streaming.
@param aDes The source descriptor for the data to be written into the shared 
stream.
@param aMaxLength The maximum number of bytes to be written.
@param aStatus The request status that indicates the completion status of this 
asynchronous request.
@return The maximum number of bytes to be written, as used in this request. 
This can be different to the value supplied in aMaxLength; this is dependent 
on the implementation. */
	{return aHost.DoWriteL(aDes,aMaxLength,aStatus,*this);}
inline TStreamTransfer TStreamMark::WriteL(TStreamExchange& aHost,MStreamOutput& anOutput,TStreamTransfer aTransfer)
/** Writes data into the shared stream from the specified data source.

@param aHost The object that manages shared streaming.
@param anOutput The data source for the write operation.
@param aTransfer Defines the amount of data to be pulled from the output stream 
object.
@return A stream transfer object defining the amount of data that was not consumed. */
	{return aHost.DoWriteL(anOutput,aTransfer,*this);}
inline TInt TStreamMark::WriteL(TStreamExchange& aHost,MStreamOutput& anOutput,TInt aMaxLength)
/** Writes data into the shared stream from the specified data source

@param aHost The object that manages shared streaming.
@param anOutput The data source for the write operation.
@param aMaxLength The maximum amount of data available to be written.
@return The amount of data that was not consumed. */
	{return aMaxLength-aHost.DoWriteL(anOutput,TStreamTransfer(aMaxLength),*this).Left();}
inline void TStreamMark::WriteL(TStreamExchange& aHost,MStreamOutput& anOutput)
/** Writes data into the shared stream from the specified data source.

No explicit limit is placed on the amount of data that can be written.

@param aHost The object that manages shared streaming.
@param anOutput The data source for the write operation. */
	{aHost.DoWriteL(anOutput,KStreamUnlimited,*this);}
inline void TStreamMark::SeekL(TStreamExchange& aHost,TStreamPos aPos)
	{aHost.DoSeekL(*this,EStreamBeginning,aPos.Offset());}
inline TStreamPos TStreamMark::SeekL(TStreamExchange& aHost,TStreamLocation aLocation,TInt anOffset)
/** Moves the position of the mark in the host stream.

The new position is calculated by adding the specified offset to one of:

the position of the beginning of the host stream

the position of the end of the host stream

the position of the current mark.

@param aHost The object that manages shared streaming.
@param aLocation The location in the host stream on which the calculation of 
the new position is based.
@param anOffset The offset value.
@return The new position of the mark. */
	{return aHost.DoSeekL(*this,aLocation,anOffset);}
inline TStreamPos TStreamMark::SeekL(TStreamExchange& aHost,TInt anOffset)
/** Moves the position of the mark in the host stream.

@param aHost The object that manages shared streaming.
@param anOffset The amount by which the position of the mark is to be moved 
relative to the existing position of the mark.
@return The new position of the mark. */
	{return aHost.DoSeekL(*this,EStreamMark,anOffset);}
inline TStreamPos TStreamMark::TellL(TStreamExchange& aHost) const
/** Gets the position of the mark within the host stream.

@param aHost The object that manages shared streaming.
@return The stream position. */
	{return aHost.DoSeekL(CONST_CAST(TStreamMark&,*this),EStreamMark,0);}

// Class TStreamExchange
inline TStreamExchange::TStreamExchange()
	: iHost(NULL),iRMrk(NULL),iWMrk(NULL)
/** Constructs an empty object.

Call Share() to prepare for access to a shared stream buffer. */
	{}
inline TStreamExchange::TStreamExchange(MStreamBuf* aHost)
	: iHost(aHost),iRMrk(NULL),iWMrk(NULL)
/** Constructs the object, specifying the stream buffer that will act as the shared 
host.

@param aHost A pointer to a stream buffer that will act as the shared host. */
	{}
inline void TStreamExchange::Share(MStreamBuf* aHost)
/** Tells the object to use the specified stream buffer that will act as the shared 
host.

@param aHost A pointer to a stream buffer that will act as the shared host. */
	{iHost=aHost;}
inline TBool TStreamExchange::IsActive() const
/** Tests whether this object is using a stream buffer that is acting as shared 
host.

@return True, if this object is using a shared host ; false, otherwise. */
	{return iHost!=NULL;}
	
// Class RShareBuf
inline void RShareBuf::Open(TStreamExchange& aHost,TInt aMode)
/** Prepares the shared stream buffer for streaming.

The function sets the read mark and/or the write mark to the beginning of 
the host stream.

@param aHost The object that manages shared streaming.
@param aMode The streaming mode. This can be read and/or write, as indicated 
by the ERead and EWrite bits.
@see MStreamBuf::TRead
@see MStreamBuf::TWrite
@see KStreamBeginning */
	{Open(aHost,KStreamBeginning,aMode);}

// Class RShareWriteStream
inline RShareWriteStream::RShareWriteStream(const MExternalizer<TStreamRef>& anExter)
	: RWriteStream(anExter)
	{}

