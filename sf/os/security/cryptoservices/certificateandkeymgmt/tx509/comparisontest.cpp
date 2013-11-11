/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "comparisontest.h"
#include "t_input.h"
#include "t_output.h"
#include <s32file.h>
#include <x509cert.h>

_LIT8(KCertificate1Start,"<certificate1>");
_LIT8(KCertificate2Start,"<certificate2>");
_LIT8(KMatchExpectedStart, "<matchexpected>");

CTestAction* CComparisonTest::NewL(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
/**
Factory method that creates a new CComparisonTest object.

@param 	aFs		 		Shared file server session requried by base class
@param 	aConsole 		The console used by the base class for logging
@param	aOut	 		Output utilities for use by base class
@param	aTestActionSpec	Parameters for this test	
@return a new instance of a CComparisonTest
*/
	{
	CTestAction* self = CComparisonTest::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop(self);
	return self;
	}

CTestAction* CComparisonTest::NewLC(RFs& aFs, CConsoleBase& aConsole, 
		Output& aOut, const TTestActionSpec& aTestActionSpec)
/**
Factory method that creates a new CComparisonTest object and places the pointer
to the this on the cleanup stack.
@param 	aFs				Shared file server session requried by base class
@param 	aConsole		The console used by the base class for logging
@param	aOut	 		Output utilities for use by base class	
@param	aTestActionSpec	Parameters for this test
@return a new instance of a CComparisonTest
*/
	{
	CComparisonTest* self = new(ELeave) CComparisonTest(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CComparisonTest::CComparisonTest(RFs& aFs, 
								 CConsoleBase& aConsole,
								 Output& aOut)
: CTestAction(aConsole, aOut), iFs(aFs)
/**
Constructor
@param	aFs			Shared file server session required by base-class
@param	aConsole 	Console implemenation required by base class
@param	aOut		Output utilities required by base class
*/
	{	
	}

void CComparisonTest::ConstructL(const TTestActionSpec& aTestActionSpec)
/**
Second phase constructor
@param	aTestActionSpec		parameters for this test base
*/
	{
	CTestAction::ConstructL(aTestActionSpec);
	HBufC8* body = HBufC8::NewLC(aTestActionSpec.iActionBody.Length());
	body->Des().Copy(aTestActionSpec.iActionBody);
	
	TPtrC8 cert1FileName = Input::ParseElement(*body, KCertificate1Start);
	if (! cert1FileName.Length() > 0)
		{
		SetScriptError(ESyntax, _L("Missing tag: certificate1"));
		iFinished = ETrue;
		return;
		}
	
	iCert1 = ReadCertificateL(cert1FileName);
	
	TPtrC8 cert2FileName = Input::ParseElement(*body, KCertificate2Start);
	if (! cert2FileName.Length() > 0)
		{
		SetScriptError(ESyntax, _L("Missing tag: certificate1"));
		iFinished = ETrue;
		return;
		}
	iCert2 = ReadCertificateL(cert2FileName);	
	iMatchExpected = Input::ParseElementBoolL(*body, KMatchExpectedStart);
	CleanupStack::PopAndDestroy(body);
	}

CX509Certificate* CComparisonTest::ReadCertificateL(const TDesC8& aFileName)
/**
Reads an X.509 certificate from a file.
@param	aFileName	The name of the certificate file.
@return	A pointer to the new certificate object.
*/
	{
	TFileName fn;
	fn.Copy(aFileName);
	
	iOut.write(_L("Loading: %S\n"), &fn);
	RFile file;
	User::LeaveIfError(file.Open(iFs, fn, EFileRead | EFileShareReadersOnly));		
	CleanupClosePushL(file);
	TInt size;
	User::LeaveIfError(file.Size(size));
	RBuf8 buf;
	buf.CreateL(size);
	CleanupClosePushL(buf);
	
	User::LeaveIfError(file.Read(buf, size));
	
	CX509Certificate* cert = CX509Certificate::NewL(buf);
	CleanupStack::PopAndDestroy(2, &file);
	return cert;
	}

CComparisonTest::~CComparisonTest()
/**
Destructor
*/
	{
	delete iCert1;
	delete iCert2;
	}

void CComparisonTest::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iActionState = EAction;
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	}

void CComparisonTest::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CComparisonTest::PerformAction(TRequestStatus& aStatus)
	{
	TRAPD(err, DoActionL());
	TRequestStatus* status = &aStatus;
	iActionState = EPostrequisite;
	User::RequestComplete(status, err);
	}

void CComparisonTest::DoActionL()
/**
Compares iCert1 and iCert2 using CX509Certificate::IsEqualL
*/
	{
	HBufC* subject1 = iCert1->SubjectL();
	CleanupStack::PushL(subject1);
	HBufC* subject2 = iCert2->SubjectL();
	CleanupStack::PushL(subject2);
	
	HBufC* issuer1 = iCert1->IssuerL();
	CleanupStack::PushL(issuer1);
	HBufC* issuer2 = iCert2->IssuerL();
	CleanupStack::PushL(issuer2);
	
	iConsole.Printf(_L("Comparing certificates\n"));
	iOut.write(_L("Comparing certificates\n"));

	iConsole.Printf(_L("cert1:\n\tsubject: %S\n\tissuer %S\n"), subject1, issuer1);
	iOut.write(_L("cert1:\n\tsubject: %S\n\tissuer %S\n"), subject1, issuer1);

	iConsole.Printf(_L("cert2:\n\tsubject: %S\n\tissuer %S\n"), subject2, issuer2);
	iOut.write(_L("cert2:\n\tsubject: %S issuer\n\t%S\n"), subject2, issuer2);
	
	TBool match = iCert1->IsEqualL(*iCert2);	
	iConsole.Printf(_L("Match expected %d, result %d\n"), iMatchExpected, match);
	iOut.write(_L("Match expected %d, result %d\n"), iMatchExpected, match);
	
	iResult = (match == iMatchExpected);

	if (iResult)
		{
		iConsole.Printf(_L(" Success\n"));
		iOut.writeString(_L(" Success\n"));
		}
	else
		{
		iConsole.Printf(_L(" Failed\n"));
		iOut.writeString(_L(" Failed"));
		};
	CleanupStack::PopAndDestroy(4, subject1);
	}

void CComparisonTest::DoReportAction()
	{
	}

void CComparisonTest::DoCheckResult(TInt /*aError*/)
	{
	}
