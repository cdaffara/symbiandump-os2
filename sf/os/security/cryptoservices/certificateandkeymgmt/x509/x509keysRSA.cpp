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


#include <x509keys.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <x509cert.h>
#include "x509keyencoder.h"

CX509RSAPublicKey::CX509RSAPublicKey()
{}

//RSA public key
EXPORT_C CX509RSAPublicKey* CX509RSAPublicKey::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509RSAPublicKey::NewL(aBinaryData, pos);
	}

EXPORT_C CX509RSAPublicKey* CX509RSAPublicKey::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509RSAPublicKey::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509RSAPublicKey* CX509RSAPublicKey::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509RSAPublicKey* self = CX509RSAPublicKey::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509RSAPublicKey* CX509RSAPublicKey::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509RSAPublicKey* self = new(ELeave) CX509RSAPublicKey();
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509RSAPublicKey::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric gen(aBinaryData.Right(aBinaryData.Length() - aPos));
	gen.InitL();
	TInt end = aPos + gen.LengthDER();
	aPos += gen.LengthDERHeader();
	if (gen.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecInteger encInt;
	iN = encInt.DecodeDERLongL(aBinaryData, aPos);
	iE = encInt.DecodeDERLongL(aBinaryData, aPos);

	// RSA Public keys, modulus and exponent must be positive integers
	if(!iN.IsPositive() || !iE.IsPositive())
		{
		User::Leave(KErrArgument);
		}
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}

// Encodes public key to DER
EXPORT_C HBufC8* TASN1EncRSAPublicKey::EncodeDERL(const CRSAPublicKey& aKey) const
	{
	CASN1EncSequence* sequence = CASN1EncSequence::NewLC();
	CASN1EncBigInt* encModulus = CASN1EncBigInt::NewLC(aKey.N());
	sequence->AddAndPopChildL(encModulus);
	CASN1EncBigInt* encPublicExponent = CASN1EncBigInt::NewLC(aKey.E());
	sequence->AddAndPopChildL(encPublicExponent);
	HBufC8* der = HBufC8::NewMaxLC(sequence->LengthDER());
	TUint pos = 0;
	TPtr8 derptr(der->Des());
	sequence->WriteDERL(derptr, pos);
	CleanupStack::Pop(der);
	CleanupStack::PopAndDestroy(sequence);
	return der;
	}

// Decodes public key from DER
EXPORT_C CRSAPublicKey* TASN1DecRSAPublicKey::DecodeDERL(const TDesC8& aDER, 
														 TInt& aPos) const
	{
	// Enter into the containing SEQUENCE and verify if it is indeed there
	TASN1DecGeneric gen(aDER.Right(aDER.Length() - aPos));
	gen.InitL();
	TInt end = aPos + gen.LengthDER();
	aPos += gen.LengthDERHeader();
	if (gen.Tag() != EASN1Sequence)
		User::Leave(KErrArgument);

	// Decode modulus and public exponent (two large integers)
	TASN1DecInteger encInt;
	RInteger modulus = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(modulus);
	RInteger publicExponent = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(publicExponent);
	if (aPos != end)
		User::Leave(KErrArgument);

	// Construct a new key without copying
	CRSAPublicKey* key = CRSAPublicKey::NewL(modulus, publicExponent);
	CleanupStack::Pop(2); // modulus, publicExponent - owned by public key
	return key;
	}

