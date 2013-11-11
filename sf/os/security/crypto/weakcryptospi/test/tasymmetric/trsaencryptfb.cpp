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


#include "trsaencryptfb.h"
#include "t_input.h"
#include <asymmetric.h>
#include <padding.h>
#include <bigint.h>
#include "performancetest.h"
#include <securityerr.h>

_LIT8(KInputStart, "<input>");
_LIT8(KInputEnd, "</input>");
_LIT8(KKeyBitsStart, "<keybits>");
_LIT8(KKeyBitsEnd, "</keybits>");

CRSAEncryptFB::~CRSAEncryptFB()
	{
	delete iBody;
	}

CRSAEncryptFB::CRSAEncryptFB(RFs& aFs, CConsoleBase& aConsole, Output& aOut)
	: CTestAction(aConsole, aOut), iFs(aFs), iKeyBits(512)	//	Default key size = 512
	{
	}

CTestAction* CRSAEncryptFB::NewL(RFs& aFs, CConsoleBase& aConsole, Output& aOut, 
	const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSAEncryptFB::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSAEncryptFB::NewLC(RFs& aFs, CConsoleBase& aConsole,Output& aOut,
	const TTestActionSpec& aTestActionSpec)
	{
	CRSAEncryptFB* self = new(ELeave) CRSAEncryptFB(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

void CRSAEncryptFB::ConstructL(const TTestActionSpec& aTestActionSpec)
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

void CRSAEncryptFB::DoPerformPrerequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;

	TPtrC8 inputTemp = Input::ParseElement(*iBody, KInputStart, KInputEnd);
	iInput = HBufC8::NewL(inputTemp.Length());
	*iInput = inputTemp;
	Hex(*iInput);

	User::RequestComplete(status, KErrNone);
	iActionState = CTestAction::EAction;
	}

void CRSAEncryptFB::DoPerformPostrequisite(TRequestStatus& aStatus)
	{
	TRequestStatus* status = &aStatus;
	delete iInput;

	iFinished = ETrue;
	User::RequestComplete(status, iActionResult);
	}

void CRSAEncryptFB::DoReportAction(void)
	{
	}

void CRSAEncryptFB::DoCheckResult(TInt)
	{
	if (iResult)
		iConsole.Printf(_L("."));
	else
		iConsole.Printf(_L("X"));
	}

void CRSAEncryptFB::Hex(HBufC8& aString)
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


void CRSAEncryptFB::PerformAction(TRequestStatus& aStatus)
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
		{
		User::Leave(iActionResult);	//	For OOM testing
		}
	if (iActionResult==KErrKeyNotWeakEnough)
		{
		iResult = ETrue;
		iConsole.Printf(_L("Crypto libraries returned KErrKeyNotWeakEnough! Passing test automatically.\n\r"));
		iOut.writeString(_L("Crypto libraries returned KErrKeyNotWeakEnough!  Passing test automatically.\r\n"));
		}

	User::RequestComplete(status, iActionResult);

	iActionState = CTestAction::EPostrequisite;
}

void CRSAEncryptFB::DoPerformanceTestActionL()
{
	__UHEAP_MARK;
	iResult = EFalse;

	TTimeIntervalMicroSeconds keyCreateTime(0);
	TTimeIntervalMicroSeconds encryptTime(0);
	TTimeIntervalMicroSeconds decryptTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

//	Time key pair creation
	CRSAKeyPair *rsaPair = NULL;
	TInt noKeyPairCreations = 0;
	
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		rsaPair = CRSAKeyPair::NewLC(iKeyBits, TypeOfCrypto());
		CleanupStack::PopAndDestroy();
		noKeyPairCreations++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	keyCreateTime = end.MicroSecondsFrom(start);
	TReal keycreatetime = I64REAL(keyCreateTime.Int64());
	
	rsaPair = CRSAKeyPair::NewLC(iKeyBits, TypeOfCrypto()); // Create one keypair for operations
	
	CRSAPKCS1v15Encryptor* encryptor = CRSAPKCS1v15Encryptor::NewL(rsaPair->PublicKey());
	CleanupStack::PushL(encryptor);

	CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewL(rsaPair->PrivateKey());
	CleanupStack::PushL(decryptor);
	
	HBufC8 *eResult = HBufC8::NewLC(encryptor->MaxOutputLength());
	TPtr8 ePtr = eResult->Des();
	TPtr8* eResultPtr = &ePtr;

	
//	Time encryption	
	diff = 0;
	TInt noEncryptions = 0;
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		eResultPtr->Zero();
		encryptor->EncryptL(*iInput, *eResultPtr);
		noEncryptions++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	encryptTime = end.MicroSecondsFrom(start);
	TReal encrypttime = I64REAL(encryptTime.Int64());
		
//	Time decryption
	diff = 0;
	HBufC8 *dResult = HBufC8::NewLC(decryptor->MaxOutputLength());
	ePtr = eResult->Des();
	eResultPtr = &ePtr;
	TPtr8 dPtr = dResult->Des();
	TPtr8* dResultPtr = &dPtr;
	TInt noDecryptions = 0;
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		decryptor->DecryptL(*eResultPtr, *dResultPtr);
		noDecryptions++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	decryptTime = end.MicroSecondsFrom(start);
    TReal decrypttime = I64REAL(decryptTime.Int64());
	
	iResult = ETrue;
	if (*iInput != *dResult)
	{
		iResult = EFalse;		
	}
	
	CleanupStack::PopAndDestroy(5);	//	dResult,eResult,decryptor,encryptor,rsaPair
	__UHEAP_MARKEND;

	if (iResult)
	{
		TReal rate = I64REAL(keyCreateTime.Int64()) / noKeyPairCreations;
		TBuf<256> buf;
		_LIT(KKeyCreateTime, "\tKeyCreate\t%f");
  		buf.Format(KKeyCreateTime, rate);
		iOut.writeString(buf);
		
		rate = I64REAL(encryptTime.Int64()) / noEncryptions;
		_LIT(KEncryptTime, "\tEncrypt\t%f");
  		buf.Format(KEncryptTime, rate);
		iOut.writeString(buf);

		rate = I64REAL(decryptTime.Int64()) / noDecryptions;
		_LIT(KDecryptTime, "\tDecrypt\t%f");
  		buf.Format(KDecryptTime, rate);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSAEncryptFB::DoPerformActionL()
	{
	__UHEAP_MARK;
	
	iResult = EFalse;

	CRSAKeyPair* rsaPair = CRSAKeyPair::NewLC(iKeyBits, TypeOfCrypto());
	//LogKeyData(static_cast<const CRSAPublicKey*>(&rsaPair->PublicKey()), static_cast<const CRSAPrivateKeyCRT*>(&rsaPair->PrivateKey()), KNullDesC8);
	
	CRSAPKCS1v15Encryptor* encryptor = CRSAPKCS1v15Encryptor::NewL(rsaPair->PublicKey());
	CleanupStack::PushL(encryptor);

	CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewL(rsaPair->PrivateKey());
	CleanupStack::PushL(decryptor);

	HBufC8* eResult = HBufC8::NewLC(encryptor->MaxOutputLength());
	HBufC8* dResult = HBufC8::NewLC(decryptor->MaxOutputLength());

	TPtr8 eResultPtr = eResult->Des();
	encryptor->EncryptL(*iInput, eResultPtr);

	TPtr8 dResultPtr = dResult->Des();
	if (TypeOfCrypto() == EStandardCRT)
		{
		TRAPD(res, decryptor->DecryptL(eResultPtr, dResultPtr));
		if (res==KErrCorrupt)	//	Log all the key data for this failure
			LogKeyData(static_cast<const CRSAPublicKey*>(&rsaPair->PublicKey()), static_cast<const CRSAPrivateKeyCRT*>(&rsaPair->PrivateKey()), *eResult);

		User::LeaveIfError(res);
		}
	else
		{
		decryptor->DecryptL(eResultPtr, dResultPtr);
		}
		
	if(*iInput == *dResult)
		{
		iResult = ETrue;
		}

	// Check the input and output lenghts.
	if ((encryptor->MaxInputLength() == decryptor->MaxOutputLength()) && (encryptor->MaxOutputLength() == decryptor->MaxInputLength()))
		{
		iResult = ETrue;
		}
	else
		{
		iResult = EFalse;
		iOut.writeString(_L("\n\t RSA PKCS1 v15 Encryptor-Decryptor Max Input/Output length mismatch. \n\r"));
		}
	
	CleanupStack::PopAndDestroy(dResult);
	CleanupStack::PopAndDestroy(eResult);
	CleanupStack::PopAndDestroy(decryptor);
	CleanupStack::PopAndDestroy(encryptor);
	CleanupStack::PopAndDestroy(rsaPair);

	__UHEAP_MARKEND;
	}

void CRSAEncryptFB::LogKeyData(const CRSAPublicKey* aPublicKey, const CRSAPrivateKeyCRT* aPrivateKeyCRT, const TDesC8& aCipherText)
{
	iOut.writeString(_L("\n\t ----------- e -----------\n\r"));
	const TInteger& keyE = aPublicKey->E();
	HBufC8* theE = keyE.BufferLC();	
	iOut.writeOctetString(*theE);

	iOut.writeString(_L("\n\t ----------- P -----------\n\r"));
	const TInteger& keyP = aPrivateKeyCRT->P();
	HBufC8* theP = keyP.BufferLC();	
	iOut.writeOctetString(*theP);

	iOut.writeString(_L("\n\t ----------- Q -----------\n\r"));
	const TInteger& keyQ = aPrivateKeyCRT->Q();
	HBufC8* theQ = keyQ.BufferLC();	
	iOut.writeOctetString(*theQ);

	iOut.writeString(_L("\n\t ----------- DP -----------\n\r"));
	const TInteger& keyDP = aPrivateKeyCRT->DP();
	HBufC8* theDP = keyDP.BufferLC();	
	iOut.writeOctetString(*theDP);

	iOut.writeString(_L("\n\t ----------- DQ -----------\n\r"));
	const TInteger& keyDQ = aPrivateKeyCRT->DQ();
	HBufC8* theDQ = keyDQ.BufferLC();	
	iOut.writeOctetString(*theDQ);

	iOut.writeString(_L("\n\t ----------- QInv -----------\n\r"));
	const TInteger& keyQInv = aPrivateKeyCRT->QInv();
	HBufC8* theQInv = keyQInv.BufferLC();	
	iOut.writeOctetString(*theQInv);

	iOut.writeString(_L("\n\t ----------- N -----------\n\r"));
	const TInteger& keyN = aPrivateKeyCRT->N();
	HBufC8* theN = keyN.BufferLC();	
	iOut.writeOctetString(*theN);

	iOut.writeString(_L("\n\t ----------- ciphertext -----------\n\r"));	
	iOut.writeOctetString(aCipherText);
	
	iOut.writeString(_L("\n\t -------------------------\n\r"));
	
	CleanupStack::PopAndDestroy(6);	

}
