// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// T_StreamInStore.inl
// Demonstrates performance assesment of stream creation in non persistent store
// Create CStreamStoreA object and CStreamStoreB object, externalize them to
// a non-persistent store and then destroy the CStreamStoreA object
// 
//

LOCAL_C void doStoreL(CStreamStore& aStore,TStreamId& anId);

// Create an "empty" CStreamStoreA and CStreamStoreB objects and
// restore them from the in-memory store
LOCAL_C void doRestoreL(CStreamStore& aStore,const TStreamId& anId);

#define STREAMSTORERUNSIZE	121

// Declare example class CStreamStoreA
class CStreamStoreA : public CBase
	{
public :
	static CStreamStoreA* NewLC();
	~CStreamStoreA();
	void     SetTextL(const TDesC& aData);
	void     ExternalizeL(RWriteStream& aStream) const;
	void     InternalizeL(RReadStream& aStream);
public :
	HBufC*   iVarBuffer;
	TInt     iIntValue;
	TUint    iUintValue;
	};

// Declare example class CStreamStoreB
class CStreamStoreB : public CBase
	{
public :
	static CStreamStoreB* NewLC();
	~CStreamStoreB();
	void ExternalizeL(RWriteStream& aStream) const;
	void InternalizeL(RReadStream& aStream);
private:
	void		ConstructL();
public :
	TBuf<32> iFixBuffer;
	TUint    iUintValue;
	TInt     iIntValue;
	TReal    iRealValue;
	CArrayFixFlat<TElement>* iArray;
	};

/**
@SYMTestCaseID          SYSLIB-STORE-PT-1370
@SYMTestCaseDesc	CR MRLM-6A9DF7. Tests the impact of RFileBuf cache size on CBufStore performance. RFileBuf cache size must be set at a build time in EStor_Template.mmh file, DEFAULT_FILE_BUF_SIZE macro definition.
@SYMTestPriority 	High
@SYMTestActions  	The test creates a set of test objects, externalizes them STREAMRUNSIZE times and prints the execution time. Then internalizes them STREAMSTORERUNSIZE times and prints the execution time. STREAMSTORERUNSIZE is a constant, defined in the source file.
@SYMTestExpectedResults Test must show a performance improvement
@SYMPREQ                PREQ1132
@SYMREQ			REQ4883
*/
LOCAL_C void doStoreStreamsL()
    {
	// Stream id, kept in memory and used to access the 
	// (one and only stream) in non-persistent store
	TStreamId theId;

	// construct a CBufStore, an in-memory non-persistent store.
	CStreamStore* store = CBufStore::NewLC(228 /*granularity*/);
	TTime start;
	start.HomeTime();
	for(int i=0; i < STREAMSTORERUNSIZE; i++)
		{
		doStoreL(*store,theId);
		}
	TTime end;
	end.HomeTime();
	TTimeIntervalMicroSeconds us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CBufStore, %d 'write' tests. Time=%ld us\r\n"), STREAMSTORERUNSIZE, us.Int64());

	start.HomeTime();
	for(int i=0; i < STREAMSTORERUNSIZE; i++)
		{
		doRestoreL(*store,theId);
		}
	end.HomeTime();
	us = end.MicroSecondsFrom(start);
	TheTest.Printf(_L("CBufStore, %d 'read' tests.  Time=%ld us\r\n"), STREAMSTORERUNSIZE, us.Int64());

	// destroy the CBufStore object
	CleanupStack::PopAndDestroy();
	}

