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


#if !defined(__STOREWRITESTREAM_H__)
#define __STOREWRITESTREAM_H__

#include <pcstore/pcstoredef.h>
#include <pcstore/streamid.h>

namespace PCStore
{
class CDes8;
class CDes16;
class CFileStreamBuf;
class TStreamId;
class TUid;

/**
@internalAll

A write stream class to externalize data to a Store file.

By eventually calling  CFileStreamBuf's write function, it implements the externalization of 
the following data types:
	TInt8 
	TUint8 
	TInt16 
	TUint16
	TInt32 
	TUint32
	TInt64
	TUint64
	TReal32
	TReal64
	TUint8*
	TUint16*
	CDes8
	CDes16

After instantiated, it must be opened on a write-moded CDirectFileStore object to externalize data.
It must be closed or destroyed before another stream is opened on the same CDirectFileStore object.

@see CDirectFileStore 
@see CStoreReadStream
*/
class CStoreWriteStream
	{
public:
	CStoreWriteStream(CFileStreamBuf& aStreamBuf, TStreamId aId);
	~CStoreWriteStream();
	void Write(const TUint8* aPtr, TInt aLength);
	void Write(const TUint16* aPtr, TInt aLength);
	void WriteInt8(TInt8 aValue);
	void WriteUint8(TUint8 aValue);
	void WriteInt16(TInt16 aValue);
	void WriteUint16(TUint16 aValue);
	void WriteInt32(TInt32 aValue);
	void WriteUint32(TUint32 aValue);
	void WriteInt64(TInt64 aValue);
	void WriteUint64(TUint64 aValue);
	void WriteReal32(TReal32 aValue);
	void WriteReal64(TReal64 aValue);
	void Write(const CDes8& aDes);
	void Write(const CDes8& aDes, TInt aLength);
	void Write(const CDes16& aDes);
	void Write(const CDes16& aDes, TInt aLength);

private:
	CStoreWriteStream(const CStoreWriteStream& aStream);
	CStoreWriteStream& operator=(const CStoreWriteStream& aStream);

private:
	CFileStreamBuf& iSnk;
	};

// Operators on the write stream and data types
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TStreamId& aId);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt8 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint8 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt16 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint16 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt32 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint32 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TInt64 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUint64 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TReal32 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TReal64 aVal);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const CDes8& aDes);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const CDes16& aDes);
CStoreWriteStream& operator<<(CStoreWriteStream& aStream,const TUid& aUid);
}
#endif // !defined(__STOREWRITESTREAM_H__)
