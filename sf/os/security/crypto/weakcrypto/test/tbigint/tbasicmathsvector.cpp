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


#include "tbasicmathsvector.h"
#include "t_input.h"
#include <bigint.h>

CTestAction* CBasicMathsVector::NewL(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CBasicMathsVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CBasicMathsVector::NewLC(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut, const TTestActionSpec& aTestActionSpec)
	{
	CBasicMathsVector* self = new(ELeave) CBasicMathsVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CBasicMathsVector::~CBasicMathsVector()
	{
	delete iBody;
	delete iA;
	delete iB;
	delete iAns;
	}

CBasicMathsVector::CBasicMathsVector(RFs& aFs, CConsoleBase& aConsole, 
	Output& aOut) : CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CBasicMathsVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iA = Input::ParseElementHexL(*iBody, _L8("<a>"));
	iB = Input::ParseElementHexL(*iBody, _L8("<b>"));
	iAns = Input::ParseElementHexL(*iBody, _L8("<ans>"));
	TPtrC8 op = Input::ParseElement(*iBody, _L8("<op>"));
	if( op == _L8("add") )
		{
		iOp = EAdd;
		}
	else if( op == _L8("subtract") )
		{
		iOp = ESubtract;
		}
	else if( op == _L8("multiply") )
		{
		iOp = EMultiply;
		}
	else if( op == _L8("divide") )
		{
		iOp = EDivide;
		}
	else if( op == _L8("modulus") )
		{
		iOp = EModulus;
		}
	else if( op == _L8("gcd") )
		{
		iOp = EGCD;
		}
	else if( op == _L8("inversemod") )
		{
		iOp = EInverseMod;
		}
	else 
		{
		User::Panic(_L("tbasicmathsvector"), 1);
		}
	}

void CBasicMathsVector::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CBasicMathsVector::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CBasicMathsVector::DoReportAction(void)
	{
	}

void CBasicMathsVector::DoCheckResult(TInt)
	{
	}

void CBasicMathsVector::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = ETrue;

 	RInteger a = RInteger::NewL(*iA);
	CleanupStack::PushL(a);
	RInteger b = RInteger::NewL(*iB);
	CleanupStack::PushL(b);
	RInteger ans = RInteger::NewL(*iAns);
	CleanupStack::PushL(ans);
	RInteger result;
	switch(iOp)
		{
		case EAdd:
			a += b;
			break;
		case ESubtract:
			a -= b;
			break;
		case EMultiply:
			a *= b;
			break;
		case EDivide:
			a /= b;
			break;
		case EModulus:
			a %= b;
			break;
		case EGCD:
			result = a.GCDL(b);
			a.Set(result);
			break;
		case EInverseMod:
			result = a.InverseModL(b);
			a.Set(result);
			break;
		default:
			User::Panic(_L("tbasicmathsvector"), 2);
			break;
		}

	if( a != ans )
		{
		iResult = EFalse;
		}
	CleanupStack::PopAndDestroy(&ans);
	CleanupStack::PopAndDestroy(&b);
	CleanupStack::PopAndDestroy(&a);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

