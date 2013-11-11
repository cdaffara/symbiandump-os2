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


#include "tdsasignfb.h"
#include "t_input.h"
#include "t_output.h"
#include <asymmetric.h>
#include <random.h>
#include <padding.h>
#include <bigint.h>
#include "tbrokenrandom.h"

CTestAction* CDSASignFB::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDSASignFB::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CDSASignFB::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CDSASignFB* self = new(ELeave) CDSASignFB(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDSASignFB::~CDSASignFB()
	{
	delete iBody;
	delete iSeed;
	}

CDSASignFB::CDSASignFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs)
	{
	}

void CDSASignFB::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);
	

	iSeed = Input::ParseElementHexL(*iBody, _L8("<seed>"));
	iKeyBits = Input::ParseIntElement(*iBody, _L8("<keybits>"), _L8("</keybits>"));
	}

void CDSASignFB::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CDSASignFB::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	iFinished = ETrue;
	User::RequestComplete(status, KErrNone);
	}

void CDSASignFB::DoReportAction(void)
	{
	}

void CDSASignFB::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CDSASignFB::PerformAction(TRequestStatus& aStatus)
	{
	__UHEAP_MARK;
	TRequestStatus* status = &aStatus;
	iResult = EFalse;

	CRandomSetSource* rng = new(ELeave)CRandomSetSource(*iSeed);
	SetThreadRandomLC(rng);
	CDSAKeyPair* dsaPair = CDSAKeyPair::NewLC(iKeyBits);
	CDSASigner* signer = CDSASigner::NewLC(dsaPair->PrivateKey());
	CDSAVerifier* verifier = CDSAVerifier::NewLC(dsaPair->PublicKey());

	TBuf8<128> message(128);
	TRandom::RandomL(message);

	const CDSASignature* signature = signer->SignL(message);
	CleanupStack::PushL(const_cast<CDSASignature*>(signature));

	if(verifier->VerifyL(message, *signature))
		{
		iResult = ETrue;
		}
	else
		{
		iOut.writeString(_L("Failure in dsasignfb with following data:\n"));
		iOut.writeString(_L("input data: "));
		iOut.writeOctetStringL(message);
		iOut.writeString(_L("\n"));
		iOut.writeString(_L("seed: "));
		iOut.writeOctetStringL(*iSeed);
		iOut.writeString(_L("\n"));
		}
	
	CleanupStack::PopAndDestroy(const_cast<CDSASignature*>(signature));
	CleanupStack::PopAndDestroy(verifier);
	CleanupStack::PopAndDestroy(signer);
	CleanupStack::PopAndDestroy(dsaPair);
	CleanupStack::PopAndDestroy(); //SetThreadRandomLC
	
	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EPostrequisite;
	__UHEAP_MARKEND;
	}

void CDSASignFB::Hex(HBufC8& aString)
    {
    TPtr8 ptr=aString.Des();
    if (aString.Length()%2)
        {
        ptr.SetLength(0);
        return;
        }
    TInt i;
    for (i=0;i<aString.Length();i+=2)
        {
        TUint8 tmp;
        tmp=(TUint8)(aString[i]-(aString[i]>'9'?('A'-10):'0'));
        tmp*=16;
        tmp|=(TUint8)(aString[i+1]-(aString[i+1]>'9'?('A'-10):'0'));
        ptr[i/2]=tmp;
        }
    ptr.SetLength(aString.Length()/2);
    }

