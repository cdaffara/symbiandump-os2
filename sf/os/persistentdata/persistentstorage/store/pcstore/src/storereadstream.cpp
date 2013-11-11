// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <assert.h>
#include <pcstore/storereadstream.h>
#include <pcstore/descriptors.h>
#include <pcstore/uid.h>
#include <pcstore/checkeduid.h>
#include <pcstore/storeexception.h>
#include "filestreambuf.h"
#include "desheader.h"
#include "unicodecompression.h"

namespace PCStore
{
/**
Constructs a read stream object with the file stream buffer and stream id.

The file stream buffer is only created and held by CDirectFileStore. So, this object 
is only constructed by CDirectFileStore::CreateReadStream().

Calls the CFileStreamBuf::StreamOpen() to flag its opening. If an existing stream 
has been opened, an exception will be thrown and this constructor fails.

@param aStreamBuf The reference to the file stream buffer.
@param aId The stream id of the this stream.
@exception CStoreException::EStreamExclusionError another stream has been opened.
*/
CStoreReadStream::CStoreReadStream(CFileStreamBuf& aStreamBuf, TStreamId aId)
	:iSrc(aStreamBuf)
	{
	iSrc.StreamOpen(aId);
	}

/**
Destructor.

Calls the CFileStreamBuf::StreamClose() to notify the file stream buffer about its 
closing. 
*/
CStoreReadStream::~CStoreReadStream()
	{
	iSrc.StreamClose();
	}

/** 
Reads 8-bit data of specified length from this stream into the location defined by 
the specified TUint8 pointer.

aPtr must not be NULL. aLength must not be negative.

@param aPtr The target location for the streamed in data.
@param aLength The length of data to be streamed in.
@exception CStoreException::EFileReadError error occurs when reading data from the file.
*/
void CStoreReadStream::Read(TUint8* aPtr, TInt aLength)
	{
	assert(aPtr != NULL && aLength >=0);
	iSrc.Read(aPtr,aLength);
	}

/** 
Reads 16-bit data of specified length from this stream into the location defined by 
the specified TUint8 pointer.

aPtr must not be NULL. aLength must not be negative.

@param aPtr The target location for the streamed in data.
@param aLength The length of data to be streamed in.
@exception CStoreException::EFileReadError error occurs when reading data from the file.
*/
void CStoreReadStream::Read(TUint16* aPtr, TInt aLength)
	{
	assert(aPtr != NULL && aLength >=0);
	Read(reinterpret_cast<TUint8*>(aPtr),aLength * 2); 
	}

/** 
Internalizes a TInt8 value.
The function reads an 8-bit value from this stream and interprets it as a TInt8.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 8 bit value read from this stream. 
*/
TInt8 CStoreReadStream::ReadInt8()
	{
	TInt8 val;
	Read(reinterpret_cast<TUint8*>(&val),1);
	return val;
	}

/** 
Internalizes a TUint8 value. 
The function reads an 8-bit value from this stream and interprets it as a TUint8.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 8-bit value read from this stream. 
*/
TUint8 CStoreReadStream::ReadUint8()
	{
	TUint8 val;
	Read(reinterpret_cast<TUint8*>(&val),1);
	return val;
	}

/** 
Internalizes a TInt16 value. 
The function reads a 16-bit value from this stream and interprets it as a TInt16.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 16-bit value read from this stream. 
*/
TInt16 CStoreReadStream::ReadInt16()
	{
	TInt16 val;
	Read(reinterpret_cast<TUint8*>(&val),2);
	return val;
	}

/** 
Internalizes a TUint16 value. 
The function reads a 16-bit value from this stream and interprets it as a TUint16.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 16-bit value read from this stream. 
*/
TUint16 CStoreReadStream::ReadUint16()
	{
	TUint16 val;
	Read(reinterpret_cast<TUint8*>(&val),2);
	return val;
	}

/** 
Internalizes a TInt32 value. 
The function reads a 32-bit value from this stream and interprets it as a TInt32.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 32-bit value read from this stream. 
*/
TInt32 CStoreReadStream::ReadInt32()
	{
	TInt32 val;
	Read(reinterpret_cast<TUint8*>(&val),4);
	return val;
	}

/** 
Internalizes a TUint32 value. 
The function reads a 32-bit value from this stream and interprets it as a TUint32.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 32-bit value read from this stream. 
*/
TUint32 CStoreReadStream::ReadUint32()
	{
	TUint32 val;
	Read(reinterpret_cast<TUint8*>(&val),4);
	return val;
	}

/** 
Internalizes a TInt64 value. 
The function reads a 64-bit value from this stream and interprets it as a TInt64.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 64-bit value read from this stream. 
*/
TInt64 CStoreReadStream::ReadInt64()
	{
	TInt64 val;
	Read(reinterpret_cast<TUint8*>(&val),8);
	return val;
	}

/** 
Internalizes a TUint64 value. 
The function reads a 64-bit value from this stream and interprets it as a TUint64.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 64-bit value read from this stream.
*/
TUint64 CStoreReadStream::ReadUint64()
	{
	TUint64 val;
	Read(reinterpret_cast<TUint8*>(&val),8);
	return val;
	}

/** 
Internalizes a TReal32 value. 
The function reads a 32-bit value from this stream and interprets it as a TReal32.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 32-bit value read from this stream. 
*/
TReal32 CStoreReadStream::ReadReal32()
	{
	TReal32 val;
	Read(reinterpret_cast<TUint8*>(&val),4);
	return val;
	}

/** 
Internalizes a TReal64 value. 
The function reads a 64-bit value from this stream and interprets it as a TReal64.

@exception CStoreException::EFileReadError error occurs when reading data from the file.
@return The 64-bit value read from this stream. 
*/
TReal64 CStoreReadStream::ReadReal64()
	{
	TReal64 val;
	Read(reinterpret_cast<TUint8*>(&val),8);
	return val;
	}

/** 
Reads sufficient data from this stream to fill the specified 8-bit descriptor up to 
its maximum length.

This method only reads the data in the maximum length of the given descriptor. To carry 
out an intelligent internalization, which obtains the data's width and length from the 
stream and then reads the data according to the above information, use operator >>.

The length of the descriptor will be set to the maximum length after the successful reading.

@param aDes The reference to a descriptor which is to receive the data read from this stream. 
@exception CStoreException::EFileReadError error occurs when reading data from the file.
@see CStoreReadStream& operator>>(CStoreReadStream& aStream, CDes8& aDes)
*/
void CStoreReadStream::Read(CDes8& aDes)
	{
	Read(aDes,aDes.MaxLength());
	}

/** 
Reads data of specified length from this stream into the specified 8-bit descriptor. 

Only the data which the descriptor represents, but no other information, such as the 
width and length of the data, will be read from the stream.

The length of the descriptor will be set to the specified length after the successful reading.

aLength cannot be negative or greater than the maximum length of the descriptor.

@param aDes The reference to a descriptor which is to receive the data read from this stream. 
@param aLength The length of data to be read from this stream. 
@exception CStoreException::EFileReadError error occurs when reading data from the file.
*/
void CStoreReadStream::Read(CDes8& aDes, TInt aLength)
	{
	assert(aLength >=0 && aLength <= aDes.MaxLength());	
	Read(const_cast<TUint8*>(aDes.Ptr()),aLength);
	aDes.SetLength(aLength);
	}

/** 
Reads sufficient data from this stream to fill the specified 16-bit descriptor up to 
its maximum length.

This method only reads the data in the maximum length of the given descriptor. To carry 
out an intelligent internalization, which obtains the data's width and length from the 
stream and then reads the data according to the above information, use operator >>.

The length of the descriptor will be set to the maximum length after the successful reading.

@param aDes The reference to a descriptor which is to receive the data read from this stream. 
@exception CStoreException::EFileReadError error occurs when reading data from the file.
@see CStoreReadStream& operator>>(CStoreReadStream& aStream, CDes16& aDes)
*/
void CStoreReadStream::Read(CDes16& aDes)
	{
	Read(aDes,aDes.MaxLength());
	}

/**
Reads data of specified length from this stream into the specified 16-bit descriptor. 

No other information is read  from this stream.
The length of the descriptor will be set to the specified length after the successful reading.

aLength cannot be negative or greater than the maximum length of the descriptor.

@param aDes The reference to a descriptor which is to receive the data read from this stream. 
@param aLength The length of data to be read from this stream. 
@exception CStoreException::EFileReadError error occurs when reading data from the file.
*/
void CStoreReadStream::Read(CDes16& aDes, TInt aLength)
	{
	assert(aLength >=0 && aLength <= aDes.MaxLength());	
	Read(reinterpret_cast<TUint8*>(const_cast<TUint16*>(aDes.Ptr())),aLength * 2);
	aDes.SetLength(aLength);
	}

/**
Operator to internalize a TStreamId object.

@param aStream The reference to the read stream that the TStreamId object is internalized 
from.
@param aUid The reference to the TStreamId object to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TStreamId& aId)
	{
	aId.Internalize(aStream);
	return aStream;
	}

/**
Operator to internalize a TInt8 value.

@param aStream The reference to the read stream that the TInt8 value is internalized from.
@param aUid The reference to the TInt8 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt8& aVal)
	{
	aVal = aStream.ReadInt8();
	return aStream;
	}

/**
Operator to internalize a TUint8 value.

@param aStream The reference to the read stream that the TUint8 value is internalized from.
@param aUid The reference to the TUint8 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint8& aVal)
	{
	aVal = aStream.ReadUint8();
	return aStream;
	}

/**
Operator to internalize a TInt16 value.

@param aStream The reference to the read stream that the TInt16 value is internalized from.
@param aUid The reference to the TInt16 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt16& aVal)
	{
	aVal = aStream.ReadInt16();
	return aStream;
	}

/**
Operator to internalize a TUint16 value.

@param aStream The reference to the read stream that the TUint16 value is internalized from.
@param aUid The reference to the TUint16 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint16& aVal)
	{
	aVal = aStream.ReadUint16();
	return aStream;
	}

/**
Operator to internalize a TInt32 value.

@param aStream The reference to the read stream that the TInt32 value is internalized from.
@param aUid The reference to the TInt32 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt32& aVal)
	{
	aVal = aStream.ReadInt32();
	return aStream;
	}

/**
Operator to internalize a TUint32 value.

@param aStream The reference to the read stream that the TUint32 value is internalized from.
@param aUid The reference to the TUint32 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint32& aVal)
	{
	aVal = aStream.ReadUint32();
	return aStream;
	}

/**
Operator to internalize a TUint64 value.

@param aStream The reference to the read stream that the TUint64 value is internalized from.
@param aUid The reference to the TUint64 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint64& aVal)
	{
	aVal = aStream.ReadUint64();
	return aStream;
	}

/**
Operator to internalize a TInt64 value.

@param aStream The reference to the read stream that the TInt64 value is internalized from.
@param aUid The reference to the TInt64 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt64& aVal)
	{
	aVal = aStream.ReadInt64();
	return aStream;
	}

/**
Operator to internalize a TReal32 value.

@param aStream The reference to the read stream that the TReal32 value is internalized from.
@param aUid The reference to the TReal32 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TReal32& aVal)
	{
	aVal = aStream.ReadReal32();
	return aStream;
	}

/**
Operator to internalize a TReal64 value.

@param aStream The reference to the read stream that the TReal64 value is internalized from.
@param aUid The reference to the TReal64 value to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TReal64& aVal)
	{
	aVal = aStream.ReadReal64();
	return aStream;
	}

/**
Operator to internalize an 8-bit descriptor with its header.

A header of the descriptor contains the information of its width(8 or 16 bits) and length.

@param aStream The reference to the read stream that the descriptor is internalized from.
@param aDes8 The reference of the 8-bit descriptor to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EDesOverfloat The maximum length of the descriptor is less than 
the length of the content which will be restored from the stream.
@exception TStoreException::EStoreCorrupt Store file is corrupted so that an invalid cardinal 
number is read.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, CDes8& aDes)
	{
	CDesHeader header;
	header.Internalize(aStream);
	TInt len=header.Length();

	if (len>aDes.MaxLength())
		{
		throw  TStoreException(TStoreException::EDesOverfloat);
		}

	TUint8* ptr=const_cast<TUint8*>(aDes.Ptr());
	aStream.Read(ptr,len);
	aDes.SetLength(len);

	return aStream;
	}

/**

Operator to internalize an 16-bit descriptor with its header.

A header of the descriptor contains the information of its width(8 or 16 bits) and length.

@param aStream The reference to the read stream that the descriptor is internalized from.
@param aDes16 The reference of the 16-bit descriptor to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EDesOverfloat The maximum length of the descriptor is less than 
the length of the content which will be restored from the stream.
@exception TStoreException::EStoreCorrupt Store file is corrupted so that an invalid cardinal 
number is read.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, CDes16& aDes)
	{
	CDesHeader header;
	header.Internalize(aStream);
	TInt len=header.Length();
	
	if (len>aDes.MaxLength())
		{
		throw  TStoreException(TStoreException::EDesOverfloat);
		}

	TUint16* ptr=const_cast<TUint16*>(aDes.Ptr());
	// In the Unicode build 16-bit descriptors need to be expanded from the Standard Unicode Compression Scheme.
	TMemoryUnicodeSink sink(ptr);
	TUnicodeExpander expander;
	expander.ExpandL(sink,aStream,len);
	aDes.SetLength(len);

	return aStream;
	}

/**
Operator to internalize a TUid object.

@param aStream The reference to the read stream that the TUid object is internalized from.
@param aUid The reference to the TUid object to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUid& aUid)
	{
	return aStream >> aUid.iUid;
	}

/**
Operator to internalize a TCheckedUid object.

@param aStream The reference to the read stream that the TCheckedUid object is 
internalized from.
@param aUid The reference to the TCheckedUid object to receive the read data.
@return The reference to the read stream.
@exception TStoreException::EFileReadError Error occurs when reading data from the file.
*/
CStoreReadStream& operator>>(CStoreReadStream& aStream, TCheckedUid& aUid)
	{
	CDes8 des(sizeof(TCheckedUid));
	aStream.Read(des);
	aUid.Set(des);
	return aStream;
	}
}