// Decodes RSA key pair from DER-encoded buffer
EXPORT_C void TASN1DecRSAKeyPair::DecodeDERL(const TDesC8& aDER, 
												TInt& aPos, 
												CRSAPublicKey*& aPublicKey,
												CRSAPrivateKey*& aPrivateKey,
												TRSAPrivateKeyType aKeyType /*=EStandardCRT*/)
{
	aPublicKey = NULL;
	aPrivateKey = NULL;

	// Enter into the containing SEQUENCE and verify if it is 
	// indeed there
	TASN1DecGeneric gen(aDER.Right(aDER.Length() - aPos));
	gen.InitL();
	TInt end = aPos + gen.LengthDER();
	aPos += gen.LengthDERHeader();
	if (gen.Tag() != EASN1Sequence)
		User::Leave(KErrArgument);

	TASN1DecInteger encInt;
	
	// Decode and discard version, which is an integer
	encInt.DecodeDERShortL(aDER, aPos);

	// Decode public key components
	
	// Decode modulus
	RInteger publicModulus = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(publicModulus);
	
	// Decode public exponent
	RInteger publicExponent = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(publicExponent);

	//	Construct public key
	CRSAPublicKey* publicKey = CRSAPublicKey::NewL(publicModulus, publicExponent);
	CleanupStack::Pop(2, &publicModulus); // Now owned by publicKey
	CleanupStack::PushL(publicKey);

	// Decode private key components

	// Copy modulus
	RInteger privateModulus = RInteger::NewL(publicKey->N());
	CleanupStack::PushL(privateModulus);					  
	// Decode private exponent
	RInteger privateExponent = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(privateExponent);
	// Decode prime 1
	RInteger p = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(p);
	// Decode prime 2
	RInteger q = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(q);
	// Decode exponent 1
	RInteger dmp1 = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(dmp1);
	// Decode exponent 2
	RInteger dmq1 = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(dmq1);
	// Decode coefficient
	RInteger the_iqmp = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(the_iqmp);

	// We now should be at the end of the encoding. If not, the 
	// input encoding contains extra fields, and they are not 
	// supported.
	if (aPos != end)
		User::Leave(KErrArgument);
	
//	Construct private key
	CRSAPrivateKey* privateKey = NULL;
	if (EStandardCRT==aKeyType)
	{
		privateKey = CRSAPrivateKeyCRT::NewL(privateModulus, p, q, dmp1, dmq1, the_iqmp);
	}
	else if (EStandard==aKeyType)
	{
		privateKey = CRSAPrivateKeyStandard::NewL(privateModulus, privateExponent);
	}
	else
		User::Leave(KErrNotSupported);
	
	CleanupStack::Pop(8,publicKey);	 // publicKey, privateModulus, privateExponent,
	                                 // p, q, dmp1, dmq1, iqmp
	
//	Cleanup the TIntegers not owned by private key objects
	if (EStandard==aKeyType)
	{
		p.Close();
		q.Close();
		dmp1.Close();
		dmq1.Close();
		the_iqmp.Close();
	}
	else
	{
		privateExponent.Close();
	}

	aPublicKey = publicKey;
	aPrivateKey = privateKey;	
}

// TX509RSAKeyEncoder Class Implementation

EXPORT_C TX509RSAKeyEncoder::TX509RSAKeyEncoder(const CRSAPublicKey& aPublicKey, TAlgorithmId aDigestAlg)
	: TX509KeyEncoder(aDigestAlg),
	  iPublicKey(aPublicKey)
	{
	}

EXPORT_C CASN1EncBase* TX509RSAKeyEncoder::EncodeKeyLC() const
	{
	// Create higher-level sequence that will contain OID and the public key
	CASN1EncSequence* subjectPubKeyInfo = CASN1EncSequence::NewLC();

	// The next-level sequence will contain OID of the algorithm followed by NULL
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KRSA);
	seq->AddAndPopChildL(oid);
	CASN1EncNull* null = CASN1EncNull::NewLC();
	seq->AddAndPopChildL(null);
	subjectPubKeyInfo->AddAndPopChildL(seq);
	
	// Add the key itself to the higher-level sequence as a bit string
	// Obtain a copy of the entity's public key
	TASN1EncRSAPublicKey keyencoder;
	HBufC8* encoding = keyencoder.EncodeDERL(iPublicKey);
	CleanupStack::PushL(encoding);
	CASN1EncBitString* pubkeyenc = CASN1EncBitString::NewLC(*encoding);
	subjectPubKeyInfo->AddAndPopChildL(pubkeyenc);
	CleanupStack::PopAndDestroy(encoding);
	return subjectPubKeyInfo;
	}

// Returns ASN.1 sequence containing encoded signature algorithm.
EXPORT_C CASN1EncSequence* TX509RSAKeyEncoder::EncodeSignatureAlgorithmLC() const
	{
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = NULL;

	// Determine OID string for the current combination of algorithms.
	switch(iDigestAlg)
		{
		default:
			User::Leave(KErrNotSupported);
			break;

		case EMD2:
			oid = CASN1EncObjectIdentifier::NewLC(KMD2WithRSA);
			break;

		case EMD5:
			oid = CASN1EncObjectIdentifier::NewLC(KMD5WithRSA);
			break;

		case ESHA1:
			oid = CASN1EncObjectIdentifier::NewLC(KSHA1WithRSA);
			break;
		}

	// Add algorithm OID to the sequence.
	seq->AddAndPopChildL(oid);
	// Add NULL after OID.
	CASN1EncNull* null = CASN1EncNull::NewLC();
	seq->AddAndPopChildL(null);
	
	return seq;
	}
