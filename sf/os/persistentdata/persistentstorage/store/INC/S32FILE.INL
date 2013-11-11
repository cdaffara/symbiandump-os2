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

// Class RFileBuf
inline void RFileBuf::Reset(TInt aSize)
/** Frees the intermediate buffer and changes the size of any future intermediate 
buffer to the specified value.

The intermediate buffer must not contain any outstanding write data, otherwise 
the function raises a STORE-File 6 panic.

@param aSize The size of any future intermediate buffer. */
	{Reset();iSize=aSize;}
inline void RFileBuf::Detach()
/** Detaches the file from this stream buffer.

The intermediate buffer's read and write marks are not changed, and the stream 
positions are not changed. This means that the contents of the file should 
not change while it is detached. 

@see Attach()
@see Reattach() */
	{iFile=RFile();}
inline void RFileBuf::Reattach(RFile& aFile)
/** Re-attaches the specified file to this stream buffer.

The intermediate buffer's read and write marks are not changed, and the stream 
positions are not changed.

The file should be the one that was detached using the Detach() function.

@param aFile The file to be re-attached.
@see Attach()
@see Detach() */
	{iFile=aFile;}
inline RFile& RFileBuf::File() const
/** Gets a reference to the file attached to this stream buffer.

@return The file attached to this stream buffer. */
	{return MUTABLE_CAST(RFile&,iFile);}

// Class RFileWriteStream
inline RFileWriteStream::RFileWriteStream(const MExternalizer<TStreamRef>& anExter)
	: RWriteStream(anExter)
	{}

// Class CFileStore
inline const TUidType& CFileStore::Type() const
/** Gets the UID type of the file store.

@return The UID type object containing the file store type. 
@see TUid */
	{return iType;}
inline void CFileStore::Reset()
/** Frees the file store’s buffer space.

The buffer space is automatically re-allocated when needed.

This function should only be used immediately after a successful call to CommitL() 
or RevertL().

@see CStreamStore::CommitL()
@see CStreamStore::RevertL() */
	{iBuf.Reset();}
inline void CFileStore::Reset(TInt aSize)
/** Frees the file store’s buffer space and changes the size of future buffer 
space allocations.

The buffer space is automatically re-allocated when needed, using the new 
size value.

This function should only be used immediately after a successful call to CommitL() 
or RevertL().

@param aSize The size of future buffer space allocations. 
@see CStreamStore::CommitL()
@see CStreamStore::RevertL() */
	{iBuf.Reset(aSize);}
inline void CFileStore::Detach()
/** Detaches the file store from its associated file. 

In effect, the file store gives up ownership of the file.

Detaching is useful in cases where a file needs to be closed and later re-opened; 
for example, to give up a write lock for backup purposes.

It is very important that the contents of the file should not change while 
it is detached.

@see File()
@see Reattach() */
	{iBuf.Detach();}
inline void CFileStore::Reattach(RFile& aFile)
/** Reattaches a file to the file store. The file should be the one that was detached 
using the Detach() function.

@param aFile The file to be associated with this file store. 
@see File() */
	{iBuf.Reattach(aFile);}
inline RFile& CFileStore::File() const
/** Gets a reference to the file associated with this file store.

This function is called prior to detaching the file store.

@return A reference to the associated file. 
@see Detach() */
	{return iBuf.File();}
inline TStreamExchange& CFileStore::Host() const
	{return MUTABLE_CAST(TStreamExchange&,iHost);}
inline TBool CFileStore::IsHost(const MStreamBuf* aBuf) const
	{return aBuf==&iBuf;}
inline void CFileStore::SetSizeL(TInt aSize)
	{iBuf.SetSizeL(aSize);}

// Class CDirectFileStore
inline CDirectFileStore* CDirectFileStore::OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a direct file store, and constructs a direct file store 
object.

@param aFs Handle to a file server session. 
@param aName The full path name of the file containing the store. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new direct file store object.
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::OpenL(aFs,aName,aFileMode,KDirectFileStoreFactoryFunction));}
inline CDirectFileStore* CDirectFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a direct file store, constructs a direct file store 
object, and places the pointer onto the cleanup stack.

@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new direct file store object. 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::OpenLC(aFs,aName,aFileMode,KDirectFileStoreFactoryFunction));}
inline CDirectFileStore* CDirectFileStore::CreateL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file and constructs a new direct file store object to be associated 
with this file.

