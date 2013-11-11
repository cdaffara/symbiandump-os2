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
// Resource reader
// 
//

#include "BaRsReadImpl.h"
#include "BaCompileAssert.h"

/** @internalComponent
An error will be issued at compile time if the class size is not KRsReaderImplSize. */
TResourceReaderImpl::TResourceReaderImpl() :
    iBuffer(NULL),
    iCurrentPtr(NULL)
	{
	//TResourceReaderImpl size. It should be 12 because of the BC reasons.
	//12 is the size of TResourceReader class.
	enum
		{
		KRsReaderImplSize = 12
		};
	COMPILE_TIME_ASSERT(sizeof(TResourceReaderImpl) == KRsReaderImplSize);
	}

/** Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.

@internalComponent
@param aBuffer Pointer to an 8 bit non-modifiable descriptor containing 
or representing resource data.
@param aResourceId The numeric id of the resource to be read.
@post Buffer pointer is initialized.
@post Buffer current position pointer is initialized. */
void TResourceReaderImpl::SetBuffer(const TDesC8* aBuffer)
	{
	iBuffer=aBuffer;
	iCurrentPtr=iBuffer->Ptr();
	}

/** Sets the buffer and current position to NULL.
@internalComponent
@post Buffer pointer is set to NULL.
@post Buffer current position pointer is set to NULL. */
void TResourceReaderImpl::ResetBuffer()
	{
	iBuffer=NULL;
	iCurrentPtr=NULL;
	}

/** Returns the current position within the resource buffer. 

The function makes no assumption about the type of data in the buffer at the 
current position.

@internalComponent
@return A pointer to the current position within the resource buffer. */
const TAny* TResourceReaderImpl::Ptr()
    {
    return(iCurrentPtr);
    }

/** Updates iCurrentPtr with a new value.

@internalComponent
@pre iBuffer is not NULL.
@pre aPtr is not NULL.
@param aPtr The new value of iCurrentPtr.
@post iCurrentPtr is updated.
@panic BAFL 4 The new iCurrentPtr points beyond the buffer end.
@panic BAFL 70 iBuffer is NULL. DEBUG build only.
@panic BAFL 71 aPtr is NULL. DEBUG build only.
@leave KErrOff The new iCurrentPtr points beyond the buffer end. */
void TResourceReaderImpl::MovePtrL(const TUint8* aPtr)
    {
	__ASSERT_DEBUG(iBuffer != NULL, Panic(EBafPanicNullPtr1));
	__ASSERT_DEBUG(aPtr != NULL, Panic(EBafPanicNullPtr2));
	iAssertObj.AssertRelL(aPtr<=(iBuffer->Ptr()+iBuffer->Length()), EBafPanicResourceReaderEndExceeded);
    iCurrentPtr=aPtr;
    }

