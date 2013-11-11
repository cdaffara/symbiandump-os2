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

#include <barsc2.h>
#include <barsread2.h>
#include "BaRsReadImpl.h"

/** It creates the implementation in place - iImpl array,
and sets the default leave/panic behaviour of the object - 
the object "L" methods will leave if something goes wrong.
*/
EXPORT_C RResourceReader::RResourceReader() :
	iRscBuffer(NULL)
	{
	new (iImpl) TResourceReaderImpl;
	TBaAssert assertObj(TBaAssert::ELeave);
	Impl()->SetAssertObj(assertObj);
	}

/** The method calls RResourceReader::Close() method to release 
allocated by the instance resources.
*/
EXPORT_C RResourceReader::~RResourceReader()
	{
	Close();
	}

/** 
Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.
@param aRscFile A pointer to the CResourceFile object, used as a resource data supplier.
@param aResourceId The numeric id of the resource to be read.
@panic 0 If aRscFile is NULL.
*/
EXPORT_C void RResourceReader::OpenL(const CResourceFile* aRscFile, TInt aResourceId)
	{
	OpenLC(aRscFile, aResourceId);
	CleanupStack::Pop(this);
	}

/** 
Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.

A pointer to current RResourceReader instance is placed into the cleanup stack.

@param aRscFile A pointer to the CResourceFile object, used as a resource data supplier.
@param aResourceId Numeric id of the resource to be read.
@panic 0 If aRscFile is NULL.
*/
EXPORT_C void RResourceReader::OpenLC(const CResourceFile* aRscFile, TInt aResourceId)
	{
	__ASSERT_DEBUG(aRscFile, User::Invariant());
	Close();
	iRscBuffer = aRscFile->AllocReadL(aResourceId);
	Impl()->SetBuffer(iRscBuffer);
	CleanupClosePushL(*this);
	}

/** 
Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.

@param aRscData A reference to an 8�bit descriptor containing or representing resource data.
*/
EXPORT_C void RResourceReader::OpenL(const TDesC8& aRscData)
    {
	OpenLC(aRscData);
	CleanupStack::Pop(this);
    }

/** 
Sets the buffer containing the resource data.

The current position within the buffer is set to the start of the buffer so 
that subsequent calls to the interpreting functions, for example ReadInt8(), 
start at the beginning of this buffer.

A pointer to current RResourceReader instance is placed into the cleanup stack.

@param aRscData A reference to an 8�bit descriptor containing or representing resource data.
*/
EXPORT_C void RResourceReader::OpenLC(const TDesC8& aRscData)
    {
	Close();
    CleanupClosePushL(*this);
    iRscBuffer = aRscData.AllocL();
    Impl()->SetBuffer(iRscBuffer);
    }

