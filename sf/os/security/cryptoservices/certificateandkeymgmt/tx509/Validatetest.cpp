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


#include "ValidateTest.h"
#include "t_input.h"

_LIT(KFilenameStart, "<filename>");
_LIT(KExpectedKeyIDStart, "<expectedkeyid>");
_LIT(KResultStart, "<result>");

CTestAction* CValidateTest::NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CValidateTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CValidateTest::NewLC(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CValidateTest* self = new(ELeave) CValidateTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}


CValidateTest::CValidateTest(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CValidateTest::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	TInt pos=0, resultPos=0;
	HBufC* aBody = HBufC::NewLC(aTestActionSpec.iActionBody.Length());

	aBody->Des().Copy(aTestActionSpec.iActionBody);
	// creates the test chan validation object
	TPtrC ioFilename = Input::ParseElement(*aBody, KFilenameStart, pos);
	iFilename.Copy(ioFilename);
	TPtrC ioKeyId = Input::ParseElement(*aBody, KExpectedKeyIDStart, pos);	

	if(ioKeyId.Length() == 40)
		{
		iResultKeyID.SetLength(iResultKeyID.MaxLength());
		for(TInt byte =0; byte < 40; byte+=2)
			{
			TUint8 firstHalfOfOutput = ConvertByteA2H(static_cast<TInt8>(ioKeyId[byte]));
			TUint8 secondHalfOfOutput = ConvertByteA2H(static_cast<TInt8>(ioKeyId[byte+1]));
			iResultKeyID[resultPos] = static_cast<TInt8>((firstHalfOfOutput << 4)|secondHalfOfOutput);
			resultPos++;
			};
		}
	TPtrC ioResult = Input::ParseElement(*aBody, KResultStart, pos);	

	if(ioResult.CompareF(_L("Valid"))==0)
		iValid = ETrue;
	else
		iValid = EFalse;

	CleanupStack::PopAndDestroy(aBody);
	}

CValidateTest::~CValidateTest()
	{
	}

TUint8 CValidateTest::ConvertByteA2H(TUint8 aInput)
	{
	TUint8 output=0;
	if ((aInput > 0x29) && (aInput < 0x3a))
		{
		output = static_cast<TInt8>(aInput-0x30);
		}
	if ((aInput > 0x40) && (aInput < 0x47))
		{
		output = static_cast<TInt8>(aInput-0x37);
		}
	return output;
	}


void CValidateTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = EAction;
	}

void CValidateTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iFinished = ETrue;
	}

void CValidateTest::PerformAction(TRequestStatus& aStatus)
	{
	CX509Certificate* cert = NULL;
	TInt err;
	TFileName filename(iFilename);

	HBufC8* buf=NULL;
	TRAP(err, buf = Input::ReadFileL(filename, iFs));

	if (err == KErrNotFound)
		{
		iResult = EFalse;
		iFinished = ETrue;
		SetScriptError(EFileNotFound, iFilename);
		TRequestStatus* status = &aStatus;
		iActionState = EPostrequisite;
		User::RequestComplete(status, KErrNone);
		return;
		}
	else if (err != KErrNone)
		User::Leave(err);

	CleanupStack::PushL(buf);
	cert = CX509Certificate::NewLC(buf->Des());

	TKeyIdentifier id = cert->KeyIdentifierL();
	CleanupStack::PopAndDestroy(2); // buf & cert

	iConsole.Printf(_L("Validating file "));
	iConsole.Printf(iFilename);
	iOut.writeString(_L("Validating file "));
	iOut.writeString(iFilename);

	if(id == iResultKeyID)
		{
		iConsole.Printf(_L(" Match "));
		iOut.writeString(_L(" Match "));
		if(iValid)
			iResult = ETrue;
		else
			iResult = EFalse;
		}
	else
		{
		iConsole.Printf(_L(" No Match "));
		iOut.writeString(_L(" No Match "));
		if(!iValid)
			iResult = ETrue;
		else
			iResult = EFalse;
		}
	if(iResult)
		{
		iConsole.Printf(_L(" Success\n"));
		iOut.writeString(_L(" Success"));
		iOut.writeNewLine();
		}
	else
		{
		iConsole.Printf(_L(" Failed\n"));
		iOut.writeString(_L(" Failed"));
		iOut.writeNewLine();
		};

	TRequestStatus* status = &aStatus;
	iActionState = EPostrequisite;
	User::RequestComplete(status, KErrNone);
	}


void CValidateTest::DoReportAction()
	{
	}

void CValidateTest::DoCheckResult(TInt /*aError*/)
	{
	}

