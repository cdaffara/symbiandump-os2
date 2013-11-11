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

EXPORT_C void RReadStream::Release()
/** Frees resources before abandoning the stream.

Note that, if a cleanup item for the stream was placed on the cleanup stack 
when the stream was opened by a call to OpenLC(), then this function need 
not be called explicitly; clean up is implicitly done by CleanupStack::PopAndDestroy(). */
	{
	if (iSrc==NULL)
		return;
//
	iSrc->Release();
	iSrc=NULL;
	}

EXPORT_C void RReadStream::PushL()
/** Puts a cleanup item for this read stream object onto the cleanup stack. This 
allows allocated resources to be cleaned up if a subsequent leave occurs. */
	{
	CleanupReleasePushL(*this);
	}

EXPORT_C void RReadStream::ReadL(TDes8& aDes)
/** Reads sufficient data from this stream to fill the specified 8 bit descriptor up to its maximum length.
No other information is read from this read stream.

@param aDes A reference to a modifiable descriptor which is to receive the data read from this stream. Passing the build 
independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.*/
	{
	ReadL(aDes,aDes.MaxLength());
	}

EXPORT_C void RReadStream::ReadL(TDes8& aDes,TInt aLength)
/** Reads data of specified length from this stream into the specified 8 bit descriptor. No other information is read 
from this stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. 
Passing the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit 
or the 16 bit) at build time.
@param aLength The length of data to be read from this stream. This value must be non-negative and must not be greater than the maximum length of the descriptor otherwise the function raises a USER 11 panic.*/
	{
	__ASSERT_DEBUG(aLength<=aDes.MaxLength(),Panic(EStreamReadBeyondEnd));
	aDes.SetLength(aLength);
	ReadL((TUint8*)aDes.Ptr(),aLength);
	}

EXPORT_C void RReadStream::ReadL(TDes8& aDes,TChar aDelim)
/** Reads data from this stream into the 8 bit descriptor, until either the specified delimiter is encountered or the descriptor is filled to its maximum length.
The resulting data in aDes always includes the delimiter aDelim, if aDes is large enough.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing 
the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.
@param aDelim The delimiter marking the end of the data in the stream.*/

	{
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TUint8* ptr=(TUint8*)aDes.Ptr();
	TDelimitedInput8 input(ptr,aDes.MaxLength(),aDelim);
	do
		{
		iSrc->ReadL(input);
		} while (!input.Done());
	aDes.SetLength(input.Ptr()-ptr);
	}

EXPORT_C void RReadStream::ReadL(TUint8* aPtr,TInt aLength)
/** Reads data of specified length from this stream into the location defined by the specified TUint8 pointer.

@param aPtr The target location for the streamed in data.
@param aLength The length of data to be streamed in.*/
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamReadLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TInt len=iSrc->ReadL(aPtr,aLength);
	__ASSERT_DEBUG(len>=0&&len<=aLength,Panic(EStreamReadInBreach));
	if (len<aLength)
		__LEAVE(KErrEof);
	}

EXPORT_C void RReadStream::ReadL(TInt aLength)
/** Discards data of specified length read from this stream.

@param aLength The length of data to be discarded from this read stream.*/
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamReadLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TNullInput input;
	TInt len=iSrc->ReadL(input,aLength);
	__ASSERT_DEBUG(len>=0&&len<=aLength,Panic(EStreamReadInBreach));
	if (len<aLength)
		__LEAVE(KErrEof);
	}

EXPORT_C void RReadStream::ReadL(TDes16& aDes)
/** Reads sufficient data from this stream to fill the specified 16 bit descriptor up to its maximum length. 
No other information is read from this read stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing 
the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 
bit) at build time.*/
	{
	ReadL(aDes,aDes.MaxLength());
	}

