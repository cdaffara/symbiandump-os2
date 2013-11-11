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
#include <pcstore/storewritestream.h>
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
Constructs a write stream object.

The file stream buffer is only created and held by CDirectFileStore. So, this object 
is only constructed by CDirectFileStore::CreateWriteStream().

Calls the CFileStreamBuf::StreamOpen() to flag its opening. If an existing stream 
has been opened, an exception will be thrown and this constructor fails.

@param aStreamBuf The reference to the file stream buffer.
@param aId The stream id of the this stream.
@exception CStoreException::EStreamExclusionError another stream has been opened.
*/
CStoreWriteStream::CStoreWriteStream(CFileStreamBuf& aStreamBuf, TStreamId aId)
	:iSnk(aStreamBuf)
	{
	iSnk.StreamOpen(aId);
	}

/**
Destructor.

Calls the CFileStreamBuf::StreamClose() to notify the file stream buffer about its 
closing.
*/
CStoreWriteStream::~CStoreWriteStream()
	{
	iSnk.StreamClose();
	}


/** 
Writes the specified length of 8-bit data to this stream from the location defined by 
the specified TUint8 pointer.

aPtr must not be NULL. aLength must not be negative.

@param aPtr The location from where data is to be streamed out.
@param aLength The length of data to be streamed out. 
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::Write(const TUint8* aPtr, TInt aLength)
	{
	assert(aPtr != NULL && aLength >=0);
	iSnk.Write(aPtr, aLength);
	}

/** 
Writes the specified length of 16-bit data to this stream from the location defined by 
the specified TUint16 pointer.

aPtr must not be NULL. aLength must not be negative.

@param aPtr The location from where data is to be streamed out.
@param aLength The length of data to be streamed out. 
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::Write(const TUint16* aPtr, TInt aLength)
	{
	assert(aPtr != NULL && aLength >=0);
	Write(reinterpret_cast<const TUint8*>(aPtr),aLength * 2);
	}

/** 
Externalizes a TInt8 value to this stream.

@param aValue The TInt8 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteInt8(TInt8 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue), 1);
	}

/** 
Externalizes a TUint8 value to this stream.

@param aValue The TUint8 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteUint8(TUint8 aValue)
	{
	Write(&aValue, 1);
	}

/** 
Externalizes a TInt16 value to this stream.

@param aValue The TInt16 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteInt16(TInt16 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),2); 
	}

/** 
Externalizes a TUint16 value to this stream.

@param aValue The TUint16 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteUint16(TUint16 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),2); 
	}

/** 
Externalizes a TInt32 value to this stream.

@param aValue The TInt32 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteInt32(TInt32 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),4); 
	}

/** 
Externalizes a TUint32 value to this stream.

@param aValue The TUint32 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteUint32(TUint32 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),4); 
	}

/** 
Externalizes a TInt64 value to this stream.

@param aValue The TInt64 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteInt64(TInt64 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),8); 
	}

/** 
Externalizes a TUint64 value to this stream.

@param aValue The TUint64 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteUint64(TUint64 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),8); 
	}

/** 
Externalizes a TReal32 value to this stream.

@param aValue The TReal32 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteReal32(TReal32 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),4); 
	}

/** 
Externalizes a TReal64 value to this stream.

@param aValue The TReal64 value to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::WriteReal64(TReal64 aValue)
	{
	Write(reinterpret_cast<const TUint8*>(&aValue),8); 
	}

/** 
Externalizes the content of an 8-bit descriptor to this stream.

This method only writes the data in the length of the given descriptor directly to the 
stream. To carry out an intelligent externalization, which outputs the descriptor's 
width and length as a packed value as well as the data, use operator <<.

@param aDes The reference to the 8-bit descriptor to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
@see CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const CDes8& aDes)
*/
void CStoreWriteStream::Write(const CDes8& aDes)
	{
	Write(reinterpret_cast<const TUint8*>(aDes.Ptr()),aDes.Length());
	}

/** 
Externalizes the content of an 8-bit descriptor to this stream.
Only the data which the descriptor represents, but no other information, such as 
the width and length of the data, will be written to the stream.

aLength cannot be negative or greater than the length of the descriptor.

@param aDes The reference to the 8-bit descriptor to stream out.
@param aLength The length of data to be written to this stream. 
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::Write(const CDes8& aDes, TInt aLength)
	{
	assert(aLength >=0 && aLength <= aDes.Length());
	Write(aDes.Ptr(),aLength);
	}

/** 
Externalizes the content of a 16-bit descriptor to this stream.

This method only writes the data in the length of the given descriptor directly to the 
stream. To carry out an intelligent externalization, which outputs the descriptor's 
width and length as a packed value as well as the data, use operator <<.

@param aDes The reference to the 16-bit descriptor to stream out.
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
@see CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const CDes16& aDes)
*/
void CStoreWriteStream::Write(const CDes16& aDes)
	{
	Write(aDes.Ptr(),aDes.Length());
	}