LOCAL_C void doStoreL(CStreamStore& aStore,TStreamId& anId)
	{
	// Create CStreamStoreA and put some data into it
	_LIT(KTxtForClassA,"Text for CStreamStoreA");
	CStreamStoreA* theA = CStreamStoreA::NewLC();
	theA->SetTextL(KTxtForClassA);
	theA->iIntValue  = -1;
	theA->iUintValue = 2;

	// Create CStreamStoreB and put some data into it
	_LIT(KTxtForClassB,"Text for CStreamStoreB");
	CStreamStoreB* theB = CStreamStoreB::NewLC();
	theB->iFixBuffer = KTxtForClassB;
	theB->iIntValue  = -3;
	theB->iUintValue = 4; 
	theB->iRealValue = 5.6;

	TElement theElement;
	_LIT(KFormatTxt,"BenchMarkingStreamStore%4u");
	TBuf<256> str(KFormatTxt);
	
	for (TInt index = 0; index < STREAMSTORERUNSIZE; index++)
		{
		theElement.iData.Format(KFormatTxt,index);
		theB->iArray->AppendL(theElement);
		}

	// Create the output stream. Stream id (the only one) is kept in memory.
	RStoreWriteStream outstream;
	anId = outstream.CreateLC(aStore);

	outstream  << *theA; 
	outstream  << *theB;
	outstream.CommitL();
						//outstream
						//theB
						//theA
	CleanupStack::PopAndDestroy(3);
	}

LOCAL_C void doRestoreL(CStreamStore& aStore,const TStreamId& anId)
	{
	__UHEAP_MARK;
	// Create  an "empty" CStreamStoreA and CStreamStoreB objects
	CStreamStoreA* theA = CStreamStoreA::NewLC();
	CStreamStoreB* theB = CStreamStoreB::NewLC();
		
	// Create/open the input stream. Access the one and only stream from in-memory store.
	RStoreReadStream instream;
	instream.OpenLC(aStore,anId);

	// Stream in CStreamStoreA object first and then stream in the CStreamStoreB object. 
	// This is the order in which the objects were streamed out.
	instream >> *theA;
	instream >> *theB;
									// instream
									// theB
	CleanupStack::PopAndDestroy(3); // theA 
	__UHEAP_MARKEND;
	}
									
//***************************************************************
CStreamStoreA* CStreamStoreA::NewLC()
	{
	CStreamStoreA* self = new (ELeave) CStreamStoreA;
	CleanupStack::PushL(self);
	return self;
	}
	
CStreamStoreA::~CStreamStoreA()
	{
	delete iVarBuffer;
	}

void CStreamStoreA::SetTextL(const TDesC& aData)
	{
	iVarBuffer = aData.AllocL();	
	}

void CStreamStoreA::ExternalizeL(RWriteStream& aStream) const
	{
	aStream.WriteInt32L(iVarBuffer->Des().MaxLength());
	aStream << *iVarBuffer;
	aStream.WriteInt32L(iIntValue);
	aStream.WriteUint32L(iUintValue);
	}
 
void CStreamStoreA::InternalizeL(RReadStream& aStream)
	{
	TInt maxlen;
	maxlen     = aStream.ReadInt32L();
	iVarBuffer = HBufC::NewL(aStream,maxlen);
	iIntValue  = aStream.ReadInt32L();
	iUintValue = aStream.ReadUint32L();
	}
	 	
//***************************************************************
CStreamStoreB* CStreamStoreB::NewLC()
	{
	CStreamStoreB* self = new (ELeave) CStreamStoreB;
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CStreamStoreB::~CStreamStoreB()
	{
	delete iArray;
	}

void CStreamStoreB::ConstructL()
	{
	iArray = new (ELeave) CArrayFixFlat<TElement> (STREAMSTORERUNSIZE);
	}

void CStreamStoreB::ExternalizeL(RWriteStream& aStream) const
	{
	aStream << iFixBuffer;
	aStream.WriteInt32L(iIntValue);
	aStream.WriteUint32L(iUintValue);
	aStream.WriteReal64L(iRealValue);
	
	TElement theElement;
	TInt count = iArray->Count();
	for (TInt index = 0; index < count; index++)
		{
		theElement =((*iArray)[index]);
		aStream.WriteL(theElement.iData, 27);
		}
	}
	 
void CStreamStoreB::InternalizeL(RReadStream& aStream)
	{
	aStream >> iFixBuffer;
	iIntValue  = aStream.ReadInt32L();
	iUintValue = aStream.ReadUint32L();
	iRealValue = aStream.ReadReal64L();

	TElement theElement;
	iArray->Reset();
	for (TInt index = 0; index <STREAMSTORERUNSIZE; index++)
		{
		aStream.ReadL(theElement.iData, 27);
		iArray->AppendL(theElement);
		}
	}
	

