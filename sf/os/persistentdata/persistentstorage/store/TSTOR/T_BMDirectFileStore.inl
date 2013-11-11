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
// Demonstrates performance assesment of direct file store..
// The file name, extension and path for the file store
// 
//

_LIT(KDirectFileStoreName,"Z:\\STOR-TST\\BMDirectFileStores.dat");
extern TFileName TheDirectStoreFilePath;

// Create objects externalizes them to a direct file store and then destroys the objects
LOCAL_C void doStoreL(const TDesC& aName);

// Constructs "empty" CDFileStoreA, ClassB and CDFileStoreC objects and restores them from the direct file store.
LOCAL_C void doRestoreL(const TDesC& aName);

#define DFSTORERUNSIZE	1000

class CDFileStoreA : public CBase
	{
public :
	static CDFileStoreA* NewLC();
	~CDFileStoreA();
	void     SetTextL(const TDesC& aData);
	void     ExternalizeL(RWriteStream& aStream) const;
	void     InternalizeL(RReadStream& aStream);
public :
	HBufC*   iVarBuffer;
	TInt     iIntValue;
	TUint    iUintValue;
	};

class CDFileStoreB : public CBase
	{
public :
	static CDFileStoreB* NewLC();
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
public :
	TBuf<32> iFixBuffer;
	TUint    iUintValue;
	TInt     iIntValue;
	TReal    iRealValue;
	};

//#define KStdirectClassCGranularity 4

class CDFileStoreC : public CBase
	{
public :
	static CDFileStoreC* NewLC();
	~CDFileStoreC();
	void     ConstructL();
	//void     AddNumberToArrayL(TInt aValue);
	void     AddNumberToArrayL();
	void     ExternalizeL(RWriteStream& aStream) const;
	void     InternalizeL(RReadStream& aStream);
public :
	TBuf<32> iFixBuffer;
	CArrayFixFlat<TInt>* iArray;
	CArrayFixFlat<TElement>* nArray;
	};



/**
@SYMTestCaseID          SYSLIB-STORE-PT-1362
@SYMTestCaseDesc	CR MRLM-6A9DF7. Tests the impact of RFileBuf cache size on CDirectFileStore performance. RFileBuf cache size must be set at a build time in EStor_Template.mmh file, DEFAULT_FILE_BUF_SIZE macro definition.
@SYMTestPriority 	High
@SYMTestActions  	The test creates a set of test objects, externalizes them DFSTORERUNSIZE times and prints the execution time. Then internalizes them DFSTORERUNSIZE times and prints the execution time. DFSTORERUNSIZE is a constant, defined in the source file.
@SYMTestExpectedResults Test must show a performance improvement
@SYMPREQ                PREQ1132
@SYMREQ			REQ4883
*/
LOCAL_C void doDirectFileStoresL()
    {
    TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse directFileStoreName;
	directFileStoreName.Set(drive.Name(), &KDirectFileStoreName, NULL);
	TheDirectStoreFilePath.Copy(directFileStoreName.FullName());
    
    // make sure directory exists
	TheFs.MkDirAll(directFileStoreName.DriveAndPath());
	
	TTime start;
	start.HomeTime();
	for(int i=0; i<DFSTORERUNSIZE; i++)
		{
		doStoreL(TheDirectStoreFilePath);	
		}
	TTime end;
	end.HomeTime();
	TTimeIntervalMicroSeconds us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CDirectFileStore, %d 'write' tests. Time=%ld ms\r\n"), DFSTORERUNSIZE, us.Int64() / 1000);

	start.HomeTime();
	for(int j=0; j<DFSTORERUNSIZE;j++)
		{
		doRestoreL(TheDirectStoreFilePath);
		}
	end.HomeTime();
	us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CDirectFileStore, %d 'read' tests.  Time=%ld ms\r\n"), DFSTORERUNSIZE, us.Int64() / 1000);
	}

LOCAL_C void doStoreL(const TDesC& aName)
	{
	// Construct an object of type CDFileStoreA and put some data into it
	_LIT(KTxtForClassA,"Text for CDFileStoreA");
	CDFileStoreA* theA = CDFileStoreA::NewLC();
	theA->SetTextL(KTxtForClassA);	
	theA->iIntValue  = -1;
	theA->iUintValue = 2;

	// Construct an object of type CDFileStoreB and put some data into it
	_LIT(KTxtForClassB,"Text for CDFileStoreB");
	CDFileStoreB* theB = CDFileStoreB::NewLC();
	theB->iFixBuffer = KTxtForClassB;
	theB->iIntValue  = -3;
	theB->iUintValue = 4; 
	theB->iRealValue = 5.6;

	// Construct an object of type CDFileStoreC and put some data into it
	_LIT(KTxtForClassC,"Text for CDFileStoreC");
	CDFileStoreC* theC = CDFileStoreC::NewLC();
	theC->iFixBuffer = KTxtForClassC;	
	theC->AddNumberToArrayL();
	// Create (replace) the direct file store
	CFileStore* store = CDirectFileStore::ReplaceLC(TheFs,aName,EFileWrite);

	// Must say what kind of file store.
	store->SetTypeL(KDirectFileStoreLayoutUid);
    	
	// Construct the output stream.
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(*store);

	// Stream out the CDFileStoreA, CDFileStoreB, and the CDFileStoreC object
	outstream  << *theA << *theB << *theC; 

	// Commit changes to the stream
	outstream.CommitL();

    // Cleanup the stream object
    CleanupStack::PopAndDestroy();

	// Set this stream id as the root
	store->SetRootL(id);

	// Commit changes to the store
	store->CommitL();
									// store
									// theC
									// theB
	CleanupStack::PopAndDestroy(4); // theA
	}

