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

#include "UF_STD.H"

const TFileStoreFactoryFunction KDefaultFileStoreFactory[]=
	{
	KDirectFileStoreFactoryFunction,
	KPermanentFileStoreFactoryFunction,
	NULL
	};

EXPORT_C CFileStore* CFileStore::OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a store and constructs an appropriate file store object.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store. Note that if the store object is CPermanentFileStore and the file is writable,
it is strongly recommended to set EFileWriteDirectIO bit in aFileMode. This is because that
when the file server write caching is enabled, the order of file write operations is not guaranteed. 
This could cause data inconsistency in some circumstances, for example, when the power  is lost in
 the middle of database transaction. Therefore, the file write caching should be switched off to maintain integrity.


@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@return A pointer to the new file store object. 
@see TFileMode
@see CFileStore::Layout() */
	{
	return OpenL(aFs,aName,aFileMode,KDefaultFileStoreFactory);
	}

EXPORT_C CFileStore* CFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode)
/** Opens a file containing a store, constructs an appropriate file store object 
and places the pointer onto the cleanup stack.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store. Note that if the store object is CPermanentFileStore and the file is writable,
it is strongly recommended to set EFileWriteDirectIO bit when opening the file. This is because that
when the file server write caching is enabled, the order of file write operations is not guaranteed. 
This could cause data inconsistency in some circumstances, for example, when the power  is lost in
 the middle of database transaction. Therefore, the file write caching should be switched off to maintain integrity.


@param aFs Handle to a file server session. 
@param aName The full path name of the file containing the store. 
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type. 
@return A pointer to the new file store object. 
@see TFileMode
@see CFileStore::Layout() */
	{
	return OpenLC(aFs,aName,aFileMode,KDefaultFileStoreFactory);
	}

EXPORT_C CFileStore* CFileStore::FromL(RFile& aFile)
/** Constructs a file store object from an opened file.

The file must already be open before calling this function.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid:

@param aFile A reference to the opened file. 
@return A pointer to the new file store object. 
@see CFileStore::Layout() */
	{
	return FromL(aFile,KDefaultFileStoreFactory);
	}

EXPORT_C CFileStore* CFileStore::FromLC(RFile& aFile)
/** Constructs a file store object from an opened file, and places the pointer 
onto the cleanup stack.

The file must already be open before calling this function.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store.

Note that ownership of the file passes to the store. The referenced RFile 
is cleared and is no longer valid:

@param aFile A reference to the opened file. 
@return A pointer to the new file store object. 
@see CFileStore::Layout() */
	{
	return FromLC(aFile,KDefaultFileStoreFactory);
	}

/** Opens a file containing a store and constructs an appropriate file store object.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store. Note that if the store object is CPermanentFileStore and the file is writable,
it is strongly recommended to set EFileWriteDirectIO bit in aFileMode. This is because that
when the file server write caching is enabled, the order of file write operations is not guaranteed. 
This could cause data inconsistency in some circumstances, for example, when the power  is lost in
the middle of database transaction. Therefore, the file write caching should be switched off to maintain integrity.


@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@param TFileStoreFactoryFunction An array of  file store factory function.
@return A pointer to the new file store object. 
@see TFileMode
@see TFileStoreFactoryFunction
*/
EXPORT_C CFileStore* CFileStore::OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode,const TFileStoreFactoryFunction aFactory[])
//
// Open a file store of any of the types supported by aFactory.
//
	{
	RFile file;
	__LEAVE_IF_ERROR(file.Open(aFs,aName,aFileMode));
	return FromL(file,aFactory);
	}