/** 
Destroys the buffer containing the resource data.

Open() method should be called if you want to set
the buffer and current position again.

If a one or more copies of the same RResourceReader object exist - they share the same 
resource data buffer. So destroying the RResourceReader object you will destroy the
shared resource data buffer.

@post Buffer pointer is set to NULL.
@post Buffer current position pointer is set to NULL. 
*/
EXPORT_C void RResourceReader::Close()
	{
	delete iRscBuffer;
	iRscBuffer = NULL;
	Impl()->ResetBuffer();
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

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to the 8 bit heap descriptor containing a copy of the data 
following the leading byte count at the current position within the resource 
buffer. The pointer can be NULL.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C HBufC8* RResourceReader::ReadHBufC8L()
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

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadHBufCL().

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to the 16 bit heap descriptor containing a copy of the 
data following the leading byte count at the current position within the resource 
buffer. The pointer can be NULL.
@post Current buffer position is updated.
@leave KErrCorrupt The new buffer position is beyond the buffer end. */
EXPORT_C HBufC16* RResourceReader::ReadHBufC16L()
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

The current position within the resource buffer is updated.

Use this explicit 8 bit variant when the resource contains binary data. If 
the resource contains text, then use the build independent variant ReadTPtrC().

In general, this type of resource data corresponds to one of the following:

a LTEXT type in a resource STRUCT declaration.

a variable length array within a STRUCT declaration which includes the LEN 
BYTE keywords.

@pre Open() is called to initialize RResourceReader data members.
@return 8bit non modifiable pointer descriptor representing
the data following the leading byte count at the
current position within the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TPtrC8 RResourceReader::ReadTPtrC8L()
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

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadTPtrC().

@pre Open() is called to initialize RResourceReader data members.
@return 16 bit non modifiable pointer descriptor representing
the data following the leading byte count at the
current position within the resource buffer.
@post Current buffer position is updated.
@leave KErrCorrupt The new buffer position is beyond the buffer end. */
EXPORT_C TPtrC16 RResourceReader::ReadTPtrC16L()
	{
	return Impl()->ReadTPtrC16L();
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

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to an 8bit variant flat descriptor array.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C CDesC8ArrayFlat* RResourceReader::ReadDesC8ArrayL()
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

The current position within the resource buffer is updated.

Do not use this explicit 16 bit variant when the resource contains binary 
data; use the explicit 8 bit variant instead. If the resource contains text, 
use the build independent variant ReadDesCArrayL().

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to a 16bit variant flat descriptor array.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C CDesC16ArrayFlat* RResourceReader::ReadDesC16ArrayL()
    {
	return Impl()->ReadDesC16ArrayL();
    }

/** Interprets the data at the current buffer position as a TInt8 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt8.

@pre Open() is called to initialize RResourceReader data members.
@return The TInt8 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TInt RResourceReader::ReadInt8L()
    {
	return Impl()->ReadInt8L();
    }

/** Interprets the data at the current buffer position as a TUint8 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint8 corresponds to a BYTE type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint8.

@pre Open() is called to initialize RResourceReader data members.
@return The TUint8 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TUint RResourceReader::ReadUint8L()
    {
	return Impl()->ReadUint8L();
    }

/** Interprets the data at the current buffer position as a TInt16 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TInt is at least as big as a TInt16.

@pre Open() is called to initialize RResourceReader data members.
@return The TInt16 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TInt RResourceReader::ReadInt16L()
    {
	return Impl()->ReadInt16L();
    }

/** Interprets the data at the current buffer position as a TUint16 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint16 corresponds to a WORD type in a resource STRUCT declaration.

Note that in Symbian OS, a TUint is at least as big as a TUint16.

@pre Open() is called to initialize RResourceReader data members.
@return The TUint16 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TUint RResourceReader::ReadUint16L()
    {
	return Impl()->ReadUint16L();
    }

/** Interprets the data at the current buffer position as a TInt32 type and returns 
the value as a TInt.

The current position within the resource buffer is updated.

In general, a TInt32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS, TInt and TInt32 are the same size.

@pre Open() is called to initialize RResourceReader data members.
@return The TInt32 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TInt RResourceReader::ReadInt32L()
    {
	return Impl()->ReadInt32L();
    }

/** Interprets the data at the current buffer position as a TUint32 type and returns 
the value as a TUint.

The current position within the resource buffer is updated.

In general, a TUint32 corresponds to a LONG type in a resource STRUCT declaration.

Note that in Symbian OS a TUint is the same size as a TUint32.

@pre Open() is called to initialize RResourceReader data members.
@return The TUint32 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TUint RResourceReader::ReadUint32L()
    {
	return Impl()->ReadUint32L();
    }

/** Interprets the data at the current buffer position as a TReal64 type and returns 
the value as a TReal64.

The current position within the resource buffer is updated.

In general, a TReal64 corresponds to a DOUBLE type in a resource STRUCT declaration.

@pre Open() is called to initialize RResourceReader data members.
@return The TReal64 value taken from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C TReal64 RResourceReader::ReadReal64L() __SOFTFP
    {
	return Impl()->ReadReal64L();
    }

/** Copies a specified length of data from the resource buffer, starting at the 
current position within the buffer, into the location pointed to by a specified 
pointer. No assumption is made about the type of data at being read.

The current position within the resource buffer is updated.

@pre Open() is called to initialize RResourceReader data members.
@param aPtr Pointer to the target location for data copied from the resource buffer.
@param aLength The length of data to be copied from the resource buffer.
@post Current buffer position is updated.
@leave KErrEof The new buffer position is beyond the buffer end. */
EXPORT_C void RResourceReader::ReadL(TAny* aPtr,TInt aLength)
    {
	Impl()->ReadL(aPtr,aLength);
    }

/** Moves the current buffer position backwards by the specified amount.

@pre Open() is called to initialize RResourceReader data members.
@param aLength The length by which the current position is to be moved backward.
@post Current buffer position is updated.
@leave KErrArgument The resulting position lies before the start of the resource. */
EXPORT_C void RResourceReader::RewindL(TInt aLength)
    {
	Impl()->RewindL(aLength);
    }

/** Moves the current buffer position forwards by the specified amount.
If the resulting position lies beyond the end of the resource buffer,
then the function leaves with KErrEof code.

@pre Open() is called to initialize RResourceReader data members.
@param aLength The length by which the current position is to be advanced.
@post Current buffer position is updated.
@leave KErrEof The resulting position lies beyond the end of the resource buffer. */
EXPORT_C void RResourceReader::AdvanceL(TInt aLength)
	{
	Impl()->AdvanceL(aLength);
	}

#if defined(_UNICODE)
/** Interprets the data at the current buffer position as leading byte count data 
and constructs a build independent heap descriptor containing a copy of this 
data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the text characters or binary data. This resource data is interpreted as either 
8 bit or 16 bit, depending on the build.

If the value of the leading byte is zero, the function assumes that no data 
follows the leading byte and returns a NULL pointer.

The current position within the resource buffer is updated.

Use this build independent variant when the resource contains text. If the 
resource contains binary data, use the explicit 8 bit variant ReadHBufC8L().

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to the heap descriptor containing a copy of
the data following the leading byte count at the
current position within the resource buffer. The
pointer can be NULL.
@post Current buffer position is updated.
@leave KErrCorrupt The resulting position lies beyond the end of the resource buffer. */
EXPORT_C HBufC* RResourceReader::ReadHBufCL()
    {
	return ReadHBufC16L();
	}

/** Interprets the data at the current buffer position as leading byte count data 
and constructs a non modifiable pointer descriptor to represent this data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the text characters or binary data. This resource data is interpreted as either 
8 bit or 16 bit, depending on the build.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC returns zero.

The current position within the resource buffer is updated.

Use this build independent variant when the resource contains text. If the 
resource contains binary data, use the explicit 8 bit variant ReadTPtrC8().

@pre Open() is called to initialize RResourceReader data members.
@return Non modifiable pointer descriptor representing the
data following the leading byte count of the element
at the specified position within the array.
@post Current buffer position is updated.
@leave KErrCorrupt The resulting position lies beyond the end of the resource buffer. */
EXPORT_C TPtrC RResourceReader::ReadTPtrCL()
    {
	return ReadTPtrC16L();
	}

/** Interprets the data at the current buffer position as an array of leading byte 
count data and constructs a build independent flat array of descriptors.

Each descriptor in the descriptor array corresponds to an element of the resource 
array.

At the current buffer position, the buffer is expected to contain an array 
of data elements preceded by a TInt16 value defining the number of elements 
within that array.

Each element of the array is interpreted as:

a byte value defining the number of text characters or the length of binary 
data (the resource string/binary data length is limited to 255 characters max)

followed by:

the text characters or binary data. This resource data is interpreted as either 
8 bit or 16 bit, depending on the build.

The current position within the resource buffer is updated.

Use this build independent variant when the elements contain text. If the 
elements contain binary data, use the explicit 8 bit variant ReadDesC8ArrayL().

@pre Open() is called to initialize RResourceReader data members.
@return Pointer to a build independent flat descriptor array.
@post Current buffer position is updated.
@leave KErrEof The resulting position lies beyond the end of the resource buffer. */
EXPORT_C CDesCArrayFlat* RResourceReader::ReadDesCArrayL()
    {
	return ReadDesC16ArrayL();
	}

#else //if defined(_UNICODE)

EXPORT_C HBufC* RResourceReader::ReadHBufCL()
    {
	return ReadHBufC8L();
	}

EXPORT_C TPtrC RResourceReader::ReadTPtrCL()
    {
	return ReadTPtrC8L();
	}

EXPORT_C CDesCArrayFlat* RResourceReader::ReadDesCArrayL()
    {
	return ReadDesC8ArrayL();
	}

#endif //if defined(_UNICODE)

/** @internalComponent
@return Non-const pointer to the implementation object. */
TResourceReaderImpl* RResourceReader::Impl()
	{
	return reinterpret_cast <TResourceReaderImpl*> (iImpl);
	}

/** @internalComponent
@return Const pointer to the implementation object. */
const TResourceReaderImpl* RResourceReader::Impl() const
	{
	return reinterpret_cast <const TResourceReaderImpl*> (iImpl);
	}

