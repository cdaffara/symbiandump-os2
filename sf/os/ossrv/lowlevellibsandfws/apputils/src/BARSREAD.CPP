// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <barsread.h>
#include "BaRsReadImpl.h"

#define UNUSED_VAR(a) a = a

/** Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.

@param aBuffer A pointer to an 8-bit non-modifiable descriptor containing 
or representing resource data. */
EXPORT_C void TResourceReader::SetBuffer(const TDesC8* aBuffer)
	{
	CreateImpl();
	Impl()->SetBuffer(aBuffer);
	}

/** Returns the current position within the resource buffer. 

The function makes no assumption about the type of data in the buffer at the 
current position.

@return A pointer to the current position within the resource buffer. */
EXPORT_C const TAny* TResourceReader::Ptr()
    {
	return Impl()->Ptr();
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this explicit 8-bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadHBufCL().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@return A pointer to the 8-bit heap descriptor containing a copy of the data 
following the leading byte count at the current position within the resource 
buffer. The pointer can be NULL. */
EXPORT_C HBufC8* TResourceReader::ReadHBufC8L()
	{
	return Impl()->ReadHBufC8L();
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Do not use this explicit 16-bit variant when the resource contains binary 
data; use the explicit 8-bit variant instead. If the resource contains text, 
use the build independent variant ReadHBufCL().

@return A pointer to the 16-bit heap descriptor containing a copy of the 
data following the leading byte count at the current position within the resource 
buffer. The pointer can be NULL. */
EXPORT_C HBufC16* TResourceReader::ReadHBufC16L()
	{
	return Impl()->ReadHBufC16L();
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this explicit 8-bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadTPtrC().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@return An 8-bit non modifiable pointer descriptor representing the data 
following the leading byte count at the current position within the resource 
buffer. */
EXPORT_C TPtrC8 TResourceReader::ReadTPtrC8()
	{
	TPtrC8 retPtr;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retPtr.Set(ReadTPtrC8L ()));
    UNUSED_VAR(errCode);
	return retPtr;
	}

TPtrC8 TResourceReader::ReadTPtrC8L()
	{
	return Impl()->ReadTPtrC8L();
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Do not use this explicit 16-bit variant when the resource contains binary 
data; use the explicit 8-bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrC().

@return A 16-bit non modifiable pointer descriptor representing the data 
following the leading byte count at the current position within the resource 
buffer. */
EXPORT_C TPtrC16 TResourceReader::ReadTPtrC16()
	{
	TPtrC16 retPtr;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retPtr.Set(ReadTPtrC16L ()));
    UNUSED_VAR(errCode);
	return retPtr;
	}

TPtrC16 TResourceReader::ReadTPtrC16L()
	{
	return Impl()->ReadTPtrC16L();
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this explicit 8 bit variant when the resource contains binary data, If 
the resource contains text, then use the build independent variant ReadTPtrC(TInt,const TDesC8*).

@param aIndex The position of the element within the array. This value is 
relative to zero.
@param aBuffer The buffer containing the resource data.
@return An 8-bit non modifiable pointer descriptor representing the data following 
the leading byte count of the element at the specified position within the 
array. */
EXPORT_C TPtrC8 TResourceReader::ReadTPtrC8(TInt aIndex,const TDesC8* aBuffer)
    {
	TPtrC8 retPtr;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retPtr.Set(ReadTPtrC8L(aIndex, aBuffer)));
    UNUSED_VAR(errCode);
	return retPtr;
    }

TPtrC8 TResourceReader::ReadTPtrC8L(TInt aIndex,const TDesC8* aBuffer)
    {
	CreateImpl();
	return Impl()->ReadTPtrC8L(aIndex,aBuffer);
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Do not use this explicit 16-bit variant when the resource contains binary 
data; use the explicit 8-bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrC(TInt,const TDesC8*).

@param aIndex The position of the element within the array. This value is 
relative to zero.
@param aBuffer The buffer containing the resource data.
@return A 16-bit non modifiable pointer descriptor representing the data following 
the leading byte count of the element at position within the array */
EXPORT_C TPtrC16 TResourceReader::ReadTPtrC16(TInt aIndex,const TDesC8* aBuffer)
    {
	TPtrC16 retPtr;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retPtr.Set(ReadTPtrC16L(aIndex, aBuffer)));
    UNUSED_VAR(errCode);
	return retPtr;
    }

TPtrC16 TResourceReader::ReadTPtrC16L(TInt aIndex,const TDesC8* aBuffer)
    {
	CreateImpl();
	return Impl()->ReadTPtrC16L(aIndex,aBuffer);
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this explicit 8-bit variant when the resource contains binary data. If 
the elements of the resource array contain text, use the build independent 
variant of ReadDesCArrayL().

@return A pointer to an 8-bit variant flat descriptor array. */
EXPORT_C CDesC8ArrayFlat* TResourceReader::ReadDesC8ArrayL()
    {
	return Impl()->ReadDesC8ArrayL();
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

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Do not use this explicit 16-bit variant when the resource contains binary 
data; use the explicit 8-bit variant instead. If the resource contains text, 
use the build independent variant ReadDesCArrayL().

@return A pointer to a 16-bit variant flat descriptor array. */
EXPORT_C CDesC16ArrayFlat* TResourceReader::ReadDesC16ArrayL()
    {
	return Impl()->ReadDesC16ArrayL();
    }

/** Interprets the data at the current buffer position as a TInt8 type and returns 
the value as a TInt.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TInt8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt8.

@return The TInt8 value taken from the resource buffer. */
EXPORT_C TInt TResourceReader::ReadInt8()
    {
	TInt retInt=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retInt=ReadInt8L());
    UNUSED_VAR(errCode);
	return retInt;
    }

TInt TResourceReader::ReadInt8L()
    {
	return Impl()->ReadInt8L();
    }

/** Interprets the data at the current buffer position as a TUint8 type and returns 
the value as a TUint.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TUint8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint8.

@return The TUint8 value taken from the resource buffer. */
EXPORT_C TUint TResourceReader::ReadUint8()
    {
	TUint retInt=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retInt=ReadUint8L());
    UNUSED_VAR(errCode);
	return retInt;
    }

TUint TResourceReader::ReadUint8L()
    {
	return Impl()->ReadUint8L();
    }

/** Interprets the data at the current buffer position as a TInt16 type and returns 
the value as a TInt.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TInt16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt16.

@return The TInt16 value taken from the resource buffer. */
EXPORT_C TInt TResourceReader::ReadInt16()
    {
	TInt retInt=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retInt=ReadInt16L());
    UNUSED_VAR(errCode);
	return retInt;
    }

TInt TResourceReader::ReadInt16L()
    {
	return Impl()->ReadInt16L();
    }

/** Interprets the data at the current buffer position as a TUint16 type and returns 
the value as a TUint.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TUint16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint16.

@return The TUint16 value taken from the resource buffer. */
EXPORT_C TUint TResourceReader::ReadUint16()
    {
	TUint retInt=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retInt=ReadUint16L());
    UNUSED_VAR(errCode);
	return retInt;
    }