@param aFs Handle to a file server session.
@param aName The full path name of the new file. A file with this name must 
not already exist, otherwise the function leaves.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type. 
@return A pointer to the new direct file store object 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::CreateL(aFs,aName,aFileMode,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::CreateLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file and constructs a new direct file store object to be associated 
with this file, and places the pointer onto the cleanup stack.

@param aFs Handle to a file server session. 
@param aName The full path name of the new file. A file with this name must 
not already exist, otherwise the function leaves.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type. 
@return A pointer to the new direct file store object 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::CreateLC(aFs,aName,aFileMode,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::ReplaceL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a file and constructs a direct file store object to be associated with 
it.

The file replaces any existing file of the same name.

@param aFs Handle to a file server session. 
@param aName The full path name of the file to be replaced. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new direct file store object. 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::ReplaceL(aFs,aName,aFileMode,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::ReplaceLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a file, constructs a direct file store object to be associated with 
it, and places the pointer onto the cleanup stack.

The file replaces any existing file of the same name.

@param aFs Handle to a file server session. 
@param aName The full path name of the file to be replaced.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new direct file store object. 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::ReplaceLC(aFs,aName,aFileMode,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::TempL(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates a temporary file and constructs a direct file store object to be associated 
with it.

The new file is created in the specified path and a unique file name is generated 
by the file server.

Note that the store framework does not delete a temporary file after it is 
closed.

@param aFs Handle to a file server session.
@param aPath The path where the new file is to be created. 
@param aName On return, contains the full path name of the new file.
@param aFileMode The mode in which the new file is to be accessed. The mode 
is defined by the TFileMode type.
@return A pointer to the new direct file store object. 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::TempL(aFs,aPath,aName,aFileMode,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::TempLC(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates a temporary file, constructs a direct file store object to be associated 
with it, and places the pointer onto the cleanup stack.

The new file is created in the specified path and a unique file name is generated 
by the file server.

Note that the store framework does not delete a temporary file after it is 
closed.

@param aFs Handle to a file server session.
@param aPath The path where the new file is to be created. 
@param aName On return, contains the full path name of the new file.
@param aFileMode The mode in which the new file is to be accessed. The mode 
is defined by the TFileMode type.
@return A pointer to the new direct file store object. 
@see TFileMode */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::TempLC(aFs,aPath,aName,aFileMode,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::FromL(RFile& aFile)
/** Constructs a direct file store object from an already opened file.

The file must already be open before calling this function.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid.

@param aFile A reference to the opened file.
@return A pointer to the new direct file store object. */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::FromL(aFile,KDirectFileStoreFactoryFunction));}
inline CDirectFileStore* CDirectFileStore::FromLC(RFile& aFile)
/** Constructs a direct file store object from an already opened file, and places 
the pointer onto the cleanup stack.

The file must already be open before calling this function.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid.

@param aFile A reference to the opened file. 
@return A pointer to the new direct file store object. */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::FromLC(aFile,KDirectFileStoreFactoryFunction));}
inline CDirectFileStore* CDirectFileStore::NewL(RFile& aFile)
/** Constructs a new direct file store object in an already opened file.

The file must already be open before calling the function. The existing content 
of the file is discarded.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid:

@param aFile A reference to the opened file. 
@return A pointer to the new direct file store object. */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::NewL(aFile,&DoNewL));}
inline CDirectFileStore* CDirectFileStore::NewLC(RFile& aFile)
/** Constructs a new direct file store object in an already opened file and places 
the pointer onto the cleanup stack.

The file must already be open before calling the function. The existing content 
of the file is discarded.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid:

@param aFile A reference to the opened file. 
@return A pointer to the new direct file store object. */
	{return STATIC_CAST(CDirectFileStore*,CFileStore::NewLC(aFile,&DoNewL));}

// Class CPermanentFileStore
inline CPermanentFileStore* CPermanentFileStore::OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a permanent file store, and constructs a permanent 
file store object.

@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type. 
@return A pointer to the new permanent file store object.
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::OpenL(aFs,aName,aFileMode,KPermanentFileStoreFactoryFunction));
	}
inline CPermanentFileStore* CPermanentFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a permanent file store, constructs a permanent file 
store object, and places the pointer onto the cleanup stack.

