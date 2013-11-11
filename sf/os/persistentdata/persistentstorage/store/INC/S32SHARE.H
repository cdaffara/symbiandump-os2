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

#ifndef __S32SHARE_H__
#define __S32SHARE_H__

#include <s32buf.h>
#include <s32strm.h>

class TStreamExchange;
/**
 * @publishedAll 
 * @released
 * Manages the position of a read mark or a write mark within a shared stream.

A shared stream is one that shares its host with other streams. In practice, 
the host is a stream buffer that is, itself, hosted by a file, an RFileBuf 
object. 

TStreamMark objects are owned by RShareBuf, one for reading and one for writing. 
They re-direct read/write operations through a TStreamExchange object to the 
RFileBuf object, which handles the actual file read and write operations.

@see RFileBuf
@see RShareBuf
@see TStreamExchange  
*/
class TStreamMark
	{
public:
	inline TStreamMark();
	inline TStreamMark(TStreamPos aPos);
	inline TStreamMark& operator=(TStreamPos aPos);
	inline operator TStreamMark*();
	inline operator const TStreamMark*() const;
//
	inline TBool operator==(const TStreamMark& aMark) const;
	inline TBool operator==(const TStreamMark* aPtr) const;
	inline TBool operator!=(const TStreamMark& aMark) const;
	inline TBool operator!=(const TStreamMark* aPtr) const;
//
	inline TBool IsEmpty() const;
	inline void Clear();
	inline TStreamPos Position() const;
//
	inline TBool IsWith(TStreamExchange& aHost) const;
	inline TBool RelatesTo(TStreamExchange& aHost) const;
	inline void Withdraw(TStreamExchange& aHost);
	inline void ExtractL(TStreamExchange& aHost);
//
	inline TInt ReadL(TStreamExchange& aHost,TAny* aPtr,TInt aMaxLength);
	IMPORT_C TInt ReadL(TStreamExchange& aHost,TDes8& aDes,TRequestStatus& aStatus);
	inline TInt ReadL(TStreamExchange& aHost,TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	inline TStreamTransfer ReadL(TStreamExchange& aHost,MStreamInput& anInput,TStreamTransfer aTransfer);
	inline TInt ReadL(TStreamExchange& aHost,MStreamInput& anInput,TInt aMaxLength);
	inline void ReadL(TStreamExchange& aHost,MStreamInput& anInput);
//
	inline void WriteL(TStreamExchange& aHost,const TAny* aPtr,TInt aLength);
	IMPORT_C TInt WriteL(TStreamExchange& aHost,const TDesC8& aDes,TRequestStatus& aStatus);
	inline TInt WriteL(TStreamExchange& aHost,const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	inline TStreamTransfer WriteL(TStreamExchange& aHost,MStreamOutput& anOutput,TStreamTransfer aTransfer);
	inline TInt WriteL(TStreamExchange& aHost,MStreamOutput& anOutput,TInt aMaxLength);
	inline void WriteL(TStreamExchange& aHost,MStreamOutput& anOutput);
//
	inline void SeekL(TStreamExchange& aHost,TStreamPos aPos);
	inline TStreamPos SeekL(TStreamExchange& aHost,TStreamLocation aLocation,TInt anOffset=0);
	inline TStreamPos SeekL(TStreamExchange& aHost,TInt anOffset);
//
	inline TStreamPos TellL(TStreamExchange& aHost) const;
private:
	/** Constructs a default shared stream mark object.
	
	The position for the mark is uninitialised.
	
	An uninitialised mark means that a call to IsEmpty() returns true.
	
	@see IsEmpty() */
	TStreamMark(const TStreamMark&);
	TStreamMark& operator=(const TStreamMark&);
private:
	TStreamPos iPos;
private:
	inline TBool IsTracking(TStreamMark*const& aRef) const;
	inline void Track(TStreamMark*const& aRef);
private:
	friend class TStreamExchange;
private:
	IMPORT_C static void __DbgChkPos(TStreamPos aPos);
	};

/**
 * @publishedAll 
 * @released
 * Manages access to a shared host buffer for shared stream buffers.

The class maintains independent markers for the shared stream buffer that 
has most recently read or written to the shared host buffer.

This is the way in which multiple requests from the same shared stream buffer 
are handled without the need for repeated seeking. The shared host buffer 
only needs to seek when a new shared buffer needs to use it

@see RShareBuf  
*/
class TStreamExchange
	{
public:
	inline TStreamExchange();
	inline TStreamExchange(MStreamBuf* aHost);
	inline void Share(MStreamBuf* aHost);
	inline TBool IsActive() const;
	IMPORT_C MStreamBuf* Host();
	IMPORT_C MStreamBuf* HostL();
	IMPORT_C void Release();
//
	IMPORT_C TInt SizeL() const;
private:
/** Used to identify the type of mark in a stream.

The type is used by functions of this class and derived classes that perform 
seek operations to marks within a stream.

The type uses the ERead and EWrite enumeration values, as bit flags, to identify 
the read and write marks respectively.

ERead is an MStreamBuf::TRead enumerator. EWrite is an MStreamBuf::EWrite 
enumerator.

@see MStreamBuf::TRead
@see MStreamBuf::TWrite */
	typedef MStreamBuf::TMark TMark;
private:
	/** Constructs an empty object.
	
	Call Share() to prepare for access to a shared stream buffer. */
	TStreamExchange(const TStreamExchange&);
	TStreamExchange& operator=(const TStreamExchange&);
//
	IMPORT_C TBool RefersTo(const TStreamMark& aMark);
	IMPORT_C void Drop(const TStreamMark& aMark);
	IMPORT_C void GetL(TStreamMark& aMark);
	IMPORT_C TInt DoReadL(TAny* aPtr,TInt aMaxLength,TStreamMark& aMark);
	IMPORT_C TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark);
	IMPORT_C TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer,TStreamMark& aMark);
	IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength,TStreamMark& aMark);
	IMPORT_C TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark);
	IMPORT_C TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer,TStreamMark& aMark);
	IMPORT_C TStreamPos DoSeekL(TStreamMark& aMark,TStreamLocation aLocation,TInt anOffset);
