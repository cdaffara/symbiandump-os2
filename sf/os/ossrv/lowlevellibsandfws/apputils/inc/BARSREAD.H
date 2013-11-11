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
//

#ifndef __BARSREAD_H__
#define __BARSREAD_H__

#include <e32std.h>
#include <badesca.h>

class TResourceReaderImpl;

/** 
Interprets resource data read from a resource file.

To use an instance of this class, pass the buffer containing the resource 
data to it by calling SetBuffer().

The buffer containing the resource data is created by RResourceFile::AllocReadLC() or 
RResourceFile::AllocReadL() which reads the specified resource into it.

The current position within the buffer is always maintained and any request 
for data is always supplied from the current position. The current position 
is always updated .

@see TResourceReader::SetBuffer()
@see RResourceFile::AllocReadL()
@see RResourceFile::AllocReadLC() 
@publishedAll
@released
*/
class TResourceReader
    {
public:
    IMPORT_C void SetBuffer(const TDesC8* aBuffer);
    IMPORT_C const TAny* Ptr();

    // Read counted strings into allocated buffer
    inline HBufC*   ReadHBufCL();
    IMPORT_C HBufC8*  ReadHBufC8L();
    IMPORT_C HBufC16* ReadHBufC16L();

    // Build pointer from a counted string
    inline TPtrC   ReadTPtrC();
    IMPORT_C TPtrC8  ReadTPtrC8();
	IMPORT_C TPtrC16 ReadTPtrC16();

    // Build pointer from a counted string in an array of counted strings,
    // also setting the buffer to be used.
    inline TPtrC ReadTPtrC(TInt aIndex,const TDesC8* aBuffer);
    IMPORT_C TPtrC8 ReadTPtrC8(TInt aIndex,const TDesC8* aBuffer);
    IMPORT_C TPtrC16 ReadTPtrC16(TInt aIndex,const TDesC8* aBuffer);

    // Build an array of strings from a resource array
    inline CDesCArrayFlat*   ReadDesCArrayL();
    IMPORT_C CDesC8ArrayFlat*  ReadDesC8ArrayL();
    IMPORT_C CDesC16ArrayFlat* ReadDesC16ArrayL();

    IMPORT_C TInt ReadInt8();
    IMPORT_C TUint ReadUint8();
    IMPORT_C TInt ReadInt16();
    IMPORT_C TUint ReadUint16();
    IMPORT_C TInt ReadInt32();
    IMPORT_C TUint ReadUint32();
    IMPORT_C TReal64 ReadReal64() __SOFTFP;

    IMPORT_C void Read(TAny* aPtr,TInt aLength);
    IMPORT_C void Rewind(TInt aLength);
    IMPORT_C void Advance(TInt aLength);

private:
	void CreateImpl();
	TResourceReaderImpl* Impl();
	const TResourceReaderImpl* Impl() const;

	TPtrC8 ReadTPtrC8L();
	TPtrC16 ReadTPtrC16L();
	TPtrC8 ReadTPtrC8L(TInt aIndex,const TDesC8* aBuffer);
	TPtrC16 ReadTPtrC16L(TInt aIndex,const TDesC8* aBuffer);
	TInt ReadInt8L();
	TUint ReadUint8L();
	TInt ReadInt16L();
	TUint ReadUint16L();
	TInt ReadInt32L();
	TUint ReadUint32L();
	TReal64 ReadReal64L() __SOFTFP;

private:
	enum
		{
		KRsReaderSize = 12
		};
	TUint8 iImpl[KRsReaderSize];
    };

#if defined(_UNICODE)

/** 
Interprets the data at the current buffer position as leading byte count data 
and constructs a build independent heap descriptor containing a copy of this 
data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data

followed by:

the text characters or binary data. This resource data is interpreted as either 
8-bit or 16-bit, depending on the build.

If the value of the leading byte is zero, the function assumes that no data 
follows the leading byte and returns a NULL pointer.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this build independent variant when the resource contains text. If the 
resource contains binary data, use the explicit 8-bit variant ReadHBufC8L().

@return A pointer to the heap descriptor containing a copy of the data following 
the leading byte count at the current position within the resource buffer. 
The pointer can be NULL. 
*/
inline HBufC* TResourceReader::ReadHBufCL()
    {
	return ReadHBufC16L();
	}

