// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// This is the file name, extension and path specification for the file store 
// and should not be used as is
// 
//

_LIT(KStreamsName, "Z:\\STOR-TST\\BMStreams.dat");
extern TFileName TheStreamsFilePath;

// Construct CCompound object and externalize it to a single stream
LOCAL_C void doExternalizeL(const TDesC& aName);

// Internalize CCompound object from the stream
LOCAL_C void doInternalizeL(const TDesC& aName);

#define STREAMRUNSIZE 500
// Declare a class used by the example
class CStreamA
	{
public :
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
public :
	TBuf<32> iBufferA;
	TInt     iXA;
	TUint    iYA;
	};

class TStream
	{
public :
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
public :
	TReal iZC;
	};
	
class CCompound : public CBase
	{
public :
	~CCompound();
	static		CCompound* NewLC();
	static		CCompound* NewLC(CStreamStore& aStore,TStreamId anId);
	static		CCompound* NewL(CStreamStore& aStore,TStreamId anId);
	TStreamId	StoreL(CStreamStore& store);
	void		RestoreL(CStreamStore& aStore,TStreamId anId);
	void		InternalizeL(RReadStream& aStream);
	void		ExternalizeL(RWriteStream& aStream) const;
private:
    void		ConstructL();
	void		ConstructL(CStreamStore& aStore,TStreamId anId);
public :
	CStreamA* iCa;
	TStream  iTc;
	CArrayFixFlat<TElement>* iArray;
	};


/**
@SYMTestCaseID          SYSLIB-STORE-PT-1369
@SYMTestCaseDesc	CR MRLM-6A9DF7. Tests the impact of RFileBuf cache size on CDirectFileStore performance. RFileBuf cache size must be set at a build time in EStor_Template.mmh file, DEFAULT_FILE_BUF_SIZE macro definition.
@SYMTestPriority 	High
@SYMTestActions  	The test creates a set of test objects, externalizes them STREAMRUNSIZE times and prints the execution time. Then internalizes them STREAMRUNSIZE times and prints the execution time.
STREAMRUNSIZE is a constant, defined in the source file.
@SYMTestExpectedResults Test must show a performance improvement
@SYMPREQ                PREQ1132
@SYMREQ			REQ4883
*/
LOCAL_C void doStreamingL()
    {
    TDriveUnit drive(static_cast<TUint>(RFs::GetSystemDrive()));	
	TParse streamsName;
	streamsName.Set(drive.Name(), &KStreamsName, NULL);
	TheStreamsFilePath.Copy(streamsName.FullName());
	
	// make sure directory exists
	TheFs.MkDirAll(streamsName.DriveAndPath());
	TTime start;
	start.HomeTime();
	for(int i=0; i<STREAMRUNSIZE;i++)
		{
		doExternalizeL(TheStreamsFilePath);	
		}
	TTime end;
	end.HomeTime();
	TTimeIntervalMicroSeconds us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CDirectFileStore, externalise %d streams. Time=%ld ms\r\n"), STREAMRUNSIZE, us.Int64() / 1000);

	start.HomeTime();
	for(int j=0; j<STREAMRUNSIZE;j++)
		{
		doInternalizeL(TheStreamsFilePath);
		}
	end.HomeTime();
	us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CDirectFileStore, internalise %d streams. Time=%ld ms\r\n"), STREAMRUNSIZE, us.Int64() / 1000);
	}

LOCAL_C void doExternalizeL(const TDesC& aName)
	{
	// construct file store object - the file to contain the store
	CFileStore* store = CDirectFileStore::ReplaceLC(TheFs,aName,EFileWrite);

	// Must say what kind of file store
    store->SetTypeL(KDirectFileStoreLayoutUid);

	// Construct an object of type CCompound and prepare some data into it
	CCompound* thecompound = CCompound::NewLC();

	_LIT(KTxtClassAText,"CStreamA text");

	thecompound->iCa->iBufferA = KTxtClassAText;
	thecompound->iCa->iXA      = -1;
	thecompound->iCa->iYA      = 2;
	thecompound->iTc.iZC       = 3.456;
	
	TElement theElement;
	_LIT(KFormatTxt,"BenchMarkingTheStream%4u");
	TBuf<256> str(KFormatTxt);
		
	for (TInt index = 0; index < STREAMRUNSIZE; index++)
		{
		theElement.iData.Format(KFormatTxt,index);
		thecompound->iArray->AppendL(theElement);
		}
					
	// Store the compound object to a single stream and save the stream id as the root id
	TStreamId  id = thecompound->StoreL(*store);

	// Set the stream id as the root
	store->SetRootL(id);

	// Commit changes to the store
	store->CommitL();
									// thecompound
	CleanupStack::PopAndDestroy(2); // store
	}

