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

#if !defined(__S32MEM_H__)
#define __S32MEM_H__
#if !defined(__S32BUF_H__)
#include <s32buf.h>
#endif
#if !defined(__S32STOR_H__)
#include <s32stor.h>
#endif

/**
 * @publishedAll
 * @released
 * A stream buffer that uses plain memory for its implementation.

A stream of this type is used by RMemWriteStream and RMemReadStream objects.
It also has intermediate buffering capabilities.

This is a seekable stream buffer.

@see RMemWriteStream
@see RMemReadStream
*/
class TMemBuf : public TStreamBuf
	{
public:
	IMPORT_C TMemBuf();
	IMPORT_C void Set(TUint8* aPtr,TUint8* anEnd,TInt aMode=ERead|EWrite);
protected:
	IMPORT_C TInt UnderflowL(TInt aMaxLength);
	IMPORT_C void OverflowL();
	IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	inline TUint8* Base() const;
	inline TUint8* End() const;
private:
	TUint8* iBase;
	};

/**
 * @publishedAll
 * @released
 * A stream buffer that uses a descriptor for its implementation.

A stream of this type is used by RDesWriteStream and RDesReadStream objects.
It also has intermediate buffering capabilities.

This is a seekable stream buffer.

When used in write mode, the length of the descriptor is only updated when
the stream buffer's SynchL() function is called, i.e. as a result of a call
to RWriteStream::CommitL().

@see RDesWriteStream
@see RDesReadStream
@see RWriteStream::CommitL()
@see MStreamBuf::SynchL()
*/
class TDesBuf : public TStreamBuf
	{
public:
	IMPORT_C TDesBuf();
	IMPORT_C void Set(TDes8& aDes,TInt aMode=ERead|EWrite);
protected:
	IMPORT_C TInt UnderflowL(TInt aMaxLength);
	IMPORT_C void OverflowL();
	IMPORT_C void DoSynchL();
	IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	inline TDes8& Des() const;
	inline TUint8* Base() const;
 	void Consolidate();
private:
	TDes8* iDes;
	};

/**
 * @publishedAll
 * @released
 * A stream buffer that uses a dynamic buffer for its implementation.

A stream of this type is used by RBufWriteStream and RBufReadStream objects.
It also has intermediate buffering capabilities.

This is a seekable stream buffer.

There are three write modes:

insert mode - inserts new data into the buffer at the offset passed to Set()

overwrite mode - replaces the data in the buffer starting at the offset passed
to Set(). Once the end of the buffer is reached, it is automatically extended
as more data is written. This is the default mode.

truncate mode - truncates the buffer to the offset passed to Set() before
data is written, extending the buffer. When writing, the buffer size as reported
by CBufBase::Size() may be larger than the data written to the stream. To
synchronise the buffer's reported size with the stream, call the MStreamBuf::SynchL()
function.

Note that this object never takes ownership of the dynamic buffer, the CBufBase
type object.

@see RBufWriteStream
@see RBufReadStream
@see CBufBase::Size()
@see MStreamBuf::SynchL()
*/
class TBufBuf : public TStreamBuf
	{
public:
	enum {ETruncate=0x10,EInsert=0x20};
public:
	IMPORT_C TBufBuf();
	IMPORT_C void Set(CBufBase& aBuf,TInt aPos,TInt aMode=ERead|EWrite);
protected:
	IMPORT_C TInt UnderflowL(TInt aMaxLength);
	IMPORT_C void OverflowL();
	IMPORT_C void DoSynchL();
	IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
	IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	inline CBufBase& Buf() const;
 	void Consolidate();
//
	void SetPos(TMark aMark,TInt aPos);
	inline void SetPos(TRead,TInt aPos);
	inline void SetPos(TWrite,TInt aPos);
	TInt Pos(TMark aMark) const;
	inline TInt Pos(TRead) const;
	inline TInt Pos(TWrite) const;
	inline TInt MovePos(TRead,TInt anOffset);
	inline TInt MovePos(TWrite,TInt anOffset);
	inline TInt Mark(TRead) const;
	inline TInt Mark(TWrite) const;
private:
	CBufBase* iBuf;
	TInt iRPos;
	TInt iWPos;
	TInt iMode;
	};

/**
@publishedAll
@released

Supports the reading of a stream from a pointer of any type.

@see TMemBuf
@see RReadStream
 */
class RMemReadStream : public RReadStream
	{
public:
/**
Constructs an empty object.

Call Open() to prepare the stream for reading.
*/
	RMemReadStream() {}
	IMPORT_C RMemReadStream(const TAny* aPtr,TInt aLength);
	IMPORT_C void Open(const TAny* aPtr,TInt aLength);
private:
	TMemBuf iSource;
	};

