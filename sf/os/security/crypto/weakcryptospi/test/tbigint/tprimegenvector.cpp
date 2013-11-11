/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* tprimeproblem.cpp
*
*/


#include <bigint.h>
#include <random.h>
#include "tprimegenvector.h"
#include "t_input.h"
#include "t_output.h"
#include "tutils.h"
#include "../../source/bigint/mont.h"

CTestAction* CPrimeGenVector::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CPrimeGenVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CPrimeGenVector::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CPrimeGenVector* self = new(ELeave) CPrimeGenVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPrimeGenVector::~CPrimeGenVector()
	{
	delete iBody;
	delete iRandomDes;
	delete iPrimeDes;
	iRandom.Close();
	iPrime.Close();
	}

CPrimeGenVector::CPrimeGenVector(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CPrimeGenVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);

	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iBits = Input::ParseIntElement(*iBody, _L8("<bits>"), _L8("</bits>"));
	iRandomDes = Input::ParseElementHexL(*iBody, _L8("<random>"));
	iPrimeDes = Input::ParseElementHexL(*iBody, _L8("<prime>"));
	}

void CPrimeGenVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	iRandom = RInteger::NewL(*iRandomDes);
	iPrime = RInteger::NewL(*iPrimeDes);
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CPrimeGenVector::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CPrimeGenVector::DoReportAction(void)
	{
	}

void CPrimeGenVector::DoCheckResult(TInt)
	{
	iResult = (iResult && iExpectedResult) || (!iResult && !iExpectedResult);
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CPrimeGenVector::PerformAction(TRequestStatus& aStatus)
	{
	TRAPD(res, PerformActionL());
	__ASSERT_ALWAYS(!res, User::Panic(_L("CPrimeGenVector::PerformAction"), res));
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}

void CPrimeGenVector::PerformActionL()
	{
	__UHEAP_MARK;
	
	iResult = EFalse;;

	//Returned number should be the next prime after our initial "random" number

	//This CRandomSetSource stuff is a little dodgy as:
	// 1) it has no NewL's with is fine as long as it doesn't allocate memory
	// 2) more importantly, the prime generation routines use random numbers to
	// determine whether or not the numbers it generates are prime.
	// Since CRandomSetSource has already exhausted its given list of "random"
	// numbers it returns 0's for use with anything else that calls it.  It
	// makes the primality testing a little bogus if you were using it for real
	// things, but the test vectors that are there have already been verified
	// and are known to be prime.
	CRandomSetSource* incrementingRandom = new(ELeave)CRandomSetSource(*iRandomDes);
	SetThreadRandomLC(incrementingRandom);

	RInteger prime = RInteger::NewPrimeL(iBits);

	if(prime == iPrime)
		{
		iResult = ETrue;
		}
	prime.Close();

	CleanupStack::PopAndDestroy(1); //SetThreadRandomLC

	__UHEAP_MARKEND;
	}

