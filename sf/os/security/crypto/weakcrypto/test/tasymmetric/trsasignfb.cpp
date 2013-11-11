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


#include "trsasignfb.h"
#include "t_input.h"
#include <asymmetric.h>
#include <padding.h>
#include <bigint.h>
#include "performancetest.h"

_LIT8(KInputStart, "<input>");
_LIT8(KKeyBitsStart, "<keybits>");
_LIT8(KKeyBitsEnd, "</keybits>");

CRSASignFB::~CRSASignFB()
	{
	delete iBody;
	}

CRSASignFB::CRSASignFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs), iPerfTestIterations(100)
	{
	}

CTestAction* CRSASignFB::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSASignFB::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSASignFB::NewLC(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CRSASignFB* self = new(ELeave) CRSASignFB(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}


TRSAPrivateKeyType CRSASignFB::TypeOfCrypto()
	{
	return iCryptoType;
	}
	
void CRSASignFB::ConstructL(const TTestActionSpec& aTestActionSpec)
{
	CTestAction::ConstructL(aTestActionSpec);
	iBody = HBufC8::NewL(aTestActionSpec.iActionBody.Length());
	iBody->Des().Copy(aTestActionSpec.iActionBody);

	if (CPerformance::PerformanceTester()->IsTestingPerformance())
	{//	Number of test iterations
		TPtrC8 itsPtr = Input::ParseElement(aTestActionSpec.iActionBody, KIterationsStart, KIterationsEnd);
		if (itsPtr!=KNullDesC8)
		{
			TLex8 lex;
			lex.Assign(itsPtr);
			User::LeaveIfError(lex.Val(iPerfTestIterations));
			if (iPerfTestIterations > KMaxIterations)
				User::Panic(_L("AsymmetricPerformance.exe"), KErrArgument);
			
		}
		TPtrC8 cryptoPtr = Input::ParseElement(aTestActionSpec.iActionBody, KTypeOfCryptoStart, KTypeOfCryptoEnd);
		if (cryptoPtr.CompareF(KRSAStandard) == 0)
			{
			iCryptoType = EStandard;
			}
		else if (cryptoPtr.CompareF(KRSAStandardCRT) == 0)
			{
			iCryptoType = EStandardCRT;
			}
		else
			{
			User::Panic(_L("AsymmetricPerformance.exe"), KErrArgument);
			}
	}

	TPtrC8 keyBitsPtr = Input::ParseElement(*iBody, KKeyBitsStart, KKeyBitsEnd); 
	if (keyBitsPtr!=KNullDesC8)
	{
		TLex8 lex;
		lex.Assign(keyBitsPtr);
		User::LeaveIfError(lex.Val(iKeyBits));
	}
}

void CRSASignFB::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	HBufC8* input = Input::ParseElementHexL(*iBody, KInputStart);
	CleanupStack::PushL(input);
	iSigInput = CHashingSignatureInput::NewL(CMessageDigest::ESHA1);
	iSigInput->Update(*input);
	iInput = iSigInput->Final().AllocL();
	CleanupStack::PopAndDestroy(input);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CRSASignFB::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	delete iInput;
	delete iSigInput;

	iFinished = ETrue;
	User::RequestComplete(status, iActionResult);
	}

void CRSASignFB::DoReportAction(void)
	{
	}

void CRSASignFB::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CRSASignFB::Hex(HBufC8& aString)
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

void CRSASignFB::PerformAction(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	if (CPerformance::PerformanceTester()->IsTestingPerformance())
	{
		iConsole.Printf(_L(">"));	//	Indicates start of test
		TRAP(iActionResult, DoPerformanceTestActionL());
	}
	else
	{
		TRAP(iActionResult, DoPerformActionL());
	}

	if (iActionResult==KErrNoMemory)
		User::Leave(iActionResult);	//	For OOM testing
	
	User::RequestComplete(status, iActionResult);
	iActionState = CTestAction::EPostrequisite;
	}