LOCAL_C void doRestoreL(const TDesC& aName)
	{
	CDFileStoreA* theA = CDFileStoreA::NewLC();
	CDFileStoreB* theB = CDFileStoreB::NewLC();
	CDFileStoreC* theC = CDFileStoreC::NewLC();
					
	// Open the direct file store
	CFileStore* store = CDirectFileStore::OpenLC(TheFs,aName,EFileRead);

	// Construct and open the root stream which contains the representation of our objects.
	RStoreReadStream instream;
	instream.OpenLC(*store,store->Root());
	
	// Stream in the CDFileStoreA object, the CDFileStoreB object and the CDFileStoreC object
	instream  >> *theA >> *theB >> *theC; 

	CleanupStack::PopAndDestroy();
	CleanupStack::PopAndDestroy(4);
	}

//***************************************************************
CDFileStoreA* CDFileStoreA::NewLC()
	{
	CDFileStoreA* self = new (ELeave) CDFileStoreA;
	CleanupStack::PushL(self);
	return self;
	}
	
CDFileStoreA::~CDFileStoreA()
	{
	delete iVarBuffer;
	}

void CDFileStoreA::SetTextL(const TDesC& aData)
	{
	iVarBuffer = aData.AllocL();	
	}

void CDFileStoreA::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iVarBuffer->Des().MaxLength());
	aStream << *iVarBuffer;
	aStream.WriteInt32L(iIntValue);
	aStream.WriteUint32L(iUintValue);
	}  
 
void CDFileStoreA::InternalizeL(RReadStream& aStream)
	{
	TInt maxlen;
	maxlen     = aStream.ReadInt32L();
	iVarBuffer = HBufC::NewL(aStream,maxlen);
	iIntValue  = aStream.ReadInt32L();
	iUintValue = aStream.ReadUint32L();
	}  

//***************************************************************
CDFileStoreB* CDFileStoreB::NewLC()
	{
	CDFileStoreB* self = new (ELeave) CDFileStoreB;
	CleanupStack::PushL(self);
	return self;
	}

void CDFileStoreB::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iFixBuffer;
	aStream.WriteInt32L(iIntValue);
	aStream.WriteUint32L(iUintValue);
	aStream.WriteReal64L(iRealValue);
	}  
 
void CDFileStoreB::InternalizeL(RReadStream& aStream)
	{
	aStream >> iFixBuffer;
	iIntValue  = aStream.ReadInt32L();
	iUintValue = aStream.ReadUint32L();
	iRealValue = aStream.ReadReal64L();
	}  

//***************************************************************
CDFileStoreC* CDFileStoreC::NewLC()
	{
	CDFileStoreC* self = new (ELeave) CDFileStoreC;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
void CDFileStoreC::ConstructL()
	{
	iArray = new (ELeave) CArrayFixFlat<TInt>(DFSTORERUNSIZE);
	nArray = new (ELeave) CArrayFixFlat<TElement>(DFSTORERUNSIZE);
	}

void CDFileStoreC::AddNumberToArrayL(/*TInt aValue*/)
	{
	TElement theElement;
	_LIT(KFormatTxt,"BenchMarkingDirectFileStore%4u");
	TBuf<256> str(KFormatTxt);
		
	for (TInt index = 0; index < DFSTORERUNSIZE; index++)
		{
		iArray->AppendL((index+99999));
		theElement.iData.Format(KFormatTxt,index);
		nArray->AppendL(theElement);
		}
	}

CDFileStoreC::~CDFileStoreC()
	{
	delete iArray;
	delete nArray;
	}

void CDFileStoreC::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iFixBuffer;

	TInt count = iArray->Count();
	aStream.WriteInt32L(count);
	TElement theElement;
	for (TInt index = 0; index < count; index++)
		{
		aStream.WriteInt32L((*iArray)[index]);
		theElement =((*nArray)[index]);
		aStream.WriteL(theElement.iData, 31);
		}
	}  
 
void CDFileStoreC::InternalizeL(RReadStream& aStream)
	{
	aStream >> iFixBuffer;

	TInt count;
	count  = aStream.ReadInt32L();
	TElement theElement;
	//nArray = new (ELeave) CArrayFixFlat<TElement>(10);
	nArray->Reset();
	for (TInt index = 0; index < count; index++)
		{
		iArray->AppendL(aStream.ReadInt32L());
		aStream.ReadL(theElement.iData, 31);
		nArray->AppendL(theElement);
		}
	}