/** 
Interprets the data at the current buffer position as leading byte count data 
and constructs a non modifiable pointer descriptor to represent this data.

The data is interpreted as:

a byte value defining the number of text characters or the length of binary 
data

followed by:

the text characters or binary data. This resource data is interpreted as either 
8-bit or 16-bit, depending on the build.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC returns zero.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this build independent variant when the resource contains text. If the 
resource contains binary data, use the explicit 8-bit variant ReadTPtrC8().

@return A non modifiable pointer descriptor representing the data following 
the leading byte count at the current position within the resource buffer. 
*/
inline TPtrC TResourceReader::ReadTPtrC()
    {
	return ReadTPtrC16();
	}

/** 
Interprets the data within the specified resource buffer as an array of leading 
byte count data and constructs a non modifiable pointer descriptor to represent 
an element within this array.

The function sets the buffer containing the resource data and sets the current 
position to the start of this buffer. Any buffer set by a previous call to 
SetBuffer() etc, is lost.

The buffer is expected to contain an array of data elements preceded by a 
TInt16 value defining the number of elements within that array.

Each element of the array is interpreted as:

a byte value defining the number of text characters or the length of binary 
data

followed by:

the text characters or binary data. This resource data is interpreted as either 
8-bit or 16-bit, depending on the build.

If the value of the leading byte is zero, calling Length() on the returned 
TPtrC returns zero.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this build independent variant when the elements contain text. If the 
elements contain binary data, use the explicit 8-bit variant ReadTPtrC8(TInt,const TDesC8*).

@param aIndex The position of the element within the array. This value is 
relative to zero.
@param aBuffer The buffer containing the resource data.
@return A non modifiable pointer descriptor representing the data following 
the leading byte count of the element at the specified position within the 
array. 
*/
inline TPtrC TResourceReader::ReadTPtrC(TInt aIndex,const TDesC8* aBuffer)
    {
	return ReadTPtrC16(aIndex, aBuffer);
	}

/** 
Interprets the data at the current buffer position as an array of leading byte 
count data and constructs a build independent flat array of descriptors.

Each descriptor in the descriptor array corresponds to an element of the resource 
array.

At the current buffer position, the buffer is expected to contain an array 
of data elements preceded by a TInt16 value defining the number of elements 
within that array.

Each element of the array is interpreted as:

a byte value defining the number of text characters or the length of binary 
data

followed by:

the text characters or binary data. This resource data is interpreted as either 
8-bit or 16-bit, depending on the build.

The current position within the resource buffer is updated. If the resulting 
position lies beyond the end of the resource buffer, then the function raises 
a BAFL 4 panic.

Use this build independent variant when the elements contain text. If the 
elements contain binary data, use the explicit 8-bit variant ReadDesC8ArrayL().

@return A pointer to a build independent flat descriptor array. 
*/
inline CDesCArrayFlat* TResourceReader::ReadDesCArrayL()
    {
	return ReadDesC16ArrayL();
	}

#else // defined(_UNICODE)

inline HBufC* TResourceReader::ReadHBufCL()
    {
	return ReadHBufC8L();
	}

inline TPtrC TResourceReader::ReadTPtrC()
    {
	return ReadTPtrC8();
	}

inline TPtrC TResourceReader::ReadTPtrC(TInt aIndex,const TDesC8* aBuffer)
    {
	return ReadTPtrC8(aIndex, aBuffer);
	}

inline CDesCArrayFlat* TResourceReader::ReadDesCArrayL()
    {
	return ReadDesC8ArrayL();
	}

#endif// defined(_UNICODE)



#endif//__BARSREAD_H__