/** Opens a file containing a store and constructs an appropriate file store object.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store. Note that if the store object is CPermanentFileStore and the file is writable,
it is strongly recommended to set EFileWriteDirectIO bit in aFileMode. This is because that
when the file server write caching is enabled, the order of file write operations is not guaranteed. 
This could cause data inconsistency in some circumstances, for example, when the power  is lost in
the middle of database transaction. Therefore, the file write caching should be switched off to maintain integrity.


@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@param TFileStoreFactoryFunction An array of file store factory function.
@return A pointer to the new file store object. 
@see TFileMode
@see TFileStoreFactoryFunction
*/
EXPORT_C CFileStore* CFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode,const TFileStoreFactoryFunction aFactory[])
//
// Open and leave on cleanup stack.
//
	{
	CFileStore* store=OpenL(aFs,aName,aFileMode,aFactory);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore* CFileStore::FromL(RFile& aFile,const TFileStoreFactoryFunction aFactory[])
//
// Read the file header and let every factory function in turn try to open it.
//
	{
	RFileBuf buf;
	buf.Attach(aFile);
	buf.PushL();
	RReadStream stream(&buf);
	TCheckedUid chk;
	stream>>chk;
	if (chk.UidType().IsValid())
		{
		const TFileStoreFactoryFunction* iter=&aFactory[0];
		TFileStoreFactoryFunction func;
		while ((func=*iter++)!=NULL)
			{
			CFileStore* store=(*func)(buf,chk.UidType());
			if (store!=NULL)
				{
				CleanupStack::Pop(2);
				return store;
				}
			}
		}
//
	__LEAVE(KErrNotSupported);
	return NULL;
	}

EXPORT_C CFileStore* CFileStore::FromLC(RFile& aFile,const TFileStoreFactoryFunction aFactory[])
//
// Open and leave on cleanup stack.
//
	{
	CFileStore* store=FromL(aFile,aFactory);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C void CFileStore::SetTypeL(const TUidType& aType)
/** Sets the UID type of the file store.

The first UID, i.e. the first TUid component, of the TUidType must be the 
file store type as returned by CFileStore::Layout(), otherwise the function 
raises a STORE-File 9 panic.

@param aType The UID type object containing the file store type. 
@see TUid */
	{
	__ASSERT_ALWAYS(aType[0]==Layout(),Panic(EFileStoreBadType));
	TCheckedUid chk(aType);
	if (iHost.IsActive())
		{
		RShareWriteStream stream(iHost);
		stream.PushL();
		stream<<chk; // failure may mean the file's type information is lost
		CleanupStack::PopAndDestroy();
		}
	else
		{
		TInt size=iBuf.SizeL();
		RWriteStream stream(&iBuf);
		stream<<chk;
		if (size==0)
			{
			ExternalizeL(stream);
			iHost.Share(&iBuf);
			}
		}
	iType=aType;
	}

EXPORT_C void CFileStore::MarshalL()
//
// Second-phase construction for opening existing file stores.
//
	{
	__ASSERT_DEBUG(!iHost.IsActive()&&(!iType.IsValid()||iType[0]==Layout()),User::Invariant());
	RReadStream stream(&iBuf);
	InternalizeL(stream);
	iHost.Share(&iBuf);
	}

EXPORT_C CFileStore::~CFileStore()
/** Frees resources owned by the object, prior to its destruction. In particular, 
it closes the associated file. */
	{
	Destruct();
	iBuf.Close();
	}

/** Opens a file containing a store and constructs an appropriate file store object.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store. Note that if the store object is CPermanentFileStore and the file is writable,
it is strongly recommended to set EFileWriteDirectIO bit in aFileMode. This is because that
when the file server write caching is enabled, the order of file write operations is not guaranteed. 
This could cause data inconsistency in some circumstances, for example, when the power  is lost in
the middle of database transaction. Therefore, the file write caching should be switched off to maintain integrity.


@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@param TFileStoreFactoryFunction A file store factory function.
@return A pointer to the new file store object. 
@see TFileMode
@see TFileStoreFactoryFunction
*/
EXPORT_C CFileStore* CFileStore::OpenL(RFs& aFs,const TDesC& aName,TUint aFileMode,TFileStoreFactoryFunction aFunction)
//
// Open a file store using aFunction.
//
	{
	RFile file;
	__LEAVE_IF_ERROR(file.Open(aFs,aName,aFileMode));
	return FromL(file,aFunction);
	}

/** Opens a file containing a store and constructs an appropriate file store object.

The resulting file store object is of concrete type, i.e. either CDirectFileStore 
or CPermanentFileStore. The specific type is determined from the layout information 
held in the file store. Note that if the store object is CPermanentFileStore and the file is writable,
it is strongly recommended to set EFileWriteDirectIO bit in aFileMode. This is because that
when the file server write caching is enabled, the order of file write operations is not guaranteed. 
This could cause data inconsistency in some circumstances, for example, when the power  is lost in
the middle of database transaction. Therefore, the file write caching should be switched off to maintain integrity.


@param aFs Handle to a file server session.
@param aName The full path name of the file containing the store.
@param aFileMode The mode in which the file is to be accessed. The mode is 
defined by the TFileMode type.
@param TFileStoreFactoryFunction A file store factory function.
@return A pointer to the new file store object. 
@see TFileMode
@see TFileStoreFactoryFunction
*/
EXPORT_C CFileStore* CFileStore::OpenLC(RFs& aFs,const TDesC& aName,TUint aFileMode,TFileStoreFactoryFunction aFunction)
//
// Open and leave on cleanup stack.
//
	{
	CFileStore* store=OpenL(aFs,aName,aFileMode,aFunction);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore* CFileStore::CreateL(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction)
//
// Create a file store using aFunction. Must not already exist.
//
	{
	RFile file;
	__LEAVE_IF_ERROR(file.Create(aFs,aName,aFileMode));
	return DoNewL(file,aFunction);
	}

EXPORT_C CFileStore* CFileStore::CreateLC(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction)
//
// Create and leave on cleanup stack.
//
	{
	CFileStore* store=CreateL(aFs,aName,aFileMode,aFunction);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore* CFileStore::ReplaceL(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction)
//
// Replace a file store using aFunction. May exist already.
//
	{
	RFile file;
	__LEAVE_IF_ERROR(file.Replace(aFs,aName,aFileMode));
	return DoNewL(file,aFunction);
	}

EXPORT_C CFileStore* CFileStore::ReplaceLC(RFs& aFs,const TDesC& aName,TUint aFileMode,TNewFunction aFunction)
//
// Replace and leave on cleanup stack.
//
	{
	CFileStore* store=ReplaceL(aFs,aName,aFileMode,aFunction);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore* CFileStore::TempL(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode,TNewFunction aFunction)
//
// Create a temporary file store using aFunction.
//
	{
	RFile file;
	__LEAVE_IF_ERROR(file.Temp(aFs,aPath,aName,aFileMode));
	return DoNewL(file,aFunction);
	}

EXPORT_C CFileStore* CFileStore::TempLC(RFs& aFs,const TDesC& aPath,TFileName& aName,TUint aFileMode,TNewFunction aFunction)
//
// Create temporary and leave on cleanup stack.
//
	{
	CFileStore* store=TempL(aFs,aPath,aName,aFileMode,aFunction);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore* CFileStore::FromL(RFile& aFile,TFileStoreFactoryFunction aFunction)
//
// Open a file store using aFunction, given its file handle.
//
	{
	RFileBuf buf;
	buf.Attach(aFile);
	buf.PushL();
	RReadStream stream(&buf);
	TCheckedUid chk;
	stream>>chk;
	CFileStore* store=(*aFunction)(buf,chk.UidType());
	if (store==NULL) 
		{
		aFile.Close();
		__LEAVE(KErrNotSupported);
		}
//
	CleanupStack::Pop(2);
	return store;
	}

EXPORT_C CFileStore* CFileStore::FromLC(RFile& aFile,TFileStoreFactoryFunction aFunction)
//
// Open and leave on cleanup stack.
//
	{
	CFileStore* store=FromL(aFile,aFunction);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore* CFileStore::NewL(RFile& aFile,TNewFunction aFunction)
//
// Create a file store using aFunction, given its file handle.
//
	{
	TInt r=aFile.SetSize(0);
	if (r!=KErrNone)
		{
		aFile.Close();
		__LEAVE(r);
		}
	return DoNewL(aFile,aFunction);
	}

EXPORT_C CFileStore* CFileStore::NewLC(RFile& aFile,TNewFunction aFunction)
//
// Create and leave on cleanup stack.
//
	{
	CFileStore* store=NewL(aFile,aFunction);
	CleanupStack::PushL(store);
	return store;
	}

EXPORT_C CFileStore::CFileStore(RFile& aFile)
//
// Constructor creating a new file store.
//
	{
	iBuf.Attach(aFile);
	iBuf.iExt = 0;
	}

EXPORT_C CFileStore::CFileStore(RFileBuf& aBuf,const TUidType& aType)
//
// Constructor opening an existing file store.
//
	: iBuf(Capture(aBuf)),iType(aType)
	{}

EXPORT_C void CFileStore::Destruct()
//
// Early destruction, for derived classes overriding DoRevertL().
//
	{
	if (iHost.IsActive())
		{
		iHost.Release();
		}
	}

EXPORT_C void CFileStore::SynchL()
//
// Synchronise this file store's file buffer.
//
	{
	TStreamExchange& host=iHost;
	if (host.IsActive())
		{
		MStreamBuf* buf=host.HostL();
		__ASSERT_DEBUG(IsHost(buf),User::Invariant());
		host.Release(); // make sure no writes fail silently by shutting down on failure
		buf->SynchL();
		host.Share(buf);
		}
	else
		iBuf.SynchL();
	}

EXPORT_C void CFileStore::ChangedL()
//
// Re-write this file store's structure.
//
	{
	__ASSERT_DEBUG(iHost.IsActive(),User::Invariant());
	RShareWriteStream stream(iHost,KFileStoreStart);
	stream.PushL();
	ExternalizeL(stream);
	CleanupStack::PopAndDestroy();
	}

EXPORT_C void CFileStore::RefreshL()
//
// Re-read this file store's structure.
//
	{
	if (iHost.IsActive())
		{
		RShareReadStream stream(iHost,KFileStoreStart);
		stream.PushL();
		InternalizeL(stream);
		CleanupStack::PopAndDestroy();
		}
	else
		{
		iBuf.SeekL(iBuf.ERead,KFileStoreStart);
		MarshalL();
		}
	}

EXPORT_C void CFileStore::DoCommitL()
//
// Default implementation just synchronising.
//
	{
	SynchL();
	}

EXPORT_C void CFileStore::DoRevertL()
//
// Default implementation failing after synchronisation.
//
	{
	SynchL();
	__LEAVE(KErrNotSupported);
	}

CFileStore* CFileStore::DoNewL(RFile& aFile,TNewFunction aFunction)
	{
	CleanupClosePushL(aFile);
	CFileStore* store=(*aFunction)(aFile);
	CleanupStack::Pop();
	return store;
	}

