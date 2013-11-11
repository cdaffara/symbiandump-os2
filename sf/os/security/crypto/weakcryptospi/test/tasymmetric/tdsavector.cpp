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


#include "tdsavector.h"
#include "tvectorutils.h"
#include "t_input.h"
#include <bigint.h>
#include "tbrokenrandom.h"

const TInt KSha1HashLength = 20; 
////////////////////////////////////////////////////////////////////////////////
// CDSASignVector
////////////////////////////////////////////////////////////////////////////////

CTestAction* CDSASignVector::NewL(RFs& aFs,
                                  CConsoleBase& aConsole,
                                  Output& aOut, 
                                  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDSASignVector::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CDSASignVector::NewLC(RFs& aFs,
                                   CConsoleBase& aConsole,
                                   Output& aOut, 
                                   const TTestActionSpec& aTestActionSpec)
	{
	CDSASignVector* self = new(ELeave) CDSASignVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDSASignVector::~CDSASignVector()
	{
    delete iPrivKey;
	delete iSignature;
	delete iK;
	delete iSigInput;
	}

CDSASignVector::CDSASignVector(RFs& /*aFs*/, 
                               CConsoleBase& aConsole,
                               Output& aOut)					 
    : CVectorTest(aConsole, aOut)
	{
	}

void CDSASignVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPrivKey = VectorUtils::ReadDSAPrivateKeyL(aTestActionSpec.iActionBody);

	iMessage.Set(Input::ParseElement(aTestActionSpec.iActionBody, _L8("<m>")));
	iK = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<k>"));

	iSignature = VectorUtils::ReadDSASignatureL(aTestActionSpec.iActionBody);
	iSigInput = CHashingSignatureInput::NewL(CMessageDigest::ESHA1);
	iSigInput->Update(iMessage);
	}

void CDSASignVector::DoPerformActionL()
	{
	__UHEAP_MARK;

	CRandomSetSource* random = new(ELeave)CRandomSetSource(*iK);
	SetThreadRandomLC(random);

    CDSASigner* signer = CDSASigner::NewLC(*iPrivKey);
	const CDSASignature* testSig = signer->SignL(iSigInput->Final());
    iResult = (*testSig == *iSignature);
	delete testSig;
    CleanupStack::PopAndDestroy(signer);

    CDSASigner* signer1 = CDSASigner::NewL(*iPrivKey);
    CleanupStack::PushL(signer1);
    if (signer1->MaxInputLength() != KSha1HashLength)
    	{
        iResult = EFalse;
        }
    CleanupStack::PopAndDestroy(signer1); 
	CleanupStack::PopAndDestroy(); //SetThreadRandomLC

	__UHEAP_MARKEND;
	}

void CDSASignVector::DoPerformanceTestActionL()
	{
	iResult = ETrue;
	}

void CDSASignVector::DoCheckResult(TInt /*aError*/)
	{
//	If using Keith's fixed up testframework for testing failures, iResult will 
//	have already been corrected for a deliberate fail result.
//	If not using Keith's testframework iResult is still a fail result at this
//	point, so now's the time to adjust for deliberate failure.

	if (!iResult)
		iResult = (iResult && iExpectedResult) || (!iResult && !iExpectedResult);
	
	if( iResult == EFalse )
		{
		iConsole.Printf(_L("X"));
		}
	else 
		{
		iConsole.Printf(_L("."));
		}
	}

////////////////////////////////////////////////////////////////////////////////
// CDSAVerifyVector
////////////////////////////////////////////////////////////////////////////////

CTestAction* CDSAVerifyVector::NewL(RFs& aFs,
                                  CConsoleBase& aConsole,
                                  Output& aOut, 
                                  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CDSAVerifyVector::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CDSAVerifyVector::NewLC(RFs& aFs,
                                   CConsoleBase& aConsole,
                                   Output& aOut, 
                                   const TTestActionSpec& aTestActionSpec)
	{
	CDSAVerifyVector* self = new(ELeave) CDSAVerifyVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CDSAVerifyVector::~CDSAVerifyVector()
	{
    delete iPubKey;
	delete iSignature;
	delete iMessage;
	delete iSigInput;
	}

CDSAVerifyVector::CDSAVerifyVector(RFs& /*aFs*/, 
                                   CConsoleBase& aConsole,
                                   Output& aOut)					 
    : CVectorTest(aConsole, aOut)
	{
	}

void CDSAVerifyVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPubKey = VectorUtils::ReadDSAPublicKeyL(aTestActionSpec.iActionBody);

	TPtrC8 message(Input::ParseElement(aTestActionSpec.iActionBody, _L8("<m>")));
	if (message.Length()==0)	
		iMessage = Input::ParseElementHexL(aTestActionSpec.iActionBody, _L8("<hexm>"));
	else
		iMessage = message.AllocL();
		
	iSignature = VectorUtils::ReadDSASignatureL(aTestActionSpec.iActionBody);
	iSigInput = CHashingSignatureInput::NewL(CMessageDigest::ESHA1);
	iSigInput->Update(*iMessage);
	}	

void CDSAVerifyVector::DoPerformActionL()
	{
	__UHEAP_MARK;

    CDSAVerifier* verifier = CDSAVerifier::NewLC(*iPubKey);
    iResult = verifier->VerifyL(iSigInput->Final(), *iSignature);

    CleanupStack::PopAndDestroy(verifier);
	__UHEAP_MARKEND;
	}

void CDSAVerifyVector::DoPerformanceTestActionL()
	{
	iResult = ETrue;
	}

void CDSAVerifyVector::DoCheckResult(TInt /*aError*/)
	{
//	If using Keith's fixed up testframework for testing failures, iResult will 
//	have already been corrected for a deliberate fail result.
//	If not using Keith's testframework iResult is still a fail result at this
//	point, so now's the time to adjust for deliberate failure.

	if (!iResult)
		iResult = (iResult && iExpectedResult) || (!iResult && !iExpectedResult);
	
	if( iResult == EFalse )
		{
		iConsole.Printf(_L("X"));
		}
	else 
		{
		iConsole.Printf(_L("."));
		}
	}