/** 
Externalizes the content of an 8-bit descriptor to this stream.
Only the data which the descriptor represents, but no other information, such as 
the width and length of the data, will be written to the stream.

aLength cannot be negative or greater than the length of the descriptor.

@param aDes The reference to the 8-bit descriptor to stream out.
@param aLength The length of data to be written to this stream. 
@exception CStoreException::EFileWriteError Error occurs when writing data to the file.
*/
void CStoreWriteStream::Write(const CDes16& aDes, TInt aLength)
	{
	assert(aLength >=0 && aLength <= aDes.Length());
	Write(aDes.Ptr(),aLength);
	}


/**
Operator to externalize a TStreamId object.

@param aStream The reference to the write stream that the TStreamId is externalized to.
@param aId The reference to the TStreamId to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TStreamId& aId)
	{
	aId.Externalize(aStream);
	return aStream;
	}

/**
Operator to externalize a TInt8 value.

@param aStream The reference to the write stream that the TInt8 value is externalized to.
@param aVal The TInt8 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt8 aVal)
	{
	aStream.WriteInt8(aVal);
	return aStream;
	}

/**
Operator to externalize a TUint8 value.

@param aStream The reference to the write stream that the TUint8 value is externalized to.
@param aVal The TUint8 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint8 aVal)
	{
	aStream.WriteUint8(aVal);
	return aStream;
	}

/**
Operator to externalize a TInt16 value.

@param aStream The reference to the write stream that the TInt16 value is externalized to.
@param aVal The TInt16 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt16 aVal)
	{
	aStream.WriteInt16(aVal);
	return aStream;
	}


/**
Operator to externalize a TUint16 value.

@param aStream The reference to the write stream that the TUint16 is externalized to.
@param aVal The value of the TUint16 to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint16 aVal)
	{
	aStream.WriteUint16(aVal);
	return aStream;
	}

/**
Operator to externalize a TInt32 value.

@param aStream The reference to the write stream that the TInt32 value is externalized to.
@param aVal The TInt32 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt32 aVal)
	{
	aStream.WriteInt32(aVal);
	return aStream;
	}

/**
Operator to externalize a TUint32 value.

@param aStream The reference to the write stream that the TUint32 value is externalized to.
@param aVal The TUint32 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint32 aVal)
	{
	aStream.WriteUint32(aVal);
	return aStream;
	}

/**
Operator to externalize a TInt64 value.

@param aStream The reference to the write stream that the TInt64 value is externalized to.
@param aVal The TInt64 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt64 aVal)
	{
	aStream.WriteInt64(aVal);
	return aStream;
	}

/**
Operator to externalize a TUint64 value.

@param aStream The reference to the write stream that the TUint64 value is externalized to.
@param aVal The TUint64 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint64 aVal)
	{
	aStream.WriteUint64(aVal);
	return aStream;
	}

/**
Operator to externalize a TReal32 value.

@param aStream The reference to the write stream that the TReal32 value is externalized to.
@param aVal The TReal32 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TReal32 aVal)
	{
	aStream.WriteReal32(aVal);
	return aStream;
	}

/**
Operator to externalize a TReal64 value.

@param aStream The reference to the write stream that the TReal64 value is externalized to.
@param aVal The TReal64 value to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TReal64 aVal)
	{
	aStream.WriteReal64(aVal);
	return aStream;
	}

/**
Operator to externalize an 8-bit descriptor with its header.

A header of the descriptor contains the information of its width(8 or 16 bits) and length.

@param aStream The reference to the write stream that the descriptor is externalized to.
@param aDes8 The reference to the 8-bit descriptor to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream, const CDes8& aDes8)
	{
	CDesHeader header(aDes8);
	header.Externalize(aStream);
	aStream.Write(aDes8.Ptr(),aDes8.Length());
	return aStream;
	}

/**
Operator to externalize a 16-bit descriptor with its header.

A header of the descriptor contains the information of its width(8 or 16 bits) and length.

@param aStream The reference to the write stream that the descriptor is externalized to.
@param aDes8 The reference of the 8-bit descriptor to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const CDes16& aDes)
	{
	CDesHeader header(aDes);
	header.Externalize(aStream);

	// Compress the data using the Standard Unicode Compression Scheme.
	TMemoryUnicodeSource source(aDes.Ptr());
	TUnicodeCompressor compressor;
	compressor.CompressL(aStream,source,KMaxTInt,aDes.Length());
	return aStream;
	}

/**
Operator to externalize a TUid object.

@param aStream The reference to the write stream that the TUid object is externalized to.
@param aUid The reference to the TUid object to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUid& aUid)
	{
	return aStream << aUid.iUid;
	}

/**
Operator to externalize a TCheckedUid object.

@param aStream The reference to the write stream that the TCheckedUid object is 
externalized to.
@param aUid The reference to the TCheckedUid object to externalize.
@return The reference to the write stream.
@exception TStoreException::EFileWriteError Error occurs when writing data to the file.
*/
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TCheckedUid& aUid)
	{
	aStream.Write(aUid.Des());
	return aStream;
	}

}
