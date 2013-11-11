/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <asn1dec.h>
#include <x509keys.h>
#include <x509cert.h>
#include "asnpkcs.h"
#include <pbe.h>
#include <pbedata.h>
#include <keyidentifierutil.h>

/*static*/ EXPORT_C CDecPKCS8Data* TASN1DecPKCS8::DecodeDERL(const TDesC8& aBinaryData)
{
	return (CDecPKCS8Data::NewL(aBinaryData));
}

/*
EncryptedPrivateKeyInfo ::= SEQUENCE {
  encryptionAlgorithm EncryptionAlgorithmIdentifier,
  encryptedData EncryptedData }
EncryptionAlgorithmIdentifier ::= AlgorithmIdentifier
EncryptedData ::= OCTET STRING
*/

/*static*/ EXPORT_C CDecPKCS8Data* TASN1DecPKCS8::DecodeEncryptedDERL(const TDesC8& aBinaryData, const TDesC8& aPassword)
{
	TASN1DecGeneric seqGen(aBinaryData);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence dec;
	CArrayPtrFlat<TASN1DecGeneric>* theData = dec.DecodeDERLC(seqGen);
	TInt seqIndex = 0;
	TInt count = theData->Count();
	if (seqIndex >= count)
		{
		User::Leave(KErrArgument);		
		}

//	Get the first part of the sequence -> PKCS5 data
	TASN1DecGeneric* pkcs5 = theData->operator[](seqIndex);
	TPtrC8 theContent(pkcs5->Encoding());	//	expect this to be a sequence
	CPBEncryptParms* encryptParams = TASN1DecPKCS5::DecodeDERL(theContent);
	CleanupStack::PushL(encryptParams);

//	Create the decryptor	
	CPBEncryptElement* encryptElement = CPBEncryptElement::NewLC(aPassword, *encryptParams);
	CPBDecryptor* decryptor = encryptElement->NewDecryptLC();
//	Decrypt the final part of the sequence -> encrypted PKCS8 object
	TASN1DecGeneric* pkcs8 = theData->operator[](count-1);
	if (pkcs8->Tag() != EASN1OctetString)
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 encryptedKey(pkcs8->GetContentDER());
	TUint encryptLength = encryptedKey.Length();
	TUint maxDecryptLength = decryptor->MaxOutputLength(encryptLength);
	if (maxDecryptLength<=0)
		{
		User::Leave(KErrGeneral);		
		}
	
	HBufC8* decryptOutput = HBufC8::NewLC(encryptLength);
	TPtr8 decryptOutputPtr(decryptOutput->Des());
	decryptOutputPtr.FillZ();
	decryptor->Process(encryptedKey, decryptOutputPtr);
			
	CDecPKCS8Data* pkcs8Data = CDecPKCS8Data::NewL(decryptOutputPtr);

	//	decryptOutput decryptor encryptElement 
	//	encryptParams theData
	CleanupStack::PopAndDestroy(5, theData);
	
	return (pkcs8Data);
}

/*
Sample cleartext pkcs8 data from pkcs8dsa.001:

          SEQUENCE
30          Tag: Constructed sequence
81 c8       Length (may be one or more bytes)

            INTEGER
02            Tag: Integer
01            Length: 1 byte
00            Value: 0

            SEQUENCE    
30            Tag: Constructed sequence
81 a9         Length (may be one or more bytes)

              OID
06              Tag: OID
07              Length: 7 bytes
2a 86 48 ce 38 04 01
                Value: dsa (1 2 840 10040 4 1)
*/

_LIT8(KPKCS8DataVersion0, "\x02\x01\x00");
_LIT8(KPKCS8DataOIDDSA, "\x06\x07\x2a\x86\x48\xce\x38\x04\x01");
_LIT8(KPKCS8DataOIDRSA, "\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x01\x01");

