/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "tasymmetricmisc.h"
#include "t_input.h"
#include <asymmetric.h>
#include <random.h>
#include <padding.h>
#include <bigint.h>
#include "tbrokenrandom.h"

enum TActionMode {EDHKeyPair = 1, EDHPrivateKey =2, EDHPublicKey =3};

CTestAction* CAsymmetricMiscellaneous::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CAsymmetricMiscellaneous::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CAsymmetricMiscellaneous::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CAsymmetricMiscellaneous* self = new(ELeave) CAsymmetricMiscellaneous(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CAsymmetricMiscellaneous::~CAsymmetricMiscellaneous()
	{	
	delete iBody;	
	}

CAsymmetricMiscellaneous::CAsymmetricMiscellaneous(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CAsymmetricMiscellaneous::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	iActionMode = Input::ParseIntElement(*iBody, _L8("<actionmode>"), _L8("</actionmode>"));
	
	HBufC8* g = Input::ParseElementHexL(*iBody, _L8("<g>"));
	CleanupStack::PushL(g);
	iG = RInteger::NewL(*g);
	CleanupStack::PopAndDestroy(g);
	
	HBufC8* n = Input::ParseElementHexL(*iBody, _L8("<n>"));
	CleanupStack::PushL(n);
	iN = RInteger::NewL(*n);
	CleanupStack::PopAndDestroy(n);

	HBufC8* x = Input::ParseElementHexL(*iBody, _L8("<x>"));
	CleanupStack::PushL(x);
	iX = RInteger::NewL(*x);
	CleanupStack::PopAndDestroy(x);
	}

void CAsymmetricMiscellaneous::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CAsymmetricMiscellaneous::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CAsymmetricMiscellaneous::DoReportAction(void)
	{
	}

void CAsymmetricMiscellaneous::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CAsymmetricMiscellaneous::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;	
	TRequestStatus* status = &aStatus;
	iResult = ETrue;
	
	// Construction of dummy objects just to cover the left over methods.
	switch (iActionMode)
		{
		case EDHKeyPair:
			{
			CDHKeyPair* dhKeyPair = CDHKeyPair::NewLC(iN, iG, iX);
			if (dhKeyPair == NULL)
				{
				iResult = EFalse;
				}
			CleanupStack::PopAndDestroy(dhKeyPair);
			}
			break;
			
		case EDHPrivateKey:
			{			
			CDHPrivateKey* dhPrivateKey = CDHPrivateKey::NewLC(iN, iG, iX);
			if (dhPrivateKey == NULL)
				{
				iResult = EFalse;
				}
			CleanupStack::PopAndDestroy(dhPrivateKey);
			}
			break;
	
		case EDHPublicKey:
			{
			CDHPublicKey* dhPublicKeyPtr = CDHPublicKey::NewLC(iN, iG, iX);
			if (dhPublicKeyPtr == NULL)
				{
				iResult = EFalse;
				}
			CleanupStack::PopAndDestroy(dhPublicKeyPtr);
			}
			break;
		
		default:
			break;
		}
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}