EXPORT_C void RReadStream::ReadL(TDes16& aDes,TInt aLength)
/** Reads data of specified length from this stream into the specified 16 bit descriptor. No other information is read from this stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing the 
build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) 
at build time.
@param aLength The length of data to be read from this stream. This value must be non-negative and must not be greater
than the maximum length of the descriptor otherwise the function raises a USER 11 panic.*/
	{
	__ASSERT_DEBUG(aLength<=aDes.MaxLength(),Panic(EStreamReadBeyondEnd));
	aDes.SetLength(aLength);
	ReadL((TUint16*)aDes.Ptr(),aLength);
	}

EXPORT_C void RReadStream::ReadL(TDes16& aDes,TChar aDelim)
/** Reads data from this stream into the 16 bit descriptor, until either the specified delimiter is encountered or 
the descriptor is filled to its maximum length.
The resulting data in aDes always includes the delimiter aDelim, if aDes is large enough.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing 
the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.
@param aDelim The delimiter marking the end of the data in the stream.*/
	{
	__ASSERT_DEBUG(iSrc!=NULL,Panic(EStreamNotOpen));
	TUint16* ptr=(TUint16*)aDes.Ptr();
	TDelimitedInput16 input(ptr,aDes.MaxLength(),aDelim);
	do
		{
		iSrc->ReadL(input);
		} while (!input.Done());
	aDes.SetLength(input.Ptr()-ptr);
	}

EXPORT_C void RReadStream::ReadL(TUint16* aPtr,TInt aLength)
/** Reads data of specified length from this stream into the specified 16 bit descriptor.
 No other information is read from this stream.

@param aDes A reference to a modifiable type descriptor which is to receive the data read from this stream. Passing the build independent type TDes& allows the compiler to choose the appropriate ReadL() variant (i.e the 8 bit or the 16 bit) at build time.
@param aLength The length of data to be read from this stream. This value must be non-negative and must not be greater than the maximum length of the descriptor otherwise the function raises a USER 11 panic.*/
	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamReadLengthNegative));
	ReadL((TUint8*)aPtr,aLength<<1); // platform dependency
	}

EXPORT_C TInt8 RReadStream::ReadInt8L()
/** Internalises a TInt8 value The function reads an 8 bit value from this stream 
and interprets it as a TInt8.

@return The 8�bit value read from this stream. */
	{
	TInt8 val;
	ReadL((TUint8*)&val,1); // platform dependency
	return val;
	}

EXPORT_C TInt16 RReadStream::ReadInt16L()
/** Internalises a TInt16 value. The function reads a 16 bit value from this stream 
and interprets it as a TInt16.

@return The 16�bit value read from this stream. */
	{
	TInt16 val;
	ReadL((TUint8*)&val,2); // platform dependency
	return val;
	}

EXPORT_C TInt32 RReadStream::ReadInt32L()
/** Internalises a TInt32 value. The function reads a 32 bit value from this stream 
and interprets it as a TInt32.

@return The 32 bit value read from this stream. */
	{
	TInt32 val;
	ReadL((TUint8*)&val,4); // platform dependency
	return val;
	}

EXPORT_C TUint8 RReadStream::ReadUint8L()
/** Internalises a TUint8 value. The function reads an 8 bit value from this stream 
and interprets it as a TUint8.

@return The 8 bit value read from this stream. */
	{
	TUint8 val;
	ReadL(&val,1);
	return val;
	}

EXPORT_C TUint16 RReadStream::ReadUint16L()
/** Internalises a TUint16 value. The function reads a 16 bit value from this 
stream and interprets it as a TUint16.

@return The 16 bit value read from this stream. */
	{
	TUint16 val;
	ReadL((TUint8*)&val,2); // platform dependency
	return val;
	}

EXPORT_C TUint32 RReadStream::ReadUint32L()
/** Internalises a TUint32 value. The function reads a 32 bit value from this 
stream and interprets it as a TUint32.

@return The 32 bit value read from this stream. */
	{
	TUint32 val;
	ReadL((TUint8*)&val,4); // platform dependency
	return val;
	}

EXPORT_C TReal32 RReadStream::ReadReal32L() __SOFTFP
/** Internalises a TReal32 value. The function reads a 32 bit value from this 
stream and interprets it as a TReal32.

@return The 32 bit value read from this read stream. */
	{
	TReal32 val;
	ReadL((TUint8*)&val,4); // platform dependency
	return val;
	}