TUint TResourceReader::ReadUint16L()
    {
	return Impl()->ReadUint16L();
    }

/** Interprets the data at the current buffer position as a TInt32 type and returns 
the value as a TInt.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TInt32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS, TInt and TInt32 are the same size.

@return The TInt32 value taken from the resource buffer. */
EXPORT_C TInt TResourceReader::ReadInt32()
    {
	TInt retInt=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retInt=ReadInt32L());
    UNUSED_VAR(errCode);
	return retInt;
    }

TInt TResourceReader::ReadInt32L()
    {
	return Impl()->ReadInt32L();
    }

/** Interprets the data at the current buffer position as a TUint32 type and returns 
the value as a TUint.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TUint32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS a TUint is the same size as a TUint32.

@return The TUint32 value taken from the resource buffer. */
EXPORT_C TUint TResourceReader::ReadUint32()
    {
	TUint retInt=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retInt=ReadUint32L());
    UNUSED_VAR(errCode);
	return retInt;
    }

TUint TResourceReader::ReadUint32L()
    {
	return Impl()->ReadUint32L();
    }

/** Interprets the data at the current buffer position as a TReal64 type and returns 
the value as a TReal64.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

In general, a TReal64 corresponds to a DOUBLE type in a resource STRUCT declaration.

@return The TReal64 value taken from the resource buffer. */
EXPORT_C TReal64 TResourceReader::ReadReal64() __SOFTFP
    {
	TReal64 retReal=0;
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, retReal=ReadReal64L());
    UNUSED_VAR(errCode);
	return retReal;
    }

TReal64 TResourceReader::ReadReal64L() __SOFTFP
    {
	return Impl()->ReadReal64L();
    }

/** Copies a specified length of data from the resource buffer, starting at the 
current position within the buffer, into the location pointed to by a specified 
pointer. No assumption is made about the type of data at being read.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

@param aPtr A pointer to the target location for data copied from the resource 
buffer.
@param aLength The length of data to be copied from the resource buffer. */
EXPORT_C void TResourceReader::Read(TAny* aPtr,TInt aLength)
    {
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, Impl()->ReadL(aPtr,aLength));
    UNUSED_VAR(errCode);
    }

/** Moves the current buffer position backwards by the specified amount.

If the resulting position lies before the start of the resource buffer, then 
the function raises a BAFL 5 panic.

@param aLength The length by which the current position is to be moved backward. */
EXPORT_C void TResourceReader::Rewind(TInt aLength)
    {
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, Impl()->RewindL(aLength));
    UNUSED_VAR(errCode);
    }

/** Moves the current buffer position forwards by the specified amount.

If the resulting position lies beyond the end of the resource buffer, then 
the function raises a BAFL 4 panic.

@param aLength The length by which the current position is to be advanced. */
EXPORT_C void TResourceReader::Advance(TInt aLength)
	{
	// TRAP and ignore the Error code as its non leaving method
	TRAPD(errCode, Impl()->AdvanceL(aLength));
    UNUSED_VAR(errCode);
	}

/** Placement new operator is used TResourceReaderImpl instance to be created.
TResourceReaderImpl instance default behaviour - it will panic when there is a problem.

@internalComponent */
void TResourceReader::CreateImpl()
	{
	new (iImpl) TResourceReaderImpl;
	}

/** Placement new operator is used TResourceReaderImpl instance to be created.
TResourceReaderImpl instance default behaviour - it will panic when there is a problem.

@internalComponent */
TResourceReaderImpl* TResourceReader::Impl()
	{
	return reinterpret_cast <TResourceReaderImpl*> (iImpl);
	}

/** The method returns a const pointer to TResourceReader implementation.

@internalComponent */
const TResourceReaderImpl* TResourceReader::Impl() const
	{
	return reinterpret_cast <const TResourceReaderImpl*> (iImpl);
	}

