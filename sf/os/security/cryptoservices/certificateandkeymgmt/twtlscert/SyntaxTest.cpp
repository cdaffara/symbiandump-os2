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
_LIT(KSyntaxLogFile, "WTLSCERTSyntaxLog.txt");
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
	delete iWriter;
	delete iSyntaxOut;
	iLogFile.Close();
	};

void CSyntaxTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	if(nInstances==0)
		{
		nInstances++;
		HBufC* body = HBufC::NewLC(aTestActionSpec.iActionBody.Length());

		body->Des().Copy(aTestActionSpec.iActionBody);
		TPtrC chainBuf = Input::ParseElement(*body, KPathStart);
		iPath.Copy(chainBuf);
		CleanupStack::PopAndDestroy();
		User::LeaveIfError(iLogFile.Replace(iFs,KSyntaxLogFile,EFileWrite));
		iSyntaxOut = new(ELeave) FileOutput(iLogFile);
		iWriter = new(ELeave) WTLSCertWriter(iSyntaxOut);
		}
	else
		{
		SetScriptError(ESyntax, _L("Only one syntax test can be run in each script"));
		iFinished = ETrue;
		}
	}


void CSyntaxTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	HBufC *searchPath = HBufC::NewLC(iPath.Size() + 1);
	TPtr searchPathPtr(searchPath->Des());
	searchPathPtr.Copy(iPath);
	searchPathPtr.Append(_L("*"));
    TInt err = iFs.GetDir(searchPathPtr, KEntryAttMaskSupported, ESortByName, iDirList);
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
	CleanupStack::PopAndDestroy();
	}

void CSyntaxTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CSyntaxTest::ReadAndSyntaxCheckL(const TDesC &aFilename)
	{
	iSyntaxOut->writeString(aFilename);
	iSyntaxOut->writeNewLine();
    HBufC8* buf = Input::ReadFileLC(aFilename, iPath, iFs);
	CWTLSCertificate* cert = CWTLSCertificate::NewLC(buf->Des());

	iWriter->WriteCert(*cert);
	CleanupStack::PopAndDestroy(2);//cert, buf
	}

void CSyntaxTest::PerformAction(TRequestStatus& aStatus)
	{
	TBuf<256> filename = (*iDirList)[nFileNumber].iName;

	TRAPD(error, ReadAndSyntaxCheckL(filename));

	if (error !=KErrNone)
		{
		iResult = EFalse;
		if (error == KErrArgument)
			{
			iConsole.Printf(_L("certificate file %S wrongly encoded\n"),&filename);
			iOut.writeString(filename);
			iOut.writeString(_L(" wrongly encoded"));
			iOut.writeNewLine();
			}
		if (error == KErrNotSupported)
			{
			
			iConsole.Printf(_L("certificate file %S not supported\n"),&filename);
			iOut.writeString(filename);
			iOut.writeString(_L(" not supported"));
			iOut.writeNewLine();
			}
		else
			{
			iConsole.Printf(_L("err=%d\n"), error);
			}
		}
	else
		{
		iSyntaxOut->writeNewLine();
		iConsole.Printf(_L("certificate %S parsed successfully\n"), &filename);
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

