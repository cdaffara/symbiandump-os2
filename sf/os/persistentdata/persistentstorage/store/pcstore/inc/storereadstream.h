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

#if !defined(__STOREREADSTREAM_H__)
#define __STOREREADSTREAM_H__

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

A read stream class to internalize data from a Store file.

By eventually calling  CFileStreamBuf's read function, it implements the internalization of 
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

After instantiated, it must be opened on a read-moded CDirectFileStore object to internalize data.
It must be closed or destroyed before another stream is opened on the same CDirectFileStore object.

@see CDirectFileStore 
@see CStoreWriteStream
*/
class CStoreReadStream
	{
public:
	CStoreReadStream(CFileStreamBuf& aStreamBuf, TStreamId aId);
	~CStoreReadStream();
	void Read(TUint8* aPtr, TInt aLength);
	void Read(TUint16* aPtr, TInt aLength);
	TInt8 ReadInt8();
	TUint8 ReadUint8();
	TInt16 ReadInt16();
	TUint16 ReadUint16();
	TInt32 ReadInt32();
	TUint32 ReadUint32();
	TInt64 ReadInt64();
	TUint64 ReadUint64();
	TReal32 ReadReal32();
	TReal64 ReadReal64();
	void Read(CDes8& aDes);
	void Read(CDes8& aDes, TInt aLength);
	void Read(CDes16& aDes);
	void Read(CDes16& aDes, TInt aLength);

private:
	CStoreReadStream(const CStoreReadStream& aStream);
	CStoreReadStream& operator=(const CStoreReadStream& aStream);

private:
	CFileStreamBuf& iSrc;
	};


// Operators on the read stream and data types
CStoreReadStream& operator>>(CStoreReadStream& aStream, TStreamId& aId);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt8& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint8& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt16& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint16& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt32& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint32& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TInt64& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUint64& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TReal32& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TReal64& aVal);
CStoreReadStream& operator>>(CStoreReadStream& aStream, CDes8& aDes);
CStoreReadStream& operator>>(CStoreReadStream& aStream, CDes16& aDes);
CStoreReadStream& operator>>(CStoreReadStream& aStream, TUid& aUid);
}
#endif // !defined(__STOREREADSTREAM_H__)