@param aFs Handle to a file server session. 
@param aName The full path name of the file containing the store. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type. 
@return A pointer to the new permanent file store object.
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::OpenLC(aFs,aName,aFileMode,KPermanentFileStoreFactoryFunction));
	}
inline CPermanentFileStore* CPermanentFileStore::CreateL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file and constructs a new permanent file store object to be associated 
with this file.

@param aFs Handle to a file server session.
@param aName The full path name of the new file. A file with this name must 
not already exist, otherwise the function leaves.
@param aFileMode The mode in which the new file is to be accessed. This mode 
is defined by the TFileMode type. 
@return A pointer to the new permanent file store object.
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::CreateL(aFs,aName,aFileMode,&DoNewL));
	}
inline CPermanentFileStore* CPermanentFileStore::CreateLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file and constructs a new permanent file store object to be associated 
with this file, and places the pointer onto the cleanup stack.

@param aFs Handle to a file server session. 
@param aName The full path name of the new file. A file with this name must 
not already exist, otherwise the function leaves.
@param aFileMode The mode in which the new file is to be accessed. This mode 
is defined by the TFileMode type. 
@return A pointer to the new permanent file store object. 
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::CreateLC(aFs,aName,aFileMode,&DoNewL));
	}
inline CPermanentFileStore* CPermanentFileStore::ReplaceL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a file, constructs a permanent file store object to be associated with 
it.

This file replaces any existing file of the same name.

@param aFs Handle to a file server session. 
@param aName The full path name of the file to be replaced.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new permanent file store object.
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::ReplaceL(aFs,aName,aFileMode,&DoNewL));
	}
inline CPermanentFileStore* CPermanentFileStore::ReplaceLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a file, constructs a permanent file store object to be associated with 
it, and places the pointer onto the cleanup stack.

This file replaces any existing file of the same name.

@param aFs Handle to a file server session. 
@param aName The full path name of the file to be replaced.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new permanent file store object. 
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
    
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::ReplaceLC(aFs,aName,aFileMode,&DoNewL));
	}
inline CPermanentFileStore* CPermanentFileStore::TempL(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates a temporary file and constructs a permanent file store object to be 
associated with it.

The new file is created in the specified path and a unique file name is generated 
by the file server.

Note that the store framework does not delete a temporary file after it is 
closed.

@param aFs Handle to a file server session.
@param aPath The path where the new file is to be created.
@param aName On return, contains the full path name of the new file.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new permanent file store object.
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::TempL(aFs,aPath,aName,aFileMode,&DoNewL));
	}
