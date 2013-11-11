/*
* Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*
*/


#include "SyntaxTest.h"
#include "wtlscert.h"
#include "t_input.h"

TInt CSyntaxTest::nInstances = 0;
_LIT(KSyntaxLogFile, "X509SyntaxLog.txt");
_LIT(KPathStart, "<path>");

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestAction* CSyntaxTest::NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CSyntaxTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CSyntaxTest::NewLC(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CSyntaxTest* self = new(ELeave) CSyntaxTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CSyntaxTest::CSyntaxTest(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	nFileNumber = 0;
	}

CSyntaxTest::~CSyntaxTest(void)
	{
	delete iDirList;
	delete iSyntaxOut;
	iLogFile.Close();
	delete iWriter;
	iExpectedResults->ResetAndDestroy();
	delete iExpectedResults;
	};

void CSyntaxTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	if(nInstances==0)
		{
		nInstances++;
		HBufC* body = HBufC::NewLC(aTestActionSpec.iActionBody.Length());
		iExpectedResults = new (ELeave)RPointerArray<CSyntaxResult>;
		body->Des().Copy(aTestActionSpec.iActionBody);
		TPtrC chainBuf = Input::ParseElement(*body, KPathStart);
		iPath.Copy(chainBuf);
		CleanupStack::PopAndDestroy();
		User::LeaveIfError(iLogFile.Replace(iFs,KSyntaxLogFile,EFileWrite));
		iSyntaxOut = new(ELeave) FileOutput(iLogFile);
		iWriter = new(ELeave) CertWriter(iSyntaxOut);
		LoadResultsL();
		}
	else
		{
		SetScriptError(ESyntax, _L("Only one syntax test can be run in each script"));
		iFinished = ETrue;		
		}
	}

TBool CSyntaxTest::LoadResultsL()
	{
	RFile resultsFile;
	TFileName fullPath;
	TInt err, fileSize;
	HBufC8 *fileInfo;

	fullPath.Append(iPath);
	fullPath.Append(KResultsFile);

	err = resultsFile.Open(iFs, fullPath, EFileRead);
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error opening results file : "));
		iConsole.Printf(fullPath);
		iConsole.Printf(_L("\n"));
		return(EFalse);
		}

	CleanupClosePushL(resultsFile);
	resultsFile.Size(fileSize);
	CleanupStack::PopAndDestroy(); // resultsFile
	
	fileInfo = HBufC8::NewLC(fileSize);
	TPtr8 fileInfoPtr(fileInfo->Des());
	fileInfoPtr.SetLength(fileSize);

	RFileReadStream fileStream;
	User::LeaveIfError(fileStream.Open(iFs, fullPath, EFileStream));
	CleanupClosePushL(fileStream);
	fileStream.ReadL(fileInfoPtr, fileSize);

	TLex8 theLex(fileInfoPtr);
	CSyntaxResult *syntaxResult = NULL;
	
	do
		{
		syntaxResult = CSyntaxResult::NewLC();
		syntaxResult->SetFilename(theLex.NextToken());
		syntaxResult->SetResult(theLex.NextToken());
		iExpectedResults->Append(syntaxResult);
		CleanupStack::Pop(); // syntaxResult
		}
	while(!theLex.Eos());

	CleanupStack::PopAndDestroy(2);  // fileinfo & fileStream
	return(ETrue);
	};


void CSyntaxTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	HBufC *searchPath = HBufC::NewLC(iPath.Size() + 1);
	TPtr searchPathPtr(searchPath->Des());
	searchPathPtr.Copy(iPath);
	searchPathPtr.Append(_L("*"));
    TInt err = iFs.GetDir(searchPathPtr, KEntryAttNormal, ESortByName, iDirList);
	if (err != KErrNone)
		{
		iConsole.Printf(_L("Error getting directory "));
		iConsole.Printf(searchPathPtr);
		iConsole.Printf(_L("\n"));
		iOut.writeString(_L("Error getting directory "));
		iOut.writeString(searchPathPtr);
		iOut.writeNewLine();
		iFinished = ETrue;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		SetScriptError(EFileNotFound, searchPathPtr);
		}
	else
		{
		iConsole.Printf(_L("Please view "));
		iConsole.Printf(KSyntaxLogFile);
		iConsole.Printf(_L(" for results\n"));
		iOut.writeString(_L("Please view "));
		iOut.writeString(KSyntaxLogFile);
		iOut.writeString(_L(" for results"));
		iOut.writeNewLine();
		iActionState = EAction;
		TRequestStatus* status = &aStatus;
		User::RequestComplete(status, KErrNone);
		iResult = ETrue;
		}
	CleanupStack::PopAndDestroy(); // searchPath
	}

void CSyntaxTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CSyntaxTest::ReadAndSyntaxCheckL(const TDesC &aFilename)
	{
	HBufC8* buf = Input::ReadFileLC(aFilename, iPath, iFs);
	CX509Certificate* cert = CX509Certificate::NewLC(buf->Des());
	iWriter->WriteCert(*cert);
	
	TestInternalizeExternalizeL(cert);
	
	CleanupStack::PopAndDestroy(2, buf);
	};

//////////////////////////////////////////////////////////////////////////////////
//	Test fix for INC023303: CX509Certificate externalize - internalize don't work 
//////////////////////////////////////////////////////////////////////////////////
void CSyntaxTest::TestInternalizeExternalizeL(CX509Certificate* aCert)
	{
	RFileWriteStream newwriter;
	newwriter.PushL();
	User::LeaveIfError(newwriter.Replace(iFs,_L("x509stream"),EFileStream));
	
	aCert->ExternalizeL(newwriter);
	newwriter.CommitL();
	CleanupStack::PopAndDestroy(1);//newwriter
	
	RFileReadStream newreader;
	newreader.PushL();
	newreader.Open(iFs,_L("x509stream"),EFileStream);
	CX509Certificate* readCert=CX509Certificate::NewLC(newreader); //Use the stream to create new cert

	if (!readCert->IsEqualL(*aCert))
		User::Leave(KErrGeneral);

//	iFs.Delete(_L("x509stream"));
	CleanupStack::PopAndDestroy(2);	//	readCert, newreader
	}

//////////////////////////////////////////////////////////////////////////////////

void CSyntaxTest::PerformAction(TRequestStatus& aStatus)
	{
	TBuf<256> filename = (*iDirList)[nFileNumber].iName;

	// dont try and do the results.txt file
	if(filename.CompareF(KResultsFile)!=0)
		{
		TRAPD(error, ReadAndSyntaxCheckL(filename));

		if(error == KErrNoMemory)
			User::Leave(error);
		if(!CheckResult(filename, error))
			iResult = EFalse;
		}

	nFileNumber++;
	if(nFileNumber == iDirList->Count())
		{
		iActionState = EPostrequisite;
		};
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CSyntaxTest::DoReportAction()
	{
	iConsole.Printf(_L("\n"));
	}

void CSyntaxTest::DoCheckResult(TInt /*aError*/)
	{
	}


TBool CSyntaxTest::CheckResult(const TDesC &aFilename, const TInt &aError)
	{
	CSyntaxResult *syntaxResult;
	TPtrC filename;

	iConsole.Printf(aFilename);
	iOut.writeString(aFilename);

	for(TInt element = 0; element < iExpectedResults->Count(); element++)
		{
		syntaxResult = (*iExpectedResults)[element];
	
		syntaxResult->GetFilename(filename);
		if(filename.CompareF(aFilename)==0)
			{
			
			if(syntaxResult->Result() != aError)
				{	
				iConsole.Printf(_L(" FAILED. Expecting "));
				iOut.writeString(_L(" FAILED. Expecting "));
				PrintError(syntaxResult->Result());
				iConsole.Printf(_L("  Recieved "));
				iOut.writeString(_L(" Recieved "));
				PrintError(aError);
				iConsole.Printf(_L("\n"));
				iOut.writeNewLine();
				return(EFalse);
				}
			else
				{
				iConsole.Printf(_L(" Success\n"));
				iOut.writeString(_L(" Success "));
				iOut.writeNewLine();
				return(ETrue);
				}
			}
		}

	iConsole.Printf(_L(" FAILED to find expected result\n"));
	iOut.writeString(_L(" FAILED to find expected result"));
	iOut.writeNewLine();
	return(EFalse);
	};

void CSyntaxTest::PrintError(const TInt &aError)
	{
	switch(aError) 
		{
		case KErrArgument:
			{
			iConsole.Printf(_L(" Wrongly encoded"));
			iOut.writeString(_L(" Wrongly encoded"));
			break;
			}
		case KErrNotSupported:
			{
			iConsole.Printf(_L(" Not Supported"));
			iOut.writeString(_L(" Not Supported"));
			break;
			}
		case KErrNone:
			{
			iConsole.Printf(_L(" None "));
			iOut.writeString(_L(" None "));
			break;
			}
		default:
			{
			iConsole.Printf(_L(" UNKNOWN "));
			iOut.writeString(_L(" UNKNOWN "));
			}
		};
	}

void CSyntaxTest::Reset()
    {
    // nothing to do
    }
