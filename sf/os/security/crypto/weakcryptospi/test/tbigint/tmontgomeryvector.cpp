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


#include "tmontgomeryvector.h"
#include "t_input.h"
#include <bigint.h>
#include "../../source/bigint/mont.h"

CTestAction* CMontgomeryVector::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CMontgomeryVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CMontgomeryVector::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CMontgomeryVector* self = new(ELeave) CMontgomeryVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CMontgomeryVector::~CMontgomeryVector()
	{
	delete iBody;
	delete iA;
	delete iB;
	delete iModulus;
	delete iAns;
	}

CMontgomeryVector::CMontgomeryVector(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CMontgomeryVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iA = Input::ParseElementHexL(*iBody, _L8("<a>"));
	iB = Input::ParseElementHexL(*iBody, _L8("<b>"));
	iModulus = Input::ParseElementHexL(*iBody, _L8("<modulus>"));
	iAns = Input::ParseElementHexL(*iBody, _L8("<ans>"));
	TPtrC8 op = Input::ParseElement(*iBody, _L8("<op>"));
	if( op == _L8("multiply") )
		{
		iOp = EMultiply;
		}
	else if( op == _L8("square") )
		{
		iOp = ESquare;
		}
	else if( op == _L8("reduce") )
		{
		iOp = EReduce;
		}
	else if( op == _L8("exponentiate") )
		{
		iOp = EExponentiate;
		}
	else 
		{
		User::Panic(_L("tmontgomeryvector"), 1);
		}
	}

void CMontgomeryVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CMontgomeryVector::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CMontgomeryVector::DoReportAction(void)
	{
	}

void CMontgomeryVector::DoCheckResult(TInt)
	{
	}

void CMontgomeryVector::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = ETrue;

 	RInteger a = RInteger::NewL(*iA);
	CleanupStack::PushL(a);
	RInteger b = RInteger::NewL(*iB);
	CleanupStack::PushL(b);
	RInteger modulus = RInteger::NewL(*iModulus);
	CleanupStack::PushL(modulus);
	RInteger ans = RInteger::NewL(*iAns);
	CleanupStack::PushL(ans);
	CMontgomeryStructure* mont = CMontgomeryStructure::NewL(modulus);
	CleanupStack::PushL(mont);
	//we don't own out at any point, it remains the propery of mont
	const TInteger* out = 0;
	switch(iOp)
		{
		case EMultiply:
			out = &(mont->MultiplyL(a, b));
			break;
		case ESquare:
			out = &(mont->SquareL(a));
			break;
		case EReduce:
			out = &(mont->ReduceL(a));
			break;
		case EExponentiate:
			out = &(mont->ExponentiateL(a, b));
			break;
		default:
			User::Panic(_L("tbasicmathsvector"), 2);
			break;
		}

	if( *out != ans )
		{
		iResult = EFalse;
		}
	CleanupStack::PopAndDestroy(mont);
	CleanupStack::PopAndDestroy(4); //ans, modulus, b,a 

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