/*static*/ EXPORT_C TBool TASN1DecPKCS8::IsPKCS8Data(const TDesC8& aBinaryData)
	{
	// We don't decode the data because we may only have the first few bytes -
	// instead we check the ASN1 by hand.

	ASSERT(aBinaryData.Length() >= KIsPKCS8DataMinLength);
	TInt pos = 0;

	return
		IsASN1Sequence(aBinaryData, pos) &&
		IsExpectedData(aBinaryData, pos, KPKCS8DataVersion0) &&
		IsASN1Sequence(aBinaryData, pos) &&
		(IsExpectedData(aBinaryData, pos, KPKCS8DataOIDDSA) ||
		 IsExpectedData(aBinaryData, pos, KPKCS8DataOIDRSA));
	}

/*
Sample encrypted pkcs8 data from encryptPK8rsa1.txt

          SEQUENCE
30          Tag: Constructed sequence
82 01 a3    Length (may be one or more bytes)

            SEQUENCE
30            Tag: Constructed sequence
3d            Length (may be one or more bytes)

              OID
06              Tag: OID
09              Length: 9 bytes
2a 86 48 86 f7 0d 01 05 0d
                Value: pkcs5PBES2 (1 2 840 113549 1 5 13)

              SEQUENCE
30              Tag: Constructed sequence
30              Length (may be one or more bytes)

                SEQUENCE
30                Tag: Constructed sequence
1b                Length (may be one or more bytes)

                  OID
06                  Tag: OID
09                  Length: 9 bytes
2a 86 48 86 f7 0d 01 05 0c
                    Value: pkcs5PBKDF2 (1 2 840 113549 1 5 12)
*/

_LIT8(KEncryptedPKCS8DataOIDpkcs5PBES2, "\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x05\x0d");
_LIT8(KEncryptedPKCS8DataOIDpkcs5PBKDF2, "\x06\x09\x2a\x86\x48\x86\xf7\x0d\x01\x05\x0c");

EXPORT_C TBool TASN1DecPKCS8::IsEncryptedPKCS8Data(const TDesC8& aBinaryData)
	{
	// We don't decode the data because we may only have the first few bytes -
	// instead we check the ASN1 by hand.

	ASSERT(aBinaryData.Length() >= KIsEncryptedPKCS8DataMinLength);
	TInt pos = 0;

	return
		IsASN1Sequence(aBinaryData, pos) &&
		IsASN1Sequence(aBinaryData, pos) &&
		IsExpectedData(aBinaryData, pos, KEncryptedPKCS8DataOIDpkcs5PBES2) &&
		IsASN1Sequence(aBinaryData, pos) &&
		IsASN1Sequence(aBinaryData, pos) &&
		IsExpectedData(aBinaryData, pos, KEncryptedPKCS8DataOIDpkcs5PBKDF2);
	}

/**
 * Determine if the some data represents the start of an ASN1 sequence.  The
 * data is specified by a descriptor and an offset.  If the data matches, the
 * offset is advanced to point to the contents of the sequence.
 */
TBool TASN1DecPKCS8::IsASN1Sequence(const TDesC8& aBinaryData, TInt& aPos)
	{
	// Check we have enough data
	if ((aPos + 2) >= aBinaryData.Length())
		{
		return EFalse;
		}
	// Check the outermost sequence is valid
	if (aBinaryData[aPos++] != 0x30 /* constructed sequence */)
		{
		return EFalse;
		}
	// Skip sequence length
	TInt length0 = aBinaryData[aPos++];
	if (length0 & 0x80)
		{
		aPos += length0 & 0x7f;
		}
	return ETrue;
	}

/**
 * Determine if some data starts with an expected string.  The data is specified
 * by a descriptor and an offset.  If the data matches, the offset is advanced
 * to point after the match.
 */
TBool TASN1DecPKCS8::IsExpectedData(const TDesC8& aBinaryData, TInt& aPos, const TDesC8& aExpectedData)
	{
	TInt length = aExpectedData.Length();
	// Check we have enough data
	if (aPos + length >= aBinaryData.Length())
		{
		return EFalse;
		}
	// Check data matches	
	if (aBinaryData.Mid(aPos, length) != aExpectedData)
		{
		return EFalse;
		}
	aPos += length;
	return ETrue;
	}

