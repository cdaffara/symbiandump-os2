// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Demonstrates performance assesment of CPermanentFileStore
// The Store root stream serves as an example of persistent data structure
// 
//

// The file name, extension and path for the file store
_LIT(KPermFileStoreName,"Z:\\STOR-TST\\BMPermFileStores.dat");
extern TFileName ThePermStoreFilePath;

// Create a permanent file store and initialise its stream structure
LOCAL_C void doPrepareStoreL(const TDesC& aName);

// Use the store, preserving the stream structure
LOCAL_C void doPermFileStoreL(const TDesC& aName);

// Update the data in the store
LOCAL_C void doUpdateStoreL(CPersistentStore& aStore);
#define PFSTORERUNSIZE	1000

typedef TBuf<100> TItem;

// The main object's in-memory representation
class CItemArray : public CBase
	{
public:
	static TStreamId CreateL(CStreamStore& aStore);
	~CItemArray();
	static CItemArray* NewLC(CStreamStore& aStore,TStreamId anId);
	void RestoreL();
	void StoreL() const;
	void ExternalizeL(RWriteStream& aStream) const;
	//
	void AddItemL(const TItem& anItem);
	void RemoveItemL(TInt anIndex);
	TInt Count() const;
	void GetItemL(TItem& anItem,TInt anIndex) const;
protected:
	CItemArray(CStreamStore& aStore);
	void ConstructL();
	void InternalizeL(RReadStream& aStream);
private:
	CStreamStore& iStore;
	TStreamId iMyId;
	CArrayFixFlat<TStreamId>* iArray;
	CArrayFixFlat<TElement>* nArray;
	};

/**
@SYMTestCaseID          SYSLIB-STORE-PT-1371
@SYMTestCaseDesc	CR MRLM-6A9DF7. Tests the impact of RFileBuf cache size on CPermanentFileStore performance. RFileBuf cache size must be set at a build time in EStor_Template.mmh file, DEFAULT_FILE_BUF_SIZE macro definition.
@SYMTestPriority 	High
@SYMTestActions  	The test creates a set of test objects, externalizes them PFSTORERUNSIZE times and prints the execution time. Then internalizes them PFSTORERUNSIZE times and prints the execution time. PFSTORERUNSIZE is a constant, defined in the source file.
@SYMTestExpectedResults Test must show a performance improvement
@SYMPREQ                PREQ1132
@SYMREQ			REQ4883
*/
LOCAL_C void doPermanentFileStoreL()
    {
    TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse permFileStoreName;
	permFileStoreName.Set(drive.Name(), &KPermFileStoreName, NULL);
	ThePermStoreFilePath.Copy(permFileStoreName.FullName());
    
	TheFs.MkDirAll(permFileStoreName.DriveAndPath());
	doPrepareStoreL(ThePermStoreFilePath);
	
	TTime start;
	start.HomeTime();
	for(int i=0; i<PFSTORERUNSIZE; i++)
		{
		doPermFileStoreL(ThePermStoreFilePath);
		}
	TTime end;
	end.HomeTime();
	TTimeIntervalMicroSeconds us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CPermanentFileStore, %d 'write' tests. Time=%ld ms\r\n"), PFSTORERUNSIZE, us.Int64() / 1000);
	}

LOCAL_C void doPrepareStoreL(const TDesC& aName)
	{
				// construct file store object - the file to contain the
				// the store replaces any existing file of the same name.
	CFileStore* store = CPermanentFileStore::ReplaceLC(TheFs,aName,EFileRead|EFileWrite);
				// Easy way to set the layout type
    store->SetTypeL(store->Layout());
		
				// create the required stream for CItemArray
	TStreamId id=CItemArray::CreateL(*store);
	store->SetRootL(id);
	store->CommitL();
	CleanupStack::PopAndDestroy();
	}

LOCAL_C void doPermFileStoreL(const TDesC& aName)
	{
				// construct file store object - specifying the file
				// containing the store.
				// Do not need to specify the file store type, this is
				// specified by the file itself
	CFileStore* store = CFileStore::OpenL(TheFs,aName,EFileRead|EFileWrite);

				// The standard form for using permanent file stores:
				// 1. The store object is not owned by the updating code
				// 2  Failure at any point during update, including the
				//    final commit, should result in Revert() being called
				//    on the store (before destruction).
	_LIT(KTxtErrorOccurred,"\n** Error %d occured during store update");

	TRAPD(error,doUpdateStoreL(*store));
	if (error!=KErrNone)
		{
		store->Revert();
		TheTest.Printf(KTxtErrorOccurred);
		}

	// the store is not on the cleanup stack
	delete store;
	}


