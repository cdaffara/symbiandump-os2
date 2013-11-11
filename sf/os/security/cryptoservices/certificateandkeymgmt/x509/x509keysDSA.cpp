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

//DSA public key
CX509DSAPublicKey::CX509DSAPublicKey()
{}

//dsa public key
EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewL(const TDesC8& aParamsData, const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DSAPublicKey::NewL(aParamsData, aBinaryData, pos);
	}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewLC(const TDesC8& aParamsData, const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DSAPublicKey::NewLC(aParamsData, aBinaryData, pos);
	}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewL(const TDesC8& aParamsData, const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DSAPublicKey* self = CX509DSAPublicKey::NewLC(aParamsData, aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewLC(const TDesC8& aParamsData, const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DSAPublicKey* self = new(ELeave) CX509DSAPublicKey;
	CleanupStack::PushL(self);
	self->ConstructL(aParamsData, aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewL(const CDSAParameters& aParams, const TDesC8& aBinaryData)
{
	TInt pos = 0;
	CX509DSAPublicKey* self = CX509DSAPublicKey::NewLC(aParams, aBinaryData, pos);
	CleanupStack::Pop();
	return self;
}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewLC(const CDSAParameters& aParams, const TDesC8& aBinaryData)
{
	TInt pos = 0;
	CX509DSAPublicKey* self = new(ELeave) CX509DSAPublicKey;
	CleanupStack::PushL(self);
	self->ConstructL(aParams, aBinaryData, pos);
	return self;
}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewL(const CDSAParameters& aParams, const TDesC8& aBinaryData, TInt& aPos)
{
	CX509DSAPublicKey* self = CX509DSAPublicKey::NewLC(aParams, aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
}

EXPORT_C CX509DSAPublicKey* CX509DSAPublicKey::NewLC(const CDSAParameters& aParams, const TDesC8& aBinaryData, TInt& aPos)
{
	CX509DSAPublicKey* self = new(ELeave) CX509DSAPublicKey;
	CleanupStack::PushL(self);
	self->ConstructL(aParams, aBinaryData, aPos);
	return self;
}

void CX509DSAPublicKey::ConstructL(const TDesC8& aParamsData, const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric genParams(aParamsData.Right(aParamsData.Length() - aPos));
	genParams.InitL();
	TInt end = aPos + genParams.LengthDER();
	aPos += genParams.LengthDERHeader();
	if (genParams.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecInteger encInt;
	iP = encInt.DecodeDERLongL(aParamsData, aPos);
	iQ = encInt.DecodeDERLongL(aParamsData, aPos);
	iG = encInt.DecodeDERLongL(aParamsData, aPos);
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}

	aPos = 0;
	TASN1DecGeneric gen(aBinaryData.Right(aBinaryData.Length() - aPos));
	gen.InitL();
	end = aPos + gen.LengthDER();
	iY = encInt.DecodeDERLongL(aBinaryData, aPos);
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}

void CX509DSAPublicKey::ConstructL(const CDSAParameters& aParams, const TDesC8& aBinaryData, TInt& aPos)
	{
	iP = RInteger::NewL(aParams.P());
	iQ = RInteger::NewL(aParams.Q());
	iG = RInteger::NewL(aParams.G());
	
	TASN1DecGeneric gen(aBinaryData.Right(aBinaryData.Length() - aPos));
	gen.InitL();
	TInt end = aPos + gen.LengthDER();
	TASN1DecInteger encInt;
	iY = encInt.DecodeDERLongL(aBinaryData, aPos);
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}


EXPORT_C CDSAParameters* CX509DSAPublicKey::DSAParametersL(const TDesC8& aParamsData)
{
	TInt pos = 0;
	TASN1DecGeneric genParams(aParamsData.Right(aParamsData.Length() - pos));
	genParams.InitL();
	TInt end = pos + genParams.LengthDER();
	pos += genParams.LengthDERHeader();
	if (genParams.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecInteger encInt;
	RInteger P = encInt.DecodeDERLongL(aParamsData, pos);
	CleanupStack::PushL(P);
	RInteger Q = encInt.DecodeDERLongL(aParamsData, pos);
	CleanupStack::PushL(Q);
	RInteger G = encInt.DecodeDERLongL(aParamsData, pos);
	CleanupStack::PushL(G);
	if (pos != end)
		{
		User::Leave(KErrArgument);
		}
	
	CDSAParameters* theDSAParams = CDSAParameters::NewL(P, Q, G);
	CleanupStack::Pop(3, &P);
	return (theDSAParams);
}

// Decodes DSA keys from DER-encoded buffer
EXPORT_C void TASN1DecDSAKeyPair::DecodeDERL(const TDesC8& aDER, TInt& aPos, 
													CDSAPublicKey*& aPublicKey, 
													CDSAPrivateKey*& aPrivateKey)
	{
	__UHEAP_MARK;
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

	// Decode parameters
	// Decode p parameter
	RInteger p = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(p);
	RInteger p1 = RInteger::NewL(p);
	CleanupStack::PushL(p1);
	
	// Decode q parameter
	RInteger q = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(q);
	RInteger q1 = RInteger::NewL(q);
	CleanupStack::PushL(q1);

	// Decode g parameter
	RInteger g = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(g);
	RInteger g1 = RInteger::NewL(g);
	CleanupStack::PushL(g1);

	// Decode private key x
	RInteger x = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(x);
	// Decode public key y
	RInteger y = encInt.DecodeDERLongL(aDER, aPos);
	CleanupStack::PushL(y);

	// We now should be at the end of the encoding. If not, the 
	// input encoding contains extra fields, and they are not 
	// supported.
	if (aPos != end)
		User::Leave(KErrArgument);

	// Construct DSA public key
	CDSAPublicKey* dsaPublic = CDSAPublicKey::NewL(p, q, g, y);
	CleanupStack::PushL(dsaPublic);

	// Construct DSA key pair
	CDSAPrivateKey* dsaPrivate = CDSAPrivateKey::NewL(p1, q1, g1, x);
	CleanupStack::Pop(10, &p);	//	dsaPublic...p	
	
	aPublicKey = dsaPublic;
	aPrivateKey = dsaPrivate;
	__UHEAP_MARKEND;
	}

// Encodes DSA public key to DER
EXPORT_C CASN1EncSequence* TASN1EncDSAPublicKey::EncodeDERL(const CDSAPublicKey& aKey) const
	{
	// Produce ASN.1 structure of DSA key parameters in the right order
	CASN1EncSequence* keySequence = CASN1EncSequence::NewLC();
	CASN1EncSequence* encParams = EncodeParamsLC(aKey);
	keySequence->AddAndPopChildL(encParams);
	// encode public key as a bit string
	CASN1EncBitString* pubKeyBitString = EncodePublicValueLC(aKey);
	keySequence->AddAndPopChildL(pubKeyBitString);
	CleanupStack::Pop(keySequence);
	return keySequence;
	}

// Encodes DSA parameters into ASN.1 sequence
EXPORT_C CASN1EncSequence* TASN1EncDSAPublicKey::EncodeParamsLC(const CDSAPublicKey& aKey) const
	{
	CASN1EncSequence* sequence = CASN1EncSequence::NewLC();
	CASN1EncBigInt* encParamP = CASN1EncBigInt::NewLC(aKey.P());
	sequence->AddAndPopChildL(encParamP);
	CASN1EncBigInt* encParamQ = CASN1EncBigInt::NewLC(aKey.Q());
	sequence->AddAndPopChildL(encParamQ);
	CASN1EncBigInt* encParamG = CASN1EncBigInt::NewLC(aKey.G());
	sequence->AddAndPopChildL(encParamG);
	return sequence;
	}

EXPORT_C CASN1EncBitString* TASN1EncDSAPublicKey::EncodePublicValueLC(const CDSAPublicKey& aKey) const
	{
	CASN1EncBigInt* bigint = CASN1EncBigInt::NewLC(aKey.Y());
	CASN1EncBitString* pubKeyBitString = CASN1EncBitString::NewL(*bigint);
	CleanupStack::PopAndDestroy(bigint);
	CleanupStack::PushL(pubKeyBitString);
	return pubKeyBitString;
	}

CX509DSASignature::CX509DSASignature()
{}

//DSA signature
EXPORT_C CX509DSASignature* CX509DSASignature::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DSASignature::NewL(aBinaryData, pos);
	}

EXPORT_C CX509DSASignature* CX509DSASignature::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509DSASignature::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509DSASignature* CX509DSASignature::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DSASignature* self = CX509DSASignature::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509DSASignature* CX509DSASignature::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509DSASignature* self = new(ELeave) CX509DSASignature;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509DSASignature::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
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
	iR = encInt.DecodeDERLongL(aBinaryData, aPos);
	iS = encInt.DecodeDERLongL(aBinaryData, aPos);
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}

//
// TX509DSAKeyEncoder Class Implementation
//

EXPORT_C TX509DSAKeyEncoder::TX509DSAKeyEncoder(const CDSAPublicKey& aKeyPublic, 
												const TAlgorithmId aDigestAlg)
	: TX509KeyEncoder(aDigestAlg),
	  iPublicKey(aKeyPublic)
	{}

EXPORT_C CASN1EncBase* TX509DSAKeyEncoder::EncodeKeyLC() const
	{
	// Create higher-level sequence that will contain OID and the public key
	CASN1EncSequence* subjectPubKeyInfo = CASN1EncSequence::NewLC();
	
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KDSA);
	seq->AddAndPopChildL(oid);
	
	// the next sequence will contain DSA parameters
	TASN1EncDSAPublicKey keyEnc;
	CASN1EncSequence* seqParams = keyEnc.EncodeParamsLC(iPublicKey);
	seq->AddAndPopChildL(seqParams);
	subjectPubKeyInfo->AddAndPopChildL(seq);
	
	// Add the key itself to the higher-level sequence as a bit string
	CASN1EncBigInt* pubPart = CASN1EncBigInt::NewLC(iPublicKey.Y());
	HBufC8* encoding = HBufC8::NewMaxLC(pubPart->LengthDER());
	TPtr8 encodingPtr = encoding->Des();
	TUint pos = 0;
	pubPart->WriteDERL(encodingPtr, pos);
	CASN1EncBitString* pubkeyenc = CASN1EncBitString::NewLC(*encoding);
	subjectPubKeyInfo->AddAndPopChildL(pubkeyenc);
	CleanupStack::PopAndDestroy(2); // encoding, pubPart
	return subjectPubKeyInfo;
	}

EXPORT_C CASN1EncSequence* TX509DSAKeyEncoder::EncodeSignatureAlgorithmLC() const
	{
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = NULL;

	// Determine OID string for the current combination of algorithms.
	switch(iDigestAlg)
		{
		default:
			User::Leave(KErrNotSupported);
			break;

		case ESHA1:
			oid = CASN1EncObjectIdentifier::NewLC(KDSAWithSHA1);
			break;
		}

	// Add algorithm OID to the sequence.
	seq->AddAndPopChildL(oid);
	// Insert p, q, and g big parameters into the sequence
	TASN1EncDSAPublicKey keyEnc;
	CASN1EncSequence* params = keyEnc.EncodeParamsLC(iPublicKey);
	seq->AddAndPopChildL(params);
	
	return seq;
	}