//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	
//	PKCS#8 object data representation
//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	

/*static*/ CDecPKCS8Data* CDecPKCS8Data::NewL(const TDesC8& aData)
{
	CDecPKCS8Data* me = new (ELeave) CDecPKCS8Data();
	CleanupStack::PushL(me);
	me->ConstructL(aData);
	CleanupStack::Pop(me);
	return (me);
}

CDecPKCS8Data::CDecPKCS8Data()
{}

CDecPKCS8Data::~CDecPKCS8Data()
{
	if (iKeyPairData)
	{
		iKeyPairData->Release();
		iKeyPairData = NULL;
	}

	if (iAttributes)
	{
		delete iAttributes;
		iAttributes = NULL;
	}
}

//	Because this is part of construction, don't rely on
//	cleanup of partially constructed items...
void CDecPKCS8Data::ConstructL(const TDesC8& aData)
	{
	TASN1DecGeneric seqGen(aData);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}

	TASN1DecSequence seq;
	CArrayPtrFlat<TASN1DecGeneric>* seqContents = seq.DecodeDERLC(seqGen);
	if (seqContents->Count() < 3 || seqContents->Count() > 4)
		{
		User::Leave(KErrArgument);
		}
				
//	VERSION
	if (seqContents->At(0)->Tag() != EASN1Integer)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecInteger intDecoder;
	iVersion = intDecoder.DecodeDERShortL(*(seqContents->At(0)));
	if (iVersion!=0)
		{
		User::Leave(KErrArgument);
		}
	
//	ALGORITHM
	CX509AlgorithmIdentifier* algorithm = CX509AlgorithmIdentifier::NewLC(seqContents->At(1)->Encoding());
	iAlgorithmID = algorithm->Algorithm();
		
//	KEY DATA
	switch (iAlgorithmID)
		{//	Only support RSA, DSA, DH
		case ERSA:
			iKeyPairData = CPKCS8KeyPairRSA::NewL(*(seqContents->At(2)));
			break;
			
		case EDSA:
			{
			TPtrC8 params(algorithm->EncodedParams());		
			iKeyPairData = CPKCS8KeyPairDSA::NewL(params, *(seqContents->At(2)));
			break;
			}
			
		case EDH: // Currently not supported because no test data is available
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	CleanupStack::PopAndDestroy(algorithm);

	if (seqContents->Count() == 4)
		{
		// I think we should check the tag is zero here, but we're going to be
		// lenient due to lack of real test data
		
		//if (seqContents->At(3)->Tag() != 0) // Implicitly tagged
		//	{
		//	User::Leave(KErrArgument);
		//	}
		
		iAttributes = seqContents->At(3)->Encoding().AllocL();
		}

	CleanupStack::PopAndDestroy(seqContents);
	}

//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	
//	RSA decoding
//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	

/*static*/ MPKCS8DecodedKeyPairData* CPKCS8KeyPairRSA::NewL(const TASN1DecGeneric& aSource)
{
	CPKCS8KeyPairRSA* me = new (ELeave) CPKCS8KeyPairRSA();
	CleanupStack::PushL(me);
	me->ConstructL(aSource);
	CleanupStack::Pop(me);

	return (me);
}

MPKCS8DecodedKeyPairData::~MPKCS8DecodedKeyPairData()
	{	
	}

CPKCS8KeyPairRSA::~CPKCS8KeyPairRSA()
{
	delete iPublicKey;
	delete iPrivateKey;
}

void CPKCS8KeyPairRSA::Release()
{
	delete this;
}

void CPKCS8KeyPairRSA::GetKeyIdentifierL(TKeyIdentifier& aKeyIdentifier) const
{
	if (iPublicKey)
		KeyIdentifierUtil::RSAKeyIdentifierL(*iPublicKey, aKeyIdentifier);	
	else
		User::Leave(KErrNotReady);
}