EXPORT_C TReal64 RReadStream::ReadReal64L() __SOFTFP
/** Internalises a TReal64 value. The function reads a 64 bit value from this 
stream and interprets it as a TReal64.

@return The 64�bit value read from this stream. */
	{
#if defined(__DOUBLE_WORDS_SWAPPED__)
	union {TReal64 val;TUint32 buf[3];} u; // platform dependency
	ReadL((TUint8*)&u.buf[1],8);
	u.buf[0]=u.buf[2];
	return u.val;
#else
	TReal64 val;
	ReadL((TUint8*)&val,8); // platform dependency
	return val;
#endif
	}

EXPORT_C void RWriteStream::Close()
/** Commits data to the stream before freeing resources used by the stream. This 
ensures that any buffered data is written to the stream.

Note that the function cannot leave. Any errors arising from the attempt to 
commit data to the stream are ignored. */
	{
	if (iSnk==NULL)
		return;
//
	iSnk->Close();
	iSnk=NULL;
	}

EXPORT_C void RWriteStream::Release()
/** Frees resources before abandoning the stream. The function is called after 
data has been committed to the stream.

Note that if a cleanup item for the stream was placed on the cleanup stack 
when the stream was opened (e.g by a call to RStoreWriteStreamss CreateLC(), 
OpenLC(), ReplaceLC() or RDictionaryStores AssignLC() etc), then this function 
need not be called explicitly; clean up is implicitly done by CleanupStack::PopAndDestroy(). */
	{
	if (iSnk==NULL)
		return;
//
	iSnk->Release();
	iSnk=NULL;
	}

EXPORT_C void RWriteStream::CommitL()
/** Ensures that any buffered data is written to the stream. Once committed, it 
is not possible to roll back the newly written data. */
	{
	if (iSnk==NULL)
		return;
//
	iSnk->SynchL();
	}

EXPORT_C void RWriteStream::PushL()
/** Puts a cleanup item for this write stream object onto the cleanup stack. This 
allows allocated resources to be cleaned up if a subsequent leave occurs. */
	{
	CleanupReleasePushL(*this);
	}

EXPORT_C void RWriteStream::WriteL(const TDesC8& aDes)
/** Writes the content of the 8 bit descriptor to the stream. No other information 
is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time. */
	{
	WriteL(aDes.Ptr(),aDes.Length());
	}

EXPORT_C void RWriteStream::WriteL(const TDesC8& aDes,TInt aLength)
/** Writes data of the specified length from the 8 bit descriptor to the stream. 
No other information is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time.
@param aLength The length of data to be written to this stream. */

	{
	__ASSERT_DEBUG(aLength<=aDes.Length(),Panic(EStreamWriteBeyondEnd));
	WriteL(aDes.Ptr(),aLength);
	}

EXPORT_C void RWriteStream::WriteL(const TUint8* aPtr,TInt aLength)
/** Writes 8 bit data of the specified length from the specified location to this 
write stream.

@param aPtr The location from where data is to be streamed out.
@param aLength The length of data to be streamed out. */

	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	iSnk->WriteL(aPtr,aLength);
	}

EXPORT_C void RWriteStream::WriteL(RReadStream &aStream)
/** Writes the content of the specified read stream to this write stream.

@param aStream A reference to a read stream which is to be written to this 
stream. */

	{
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	TSourceOutput output(aStream.iSrc);
	iSnk->WriteL(output);
	}

EXPORT_C void RWriteStream::WriteL(RReadStream& aStream,TInt aLength)
/** Writes data of the specified length from the specified read stream to this 
stream.

@param aStream A reference to a read stream part of whose content is to be 
written to this stream.
@param aLength The length of data from the read stream to be written to this 
write stream. */

	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	if (aLength==0)
		return;