/** Interprets the data at the current buffer position as leading byte count data 
and constructs an 8 bit heap descriptor containing a copy of this data.

The data is interpreted as:

a byte value defining the number of 8 bit text characters or the length of 
binary data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 8 bit text characters or binary data.

If the value of the leading byte is zero, the function assumes that no data 
follows the leading byte and returns a NULL pointer.

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadHBufCL().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@internalComponent
@pre The same as for ReadTPtrC8L().
@return Pointer to the 8 bit heap descriptor containing a
copy of the data following the leading byte count at
the current position within the resource buffer. The
pointer can be NULL.
@post iCurrentPtr is updated.
@panic The same as ReadTPtrC8L().
@leave The same as ReadTPtrC8L().
@see ReadTPtrC8L() */
HBufC8* TResourceReaderImpl::ReadHBufC8L()
	{
	const TPtrC8 data(ReadTPtrC8L());
	return (data.Length()==0)? NULL: data.AllocL();
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a 16 bit heap descriptor containing a copy of this data.

The data is interpreted as:

a byte value defining the number of 16 bit text characters
(the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

If the value of the leading byte is zero, the function assumes that no data 
follows the leading byte and returns a NULL pointer.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadHBufCL().

@internalComponent
@pre The same as for ReadTPtrC16L().
@return Pointer to the 16bit heap descriptor containing a
copy of the data following the leading byte count at
the current position within the resource buffer. The
pointer can be NULL.
@post iCurrentPtr is updated.
@panic The same as ReadTPtrC16L().
@leave The same as ReadTPtrC16L(). 
@see ReadTPtrC16L() */
HBufC16* TResourceReaderImpl::ReadHBufC16L()
	{
	const TPtrC16 data(ReadTPtrC16L());
	return (data.Length()==0)? NULL: data.AllocL();
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs an 8 bit non modifiable pointer descriptor to represent this 
data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 8 bit text characters or binary data.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC8 returns zero.

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadTPtrC().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return 8bit non modifiable pointer descriptor representing
the data following the leading byte count at the
current position within the resource buffer.
@post iCurrentPtr is updated.
@panic BAFL 72 iCurrentPtr is NULL. DEBUG build only.
@panic The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TPtrC8 TResourceReaderImpl::ReadTPtrC8L()
	{
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr3));
	const TUint8* currentPtr=iCurrentPtr;//TUint8 pointer is used, which means that the 
	//resource string length is limited to 255 characters max.
	const TInt strLen=*currentPtr;
	++currentPtr;
	MovePtrL(currentPtr+strLen);
	return TPtrC8(currentPtr,strLen);
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a 16 bit non modifiable pointer descriptor to represent this 
data.

The data is interpreted as:

a byte value defining the number of 16 bit text characters 
(the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC16 returns zero.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrC().

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return Pointer to an 8bit variant flat descriptor array.
@post iCurrentPtr is updated.
@panic BAFL 73 iCurrentPtr is NULL. DEBUG build only.
@panic BAFL 15 The resource is a unicode string and it is not properly aligned. DEBUG build only.
@panic The same as MovePtrL(const TUint8* aPtr).
@leave KErrCorrupt The resource is a unicode string and it is not properly aligned.
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TPtrC16 TResourceReaderImpl::ReadTPtrC16L()
	{
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr4));
	const TUint8* currentPtr=iCurrentPtr;//TUint8 pointer is used, which means that the 
	//resource string length is limited to 255 characters max.
	const TInt unicodeLength=*currentPtr;
	++currentPtr;
	if (unicodeLength!=0)
		{
		if (REINTERPRET_CAST(TUint,currentPtr)&0x1)
			{
			// The resource compiler puts out a padding byte (arbitrarily 0xab)
			// to ensure the alignment of Unicode strings within each resource.
			iAssertObj.AssertDebL(*currentPtr==0xab, EBafPanicUnicodeTextPaddingError);
			++currentPtr;
			}
		}
	const TPtrC16 unicode(REINTERPRET_CAST(const TText16*,(unicodeLength==0)? NULL: currentPtr),unicodeLength);
	currentPtr+=unicodeLength*sizeof(TText16);
	MovePtrL(currentPtr);
	return unicode;
	}

/** Interprets the data within the specified resource buffer as an array of leading 
byte count data and constructs an 8 bit non modifiable pointer descriptor 
to represent an element within this array.

The function sets the buffer containing the resource data and sets the current 
position to the start of this buffer. Any buffer set by a previous call to 
SetBuffer() etc, is lost.

The buffer is expected to contain an array of data elements preceded by a 
TInt16 value defining the number of elements within that array.

Each element of the array is interpreted as:

a byte value defining the number of 8 bit text characters or the length of 
binary data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 8 bit text characters or binary data.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC8 returns zero.

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data, If 
the resource contains text, then use the build independent variant ReadTPtrC(TInt,const TDesC8*).

@internalComponent
@pre aBuffer != NULL. 
@pre The same as MovePtrL(const TUint8* aPtr).
@param aIndex Position of the element within the array. This
value is relative to zero.
@param aBuffer Buffer containing the resource data.
@return 8bit non modifiable pointer descriptor representing
the data following the leading byte count at the
current position within the resource buffer.
@post iBuffer is initialized with aBuffer.
@post The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 4 aIndex is greater or equal than the string length. DEBUG build only.
@panic The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@see MovePtrL(const TUint8* aPtr) */
TPtrC8 TResourceReaderImpl::ReadTPtrC8L(TInt aIndex,const TDesC8* aBuffer)
    { // implementation could be made more efficient if desired
	SetBuffer(aBuffer);
	TInt count=ReadInt16L();
//
	__ASSERT_DEBUG(aIndex<count,Panic(EBafPanicResourceReaderEndExceeded));
	if (aIndex>=count)
		return TPtrC8();
//
	const TUint8* ptr=iCurrentPtr;
	while (--aIndex>=0)
		ptr+=1+*ptr;
	MovePtrL(ptr);
	return ReadTPtrC8L();
    }

/** Interprets the data within the specified resource buffer as an array of leading 
byte count data and constructs a 16 bit non modifiable pointer descriptor 
to represent an element within this array.

The function sets the buffer containing the resource data and sets the current 
position to the start of this buffer. Any buffer set by a previous call to 
SetBuffer() etc., is lost.

The buffer is expected to contain an array of data elements preceded by a 
TInt16 value defining the number of elements within that array.

Each element of the array is interpreted as:

a byte value defining the number of 8 bit text characters or the length of 
binary data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC16 returns zero.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrC(TInt,const TDesC8*).

@internalComponent
@pre aBuffer != NULL.
@pre The same as ReadTPtrC16L().
@param aIndex The position of the element within the array. This
value is relative to zero.
@param aBuffer The buffer containing the resource data.
@return 16bit non modifiable pointer descriptor representing
the data following the leading byte count of the
element at position within the array
@post iBuffer is initialized with aBuffer.
@post The same as ReadTPtrC16L().
@panic BAFL 4 aIndex is greater or equal than the string length.
@panic The same as ReadTPtrC16L().
@leave KErrOff aIndex is grater or equal than the string length.
@leave The same as ReadTPtrC16L(). 
@see ReadTPtrC16L()*/
TPtrC16 TResourceReaderImpl::ReadTPtrC16L(TInt aIndex,const TDesC8* aBuffer)
    { // implementation could be made more efficient if desired
	SetBuffer(aBuffer);
	const TInt count=ReadInt16L();
	iAssertObj.AssertRelL(aIndex<count,EBafPanicResourceReaderEndExceeded);
	for (TInt i=0; i<aIndex; ++i)
		{
		ReadTPtrC16L();
		}
	return ReadTPtrC16L();
    }

/** Interprets the data at the current buffer position as an array of leading byte 
count data and constructs a flat array of 8 bit descriptors.

Each descriptor in the descriptor array corresponds to an element of the resource 
array.

At the current buffer position, the buffer is expected to contain an array 
of data elements preceded by a TInt16 value defining the number of elements 
within that array.

Each element of the array is interpreted as:

a byte value defining the number of 8 bit text characters or the length of 
binary data (the resource string/binary data length is limited to 255 characters max)

followed by:

the text characters or binary data.

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data. If 
the elements of the resource array contain text, use the build independent 
variant of ReadDesCArrayL().

@internalComponent
@pre The same as ReadTPtrC8L().
@return Pointer to an 8bit variant flat descriptor array.
@post The same as ReadTPtrC8L().
@panic The same as ReadTPtrC8L().
@leave The same as ReadTPtrC8L().
@leave KErrNoMemory There is not enough memory
for the resulting buffer.
@see ReadTPtrC8L() */
CDesC8ArrayFlat* TResourceReaderImpl::ReadDesC8ArrayL()
    {
	TInt count=ReadInt16L();
	CDesC8ArrayFlat* array=new(ELeave) CDesC8ArrayFlat(count);
    CleanupStack::PushL(array);
	while (--count>=0)
		array->AppendL(ReadTPtrC8L());
    CleanupStack::Pop();
	return(array);
    }

/** Interprets the data at the current buffer position as an array of leading byte 
count data and constructs a flat array of 16 bit descriptors.

Each descriptor in the descriptor array corresponds to an element of the resource 
array.

At the current buffer position, the buffer is expected to contain an array 
of data elements preceded by a TInt16 value defining the number of elements 
within that array.

Each element of the array is interpreted as:

a byte value defining the number of 8 bit text characters or the length of 
binary data (the resource string/binary data length is limited to 255 characters max)

followed by:

the 16 bit text characters.

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadDesCArrayL().

@internalComponent
@pre The same as ReadTPtrC16L().
@return Pointer to a 16bit variant flat descriptor array.
@post The same as ReadTPtrC16L().
@panic The same as ReadTPtrC16L().
@leave The same as ReadTPtrC16L().
@leave KErrNoMemory There is not enough memory
for the resulting buffer. 
@see ReadTPtrC16L() */
CDesC16ArrayFlat* TResourceReaderImpl::ReadDesC16ArrayL()
    {
	TInt count=ReadInt16L();
	CDesC16ArrayFlat* array=new(ELeave) CDesC16ArrayFlat(count);
    CleanupStack::PushL(array);
	while (--count>=0)
		array->AppendL(ReadTPtrC16L());
    CleanupStack::Pop();
	return(array);
    }

/** Interprets the data at the current buffer position as a TInt8 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt8.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TInt8 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 74 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
TInt TResourceReaderImpl::ReadInt8L()
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr5));
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TInt8));
    return(*(TInt8*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TUint8 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint8.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TUint8 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 75 iCurrentPtr is NULL. DEBUG build only. 
@see MovePtrL(const TUint8* aPtr) */
TUint TResourceReaderImpl::ReadUint8L()
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr6));
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TUint8));
    return(*(TUint8*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TInt16 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt16.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TInt16 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 76 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
TInt TResourceReaderImpl::ReadInt16L()
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr7));
    if (((TUint)iCurrentPtr)%2)
        {
        TInt16 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TInt16));
    return(*(TInt16*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TUint16 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint16.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TUint16 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 77 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
TUint TResourceReaderImpl::ReadUint16L()
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr8));
    if (((TUint)iCurrentPtr)%2)
        {
        TUint16 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TUint16));
    return(*(TUint16*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TInt32 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS, TInt and TInt32 are the same size.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TInt32 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 78 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
TInt TResourceReaderImpl::ReadInt32L()
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr9));
    if (((TUint)iCurrentPtr)%4)
        {
        TInt32 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TInt32));
	return(*(TInt32*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TUint32 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS a TUint is the same size as a TUint32.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@return The TUint32 value taken from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 79 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
TUint TResourceReaderImpl::ReadUint32L()
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr10));
    if (((TUint)iCurrentPtr)%4)
        {
        TUint32 ret;
        ReadL(&ret,sizeof(ret));
        return(ret);
        }
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+sizeof(TUint32));
    return(*(TUint32*)currentPtr);
    }

