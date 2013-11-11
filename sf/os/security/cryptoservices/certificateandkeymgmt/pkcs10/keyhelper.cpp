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


#include "keyhelper.h"
#include <asn1enc.h>
#include <asymmetric.h>
#include <x509cert.h>

// CPKCS10KeyHelper ////////////////////////////////////////////////////////////

CPKCS10KeyHelper* CPKCS10KeyHelper::CreateKeyHelperL(MCTKeyStore& aKeyStore,
													 const CCTKeyInfo& aKeyInfo,
													 const TDesC8& aExportedKey,
													 const TAlgorithmId aDigestId)
	{
	CPKCS10KeyHelper* result = NULL;
	
	switch (aKeyInfo.Algorithm())
		{
		case CCTKeyInfo::ERSA:
			result = new (ELeave) CPKCS10RSAKeyHelper(aKeyStore, aKeyInfo);
			break;

		case CCTKeyInfo::EDSA:
			result = new (ELeave) CPKCS10DSAKeyHelper(aKeyStore, aKeyInfo);
			break;

		default:
			User::Leave(KErrArgument);
		}

	CleanupStack::PushL(result);
	result->CreateKeyEncoderL(aExportedKey, aDigestId);
	CleanupStack::Pop(result);

	return result;
	}

CPKCS10KeyHelper::CPKCS10KeyHelper(MCTKeyStore& aKeyStore, const CCTKeyInfo& aKeyInfo) :
	iKeyStore(aKeyStore),
	iKeyInfo(aKeyInfo)
	{
	}

CPKCS10KeyHelper::~CPKCS10KeyHelper()
	{
	delete iKeyEncoder;
	}

CASN1EncBase* CPKCS10KeyHelper::EncodeKeyLC()
	{
	return iKeyEncoder->EncodeKeyLC();
	}

CASN1EncBase* CPKCS10KeyHelper::DigestInfoLC(const TDesC8& digest)
	{
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	
	// DigestAlgorithmIdentifier
	CASN1EncSequence* digestAlgID =iKeyEncoder-> EncodeDigestAlgorithmLC();

	seq->AddAndPopChildL(digestAlgID); 
	
	// Actual message digest	
	CASN1EncOctetString* octet = CASN1EncOctetString::NewLC(digest);
	seq->AddAndPopChildL(octet);
	
	return seq;
	}


CASN1EncSequence* CPKCS10KeyHelper::EncodeSignatureAlgorithmLC()
	{
	return iKeyEncoder->EncodeSignatureAlgorithmLC();
	}

// CPKCS10RSAKeyHelper /////////////////////////////////////////////////////////

CPKCS10RSAKeyHelper::CPKCS10RSAKeyHelper(MCTKeyStore& aKeyStore, const CCTKeyInfo& aKeyInfo) :
	CPKCS10KeyHelper(aKeyStore, aKeyInfo)
	{
	}

CPKCS10RSAKeyHelper::~CPKCS10RSAKeyHelper()
	{
	if (iRSASigner)
		{
		iRSASigner->Release();		
		}
	if (iDigestBuf) 
		{
		delete iDigestBuf;
		}
	delete iRSASignature;
	delete iPublicKey;	
	}

void CPKCS10RSAKeyHelper::OpenSigner(TRequestStatus& aStatus)
	{
	iKeyStore.Open(iKeyInfo, iRSASigner, aStatus);
	}

void CPKCS10RSAKeyHelper::CancelOpenSigner()
	{
	iKeyStore.CancelOpen();
	}

void CPKCS10RSAKeyHelper::SignDigestL(const TDesC8& aDigest, TRequestStatus& aStatus)
	{
	CASN1EncBase* digestInfo = DigestInfoLC(aDigest);
	
	// DER encode it!
	iDigestBuf = HBufC8::NewMaxL(digestInfo->LengthDER());
	TPtr8 oct(iDigestBuf->Des());
	oct.FillZ();

	TUint writePos = 0;
	digestInfo->WriteDERL(oct, writePos);
	
	// Sign the DER encoded digest info
	iRSASigner->Sign(*iDigestBuf, iRSASignature, aStatus);

	//CleanupStack::PopAndDestroy(octetData);	
	CleanupStack::PopAndDestroy(digestInfo);		
	}