//
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	TSourceOutput output(aStream.iSrc);
	TInt len=iSnk->WriteL(output,aLength);
	__ASSERT_DEBUG(len>=0&&len<=aLength,Panic(EStreamReadInBreach));
	if (len<aLength)
		__LEAVE(KErrEof);
	}

EXPORT_C void RWriteStream::WriteL(const TDesC16& aDes)
/** Writes the content of the 16 bit descriptor to the stream. No other information 
is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time. */

	{
	WriteL(aDes.Ptr(),aDes.Length());
	}

EXPORT_C void RWriteStream::WriteL(const TDesC16& aDes,TInt aLength)
/** Writes data of the specified length from the 16 bit descriptor to the stream. 
No other information is written to this write stream.

@param aDes A reference to a descriptor. Passing the build independent type 
TDesC& allows the compiler to choose the appropriate WriteL() variant (i.e 
the 8 bit or the 16 bit) at build time.
@param aLength The length of data to be written to this stream. */

	{
	__ASSERT_DEBUG(aLength<=aDes.Length(),Panic(EStreamWriteBeyondEnd));
	WriteL(aDes.Ptr(),aLength);
	}

EXPORT_C void RWriteStream::WriteL(const TUint16* aPtr,TInt aLength)
/** Writes 16 bit data of the specified length from the specified location to this 
write stream.

@param aPtr The location from where data is to be streamed out.
@param aLength The length of data to be streamed out. */

	{
	__ASSERT_DEBUG(aLength>=0,Panic(EStreamWriteLengthNegative));
	WriteL((const TUint8*)aPtr,aLength<<1); // platform dependency
	}

EXPORT_C void RWriteStream::WriteInt8L(TInt aValue)
/** Writes a TInt value as an 8 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	WriteL((const TUint8*)&aValue,1); // platform dependency
	}

EXPORT_C void RWriteStream::WriteInt16L(TInt aValue)
/** Writes a TInt value as a 16 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
	WriteL((const TUint8*)&aValue,2); // platform dependency
	}

EXPORT_C void RWriteStream::WriteInt32L(TInt32 aValue)
/** Writes a TInt32 value as a 32 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	WriteL((const TUint8*)&aValue,4); // platform dependency
	}

EXPORT_C void RWriteStream::WriteUint8L(TUint aValue)
/** Writes a TUint value as an 8 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
	WriteL((const TUint8*)&aValue,1); // platform dependency
	}

EXPORT_C void RWriteStream::WriteUint16L(TUint aValue)
/** Writes a TUint value as a 16 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	WriteL((const TUint8*)&aValue,2); // platform dependency
	}

EXPORT_C void RWriteStream::WriteUint32L(TUint32 aValue)
/** Writes a TUint32 value as a 32 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
	WriteL((const TUint8*)&aValue,4); // platform dependency
	}

EXPORT_C void RWriteStream::WriteReal32L(TReal aValue) __SOFTFP
/** Writes a TReal value as a 32 bit value to this stream.

@param aValue The value to be written to this stream. */

	{
	TReal32 val=TReal32(aValue);
	WriteL((const TUint8*)&val,4); // platform dependency
	}

EXPORT_C void RWriteStream::WriteReal64L(TReal64 aValue) __SOFTFP
/** Writes a TReal64 value as a 64 bit value to this stream.

@param aValue The value to be written to this stream. */
	{
#if defined(__DOUBLE_WORDS_SWAPPED__)
	union {TReal64 val;TUint32 buf[3];} u; // platform dependency
	u.val=aValue;
	u.buf[2]=u.buf[0];
	WriteL((const TUint8*)&u.buf[1],8);
#else
	WriteL((const TUint8*)&aValue,8); // platform dependency
#endif
	}

EXPORT_C void RWriteStream::WriteRefL(TStreamRef aRef)
//
// Interpret and write aRef to this stream.
//
	{
	__ASSERT_DEBUG(iSnk!=NULL,Panic(EStreamNotOpen));
	__ASSERT_DEBUG(iExterL!=NULL,Panic(EStreamDoesNotUnderstand));
	(*iExterL)(aRef,*this);
	}

