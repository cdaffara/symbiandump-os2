// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Example CTestStep derived implementation
//



/**
 @file te_perfoutputsanity.cpp
 @internalTechnology
*/
#include <e32math.h> //for cvs comparison of elements
#include "te_perfoutputsanity.h"
#include "te_ostv2integsuite_defs.h"
#include "te_perfcsvreader.h"
using namespace Ulogger; //CA:added so can use old ulogger api

COutputCSVSanityWrapper::COutputCSVSanityWrapper()
/**
 * Constructor
 */
	{
	}

COutputCSVSanityWrapper::~COutputCSVSanityWrapper()
/**
 * Destructor
 */
	{
	}

COutputCSVSanityWrapper* COutputCSVSanityWrapper::NewLC()
	{
	COutputCSVSanityWrapper* self = new (ELeave)COutputCSVSanityWrapper();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

COutputCSVSanityWrapper* COutputCSVSanityWrapper::NewL()
	{
	COutputCSVSanityWrapper* self=COutputCSVSanityWrapper::NewLC();
	CleanupStack::Pop(); // self;
	return self;
	}

void COutputCSVSanityWrapper::ConstructL()
	{
	}

TAny* COutputCSVSanityWrapper::GetObject()
	{
	return NULL;
	}

TBool COutputCSVSanityWrapper::DoCommandL(	const TTEFFunction& /*aCommand*/,
					const TTEFSectionName& /*aSection*/, 
					const TInt /*aAsyncErrorIndex*/)
	{
	
	if(BlockResult()==EPass)
		{
		SetBlockResult(TestBaselineCsvL());
		}
	
	return ETrue;
	}



TVerdict COutputCSVSanityWrapper::TestBaselineCsvL()
	{

	//read a sanity test file
	//read generated file
	//compare...
	
	//Create csvReference
	CCsvReader csvReference;
	TInt size = 0;
	//get the size of the reference file
#ifdef __WINSCW__
	TInt error = CsvFileSizeL(KReferenceWinscwCsvFile, size);
#else
	TInt error = CsvFileSizeL(KReferenceCsvFile, size);
#endif
	if(error)
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("TestBaselineCsvL, KReferenceCsvFile, test failed on with error code %d"), error);
		}
	//Set the size of the RBuf
	csvReference.Create(size);
	
	//Create csvGenerated
	CCsvReader csvGenerated;
	//get the size of the generated file
	error = CsvFileSizeL(KApiCSVFile, size);
	if(error)
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("TestBaselineCsvL, KApiCSVFile, test failed on with error code %d"), error);
		}
	//create the RBuf
	csvGenerated.Create(size);
	
	//Read the reference file into the RBuf
#ifdef __WINSCW__
	error = ReadCsvFileL(KReferenceWinscwCsvFile, csvReference.Buffer());
#else
	error = ReadCsvFileL(KReferenceCsvFile, csvReference.Buffer());
#endif
	if(error)
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("TestBaselineCsvL, ReadCsvFileL(KReferenceCsvFile) failed on with error code %d"), error);		
		}
	//Read the generated file into the RBuf
	error = ReadCsvFileL(KApiCSVFile, csvGenerated.Buffer());
	if(error)
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("TestBaselineCsvL, ReadCsvFileL(KApiCSVFile) failed on with error code %d"), error);		
		}

	//compare the two files
	if(!error)
		{
		TBool verdict = EFalse;
		RArray<TFailedResults> failedResults;
		CleanupClosePushL(failedResults);
		error = csvGenerated.CompareFile(csvReference, verdict, failedResults);
		
		if(verdict == EFalse)
			{
			for(TInt i = 0; i < failedResults.Count(); i++)
				{
				SetBlockResult(EFail);
				TFailedResults result = failedResults[i]; 
			//	TInt temp = result.iError;
				INFO_PRINTF5(_L("CompareFile failed, error %d, linenumber %d, elementtype %d, variation %d"), failedResults[i].iError, failedResults[i].iLineNumber, failedResults[i].iElementType, failedResults[i].iVariation);
				}		
			}
		CleanupStack::PopAndDestroy(&failedResults);
		}
			
	if(error)	
		{
		SetBlockResult(EFail);
		INFO_PRINTF2(_L("Output sanity test failed, with error code %d"), error);			
		}
	return BlockResult();
	}


TInt COutputCSVSanityWrapper::CsvFileSizeL(const TDesC& aFilename, TInt& aSize)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	TInt size = 0;
	TInt error = file.Open(fsSession, aFilename, EFileRead);
	if(!error)
		{
		file.Size(size);
		file.Close();
		fsSession.Close();
		}
	CleanupStack::PopAndDestroy();//fsSession
	aSize = size;
	return error;
	}

/*
 * Read a file and stick the data in a buffer and return that.
 * The caller of the method owns the buffer 
 * Ie. this method allocates the buffer but the caller is responsible for destroying it.
 */
TInt COutputCSVSanityWrapper::ReadCsvFileL(const TDesC& aFilename, RBuf8& aBuffer)
	{
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect()); 
	
	RFile file;
	TInt error = file.Open(fsSession, aFilename, EFileRead);
	if(error == KErrNone)
		{
		error = file.Read(aBuffer);
		file.Close();
		fsSession.Close();
		}
	CleanupStack::PopAndDestroy();//fsSession
	return error;
	}


// eof