LOCAL_C void doInternalizeL(const TDesC& aName)
	{
	//Create file store object - specifying the file containing the store.
	CFileStore* store = CDirectFileStore::OpenLC(TheFs,aName,EFileRead);
	
	// Construct a CCompound object from the root stream created earlier.
	CCompound* thecompound = CCompound::NewL(*store,store->Root());
	
	// destroy the store object (this also closes the file containing the store) 
	CleanupStack::PopAndDestroy();
	// Now destroy the CCompound object
	delete thecompound;
	}

//***************************************************************

// The CCompound destructor
CCompound::~CCompound()
	{
	delete iCa;
	delete iArray;
	}

CCompound* CCompound::NewLC()
	{
	CCompound* self=new (ELeave) CCompound;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

void CCompound::ConstructL()
	{
	iCa = new (ELeave) CStreamA;
	iArray = new (ELeave) CArrayFixFlat<TElement>(10);
	}

// Construct a new CCompound object from the input stream.
CCompound* CCompound::NewL(CStreamStore& aStore,TStreamId anId)
	{
	CCompound* self=CCompound::NewLC(aStore,anId);
	CleanupStack::Pop();
	return self;
	}

// Construct CCompound object from the root stream of the store
CCompound* CCompound::NewLC(CStreamStore& aStore,TStreamId anId)
	{
	CCompound* self=new (ELeave) CCompound;
	CleanupStack::PushL(self);
	self->ConstructL(aStore,anId);
	return self;
	}

void CCompound::ConstructL(CStreamStore& aStore,TStreamId anId)
	{
	iCa = new (ELeave) CStreamA;
	// Restore here at construction
	RestoreL(aStore,anId);
	}

void CCompound::RestoreL(CStreamStore& aStore,TStreamId anId)
	{
	RStoreReadStream instream;
	instream.OpenLC(aStore,anId);
	InternalizeL(instream);
	// Cleanup the stream object
	CleanupStack::PopAndDestroy();			
	}

// Read the components and data members of the CCompound object from stream
void CCompound::InternalizeL(RReadStream& aStream)
	{
	aStream >> *iCa;
	aStream >> iTc;
	
	TElement theElement;
	iArray = new (ELeave) CArrayFixFlat<TElement>(STREAMRUNSIZE);
//	iArray->Count();
//	iArray->Reset();
	for (TInt index = 0; index < STREAMRUNSIZE ; index++)
		{
		aStream.ReadL(theElement.iData, 25);
		iArray->AppendL(theElement);
		}
	}

TStreamId CCompound::StoreL(CStreamStore& aStore)
	{
	RStoreWriteStream outstream;
	TStreamId id = outstream.CreateLC(aStore);
	// Stream out this CCompound object
	ExternalizeL(outstream);
	// Commit changes to the stream
	outstream.CommitL();
	// Cleanup the stream object.
	CleanupStack::PopAndDestroy();
	return id;
	}

// Write the components and data members of the CCompound object to stream
void CCompound::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << *iCa;
	aStream << iTc;
	
	TElement theElement;
	TInt count = iArray->Count();
	for (TInt index = 0; index < count; index++)
		{
		theElement =((*iArray)[index]);
		aStream.WriteL(theElement.iData, 25);
		}
	}

//***************************************************************

// Read the data members of the CStreamA  object from the stream.
void CStreamA::InternalizeL(RReadStream& aStream)
	{
	aStream >> iBufferA;
  	iXA = aStream.ReadInt32L();
	iYA = aStream.ReadUint32L();
	}  
// Write the data members of the CStreamA object to the stream
void CStreamA::ExternalizeL(RWriteStream& aStream)const
	{
	aStream << iBufferA;
	aStream.WriteInt32L(iXA);
	aStream.WriteUint32L(iYA);
	}  

//***************************************************************

// Write the data member(s) of the TStream object to the stream
void TStream::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteReal64L(iZC);
	}  

// Read the data member(s) of the TStream object from the stream.
void TStream::InternalizeL(RReadStream& aStream)
	{
	iZC = aStream.ReadReal64L();
	}