LOCAL_C void doUpdateStoreL(CPersistentStore& aStore)
	{
				// get the root stream into memory
	CItemArray* array=CItemArray::NewLC(aStore,aStore.Root());

				// Add some items
	_LIT(KTxtHello,"hello");
	_LIT(KTxtWorld," world!");

	TItem item;
	item = KTxtHello;
	array->AddItemL(item);
	item = KTxtWorld;
	array->AddItemL(item);
				// Re-write the root stream with new data
	array->StoreL();
	aStore.CommitL();

				// remove an item
	array->RemoveItemL(1);		// " world!"
				// Re-write the root stream with new data
	array->StoreL();
	aStore.CommitL();
				// Add an item
	_LIT(KTxtCapWorld," WORLD!");
	item= KTxtCapWorld;
	array->AddItemL(item);
				// Re-write the root stream with new data
	array->StoreL();
				// Discard all changes since last store commit
	aStore.Revert();
				// array and aStore are not in snych after revert...
				// restore in-memory version to match store version
	array->RestoreL();
					// Add the item again
	array->AddItemL(item);
				// Re-write the root stream with new data
	array->StoreL();
	aStore.CommitL();
	CleanupStack::PopAndDestroy(); // array
	}

//***************************************************************
CItemArray::~CItemArray()
	{
	delete iArray;
	delete nArray;
	}

CItemArray::CItemArray(CStreamStore& aStore)
	: iStore(aStore)
	{}

TStreamId CItemArray::CreateL(CStreamStore& aStore)
// create the stream representation of the class
	{
				// use a temporary CItemArray
	CItemArray* self=new(ELeave) CItemArray(aStore);
	CleanupStack::PushL(self);
				// construct object
	self->ConstructL();
				// create new stream
	RStoreWriteStream outstream;
	TStreamId id=outstream.CreateLC(aStore);
				// write  external rep
	self->ExternalizeL(outstream);
				// commit stream
	outstream.CommitL();
				// cleanup stream and temporary self
	CleanupStack::PopAndDestroy(2);
	return id;
	}

CItemArray* CItemArray::NewLC(CStreamStore& aStore,TStreamId anId)
// construct a CItemArray from persistent storage
	{
	CItemArray* self=new(ELeave) CItemArray(aStore);
	CleanupStack::PushL(self);
				// construct object
	self->ConstructL();
				// set the stream id for StoreL/RestoreL
	self->iMyId=anId;
				// restore the internal rep.
	self->RestoreL();
	return self;
	}

void CItemArray::StoreL() const
// replace external rep. with internal one
	{
	RStoreWriteStream outstream;
	outstream.ReplaceLC(iStore,iMyId);
	ExternalizeL(outstream);
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
	}

void CItemArray::RestoreL()
// replace internal rep with external one
	{
	iArray->Reset();
	nArray->Reset();
	RStoreReadStream instream;
	instream.OpenLC(iStore,iMyId);
	InternalizeL(instream);
	CleanupStack::PopAndDestroy();
	}

void CItemArray::AddItemL(const TItem& anItem)
// add item to the collection
	{
				// write external rep of item
	RStoreWriteStream outstream;
	TStreamId id=outstream.CreateLC(iStore);
	outstream<<anItem;
	outstream.CommitL();
	CleanupStack::PopAndDestroy();
				// add new stream id to the internal array
	iArray->AppendL(id);
	}

void CItemArray::RemoveItemL(TInt anIndex)
// remove an item from the collection
	{
				// remove the stream from the store
	iStore.DeleteL((*iArray)[anIndex]);
				// remove the entry from the internal array
	iArray->Delete(anIndex);
	}

TInt CItemArray::Count() const
	{
	return iArray->Count();
	}

void CItemArray::GetItemL(TItem& anItem,TInt anIndex) const
	{
				// retrieve an item from the store
	RStoreReadStream instream;
	instream.OpenLC(iStore,(*iArray)[anIndex]);
	instream>>anItem;
	CleanupStack::PopAndDestroy();
	}

void CItemArray::ConstructL()
	{
	iArray=new(ELeave) CArrayFixFlat<TStreamId>(8);
	nArray = new (ELeave) CArrayFixFlat<TElement>(PFSTORERUNSIZE);
	TElement theElement;
	_LIT(KFormatTxt,"BenchMarkingPermanentFileStore%4u");
	TBuf<256> str(KFormatTxt);

	for (TInt index = 0; index < PFSTORERUNSIZE; index++)
		{
		theElement.iData.Format(KFormatTxt,index);
		nArray->AppendL(theElement);
		}
	}

void CItemArray::ExternalizeL(RWriteStream& aStream) const
	{
				// stream out the array
	aStream<<*iArray;
	TInt count = nArray->Count();
	TElement theElement;
	for (TInt index = 0; index < count; index++)
		{
		theElement =((*nArray)[index]);
		aStream.WriteL(theElement.iData, 34);
		}
	}

void CItemArray::InternalizeL(RReadStream& aStream)
	{
				// stream in the array
	aStream>>*iArray;
	
	TElement theElement;
	TInt count = nArray->Count();
	nArray->Reset();
	for (TInt index = 0; index < count; index++)
		{
		aStream.ReadL(theElement.iData, 34);
		nArray->AppendL(theElement);
		}
	}
