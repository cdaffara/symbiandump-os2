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


#include "trsavector.h"
#include "tvectorutils.h"
#include "t_input.h"
#include <bigint.h>
#include "performancetest.h"
#include "tbrokenrandom.h"
_LIT8(KPlaintextStart, "<plaintext>");
_LIT8(KPlaintextEnd, "</plaintext>");
_LIT8(KCiphertextStart, "<ciphertext>");
_LIT8(KCiphertextEnd, "</ciphertext>");
_LIT8(KSignatureStart, "<signature>");
_LIT8(KSignatureEnd, "</signature>");
_LIT8(KDigestInfoStart, "<digestInfo>");
_LIT8(KDigestInfoEnd, "</digestInfo>");

////////////////////////////////////////////////////////////////////////////////
// CRSAEncryptVector
////////////////////////////////////////////////////////////////////////////////

CTestAction* CRSAEncryptVector::NewL(RFs& aFs,
                               CConsoleBase& aConsole,
                               Output& aOut, 
                               const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSAEncryptVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSAEncryptVector::NewLC(RFs& aFs,
                                CConsoleBase& aConsole,
                                Output& aOut, 
                                const TTestActionSpec& aTestActionSpec)
	{
	CRSAEncryptVector* self = new(ELeave) CRSAEncryptVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSAEncryptVector::~CRSAEncryptVector()
	{
	delete iPubKey;
	delete iPlaintext;
	delete iCiphertext;
	}

CRSAEncryptVector::CRSAEncryptVector(RFs& /*aFs*/, 
                                     CConsoleBase& aConsole,
                                     Output& aOut)					 
: CVectorTest(aConsole, aOut)
	{
	}

void CRSAEncryptVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPubKey = VectorUtils::ReadRSAPublicKeyL(aTestActionSpec.iActionBody);

	TPtrC8 ptextIn = Input::ParseElement(aTestActionSpec.iActionBody, KPlaintextStart, KPlaintextEnd);
	iPlaintext = VectorUtils::ParseBinaryL(ptextIn);

	TPtrC8 ctextIn = Input::ParseElement(aTestActionSpec.iActionBody, KCiphertextStart, KCiphertextEnd);
	iCiphertext = VectorUtils::ParseBinaryL(ctextIn);
	}

void CRSAEncryptVector::DoPerformanceTestActionL()
{
	__UHEAP_MARK;

	CRSAPKCS1v15Encryptor* encryptor = CRSAPKCS1v15Encryptor::NewL(*iPubKey);
	CleanupStack::PushL(encryptor);
 
	TTimeIntervalMicroSeconds encryptTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

	TInt noEncryptions = 0;
	HBufC8 *eResult = HBufC8::NewLC(encryptor->MaxOutputLength());
	TPtr8 ePtr = eResult->Des();
	TPtr8* eResultPtr = &ePtr;
	
//	Time encryption	
	CRandomIncrementing* brokenRandom = new(ELeave)CRandomIncrementing(1);
	SetThreadRandomLC(brokenRandom);
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		eResultPtr->Zero();
		encryptor->EncryptL(*iPlaintext, *eResultPtr);
		noEncryptions++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	encryptTime = end.MicroSecondsFrom(start);
	TReal encrypttime = I64REAL(encryptTime.Int64());
	CleanupStack::PopAndDestroy(1); //SetThreadRandomLC
	
	iResult = ETrue;
	if (*iCiphertext!=*eResult)
	{
		iResult = EFalse;		
	}

    CleanupStack::PopAndDestroy(2);		// encryptor, eResult

	__UHEAP_MARKEND;

	if (iResult)
	{
		TReal rate = I64REAL(encryptTime.Int64()) / noEncryptions;
		TBuf<256> buf;
		_LIT(KEncryptTime, "\tEncrypt Time: %f us/encryption (%i encryptions in %f us)\r\n");
 		buf.Format(KEncryptTime, rate, noEncryptions, encrypttime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSAEncryptVector::DoPerformActionL()
	{
	__UHEAP_MARK;

	CRSAPKCS1v15Encryptor* encryptor = CRSAPKCS1v15Encryptor::NewL(*iPubKey);
	CleanupStack::PushL(encryptor);
    HBufC8* encryptBuf = HBufC8::NewLC(encryptor->MaxOutputLength());
	TPtr8 encryptPtr = encryptBuf->Des();
	CRandomIncrementing* brokenRandom = new(ELeave)CRandomIncrementing(1);
	SetThreadRandomLC(brokenRandom);
	encryptor->EncryptL(*iPlaintext, encryptPtr);
	CleanupStack::PopAndDestroy(1); //threadrandom;
    iResult = (*iCiphertext == *encryptBuf);

	CleanupStack::PopAndDestroy(encryptBuf);
    CleanupStack::PopAndDestroy(encryptor);

	__UHEAP_MARKEND;
	}

////////////////////////////////////////////////////////////////////////////////
// CRSADecryptVector
////////////////////////////////////////////////////////////////////////////////

CTestAction* CRSADecryptVector::NewL(RFs& aFs,
                               CConsoleBase& aConsole,
                               Output& aOut, 
                               const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSADecryptVector::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSADecryptVector::NewLC(RFs& aFs,
                                CConsoleBase& aConsole,
                                Output& aOut, 
                                const TTestActionSpec& aTestActionSpec)
	{
	CRSADecryptVector* self = new(ELeave) CRSADecryptVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSADecryptVector::~CRSADecryptVector()
	{
	delete iPrivKey;
	delete iPlaintext;
	delete iCiphertext;
	}

CRSADecryptVector::CRSADecryptVector(RFs& /*aFs*/, 
                                     CConsoleBase& aConsole,
                                     Output& aOut)					 
: CVectorTest(aConsole, aOut)
	{
	}

void CRSADecryptVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPrivKey = VectorUtils::ReadRSAPrivateKeyL(aTestActionSpec.iActionBody);

	TPtrC8 ctextIn = Input::ParseElement(aTestActionSpec.iActionBody, KCiphertextStart, KCiphertextEnd);
	iCiphertext = VectorUtils::ParseBinaryL(ctextIn);

	TPtrC8 ptextIn = Input::ParseElement(aTestActionSpec.iActionBody, KPlaintextStart, KPlaintextEnd);
	iPlaintext = VectorUtils::ParseBinaryL(ptextIn);
	}

void CRSADecryptVector::DoPerformanceTestActionL()
{
	__UHEAP_MARK;

    CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewL(*iPrivKey);
	CleanupStack::PushL(decryptor);

	TTimeIntervalMicroSeconds decryptTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

	HBufC8 *dResult = HBufC8::NewLC(decryptor->MaxOutputLength());
	TPtr8 dPtr = dResult->Des();
	TPtr8* dResultPtr = &dPtr;
	TInt noDecryptions = 0;

//	Time decryption	
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		decryptor->DecryptL(*iCiphertext, *dResultPtr);
		noDecryptions++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	decryptTime = end.MicroSecondsFrom(start);
	TReal decrypttime = I64REAL(decryptTime.Int64());
	iResult = ETrue;
	if (*iPlaintext!=*dResult)
	{
		iResult = EFalse;		
	}

    CleanupStack::PopAndDestroy(2);		// decryptor, dResult

	__UHEAP_MARKEND;
	
	if (iResult)
	{
		TReal rate = I64REAL(decryptTime.Int64()) / noDecryptions;
		TBuf<256> buf;
		_LIT(KDecryptTime, "\tDecrypt Time: %f us/decryption (%i decryptions in %f us)\r\n");
  		buf.Format(KDecryptTime, rate, noDecryptions, decrypttime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSADecryptVector::DoPerformActionL()
	{
	__UHEAP_MARK;

    CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewL(*iPrivKey);
	CleanupStack::PushL(decryptor);

	HBufC8* decryptBuf = HBufC8::NewLC(decryptor->MaxOutputLength());
	TPtr8 decryptPtr = decryptBuf->Des();
	TRAPD(err, decryptor->DecryptL(*iCiphertext, decryptPtr));
    iResult = (err == KErrNone) && (*iPlaintext == *decryptBuf);

 	CleanupStack::PopAndDestroy(decryptBuf);
 	CleanupStack::PopAndDestroy(decryptor);

	__UHEAP_MARKEND;
	}


////////////////////////////////////////////////////////////////////////////////
// CRSADecryptVectorCRT
////////////////////////////////////////////////////////////////////////////////
CTestAction* CRSADecryptVectorCRT::NewL(RFs& aFs,
                               CConsoleBase& aConsole,
                               Output& aOut, 
                               const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSADecryptVectorCRT::NewLC(aFs, aConsole,
		aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSADecryptVectorCRT::NewLC(RFs& aFs,
                                CConsoleBase& aConsole,
                                Output& aOut, 
                                const TTestActionSpec& aTestActionSpec)
	{
	CRSADecryptVectorCRT* self = new(ELeave) CRSADecryptVectorCRT(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSADecryptVectorCRT::~CRSADecryptVectorCRT()
	{
	delete iPrivKey;
	delete iPlaintext;
	delete iCiphertext;
	}

CRSADecryptVectorCRT::CRSADecryptVectorCRT(RFs& /*aFs*/, 
                                     CConsoleBase& aConsole,
                                     Output& aOut)					 
: CVectorTest(aConsole, aOut)
	{
	}

void CRSADecryptVectorCRT::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPrivKey = VectorUtils::ReadRSAPrivateKeyCRTL(aTestActionSpec.iActionBody);

	TPtrC8 ctextIn = Input::ParseElement(aTestActionSpec.iActionBody, KCiphertextStart, KCiphertextEnd);
	iCiphertext = VectorUtils::ParseBinaryL(ctextIn);

	TPtrC8 ptextIn = Input::ParseElement(aTestActionSpec.iActionBody, KPlaintextStart, KPlaintextEnd);
	iPlaintext = VectorUtils::ParseBinaryL(ptextIn);
	}

void CRSADecryptVectorCRT::DoPerformanceTestActionL()
{
	__UHEAP_MARK;

    CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewL(*iPrivKey);
	CleanupStack::PushL(decryptor);

	TTimeIntervalMicroSeconds decryptTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);
	
	TInt noDecryptions = 0;
	HBufC8 *dResult = HBufC8::NewLC(decryptor->MaxOutputLength());
	TPtr8 dPtr = dResult->Des();
	TPtr8* dResultPtr = &dPtr;
	
//	Time decryption	
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		decryptor->DecryptL(*iCiphertext, *dResultPtr);
		noDecryptions++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	decryptTime = end.MicroSecondsFrom(start);
	TReal decrypttime = I64REAL(decryptTime.Int64());
		
	iResult = ETrue;
	if (*iPlaintext!=*dResult)
	{
		iResult = EFalse;		
	}


    CleanupStack::PopAndDestroy(2);		// decryptor, dResult

	__UHEAP_MARKEND;
	
	if (iResult)
	{
		TReal rate = I64REAL(decryptTime.Int64()) / noDecryptions;
		TBuf<256> buf;
		_LIT(KDecryptTime, "\tDecrypt Time: %f us/decryption (%i decryptions in %f us)\r\n");
  		buf.Format(KDecryptTime, rate, noDecryptions, decrypttime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSADecryptVectorCRT::DoPerformActionL()
	{
	__UHEAP_MARK;

    CRSAPKCS1v15Decryptor* decryptor = CRSAPKCS1v15Decryptor::NewL(*iPrivKey);
	CleanupStack::PushL(decryptor);

	HBufC8* decryptBuf = HBufC8::NewLC(decryptor->MaxOutputLength());
	TPtr8 decryptPtr = decryptBuf->Des();
	TRAPD(err, decryptor->DecryptL(*iCiphertext, decryptPtr));
    iResult = (err == KErrNone) && (*iPlaintext == *decryptBuf);

 	CleanupStack::PopAndDestroy(decryptBuf);
 	CleanupStack::PopAndDestroy(decryptor);

	__UHEAP_MARKEND;
	}

////////////////////////////////////////////////////////////////////////////////
// CRSASignVector
////////////////////////////////////////////////////////////////////////////////

CTestAction* CRSASignVector::NewL(RFs& aFs,
                                  CConsoleBase& aConsole,
                                  Output& aOut, 
                                  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSASignVector::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSASignVector::NewLC(RFs& aFs,
                                   CConsoleBase& aConsole,
                                   Output& aOut, 
                                   const TTestActionSpec& aTestActionSpec)
	{
	CRSASignVector* self = new(ELeave) CRSASignVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSASignVector::~CRSASignVector()
	{
    delete iPrivKey;
	delete iDigestInfo;
	delete iSignature;
	}

CRSASignVector::CRSASignVector(RFs& /*aFs*/, 
                               CConsoleBase& aConsole,
                               Output& aOut)					 
    : CVectorTest(aConsole, aOut)
	{
	}

void CRSASignVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPrivKey = VectorUtils::ReadRSAPrivateKeyL(aTestActionSpec.iActionBody);

	TPtrC8 digestInfoIn = Input::ParseElement(aTestActionSpec.iActionBody, KDigestInfoStart, KDigestInfoEnd);
	iDigestInfo = VectorUtils::ParseBinaryL(digestInfoIn);

	TPtrC8 signatureIn = Input::ParseElement(aTestActionSpec.iActionBody, KSignatureStart, KSignatureEnd);
	RInteger integer = VectorUtils::ParseIntegerL(signatureIn);
	CleanupStack::PushL(integer);
	iSignature = CRSASignature::NewL(integer);
	CleanupStack::Pop(&integer);
	}

void CRSASignVector::DoPerformanceTestActionL()
{
	__UHEAP_MARK;

    CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewL(*iPrivKey);
	CleanupStack::PushL(signer);

	TPtrC8 digestPtr = iDigestInfo->Des();

	TTimeIntervalMicroSeconds signTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

	TInt noSignings = 0;

	const CRSASignature *testSig = 0;
	
//	Time signing
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		testSig = signer->SignL(digestPtr);
		delete testSig;
		noSignings++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	signTime = end.MicroSecondsFrom(start);
	TReal signtime = I64REAL(signTime.Int64());
	
	iResult = ETrue;
	testSig = signer->SignL(digestPtr);
	if (!(*testSig==*iSignature))
	{
		iResult = EFalse;		
	}
	delete testSig;

	CleanupStack::PopAndDestroy();		//	signer

	__UHEAP_MARKEND;
	
	if (iResult)
	{
		TReal rate = I64REAL(signTime.Int64()) / noSignings;
		TBuf<256> buf;
		_LIT(KSignTime, "\tSign Time: %f us/signing (%i signings in %f us)\r\n");
		buf.Format(KSignTime, rate, noSignings, signtime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSASignVector::DoPerformActionL()
	{
	__UHEAP_MARK;

	CRSAPKCS1v15Signer* digestSigner = CRSAPKCS1v15Signer::NewL(*iPrivKey);
	CleanupStack::PushL(digestSigner);
	
	TPtrC8 digestPtr2 = iDigestInfo->Des();
	const CRSASignature* testSig2 = digestSigner->SignL(digestPtr2);
    CleanupStack::PushL(const_cast<CRSASignature*>(testSig2));
    iResult = (*testSig2 == *iSignature);

	CleanupStack::PopAndDestroy(const_cast<CRSASignature*>(testSig2));
	CleanupStack::PopAndDestroy(digestSigner);

	__UHEAP_MARKEND;
	}

////////////////////////////////////////////////////////////////////////////////
// CRSASignVectorCRT
////////////////////////////////////////////////////////////////////////////////

CTestAction* CRSASignVectorCRT::NewL(RFs& aFs,
                                  CConsoleBase& aConsole,
                                  Output& aOut, 
                                  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSASignVectorCRT::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSASignVectorCRT::NewLC(RFs& aFs,
                                   CConsoleBase& aConsole,
                                   Output& aOut, 
                                   const TTestActionSpec& aTestActionSpec)
	{
	CRSASignVectorCRT* self = new(ELeave) CRSASignVectorCRT(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSASignVectorCRT::~CRSASignVectorCRT()
	{
    delete iPrivKey;
	delete iDigestInfo;
	delete iSignature;
	}

CRSASignVectorCRT::CRSASignVectorCRT(RFs& /*aFs*/, 
                               CConsoleBase& aConsole,
                               Output& aOut)					 
    : CVectorTest(aConsole, aOut)
	{
	}

void CRSASignVectorCRT::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPrivKey = VectorUtils::ReadRSAPrivateKeyCRTL(aTestActionSpec.iActionBody);

	TPtrC8 digestInfoIn = Input::ParseElement(aTestActionSpec.iActionBody, KDigestInfoStart, KDigestInfoEnd);
	iDigestInfo = VectorUtils::ParseBinaryL(digestInfoIn);

	TPtrC8 signatureIn = Input::ParseElement(aTestActionSpec.iActionBody, KSignatureStart, KSignatureEnd);
	RInteger integer = VectorUtils::ParseIntegerL(signatureIn);
	CleanupStack::PushL(integer);
	iSignature = CRSASignature::NewL(integer);
	CleanupStack::Pop(&integer);
	}

void CRSASignVectorCRT::DoPerformanceTestActionL()
{
	__UHEAP_MARK;

    CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewL(*iPrivKey);
	CleanupStack::PushL(signer);

	TPtrC8 digestPtr = iDigestInfo->Des();

	TTimeIntervalMicroSeconds signTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);
	TInt noSignings = 0;

	const CRSASignature *testSig = NULL;
	
//	Time signing
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		testSig = signer->SignL(digestPtr);
		delete testSig;
		noSignings++;
		end.UniversalTime();
		end.SecondsFrom(start, diff);
		}
	end.UniversalTime();
	signTime = end.MicroSecondsFrom(start);
	TReal signtime = I64REAL(signTime.Int64());
	
	testSig = signer->SignL(digestPtr);
	iResult = ETrue;
	if (!(*testSig==*iSignature))
	{
		iResult = EFalse;		
	}
	delete testSig;

	CleanupStack::PopAndDestroy(); // signer

	__UHEAP_MARKEND;
	
	if (iResult)
	{
		TReal rate = I64REAL(signTime.Int64()) / noSignings;
		TBuf<256> buf;
		_LIT(KSignTime, "\tSign Time: %f us/signing (%i signings in %f us)\r\n");
		buf.Format(KSignTime, rate, noSignings, signtime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSASignVectorCRT::DoPerformActionL()
	{
	__UHEAP_MARK;

	CRSAPKCS1v15Signer* signer = CRSAPKCS1v15Signer::NewL(*iPrivKey);
	CleanupStack::PushL(signer);

	TPtrC8 digestPtr = iDigestInfo->Des();
	const CRSASignature* testSig = signer->SignL(digestPtr);
    CleanupStack::PushL(const_cast<CRSASignature*>(testSig));	

#ifdef _DEBUG 
	HBufC8* theResultSig = testSig->S().BufferLC();
	HBufC8* theExpectedSig = iSignature->S().BufferLC();
	iResult = (theResultSig->Compare(*theExpectedSig)) ==0 ? ETrue : EFalse;
	CleanupStack::PopAndDestroy(2);	
#endif	
	
	iResult = (*testSig == *iSignature);

	CleanupStack::PopAndDestroy(const_cast<CRSASignature*>(testSig));
    CleanupStack::PopAndDestroy(signer);

	__UHEAP_MARKEND;
	}


////////////////////////////////////////////////////////////////////////////////
// CRSAVerifyVector
////////////////////////////////////////////////////////////////////////////////

CTestAction* CRSAVerifyVector::NewL(RFs& aFs,
                                  CConsoleBase& aConsole,
                                  Output& aOut, 
                                  const TTestActionSpec& aTestActionSpec)
	{
	CTestAction* self = CRSAVerifyVector::NewLC(aFs, aConsole, aOut, aTestActionSpec);
	CleanupStack::Pop();
	return self;
	}

CTestAction* CRSAVerifyVector::NewLC(RFs& aFs,
                                   CConsoleBase& aConsole,
                                   Output& aOut, 
                                   const TTestActionSpec& aTestActionSpec)
	{
	CRSAVerifyVector* self = new(ELeave) CRSAVerifyVector(aFs, aConsole, aOut);
	CleanupStack::PushL(self);
	self->ConstructL(aTestActionSpec);
	return self;
	}

CRSAVerifyVector::~CRSAVerifyVector()
	{
    delete iPubKey;
	delete iDigestInfo;
	delete iSignature;
	}

CRSAVerifyVector::CRSAVerifyVector(RFs& /*aFs*/, 
                                   CConsoleBase& aConsole,
                                   Output& aOut)					 
    : CVectorTest(aConsole, aOut)
	{
	}

void CRSAVerifyVector::ConstructL(const TTestActionSpec& aTestActionSpec)
	{
	CVectorTest::ConstructL(aTestActionSpec);

    iPubKey = VectorUtils::ReadRSAPublicKeyL(aTestActionSpec.iActionBody);

	TPtrC8 digestInfoIn = Input::ParseElement(aTestActionSpec.iActionBody, KDigestInfoStart, KDigestInfoEnd);
	iDigestInfo = VectorUtils::ParseBinaryL(digestInfoIn);

	TPtrC8 signatureIn = Input::ParseElement(aTestActionSpec.iActionBody, KSignatureStart, KSignatureEnd);
	RInteger integer = VectorUtils::ParseIntegerL(signatureIn);
	CleanupStack::PushL(integer);
	iSignature = CRSASignature::NewL(integer);
	CleanupStack::Pop(&integer);
	}

void CRSAVerifyVector::DoPerformanceTestActionL()
{
	__UHEAP_MARK;


	TTimeIntervalMicroSeconds verifyTime(0);
	TTime start, end;
	TTimeIntervalSeconds diff(0);
	const TTimeIntervalSeconds KIterationTime(iPerfTestIterations);

	TInt noVerifies = 0;
    
	CRSAPKCS1v15Verifier *verifier = CRSAPKCS1v15Verifier::NewLC(*iPubKey);	
	    	
//	Time verification
	start.UniversalTime();
	while (diff < KIterationTime)
		{
		iResult = verifier->VerifyL(*iDigestInfo, *iSignature);
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

	CleanupStack::PopAndDestroy(verifier);

	__UHEAP_MARKEND;
	
	if (iResult)
	{
		TReal rate = I64REAL(verifyTime.Int64()) / noVerifies;
		TBuf<256> buf;
		_LIT(KVerifyTime, "\tVerify Time: %f us/verify (%i verifies in %f us)\r\n");
		buf.Format(KVerifyTime, rate, noVerifies, verifytime);
		iOut.writeString(buf);
	}
	else
	{
		_LIT(KNoTimingInfo, "\tTest Failed! No benchmark data\n");
		iOut.writeString(KNoTimingInfo);
	}
}

void CRSAVerifyVector::DoPerformActionL()
	{
	__UHEAP_MARK;

	CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewL(*iPubKey);
	CleanupStack::PushL(verifier);
    iResult = verifier->VerifyL(*iDigestInfo, *iSignature);

	CleanupStack::PopAndDestroy(verifier);
    
	__UHEAP_MARKEND;
	}
