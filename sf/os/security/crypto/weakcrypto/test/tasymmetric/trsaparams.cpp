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
* tdsasignfb.cpp
*
*/


#include "t_input.h"
#include "t_output.h"
#include <asymmetric.h>
#include "tvectorutils.h"
#include "trsaparams.h"

_LIT8(KModStart, "<modulus>");
_LIT8(KModEnd, "</modulus>");
_LIT8(KPubExpStart, "<publicExponent>");
_LIT8(KPubExpEnd, "</publicExponent>");
_LIT8(KPrivExpStart, "<privateExponent>");
_LIT8(KPrivExpEnd, "</privateExponent>");
_LIT8(KPStart, "<P>");
_LIT8(KPEnd, "</P>");
_LIT8(KQStart, "<Q>");
_LIT8(KQEnd, "</Q>");
_LIT8(KdPStart, "<dP>");
_LIT8(KdPEnd, "</dP>");
_LIT8(KdQStart, "<dQ>");
_LIT8(KdQEnd, "</dQ>");
_LIT8(KqInvStart, "<qInv>");
_LIT8(KqInvEnd, "</qInv>");
_LIT8(KReturnCodeStart, "<return>");
_LIT8(KReturnCodeEnd, "</return>");
_LIT8(KErrArgumentString, "KErrArgument");

CTestAction* CRSATestPublicKey::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSATestPublicKey::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSATestPublicKey::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CRSATestPublicKey* self = new(ELeave) CRSATestPublicKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSATestPublicKey::~CRSATestPublicKey()
	{
	delete iBody;
	}

CRSATestPublicKey::CRSATestPublicKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CRSATestPublicKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iExpectedReturnValue = KErrNone;
	TPtrC8 returnValue = Input::ParseElement(aTestActionSpec.iActionResult, KReturnCodeStart, KReturnCodeEnd);
	if(returnValue.Compare(KErrArgumentString) == 0)
		{
		iExpectedReturnValue = KErrArgument;
		}

	}

void CRSATestPublicKey::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CRSATestPublicKey::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CRSATestPublicKey::DoReportAction(void)
	{
	}

void CRSATestPublicKey::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CRSATestPublicKey::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	TPtrC8 modIn = Input::ParseElement(*iBody, KModStart, KModEnd);
    RInteger mod = VectorUtils::ParseIntegerL(modIn);
	CleanupStack::PushL(mod);

	TPtrC8 expIn = Input::ParseElement(*iBody, KPubExpStart, KPubExpEnd);
    RInteger exp = VectorUtils::ParseIntegerL(expIn);
	CleanupStack::PushL(exp);


	CRSAPublicKey* key = NULL;
	TRAPD(err, key = CRSAPublicKey::NewL(mod, exp));
	if(err == iExpectedReturnValue)
		{
		iResult=ETrue;
		}
	else
		{
		iOut.writeString(_L("CRSAPublicKey construction did not return expected result\n"));
		}
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		}
	else
		{
		CleanupStack::PushL(key);
		CleanupStack::PopAndDestroy(3);	
		}
		
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}