//
	void PrepareForReadingL(TStreamMark& aMark);
	void PrepareForWritingL(TStreamMark& aMark);
//
	TInt PrepareAndReadL(TAny* aPtr,TInt aMaxLength,TStreamMark& aMark);
	TInt PrepareAndReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark);
	TStreamTransfer PrepareAndReadL(MStreamInput& anInput,TStreamTransfer aTransfer,TStreamMark& aMark);
	void PrepareAndWriteL(const TAny* aPtr,TInt aLength,TStreamMark& aMark);
	TInt PrepareAndWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus,TStreamMark& aMark);
	TStreamTransfer PrepareAndWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer,TStreamMark& aMark);
	TStreamPos MarkSeekL(TStreamMark& aMark,TStreamLocation aLocation,TInt anOffset);
//
	inline MStreamBuf& BufL() const;
	inline MStreamBuf& Buf() const;
private:
	MStreamBuf* iHost;
	TStreamMark* iRMrk;
	TStreamMark* iWMrk;
private:
	friend class TStreamMark;
	};

/**
 * @publishedAll 
 * @released
 * A shared stream buffer.

This class is part of the framework used by CFileStore that allows multiple 
stream buffers to concurrently access the single hosting file buffer, an RFileBuf 
object.

Each shared stream buffer maintains its own read and write positions.

Read and write operations to RShareBuf are directed through separate TStreamMark 
objects, one for read and one for write. The TStreamMark objects re-direct 
read/write operations through a TStreamExchange object to the RFileBuf object, 
which handles the actual file read and write operations. 

This stream buffer can also be accessed through RShareReadStream and RShareWriteStream 
objects.

@see TStreamMark
@see TStreamExchange
@see RFileBuf
@see RShareReadStream
@see RShareWriteStream  
*/
class RShareBuf : public MStreamBuf
	{
public:
	IMPORT_C RShareBuf();
	IMPORT_C void Open(TStreamExchange& aHost,TStreamPos aPos,TInt aMode=ERead|EWrite);
	inline void Open(TStreamExchange& aHost,TInt aMode=ERead|EWrite);
protected:
	IMPORT_C void DoRelease();
	IMPORT_C TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	IMPORT_C TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	IMPORT_C TStreamTransfer DoReadL(MStreamInput& anInput,TStreamTransfer aTransfer);
	IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
	IMPORT_C TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	IMPORT_C TStreamTransfer DoWriteL(MStreamOutput& anOutput,TStreamTransfer aTransfer);
	IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
private:
	inline TStreamExchange& Host() const;
private:
	TStreamExchange* iHost;
	TStreamMark iRMark;
	TStreamMark iWMark;
	};

/**
 * @publishedAll 
 * @released
 * Supports the opening, for reading, of a stream which shares its host with other 
streams.

This is also called a shared read stream.

The class uses a RShareBuf stream buffer.

@see RShareBuf
@see RFileBuf  
*/
class RShareReadStream : public RReadStream
	{
public:
	/** Constructs an empty shared read stream object.
	
	Call Open() to prepare the shared stream for reading.
	
	@see RShareReadStream::Open() */
	RShareReadStream() {}
	IMPORT_C RShareReadStream(TStreamExchange& aHost,TStreamPos aPos=KStreamBeginning);
	IMPORT_C void Open(TStreamExchange& aHost,TStreamPos aPos=KStreamBeginning);
private:
	RShareBuf iSource;
	};

/**
 * @publishedAll 
 * @released
 * Supports the opening, for writing, of a stream which shares its host with other 
streams.

This is also called a shared write stream.

The class uses a RShareBuf stream buffer.

@see RShareBuf
@see RFileBuf  
*/
class RShareWriteStream : public RWriteStream
	{
public:
	/** Constructs an empty shared write stream object.
	
	Call Open() to prepare the shared stream for writing.
	
	@see RShareWriteStream::Open() */
	RShareWriteStream() {}
	inline RShareWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RShareWriteStream(TStreamExchange& aHost,TStreamPos aPos=KStreamBeginning);
	IMPORT_C void Open(TStreamExchange& aHost,TStreamPos aPos=KStreamBeginning);
private:
	RShareBuf iSink;
	};

#include <s32share.inl>
#endif
