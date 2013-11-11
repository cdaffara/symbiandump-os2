/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tprimegen.h"
#include "t_input.h"
#include <bigint.h>
#include "tutils.h"
#include <random.h>

CTestAction* CPrimeGen::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CPrimeGen::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CPrimeGen::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CPrimeGen* self = new(ELeave) CPrimeGen(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CPrimeGen::~CPrimeGen()
	{
	delete iBody;
	}

CPrimeGen::CPrimeGen(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CPrimeGen::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iBits = Input::ParseIntElement(*iBody, _L8("<bits>"), _L8("</bits>"));
	}

void CPrimeGen::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CPrimeGen::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CPrimeGen::DoReportAction(void)
	{
	}

void CPrimeGen::DoCheckResult(TInt)
	{
	}

void CPrimeGen::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	RInteger prime = RInteger::NewPrimeL(iBits, TInteger::ETopBitSet);
	CleanupStack::PushL(prime);
	if(prime.BitCount() == iBits && prime.IsPrimeL())
		{
		iResult = ETrue;
		}

	CleanupStack::PopAndDestroy(&prime);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