inline CPermanentFileStore* CPermanentFileStore::TempLC(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates a temporary file, constructs a permanent file store object to be associated 
with it, and places the pointer onto the cleanup stack.

The new file is created in the specified path and a unique file name is generated 
by the file server.

Note that the store framework does not delete a temporary file after it is 
closed.

@param aFs Handle to a file server session. 
@param aPath The path where the new file is to be created.
@param aName On return, contains the full path name of the new file.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type. 
@return A pointer to the new permanent file store object.
@see TFileMode */
	{
//  When the file server write caching is enabled, the order of file write operations is not guaranteed. This could cause data inconsistency in some circumstances,
//  for example, when the power is lost in the middle of a database transaction. Therefore, the file write caching is disabled for this file to maintain integrity.
	
    if ((aFileMode&EFileWrite) != 0)
        {
        aFileMode |= EFileWriteDirectIO;
        aFileMode &= (~EFileWriteBuffered);
        }
    return STATIC_CAST(CPermanentFileStore*,CFileStore::TempLC(aFs,aPath,aName,aFileMode,&DoNewL));
	}
inline CPermanentFileStore* CPermanentFileStore::FromL(RFile& aFile)
/** Constructs a permanent file store object from an already opened file. It is strongly recommended to set EFileWriteDirectIO
bit when opening the file. This is because that when the file server write caching is enabled, the
order of file write operations is not guaranteed. This could cause data inconsistency in some
circumstances, for example, when the power is lost in the middle of database transaction.
Therefore, the file write caching should be switched off to maintain integrity.
   


The file must already be open before calling this function.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid.


@param aFile A reference to the opened file. 
@return A pointer to the new permanent file store object. */
	{return STATIC_CAST(CPermanentFileStore*,CFileStore::FromL(aFile,KPermanentFileStoreFactoryFunction));}
inline CPermanentFileStore* CPermanentFileStore::FromLC(RFile& aFile)
/** Constructs a permanent file store object from an already opened file, and places 
the pointer onto the cleanup stack.

The file must already be open before calling this function. It is strongly recommended to set EFileWriteDirectIO
bit when opening the file. This is because that when the file server write caching is enabled, the
order of file write operations is not guaranteed. This could cause data inconsistency in some
circumstances, for example, when the power is lost in the middle of database transaction.
Therefore, the file write caching should be switched off to maintain integrity.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid.

@param aFile A reference to the opened file. 
@return A pointer to the new permanent file store object. */
	{return STATIC_CAST(CPermanentFileStore*,CFileStore::FromLC(aFile,KPermanentFileStoreFactoryFunction));}
inline CPermanentFileStore* CPermanentFileStore::NewL(RFile& aFile)
/** Constructs a new permanent file store object in an already opened file.

The file must already be open before calling this function. The existing content 
of the file is discarded. It is strongly recommended to set EFileWriteDirectIO
bit when opening the file. It is because that when the file write caching in file server is on, the
order of file writing is not guaranteed which could cause data inconsistency in some
circumstances, for example, when the power is lost in the middle of data transaction.
Therefore, the file write caching should be switched off to maintain the file integrity.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid.

@param aFile A reference to the opened file. 
@return A pointer to the new permanent file store object. */
	{return STATIC_CAST(CPermanentFileStore*,CFileStore::NewL(aFile,&DoNewL));}
inline CPermanentFileStore* CPermanentFileStore::NewLC(RFile& aFile)
/** Constructs a new permanent file store object in an already opened file and 
places the pointer onto the cleanup stack.

The file must already be open before calling this function.The existing content 
of the file is discarded. It is strongly recommended to set EFileWriteDirectIO
bit when opening the file. It is because that when the write caching in file server is on, the
order of file writing is not guaranteed which could cause data inconsistency in some
circumstances, for example, when the power is lost in the middle of data transaction.
Therefore, the file write caching should be switched off to maintain the file integrity.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid.

@param aFile A reference to the opened file. 
@return A pointer to the new permanent file store object. */
	{return STATIC_CAST(CPermanentFileStore*,CFileStore::NewLC(aFile,&DoNewL));}

// Class RFilePagePool
inline TInt RFilePagePool::Open(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file to use for the page pool.

@param aFs A file server session
@param aName The name of the file
@param aFileMode The mode in which the file is opened. For more information, 
see the TFileMode enumeration. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see TFileMode */
	{return iFile.Open(aFs,aName,aFileMode);}
inline TInt RFilePagePool::Create(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates a new file for the page pool.

@param aFs A file server session
@param aName The name of the file. Any path components which are not specified 
here are taken from the session path.
@param aFileMode The mode in which the file is opened. For more information 
see the TFileMode enumeration. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see TFileMode */
	{return iFile.Create(aFs,aName,aFileMode);}
inline TInt RFilePagePool::Replace(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Creates or opens a file for the page pool. 

If there is an existing file with the same name, this function overwrites 
it. If the file does not already exist, it is created.

@param aFs A file server session.
@param aName The name of the file. Any path components which are not specified 
here are taken from the session path.
@param aFileMode The mode in which the file is opened. For more information 
see the TFileMode enumeration. 
@return KErrNone if successful, otherwise another of the system-wide error 
codes.
@see TFileMode */
	{return iFile.Replace(aFs,aName,aFileMode);}
inline TInt RFilePagePool::Temp(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode)
/** Creates and opens a file for the page pool with a unique name.

@param aFs A file server session.
@param aPath The directory in which the file should be created.
@param aName On return, contains the full path and name of the file. The filename 
is guaranteed to be unique within the directory specified by aPath.
@param aFileMode The mode in which the file is opened. For more information 
see the TFileMode enumeration.
@return KErrNone if successful, otherwise another of the system-wide error 
codes. */
	{return iFile.Temp(aFs,aPath,aName,aFileMode);}
inline void RFilePagePool::Attach(RFile& aFile)
/** Sets an existing file to be used for the page pool.

@param aFile File to use for the page pool */
	{iFile=aFile;}
inline void RFilePagePool::Detach()
/** Ends the use of the file for the page pool, but does not close the file. */
	{iFile=RFile();}
inline RFile& RFilePagePool::File() const
	{return MUTABLE_CAST(RFile&,iFile);}