void CPKCS10RSAKeyHelper::CancelSignDigest()
	{
	iRSASigner->CancelSign();
	}

void CPKCS10RSAKeyHelper::CreateKeyEncoderL(const TDesC8& aExportedKey, TAlgorithmId aDigestId)
	{
	CX509SubjectPublicKeyInfo* ki = CX509SubjectPublicKeyInfo::NewLC(aExportedKey);
	TX509KeyFactory factory; 
	iPublicKey = factory.RSAPublicKeyL(ki->KeyData());
	CleanupStack::PopAndDestroy(ki);
	iKeyEncoder = new (ELeave) TX509RSAKeyEncoder(*iPublicKey, aDigestId);
	}


CASN1EncBitString* CPKCS10RSAKeyHelper::EncodeSignatureLC()
	{
	// Get raw signature data
	HBufC8* sigData = iRSASignature->S().BufferLC();

	// Create ASN.1 bit string from the signature and return it.
	CASN1EncBitString* encSig = CASN1EncBitString::NewL(*sigData);
	CleanupStack::PopAndDestroy(sigData);
	CleanupStack::PushL(encSig);

	return encSig;
	}

// CPKCS10DSAKeyHelper /////////////////////////////////////////////////////////

CPKCS10DSAKeyHelper::CPKCS10DSAKeyHelper(MCTKeyStore& aKeyStore, const CCTKeyInfo& aKeyInfo) :
	CPKCS10KeyHelper(aKeyStore, aKeyInfo)
	{
	}

CPKCS10DSAKeyHelper::~CPKCS10DSAKeyHelper()
	{
	if (iDSASigner)
		iDSASigner->Release();
	delete iDSASignature;
	delete iPublicKey;
	}

void CPKCS10DSAKeyHelper::OpenSigner(TRequestStatus& aStatus)
	{
	iKeyStore.Open(iKeyInfo, iDSASigner, aStatus);
	}

void CPKCS10DSAKeyHelper::CancelOpenSigner()
	{
	iKeyStore.CancelOpen();
	}

void CPKCS10DSAKeyHelper::SignDigestL(const TDesC8& aDigest, TRequestStatus& aStatus)
	{
	iDSASigner->Sign(aDigest, iDSASignature, aStatus);
	}

void CPKCS10DSAKeyHelper::CancelSignDigest()
	{
	iDSASigner->CancelSign();
	}

void CPKCS10DSAKeyHelper::CreateKeyEncoderL(const TDesC8& aExportedKey, TAlgorithmId aDigestId)
	{
	CX509SubjectPublicKeyInfo* ki = CX509SubjectPublicKeyInfo::NewLC(aExportedKey);
	TX509KeyFactory factory; 
	iPublicKey = factory.DSAPublicKeyL(ki->EncodedParams(), ki->KeyData());
	CleanupStack::PopAndDestroy(ki);
	iKeyEncoder = new (ELeave) TX509DSAKeyEncoder(*iPublicKey, aDigestId);
	}

/**
 * Override default implementation - leave DSA parameters out of the
 * AlgorithmIdentifier when it appears outside of SubjectPublicKeyInfo.  
 */
CASN1EncSequence* CPKCS10DSAKeyHelper::EncodeSignatureAlgorithmLC()
	{
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	// Assume only SHA1 with DSA
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KDSAWithSHA1);
	seq->AddAndPopChildL(oid);

	// Don't add parameters!

	return seq;	
	}

CASN1EncBitString* CPKCS10DSAKeyHelper::EncodeSignatureLC()
	{
	// Create sequence that will hold the two bit integers.
	CASN1EncSequence* sigSeq = CASN1EncSequence::NewLC();
	// Stuff two signature integers into the sequence.

	CASN1EncBigInt* r = CASN1EncBigInt::NewLC(iDSASignature->R());
	sigSeq->AddAndPopChildL(r);
	CASN1EncBigInt* s = CASN1EncBigInt::NewLC(iDSASignature->S());
	sigSeq->AddAndPopChildL(s);

	// Wrap the sequence into a bit string
	// Create ASN.1 encoding from the signature and return it.
	CASN1EncBitString* sigDer = CASN1EncBitString::NewL(*sigSeq);

	CleanupStack::PopAndDestroy(sigSeq);
	CleanupStack::PushL(sigDer);

	return sigDer;
	}