/** Interprets the data at the current buffer position as a TReal64 type and returns 
the value as a TReal64.

The current position within the resource buffer is updated.

In general, a TReal64 corresponds to a DOUBLE type in a resource STRUCT declaration.

@internalComponent
@pre The same as ReadUint32L().
@return The TReal64 value taken from the resource buffer.
@post The same as ReadUint32L().
@leave The same as ReadUint32L().
@panic The same as ReadUint32L().
@see ReadUint32L() */
TReal64 TResourceReaderImpl::ReadReal64L() __SOFTFP
    {
    union
        {
        TReal64 ret;
        TUint32 tmp[2];
        };
#if defined(__DOUBLE_WORDS_SWAPPED__)
    tmp[1]=ReadUint32L();
    tmp[0]=ReadUint32L();
#else
    tmp[0]=ReadUint32L();
    tmp[1]=ReadUint32L();
#endif
    return(ret);
    }

/** Copies a specified length of data from the resource buffer, starting at the 
current position within the buffer, into the location pointed to by a specified 
pointer. No assumption is made about the type of data at being read.

The current position within the resource buffer is updated.

@internalComponent
@pre iCurrentPtr != NULL.
@pre The same as MovePtrL(const TUint8* aPtr).
@param aPtr Pointer to the target location for data copied from the resource buffer.
@param  aLength The length of data to be copied from the resource buffer.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 80 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
void TResourceReaderImpl::ReadL(TAny* aPtr,TInt aLength)
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr11));
    const TUint8* currentPtr=iCurrentPtr;
    MovePtrL(currentPtr+aLength);
    Mem::Copy(aPtr,currentPtr,aLength);
    }

/** Moves the current buffer position backwards by the specified amount.

@internalComponent
@pre iCurrentPtr != NULL.
@param aLength The length by which the current position is to be moved backward.
@post iCurrentPtr is updated.
@leave @see MovePtrL(const TUint8* aPtr).
@panic BAFL 5 If the resulting position lies before the start of the resource.
@panic BAFL 81 iCurrentPtr is NULL. DEBUG build only.
@leave KErrArgument The resulting position lies before the start of the resource. */
void TResourceReaderImpl::RewindL(TInt aLength)
    {
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr12));
	iAssertObj.AssertRelL(!(aLength>iCurrentPtr-iBuffer->Ptr()),EBafPanicResourceReaderStartExceeded);
    iCurrentPtr-=aLength;
    }

/** Moves the current buffer position forwards by the specified amount.

@internalComponent
@pre The same as MovePtrL(const TUint8* aPtr).
@param aLength The length by which the current position is to be advanced.
@post The same as MovePtrL(const TUint8* aPtr).
@leave The same as MovePtrL(const TUint8* aPtr).
@panic The same as MovePtrL(const TUint8* aPtr).
@panic BAFL 82 iCurrentPtr is NULL. DEBUG build only.
@see MovePtrL(const TUint8* aPtr) */
void TResourceReaderImpl::AdvanceL(TInt aLength)
	{
	__ASSERT_DEBUG(iCurrentPtr != NULL, Panic(EBafPanicNullPtr13));
	MovePtrL(iCurrentPtr+aLength);
	}

/** The method sets a new iAssertObj. 
If some method is called and something goes wrong - the method either
will panics or asserts depending on iAssertObj state.

@internalComponent
@param  aAssertObj The assert object.
@post iAssertObj is updated. */
void TResourceReaderImpl::SetAssertObj(const TBaAssert& aAssertObj)
	{
	iAssertObj = aAssertObj;
	}