CTestAction* CRSATestPrivateKey::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSATestPrivateKey::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSATestPrivateKey::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CRSATestPrivateKey* self = new(ELeave) CRSATestPrivateKey(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSATestPrivateKey::~CRSATestPrivateKey()
	{
	delete iBody;
	}

CRSATestPrivateKey::CRSATestPrivateKey(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CRSATestPrivateKey::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iExpectedReturnValue = KErrNone;
	TPtrC8 returnValue = Input::ParseElement(aTestActionSpec.iActionResult, KReturnCodeStart, KReturnCodeEnd);
	if(returnValue.Compare(KErrArgumentString) == 0)
		{
		iExpectedReturnValue = KErrArgument;
		}

	}

void CRSATestPrivateKey::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CRSATestPrivateKey::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CRSATestPrivateKey::DoReportAction(void)
	{
	}

void CRSATestPrivateKey::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CRSATestPrivateKey::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	TPtrC8 modIn = Input::ParseElement(*iBody, KModStart, KModEnd);
    RInteger mod = VectorUtils::ParseIntegerL(modIn);
	CleanupStack::PushL(mod);

	TPtrC8 expIn = Input::ParseElement(*iBody, KPrivExpStart, KPrivExpEnd);
    RInteger exp = VectorUtils::ParseIntegerL(expIn);
	CleanupStack::PushL(exp);


	CRSAPrivateKeyStandard* key = NULL;
	TRAPD(err, key = CRSAPrivateKeyStandard::NewL(mod, exp));
	if(err == iExpectedReturnValue)
		{
		iResult=ETrue;
		}
	else
		{
		iOut.writeString(_L("CRSAPrivateKey construction did not return expected result\n"));
		}
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(2);
		}
	else
		{
		CleanupStack::PushL(key);
		CleanupStack::PopAndDestroy(3);	
		}
		
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

CTestAction* CRSATestPrivateKeyCRT::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSATestPrivateKeyCRT::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSATestPrivateKeyCRT::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CRSATestPrivateKeyCRT* self = new(ELeave) CRSATestPrivateKeyCRT(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSATestPrivateKeyCRT::~CRSATestPrivateKeyCRT()
	{
	delete iBody;
	}

CRSATestPrivateKeyCRT::CRSATestPrivateKeyCRT(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CRSATestPrivateKeyCRT::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iExpectedReturnValue = KErrNone;
	TPtrC8 returnValue = Input::ParseElement(aTestActionSpec.iActionResult, KReturnCodeStart, KReturnCodeEnd);
	if(returnValue.Compare(KErrArgumentString) == 0)
		{
		iExpectedReturnValue = KErrArgument;
		}

	}

void CRSATestPrivateKeyCRT::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CRSATestPrivateKeyCRT::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CRSATestPrivateKeyCRT::DoReportAction(void)
	{
	}

void CRSATestPrivateKeyCRT::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CRSATestPrivateKeyCRT::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	TPtrC8 modIn = Input::ParseElement(*iBody, KModStart, KModEnd);
    RInteger mod = VectorUtils::ParseIntegerL(modIn);
	CleanupStack::PushL(mod);

	TPtrC8 PIn = Input::ParseElement(*iBody, KPStart, KPEnd);
    RInteger P = VectorUtils::ParseIntegerL(PIn);
	CleanupStack::PushL(P);

	TPtrC8 QIn = Input::ParseElement(*iBody, KQStart, KQEnd);
    RInteger Q = VectorUtils::ParseIntegerL(QIn);
	CleanupStack::PushL(Q);

	TPtrC8 dPIn = Input::ParseElement(*iBody, KdPStart, KdPEnd);
    RInteger dP = VectorUtils::ParseIntegerL(dPIn);
	CleanupStack::PushL(dP);

	TPtrC8 dQIn = Input::ParseElement(*iBody, KdQStart, KdQEnd);
    RInteger dQ = VectorUtils::ParseIntegerL(dQIn);
	CleanupStack::PushL(dQ);

	TPtrC8 QInvIn = Input::ParseElement(*iBody, KqInvStart, KqInvEnd);
    RInteger QInv = VectorUtils::ParseIntegerL(QInvIn);
	CleanupStack::PushL(QInv);


	CRSAPrivateKeyCRT* key = NULL;
	TRAPD(err, key = CRSAPrivateKeyCRT::NewL(mod, P, Q, dP, dQ, QInv));
	if(err == iExpectedReturnValue)
		{
		iResult=ETrue;
		}
	else
		{
		iOut.writeString(_L("CRSAPrivateKeyCRT construction did not return expected result\n"));
		}
	if(err != KErrNone)
		{
		CleanupStack::PopAndDestroy(6);
		}
	else
		{
		CleanupStack::PushL(key);
		CleanupStack::PopAndDestroy(7);	
		}
		
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

