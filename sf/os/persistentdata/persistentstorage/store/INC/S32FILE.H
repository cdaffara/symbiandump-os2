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

#if !defined(__S32FILE_H__)
#define __S32FILE_H__
#if !defined(__F32FILE_H__)
#include <f32file.h>
#endif
#if !defined(__S32STOR_H__)
#include <s32stor.h>
#endif

#ifndef SYMBIAN_ENABLE_SPLIT_HEADERS 
	#include <s32filebufsize.h>
#endif

template <class T>
class TCapture;
class RPermanentFileStoreIter;

/** A stream buffer hosted by a file.
 * @publishedAll 
 * @released

Instances of this class are used by file based persistent stores, i.e. CFileStore 
type objects. An RFileBuf object is associated with a file and the file is 
said to be attached to the stream buffer.

An RFileBuf object is also used by RFileReadStream and RFileWriteStream objects 
to provide buffered file I/O.

The stream buffer has intermediate buffering capabilities.

When used as the basis for a file store, it hosts multiple streams through 
the TStreamExchange and RShareBuf classes.

Open(), Close(), Attach(), Detach(), File() and Reattach() usage patterns:
 
Pattern 1: RFileBuf owns the file
step 1a: Open()/Create()/Temp() is used to connect the buffer to a file
step 1b: Use the file buffer
step 1c: Close() releases this resource.

Pattern 2: RFile is opened elsewhere and ownership is handed over to RFileBuf
This may happen if the file is already opened by another API, or from another process for example
step 2a: Attach() is used to hand ownership of the opened file to the buffer. After Attach() the supplied file handle is NULLed.
step 2b: Use the file buffer
step 2c: Close() releases the file resource.

Pattern 3: RFileBuf is used transiently to manage an existing opened file:
step 3a: Attach() is used to bind the buffer to the already open file. After Attach() the supplied file handle is NULLed.
step 3b: Use the file buffer
step 3c: RFileBuf::File() is used to retrieve the file handle again, then Detach() is called to disconnect the buffer from the file.
At this point, the destruction of the file buffer will have no effect on the file. The retrieved file handle in step 3c must be used to close the file.

Pattern 4: Transient direct file access to a buffered file
step 4a: RFileBuf::File() is used to retrieve the file handle. [Optional: Detach() is used to disconnect the file buffer]
step 4b: Use the file directly. Note that writing to the file may cause coherency problems with the RFileBuf buffer - in which case you need to Reset() the buffer as well.
step 4c: [Optional: Reattach() is used to hand the file back to the buffer]. Use of the buffer is resumed

@see CFileStore
@see RFileReadStream
@see RFileWriteStream */
class RFileBuf : public TStreamBuf
	{
public:
	IMPORT_C RFileBuf();
	IMPORT_C RFileBuf(TInt aSize);
	RFileBuf(TCapture<RFileBuf> aCapture);
	IMPORT_C void Reset();
	inline void Reset(TInt aSize);
//
	IMPORT_C TInt Open(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt Create(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt Replace(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt Temp(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
	IMPORT_C void Attach(RFile& aFile,TInt aPos=0);
	IMPORT_C void Close();
	inline void Detach();
	inline void Reattach(RFile& aFile);
//
	IMPORT_C void SetSizeL(TInt aSize);
	inline RFile& File() const;
protected:
	IMPORT_C TInt UnderflowL(TInt aMaxLength);
	IMPORT_C void OverflowL();
	IMPORT_C void DoRelease();
	IMPORT_C void DoSynchL();
	IMPORT_C TInt DoReadL(TAny* aPtr,TInt aMaxLength);
	IMPORT_C TInt DoReadL(TDes8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	IMPORT_C void DoWriteL(const TAny* aPtr,TInt aLength);
	IMPORT_C TInt DoWriteL(const TDesC8& aDes,TInt aMaxLength,TRequestStatus& aStatus);
	IMPORT_C TStreamPos DoSeekL(TMark aMark,TStreamLocation aLocation,TInt anOffset);
//
	inline void SetBuf(TRead,TUint8* aPtr,TUint8* anEnd);
	inline void SetBuf(TWrite,TUint8* aPtr,TUint8* anEnd);
	inline void SetBuf(TArea anArea,TUint8* aPtr,TUint8* anEnd);
private:
	TUint8* AllocL();
	void Free();
//
	void SetPos(TMark aMark,TInt aPos);
	inline void SetPos(TRead,TInt aPos);
	inline void SetPos(TWrite,TInt aPos);
	inline TInt Pos(TRead) const;
	inline TInt Pos(TWrite) const;
	inline TInt MovePos(TRead,TInt anOffset);
	inline TInt MovePos(TWrite,TInt anOffset);
	TInt FileReadL(TAny* aPtr,TInt aMaxLength);
	void FileWriteL(const TAny* aPtr,TInt aLength,TInt aSeek);
	void FileWriteL(const TAny* aPtr,TInt aLength);
	TInt EndL();
//
	inline TInt Lag(TRead) const;
	inline TInt Lag(TWrite) const;
	TInt Mark(TMark aMark) const;
	inline TInt Mark(TRead) const;
	inline TInt Mark(TWrite) const;
	inline TUint8* Limit(TWrite) const;
	inline void SetLimit(TWrite,TUint8* aLimit);
	inline TInt Span(TWrite) const;
	inline TInt Reach(TWrite) const;
private:
	TUint8* iBase;
	TInt iSize;
	__MUTABLE RFile iFile;
	TInt iRPos;
	TInt iWPos;
	TInt iExt;
	TUint8* iWLim;
	friend class CFileStore;
	};

/**
 * @publishedAll 
 * @released
 * Supports the reading of a stream from a file.
 */
class RFileReadStream : public RReadStream
	{
public:
	/** Constructs an empty read stream object. */
	RFileReadStream() {}
	IMPORT_C RFileReadStream(RFile& aFile,TInt aPos=0);
	IMPORT_C TInt Open(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C void Attach(RFile& aFile,TInt aPos=0);
private:
	RFileBuf iSource;
	};

/**
 * @publishedAll 
 * @released
 * Supports the writing of a stream to a file.
 */
class RFileWriteStream : public RWriteStream
	{
public:
	/** Constructs an empty write stream object. */
	RFileWriteStream() {}
	inline RFileWriteStream(const MExternalizer<TStreamRef>& anExter);
	IMPORT_C RFileWriteStream(RFile& aFile,TInt aPos=0);
	IMPORT_C TInt Open(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt Create(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt Replace(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C TInt Temp(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
	IMPORT_C void Attach(RFile& aFile,TInt aPos=0);
private:
	RFileBuf iSink;
	};
//
class CFileStore;

/**
 * @publishedAll 
 * @released
 * A class containing a set of factory functions for opening an existing direct 
 file store and an existing permanent file store identified using a Uidtype. 
 */
class FileStoreFactory
	{
public:
	IMPORT_C static CFileStore* DirectLC(RFileBuf& aBuf,const TUidType& aType);
	IMPORT_C static CFileStore* PermanentLC(RFileBuf& aBuf,const TUidType& aType);
	};
//
const TInt KDirectFileStoreLayoutUidValue=268435511;
/** The value of the KPermanentFileStoreLayoutUidValue UID. */
const TInt KPermanentFileStoreLayoutUidValue=268435536;
/** The UID that identifies a file store as being a direct file store. */
const TUid KDirectFileStoreLayoutUid={KDirectFileStoreLayoutUidValue};
/** The UID that identifies a file store as being a permanent file store. */
const TUid KPermanentFileStoreLayoutUid={KPermanentFileStoreLayoutUidValue};
//
typedef CFileStore* (*TFileStoreFactoryFunction)(RFileBuf& aBuf,const TUidType& aType);
//
#define KDirectFileStoreFactoryFunction (&FileStoreFactory::DirectLC)
#define KPermanentFileStoreFactoryFunction (&FileStoreFactory::PermanentLC)

/**
 * @publishedAll 
 * @released
 * File based persistent store abstract base class.

The class encapsulates the basic behaviour of file based stores. File based 
stores are persistent stores, i.e. they have the ability to keep the external 
representation of objects for longer than the lifetime of the applications 
which created those objects.

The class forms the base for the direct file store, CDirectFileStore, and 
the permanent file store, CPermanentFileStore. In general, it is sufficient 
for pointers to file based store objects to be of type CFileStore, rather 
than of the concrete file store type.

Existing file stores can be opened using the member functions OpenL(), OpenLC(), 
FromL() and FromLC(). New file stores, however, must be created using the 
appropriate member function of the concrete type.  
*/
class CFileStore : public CPersistentStore
	{
public:
	IMPORT_C static CFileStore* OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C static CFileStore* OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	IMPORT_C static CFileStore* FromL(RFile& aFile);
	IMPORT_C static CFileStore* FromLC(RFile& aFile);
//
	IMPORT_C static CFileStore* OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode,const TFileStoreFactoryFunction aFactory[]);
	IMPORT_C static CFileStore* OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode,const TFileStoreFactoryFunction aFactory[]);
	IMPORT_C static CFileStore* FromL(RFile& aFile,const TFileStoreFactoryFunction aFactory[]);
	IMPORT_C static CFileStore* FromLC(RFile& aFile,const TFileStoreFactoryFunction aFactory[]);
//
	inline const TUidType& Type() const;
	IMPORT_C void SetTypeL(const TUidType& aType);
	/** Gets the UID that uniquely identifies the specific type of this file store.
	
	This function must be defined and implemented by classes derived from CFileStore. 
	The direct file store, CDirectFileStore and the permanent file store, CPermanentFileStore 
	both implement suitable functions.
	
	@return The UID that uniquely identifies the specific type of file store.
	@see KDirectFileStoreLayoutUid
	@see KPermanentFileStoreLayoutUid */
	virtual TUid Layout() const=0;
//
	inline void Reset();
	inline void Reset(TInt aSize);
	inline void Detach();
	inline void Reattach(RFile& aFile);
	inline RFile& File() const;
//
	IMPORT_C void MarshalL();
	IMPORT_C ~CFileStore();
protected:
	typedef CFileStore* (*TNewFunction)(RFile& aFile);
protected:
	IMPORT_C static CFileStore* OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode,TFileStoreFactoryFunction aFunction);
	IMPORT_C static CFileStore* OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode,TFileStoreFactoryFunction aFunction);
	IMPORT_C static CFileStore* CreateL(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction);
	IMPORT_C static CFileStore* CreateLC(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction);
	IMPORT_C static CFileStore* ReplaceL(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction);
	IMPORT_C static CFileStore* ReplaceLC(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction);
	IMPORT_C static CFileStore* TempL(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode,TNewFunction aFunction);
	IMPORT_C static CFileStore* TempLC(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode,TNewFunction aFunction);
//
	IMPORT_C static CFileStore* FromL(RFile& aFile,TFileStoreFactoryFunction aFunction);
	IMPORT_C static CFileStore* FromLC(RFile& aFile,TFileStoreFactoryFunction aFunction);
	IMPORT_C static CFileStore* NewL(RFile& aFile,TNewFunction aFunction);
	IMPORT_C static CFileStore* NewLC(RFile& aFile,TNewFunction aFunction);
//
	IMPORT_C CFileStore(RFile& aFile);
	IMPORT_C CFileStore(RFileBuf& aBuf,const TUidType& aType);
	IMPORT_C void Destruct();
//
	inline TStreamExchange& Host() const;
	inline TBool IsHost(const MStreamBuf* aBuf) const;
	IMPORT_C void SynchL();
	inline void SetSizeL(TInt aSize);
	IMPORT_C void ChangedL();
	IMPORT_C void RefreshL();
//
	IMPORT_C void DoCommitL();
	IMPORT_C void DoRevertL();
private:
	virtual void ExternalizeL(RWriteStream& aStream) const=0;
	virtual void InternalizeL(RReadStream& aStream)=0;
//
	static CFileStore* DoNewL(RFile& aFile,TNewFunction aFunction);
private:
	RFileBuf iBuf;
	TUidType iType;
	__MUTABLE TStreamExchange iHost;
	};
const TInt KFileStoreStartOffset=sizeof(TCheckedUid);
#if defined(__NO_CLASS_CONSTS__)
#define KFileStoreStart TStreamPos(KFileStoreStartOffset)
#else
const TStreamPos KFileStoreStart=TStreamPos(KFileStoreStartOffset);
#endif

/**
 * @publishedAll 
 * @released
 * Direct file store.

A direct file store implements a subset of the operations defined by the store 
abstract framework. Direct file stores allow streams to be created and objects 
externalised to them however once the streams have been committed and 
closed, they cannot subsequently be changed, i.e. streams cannot be replaced, 
deleted, extended or changed in any way. 
*/
class CDirectFileStore : public CFileStore
	{
public:
	inline static CDirectFileStore* OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CDirectFileStore* OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CDirectFileStore* CreateL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CDirectFileStore* CreateLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CDirectFileStore* ReplaceL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CDirectFileStore* ReplaceLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CDirectFileStore* TempL(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
	inline static CDirectFileStore* TempLC(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
//
	inline static CDirectFileStore* FromL(RFile& aFile);
	inline static CDirectFileStore* FromLC(RFile& aFile);
	inline static CDirectFileStore* NewL(RFile& aFile);
	inline static CDirectFileStore* NewLC(RFile& aFile);
//
	IMPORT_C TUid Layout() const;
//
	IMPORT_C CDirectFileStore(RFile& aFile);
	IMPORT_C CDirectFileStore(RFileBuf& aBuf,const TUidType& aType);
protected:
	IMPORT_C MStreamBuf* DoReadL(TStreamId anId) const;
	IMPORT_C MStreamBuf* DoCreateL(TStreamId& anId);
private:
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void DoSetRootL(TStreamId anId);
//
	IMPORT_C static CFileStore* DoNewL(RFile& aFile);
	};
//
class CPermanentStoreCoord;

/**
 * @publishedAll 
 * @released
 * Permanent file store. 

This type of store supports full manipulation of store contents. Existing 
streams within this type of store can be changed. 
 */
class CPermanentFileStore : public CFileStore
	{
	friend class RPermanentFileStoreIter;
public:
	inline static CPermanentFileStore* OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CPermanentFileStore* OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CPermanentFileStore* CreateL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CPermanentFileStore* CreateLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CPermanentFileStore* ReplaceL(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CPermanentFileStore* ReplaceLC(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline static CPermanentFileStore* TempL(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
	inline static CPermanentFileStore* TempLC(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
//
	inline static CPermanentFileStore* FromL(RFile& aFile);
	inline static CPermanentFileStore* FromLC(RFile& aFile);
	inline static CPermanentFileStore* NewL(RFile& aFile);
	inline static CPermanentFileStore* NewLC(RFile& aFile);
//
	IMPORT_C TUid Layout() const;
//
	IMPORT_C CPermanentFileStore(RFile& aFile);
	IMPORT_C CPermanentFileStore(RFileBuf& aBuf,const TUidType& aType);
	IMPORT_C void MarshalL();
	IMPORT_C ~CPermanentFileStore();
protected:
	IMPORT_C MStreamBuf* DoReadL(TStreamId anId) const;
	IMPORT_C MStreamBuf* DoCreateL(TStreamId& anId);
	IMPORT_C MStreamBuf* DoWriteL(TStreamId anId);
	IMPORT_C MStreamBuf* DoReplaceL(TStreamId anId);
private:
	IMPORT_C void ExternalizeL(RWriteStream& aStream) const;
	IMPORT_C void InternalizeL(RReadStream& aStream);
	IMPORT_C void DoSetRootL(TStreamId anId);
	IMPORT_C TStreamId DoExtendL();
	IMPORT_C void DoDeleteL(TStreamId anId);
	IMPORT_C void DoCommitL();
	IMPORT_C void DoRevertL();
	IMPORT_C MIncrementalCollector* DoReclaimL();
	IMPORT_C MIncrementalCollector* DoCompactL();
//
	inline CPermanentStoreCoord& Coord() const;
	CPermanentStoreCoord& CoordL() const;
	CPermanentStoreCoord& TrimL();
//
	IMPORT_C static CFileStore* DoNewL(RFile& aFile);
private:
	__MUTABLE CPermanentStoreCoord* iCoord;
	};

/**
 * @publishedAll 
 * @released
 * File based dictionary store.

A dictionary store is a store where a stream is accessed by UID, rather than 
directly by stream ID. A dictionary store contains streams in the usual way 
but, in addition, the root stream is a stream dictionary, i.e. a CStreamDictionary 
type.  
*/
class CDictionaryFileStore : public CDictionaryStore
	{
public:
	IMPORT_C static CDictionaryFileStore* SystemL(RFs& aFs);
	IMPORT_C static CDictionaryFileStore* SystemLC(RFs& aFs);
	IMPORT_C static CDictionaryFileStore* OpenL(RFs& aFs,const TDesC& aName,TUid aUid3);
	IMPORT_C static CDictionaryFileStore* OpenLC(RFs& aFs,const TDesC& aName,TUid aUid3);
private:
	void ConstructL(RFs& aFs,const TDesC& aName,TUid aUid3);
	void CreateStoreL(RFile& aFile,const TUidType& aType);
	};

/**
 * @publishedAll 
 * @released
 * Uses a file directly to implement the page pool interface MPagePool.

The pages are written sequentially through the file. You should call Close() 
to release the file resource after CreateL(), OpenL(), ReplaceL() or Temp(). 

A file page pool uses a cache to store pages in-memory and to cache frequently 
accessed pages. You should provide a cache object (CPageCache) to the pool 
for this purpose.

@see CPageCache
@see RFile  
*/
class RFilePagePool : public TCachePagePool
	{
public:
	IMPORT_C RFilePagePool();
	IMPORT_C RFilePagePool(CPageCache& aCache);
//
	inline TInt Open(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline TInt Create(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline TInt Replace(RFs& aFs,const TDesC& aName,TUint aFileMode);
	inline TInt Temp(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode);
	inline void Attach(RFile& aFile);
	IMPORT_C void Close();
	IMPORT_C void Release();
	IMPORT_C TInt Flush();
	IMPORT_C void FlushL();
	inline void Detach();
//
	inline RFile& File() const;
protected:
	IMPORT_C TPageRef ExtendL(const TAny* aPage,TPageReclamation aReclamation);
	IMPORT_C void WriteL(TPageRef aRef,const TAny* aPage,TPageChange aChange);
	IMPORT_C void ReadL(TPageRef aRef,TAny* aPage);
private:
	__MUTABLE RFile iFile;
	};

#include <s32file.inl>
#endif
