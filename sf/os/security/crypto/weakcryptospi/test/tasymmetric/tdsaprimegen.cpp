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


#include "tdsaprimegen.h"
#include "t_input.h"
#include <asymmetric.h>
#include <random.h>
#include <padding.h>
#include <bigint.h>
#include "tbrokenrandom.h"

CTestAction* CDSAPrimeGen::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDSAPrimeGen::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CDSAPrimeGen::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CDSAPrimeGen* self = new(ELeave) CDSAPrimeGen(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDSAPrimeGen::~CDSAPrimeGen()
	{
	delete iBody;
	iP.Close();
	iQ.Close();
	delete const_cast<CDSAPrimeCertificate*>(iPrimeCert);
	}

CDSAPrimeGen::CDSAPrimeGen(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CDSAPrimeGen::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	

	iKeyBits = Input::ParseIntElement(*iBody, _L8("<keybits>"), _L8("</keybits>"));

	HBufC8* p = Input::ParseElementHexL(*iBody, _L8("<p>"));
	CleanupStack::PushL(p);
	iP = RInteger::NewL(*p);
	CleanupStack::PopAndDestroy(p);

	HBufC8* q = Input::ParseElementHexL(*iBody, _L8("<q>"));
	CleanupStack::PushL(q);
	iQ = RInteger::NewL(*q);
	CleanupStack::PopAndDestroy(q);

	TUint counter = Input::ParseIntElement(*iBody, _L8("<c>"), _L8("</c>"));
	HBufC8* seed = Input::ParseElementHexL(*iBody, _L8("<seed>"));
	CleanupStack::PushL(seed);
	
	iExpectedResult = Input::ParseIntElement(*iBody, _L8("<result>"), _L8("</result>"));
	
	iPrimeCert = CDSAPrimeCertificate::NewL(*seed, counter);
	CleanupStack::PopAndDestroy(seed);
	}

void CDSAPrimeGen::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CDSAPrimeGen::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CDSAPrimeGen::DoReportAction(void)
	{
	}

void CDSAPrimeGen::DoCheckResult(TInt)
	{
	if (iResult)
		{
		iConsole.Printf(_L("."));
		}
	else if (iResult == iExpectedResult)
		{
		iKnownFailure = ETrue;
		iConsole.Printf(_L("."));
		}
	else
		{
		iConsole.Printf(_L("X"));
		}
	}

void CDSAPrimeGen::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	CRandomSetSource* rng = new(ELeave)CRandomSetSource(iPrimeCert->Seed());
	SetThreadRandomLC(rng);
	CDSAKeyPair* dsaPair = CDSAKeyPair::NewLC(iKeyBits);
	
	//this is more or less just calling the prime generation routine over again
	//but it does test if ValidatePrimesL and the NewLC generate the same primes
	iResult = dsaPair->PublicKey().ValidatePrimesL(*iPrimeCert);

	//we've already checked that the ValidatePrime + NewLC give same results
	//this now checks against the vectors in the test scripts
	if(iP != dsaPair->PublicKey().P() ||
		iQ != dsaPair->PublicKey().Q() || 
		iP != dsaPair->PrivateKey().P() ||
		iQ != dsaPair->PrivateKey().Q() )
		{
		iResult = EFalse;
		}
	
	// Get the prime certificate and compare the seed and counter values with the initial value supplied.
	const CDSAPrimeCertificate& primeCert = dsaPair->PrimeCertificate();

	if (((iPrimeCert->Seed()).Compare(primeCert.Seed()) != 0) ||
		(iPrimeCert->Counter() != primeCert.Counter()))
		{
		iResult = EFalse;
		}
	
	// Validate the prime length values. 
	if (CDSAParameters::ValidPrimeLength(256) ||  /* for <512 */
		CDSAParameters::ValidPrimeLength(1088) || /* for >1024 */
		CDSAParameters::ValidPrimeLength(775))    /* for non multiple of 64 */
		{
		iResult = EFalse;
		}

	CleanupStack::PopAndDestroy(dsaPair); 
	CleanupStack::PopAndDestroy(); //SetThreadRandomLC
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

