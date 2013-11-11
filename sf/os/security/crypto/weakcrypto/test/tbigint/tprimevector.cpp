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
*
*/


#include "tprimevector.h"
#include "t_input.h"
#include <bigint.h>

CTestAction* CPrimeVector::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CPrimeVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CPrimeVector::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CPrimeVector* self = new(ELeave) CPrimeVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPrimeVector::~CPrimeVector()
	{
	delete iBody;
	delete iPrime;
	}

CPrimeVector::CPrimeVector(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CPrimeVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iPrime = Input::ParseElementHexL(*iBody, _L8("<prime>"));
	}

void CPrimeVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CPrimeVector::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CPrimeVector::DoReportAction(void)
	{
	}

void CPrimeVector::DoCheckResult(TInt /*aError*/)
	{
	iResult = (iResult && iExpectedResult) || (!iResult && !iExpectedResult);
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}


void CPrimeVector::PerformAction(TRequestStatus& aStatus)
	{
	PerformActionL();
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	}

void CPrimeVector::PerformActionL(void)
	{
	__UHEAP_MARK;
	iResult = ETrue;

	RInteger prime = RInteger::NewL(*iPrime);
	CleanupStack::PushL(prime);
	iResult = prime.IsPrimeL();

	CleanupStack::PopAndDestroy(&prime); 
	__UHEAP_MARKEND;
	}