TUint CPKCS8KeyPairRSA::KeySize() const
{
	if (!iPublicKey)
	{
		ASSERT(EFalse);
		return (0xffffffff);
	}
	
	const TInteger& modulus = iPublicKey->N();

	TUint size = modulus.BitCount();
	return (size);
}

void CPKCS8KeyPairRSA::ConstructL(const TASN1DecGeneric& aSource)
{
	TPtrC8 theContent(aSource.GetContentDER());
	TASN1DecRSAKeyPair keyPairDecoder;
	TInt tempPos = 0;
	keyPairDecoder.DecodeDERL(theContent, tempPos, iPublicKey, iPrivateKey);
}


//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	
//	DSA decoding
//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	//	\\	

/*static*/ MPKCS8DecodedKeyPairData* CPKCS8KeyPairDSA::NewL(const TDesC8& aParamsData, const TASN1DecGeneric& aSource)
{
	CPKCS8KeyPairDSA* me = new (ELeave) CPKCS8KeyPairDSA();
	CleanupStack::PushL(me);
	me->ConstructL(aParamsData, aSource);
	CleanupStack::Pop(me);

	return (me);
}

CPKCS8KeyPairDSA::~CPKCS8KeyPairDSA()
{
	delete iPublicKey;
	delete iPrivateKey;
}

void CPKCS8KeyPairDSA::Release()
{
	delete this;
}

void CPKCS8KeyPairDSA::GetKeyIdentifierL(TKeyIdentifier& aKeyIdentifier) const
{
	if (iPublicKey)
		KeyIdentifierUtil::DSAKeyIdentifierL(*iPublicKey, aKeyIdentifier);	
	else
		User::Leave(KErrNotReady);
}

TUint CPKCS8KeyPairDSA::KeySize() const
{
	if (!iPublicKey)
	{
		ASSERT(EFalse);
		return (0xffffffff);
	}
	
	const TInteger& P = iPublicKey->P();

	TUint size = P.BitCount();
	return (size);
}

void CPKCS8KeyPairDSA::ConstructL(const TDesC8& aParamsData, const TASN1DecGeneric& aSource)
{
	TX509KeyFactory keyFactory;
	CDSAParameters* params = keyFactory.DSAParametersL(aParamsData);
	CleanupStack::PushL(params);

	RInteger P = RInteger::NewL(params->P());
	CleanupStack::PushL(P);

	RInteger Q = RInteger::NewL(params->Q());
	CleanupStack::PushL(Q);

	RInteger G = RInteger::NewL(params->G());
	CleanupStack::PushL(G);

	if (aSource.Tag() != EASN1OctetString)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecOctetString  octetDecoder;
	HBufC8* wrapped = octetDecoder.DecodeDERL(aSource);
	CleanupStack::PushL(wrapped);	

	TASN1DecGeneric integer(*wrapped);
	integer.InitL();
	if (integer.Tag() != EASN1Integer)
		{
		User::Leave(KErrArgument);
		}		
	TASN1DecInteger decInt;
	RInteger X = decInt.DecodeDERLongL(integer);
	CleanupStack::PopAndDestroy(wrapped);
	CleanupStack::PushL(X);
			
	RInteger P1 = RInteger::NewL(params->P());
	CleanupStack::PushL(P1);
	
	RInteger Q1 = RInteger::NewL(params->Q());
	CleanupStack::PushL(Q1);
	
	RInteger G1 = RInteger::NewL(params->G());
	CleanupStack::PushL(G1);

	RInteger Y = TInteger::ModularExponentiateL(G, X, P);
	CleanupStack::PushL(Y);

	iPublicKey = CDSAPublicKey::NewL(P1, Q1, G1, Y);

	// Now have to pop everything owned by iPublicKey otherwise it will be
	// deleted twice if the CDSAPrivateKey::NewL leaves

	CleanupStack::Pop(4, &P1);	// now owned by iPublicKey
	
	iPrivateKey = CDSAPrivateKey::NewL(P, Q, G, X);

	CleanupStack::Pop(4, &P);	// now owned by iPrivateKey
		
	CleanupStack::PopAndDestroy(params);
}