void CRSASignFB::DoPerformanceTestActionL()
{
	__UHEAP_MARK;
	iResult = EFalse;

	TTimeIntervalMicroSeconds keyCreateTime(0);
	TTimeIntervalMicroSeconds signTime(0);
	TTimeIntervalMicroSeconds verifyTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

	//	Time key pair creation
	CRSAKeyPair *rsaPair = NULL;
	TUint noRSAPairs = 0;
	start.UniversalTime();
	while (diff < KIterationTime)
	{
       rsaPair = CRSAKeyPair::NewLC(iKeyBits, TypeOfCrypto());
       CleanupStack::PopAndDestroy();
       noRSAPairs++;
	   end.UniversalTime();
	   end.SecondsFrom(start, diff);
	}
	end.UniversalTime();
	keyCreateTime = end.MicroSecondsFrom(start);
	TReal keycreatetime = I64REAL(keyCreateTime.Int64());
	
	rsaPair=CRSAKeyPair::NewLC(iKeyBits, TypeOfCrypto()); // Create one keypair for operations

	CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewL(rsaPair->PrivateKey());
	CleanupStack::PushL(signer);
	CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewL(rsaPair->PublicKey());
	CleanupStack::PushL(verifier);

	const CRSASignature  *testSig;

	//	Time signing
	diff = 0;
	TInt noSignings = 0;
	start.UniversalTime();
	while (diff < KIterationTime)
	{
		testSig = signer->SignL(*iInput);
		delete testSig;
		noSignings++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
	}
	end.UniversalTime();
	signTime = end.MicroSecondsFrom(start);
	TReal signtime = I64REAL(signTime.Int64());	
	//	Time verification
	TInt noVerifies = 0;
	diff = 0;
	testSig = signer->SignL(*iInput);
	CleanupStack::PushL(const_cast<CRSASignature*>(testSig));
	start.UniversalTime();
	while (diff < KIterationTime)
	{
		iResult = verifier->VerifyL(*iInput, *testSig);

		// do as many verfies as possible 
		if (!iResult)
		{
		   break;
		}
		noVerifies++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
	}	
	end.UniversalTime();
	verifyTime = end.MicroSecondsFrom(start);
    TReal verifytime = I64REAL(verifyTime.Int64());
	CleanupStack::PopAndDestroy(4);	//	verifier,signer,testSig,rsaPairs
	__UHEAP_MARKEND;
		 
	if (iResult)
	{
		TBuf<256> buf;
		TReal rate = I64REAL(keyCreateTime.Int64()) / noRSAPairs;
		
		_LIT(KKeyCreateTime, "\n\tKeyCreate: %f us/key creation (no creations: %i in %f us)\r\n");
 		buf.Format(KKeyCreateTime, rate, noRSAPairs, keycreatetime);
		iOut.writeString(buf);

		rate = I64REAL(signTime.Int64()) / noSignings;
		_LIT(KSignTime, "\tSigning: %f us/signing (no signings: %i in %f us)\r\n");
  		buf.Format(KSignTime, rate, noSignings, signtime);
		iOut.writeString(buf);
		
		rate = I64REAL(verifyTime.Int64()) / noVerifies;
		_LIT(KVerifyTime, "\tVerifying: %f us/verify (no verfies: %i in %f us)\r\n");
 		buf.Format(KVerifyTime, rate, noVerifies, verifytime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSASignFB::DoPerformActionL()
	{
	__UHEAP_MARK;
	
	iResult = EFalse;

	CRSAKeyPair* rsaPair = CRSAKeyPair::NewLC(512, TypeOfCrypto());

	CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewL(rsaPair->PrivateKey());
	CleanupStack::PushL(signer);

	CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewL(rsaPair->PublicKey());
	CleanupStack::PushL(verifier);

	const CRSASignature* signature = signer->SignL(*iInput);
	CleanupStack::PushL(const_cast<CRSASignature*>(signature));

	if(verifier->VerifyL(*iInput, *signature))
		{
		iResult = ETrue;
		}
	
	CleanupStack::PopAndDestroy(4, rsaPair); //signature, verifier, signer, rsaPair
	
	__UHEAP_MARKEND;
	}