/**
@publishedAll
@released

Supports the writing of a stream to a pointer of any type.

@see TMemBuf
@see RWriteStream
 */
class RMemWriteStream : public RWriteStream
	{
public:
/**
Constructs an empty write stream object.

Call Open() to prepare a stream for writing.
*/
	RMemWriteStream() {}
	inline RMemWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RMemWriteStream(TAny* aPtr,TInt aMaxLength);
	IMPORT_C void Open(TAny* aPtr,TInt aMaxLength);
private:
	TMemBuf iSink;
	};

/**
@publishedAll
@released

Supports the reading of a stream from an 8-bit descriptor.

@see TMemBuf
@see RReadStream
*/
class RDesReadStream : public RReadStream
	{
public:
/**
Constructs an empty read stream object.

Call Open() to prepare the stream for reading.
*/
	RDesReadStream() {}
	IMPORT_C RDesReadStream(const TDesC8& aDes);
	IMPORT_C void Open(const TDesC8& aDes);
private:
	TMemBuf iSource;
	};

/**
@publishedAll
@released

Supports the writing of a stream to a stream buffer hosted by an 8-bit descriptor.

@see TDesBuf
@see RWriteStream
 */
class RDesWriteStream : public RWriteStream
	{
public:
/**
Constructs an empty write stream object.

Call Open() to prepare a stream for writing.
*/
	RDesWriteStream() {}
	inline RDesWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RDesWriteStream(TDes8& aDes);
	IMPORT_C void Open(TDes8& aDes);
private:
	TDesBuf iSink;
	};

/**
@publishedAll
@released

Supports the opening of an existing stream hosted by a dynamic buffer.
The stream does not take ownership of the dynamic buffer, which means
that the creator is responsible for deleting the buffer when it is no
longer needed.

@see TBufBuf
@see RReadStream
*/
class RBufReadStream : public RReadStream
	{
public:
/**
Constructs an empty read stream object.

Call Open() to prepare the stream for reading.
*/
	RBufReadStream() {}
	IMPORT_C RBufReadStream(const CBufBase& aBuf,TInt aPos=0);
	IMPORT_C void Open(const CBufBase& aBuf,TInt aPos=0);
private:
	TBufBuf iSource;
	};

/**
@publishedAll
@released

Supports the writing of a stream to a dynamic buffer.  The stream does
not take ownership of the dynamic buffer, which means that the creator
is responsible for deleting the buffer when it is no longer needed.

@see TBufBuf
@see RWriteStream
 */
class RBufWriteStream : public RWriteStream
	{
public:
/**
Default constructor.  Constructs an empty write stream object.

Call Open(), Truncate() or Insert() to prepare a stream for writing.
*/
	RBufWriteStream() {}

	inline RBufWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RBufWriteStream(CBufBase& aBuf,TInt aPos=0);
	IMPORT_C void Open(CBufBase& aBuf,TInt aPos=0);
	IMPORT_C void Truncate(CBufBase& aBuf,TInt aPos=0);
	IMPORT_C void Insert(CBufBase& aBuf,TInt aPos);
	inline void Append(CBufBase& aBuf);
private:
	TBufBuf iSink;
	};

/**
 * @publishedAll
 * @released
 * In-memory non-persistent store. The buffer store does not have a root stream
and cannot be closed without losing all the data.

It implements many of the operations defined by the store abstract framework.
Specifically, streams in this store can be: overwritten, replaced, appended,
deleted, and created in advance of being written to. However the class does
not support commit and revert operations.

Overwriting an existing stream can result in a shorter stream; however, a
stream cannot be extended beyond its original length. Replacing a stream can
result in a stream which is longer or shorter than the original. The order
in which streams are written to a memory store is not important as streams
can be changed and rewritten.
*/
class CBufStore : public CStreamStore
	{
public:
	IMPORT_C static CBufStore* NewL(TInt anExpandSize);
	IMPORT_C static CBufStore* NewLC(TInt anExpandSize);
	IMPORT_C CBufStore(TInt anExpandSize);
	IMPORT_C ~CBufStore();
protected:
	IMPORT_C TStreamId DoExtendL();
	IMPORT_C void DoDeleteL(TStreamId anId);
	IMPORT_C MStreamBuf* DoReadL(TStreamId anId) const;
	IMPORT_C MStreamBuf* DoCreateL(TStreamId& anId);
	IMPORT_C MStreamBuf* DoWriteL(TStreamId anId);
	IMPORT_C MStreamBuf* DoReplaceL(TStreamId anId);
private:
	CBufSeg& BufL(TStreamId anId) const;
private:
	CArrayFixFlat<CBufSeg*> iBufArray;
	TInt iExpandSize;
	};

#include <s32mem.inl>
#endif
