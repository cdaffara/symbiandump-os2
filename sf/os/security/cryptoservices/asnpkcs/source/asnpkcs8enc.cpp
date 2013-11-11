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


#include <asn1enc.h>
#include <pbe.h>
#include <asymmetrickeys.h>
#include <bigint.h>
#include "asnpkcs.h"

const TInt KPkcs8Version = 0;      // PKCS#8 version 0 
const TInt KPkcs8RSATwoPrime = 0;  // We support only two-prime RSA. 


/*static*/ void TASN1EncPKCS8::AddAttributesL(CASN1EncSequence& aSeq, const TDesC8& aAttribute)
	{
	if (aAttribute != KNullDesC8)
		{
		CASN1EncEncoding* encoded = CASN1EncEncoding::NewLC(aAttribute);
		aSeq.AddAndPopChildL(encoded);		
		}
	}

/**
 * Given a ASN1 sequence representing a private key and a CPBEncryptor object,
 * it returns an ASN1 octet string containing the key sequence encrypted by
 * the given encryptor.
 *
 * @param aKeySeq The key sequence to be encrypted.
 * @param aEncryptor The CPBEncryptor object used to encrypt the given key.
 * @return An ASN1 octet string containing the encrypted key.
 */
/*static*/ CASN1EncOctetString* TASN1EncPKCS8::EncryptKeySequenceL(CASN1EncSequence& aKeySeq, CPBEncryptor& aEncryptor)
	{
	//	Make an octet string of the key sequence
	TUint seqLen = aKeySeq.LengthDER();
	HBufC8* octetData = HBufC8::NewMaxLC(seqLen);
	TPtr8 oct(octetData->Des());
	oct.FillZ();
	TUint writePos = 0;
	aKeySeq.WriteDERL(oct, writePos);
	
	// Encrypt it
        HBufC8* encryptedData = HBufC8::NewLC(aEncryptor.MaxFinalOutputLength(seqLen)); 
        TPtr8 encrypted(encryptedData->Des());  

	aEncryptor.ProcessFinalL(*octetData, encrypted);
		
	CASN1EncOctetString* octet = CASN1EncOctetString::NewL(encrypted);
	
	CleanupStack::PopAndDestroy(2, octetData); // encryptedData, octetData
	
	return octet;
	}


/*static*/ CASN1EncOctetString* TASN1EncPKCS8::ElementToOctetL(CASN1EncBase& aAsnElement)
	{
	//	Make an octet string from the structure
	HBufC8* octetData = HBufC8::NewMaxLC(aAsnElement.LengthDER());
	TPtr8 oct(octetData->Des());
	oct.FillZ();

	TUint writePos = 0;
	aAsnElement.WriteDERL(oct, writePos);
	
	CASN1EncOctetString* octet = CASN1EncOctetString::NewL(oct);
	CleanupStack::PopAndDestroy(octetData);
	return octet;
	}


/*static*/ CASN1EncSequence* TASN1EncPKCS8::EncryptedSequenceL(CASN1EncSequence& aPrivateKeySequence, 
                                                               CPBEncryptor& aEncryptor, CPBEncryptParms& aData)
	{
	// Main ASN1 sequence
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	
	// PKCS#5 PBE sequence
	CASN1EncSequence* pkcs5Seq = TASN1EncPKCS5::EncodeDERL(aData);
	 
	CleanupStack::PushL(pkcs5Seq);
	seq->AddAndPopChildL(pkcs5Seq);
	
	// Encrypt the Private Key Sequence
	CASN1EncOctetString* octet = EncryptKeySequenceL(aPrivateKeySequence, aEncryptor);
	CleanupStack::PushL(octet);
	seq->AddAndPopChildL(octet);

	CleanupStack::Pop(seq);

	return seq;
	}


/*static*/ const RInteger TASN1EncPKCS8::CalculateRSAPrivExpL(const CRSAPrivateKeyCRT& aPrivateKey, const CRSAPublicKey& aPublicKey)
	{
	const TInteger& e = aPublicKey.E();

	RInteger pTemp = RInteger::NewL(aPrivateKey.P());
	CleanupStack::PushL(pTemp);
	RInteger qTemp = RInteger::NewL(aPrivateKey.Q());
	CleanupStack::PushL(qTemp);
	
  	--pTemp;
    	--qTemp;
 
	//phi = (p-1)(q-1)
	RInteger phi = pTemp.TimesL(qTemp);

	CleanupStack::PopAndDestroy(2, &pTemp);  // qTemp, pTemp
 	CleanupStack::PushL(phi);
    	
    	//d = e^(-1) mod ((p-1)(q-1))
   	 RInteger d = e.InverseModL(phi);
    	CleanupStack::PopAndDestroy(&phi); 
	return d;	
	}

/**
 * Encodes the given private key using the pkcs#8 standard.
 *
 * The returned ASN1 sequence respects the following grammar:
 * 
 *	PrivateKeyInfo ::= SEQUENCE {
 *	version Version,
 *	privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
 *	privateKey PrivateKey,
 *	attributes [0] IMPLICIT Attributes OPTIONAL }
 *	
 *	Version ::= INTEGER
 *	PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
 *	PrivateKey ::= OCTET STRING
 *	Attributes ::= SET OF Attribute
 *
 *
 * @param aKeyPair A key pair containing the private key we wish to encode.
 * @return A CASN1EncSequence encoding the private key. The sequence
 *         follows the grammar specified above. 
 * @internalComponent
 * @released
 */
/*static*/ EXPORT_C CASN1EncSequence* TASN1EncPKCS8::EncodeL(const CRSAPrivateKeyCRT& aPrivateKey, 
														  const CRSAPublicKey& aPublicKey, 
														  const TDesC8& aAttributes)
	{
	// Main ASN1 sequence
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();

	// Start with PKCS#8 header
	CASN1EncInt* version = CASN1EncInt::NewLC(KPkcs8Version); 
	seq->AddAndPopChildL(version);
	
	//	Algorithm sequence
	CASN1EncSequence* algSeq = CASN1EncSequence::NewLC();

	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KRSA);
	algSeq->AddAndPopChildL(oid);

	//	No other parameters
	CASN1EncNull* nullparams = CASN1EncNull::NewLC();
	algSeq->AddAndPopChildL(nullparams);

	seq->AddAndPopChildL(algSeq);
	
	// The actual private key sequence		
	CASN1EncSequence* keySeq = CASN1EncSequence::NewLC();

	//	TASN1DecGeneric* versionData = theData->operator[](seqIndex);
	CASN1EncInt* rsaVersion = CASN1EncInt::NewLC(KPkcs8RSATwoPrime); 
	keySeq->AddAndPopChildL(rsaVersion);
	
	//	The modulus	
	const TInteger& modulus = aPrivateKey.N();
	CASN1EncBigInt* mod = CASN1EncBigInt::NewLC(modulus);
	keySeq->AddAndPopChildL(mod);

	//	The public exponent
	const TInteger& e = aPublicKey.E();
	CASN1EncBigInt* publicExp = CASN1EncBigInt::NewLC(e);
	keySeq->AddAndPopChildL(publicExp);

	// The private exponent
	RInteger d = CalculateRSAPrivExpL(aPrivateKey, aPublicKey);
	CleanupStack::PushL(d);

	CASN1EncBigInt* privateExp = CASN1EncBigInt::NewLC(d);

	keySeq->AddAndPopChildL(privateExp);
	CleanupStack::PopAndDestroy(&d);  // RSA Private Exponent 

	//	The prime1
	const TInteger& p = aPrivateKey.P();
	CASN1EncBigInt* prime1 = CASN1EncBigInt::NewLC(p);
	keySeq->AddAndPopChildL(prime1);

	//	The prime2
	const TInteger& q = aPrivateKey.Q();
	CASN1EncBigInt* prime2 = CASN1EncBigInt::NewLC(q);
	keySeq->AddAndPopChildL(prime2);

	//	The exponent 1	
	const TInteger& dp = aPrivateKey.DP();
	CASN1EncBigInt* theExp1 = CASN1EncBigInt::NewLC(dp);
	keySeq->AddAndPopChildL(theExp1);

	//	The exponent 2
	const TInteger& dq = aPrivateKey.DQ();
	CASN1EncBigInt* theExp2 = CASN1EncBigInt::NewLC(dq);
	keySeq->AddAndPopChildL(theExp2);

	//	The coefficient
	const TInteger& coeff = aPrivateKey.QInv();
	CASN1EncBigInt* coefficient = CASN1EncBigInt::NewLC(coeff);
	keySeq->AddAndPopChildL(coefficient);
	
	CASN1EncOctetString* octet = ElementToOctetL(*keySeq);
	CleanupStack::PushL(octet);
	
	//	Now add the octet of the key sequence to the sequence
	seq->AddAndPopChildL(octet);
	
  	CleanupStack::PopAndDestroy(keySeq);

	AddAttributesL(*seq, aAttributes);

  	CleanupStack::Pop(seq);
					
	return seq;
	}

/**
 * Encodes an RSA key in encrypted format.
 * 
 * @param aPrivateKey The private key to be encoded (must be in CRT format)
 * @param aPublicKey The corresponding public key.
 * @param aEncryptor The object used to encrypt the data.
 * @param aData The encryption parameters of the given encryptor.
 *              These parameters are stored in the resulting sequence.
 *
 * @return An ASN1 Sequence encoding the encrypted key.
 */
/*static*/ EXPORT_C CASN1EncSequence* TASN1EncPKCS8::EncodeEncryptedL(const CRSAPrivateKeyCRT& aPrivateKey, const CRSAPublicKey& aPublicKey, 
                                                                      CPBEncryptor& aEncryptor, CPBEncryptParms& aData, const TDesC8& aAttributes)
	{
	// Now generate a PrivateKeyInfo type 
	CASN1EncSequence* privKeySeq = EncodeL(aPrivateKey, aPublicKey, aAttributes);
	CleanupStack::PushL(privKeySeq);
	
	CASN1EncSequence* seq = EncryptedSequenceL(*privKeySeq, aEncryptor, aData);
	
	CleanupStack::PopAndDestroy(privKeySeq);
		
	return seq;
	}


/**
 * Return the ASN1 PKCS#8 encoding of a DSA private key.
 *
 * The private key syntax for this key type is defined in 
 * the PKCS#11 document. 
 *
 * @param aPrivateKey The private key to be encoded (must be in CRT format)
 *
 * @return An ASN1 Sequence encoding the key.
 */	
/*static*/ EXPORT_C CASN1EncSequence* TASN1EncPKCS8::EncodeL(const CDSAPrivateKey& aPrivateKey, 
													          const TDesC8& aAttributes)
	{
	// Main ASN1 sequence
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();

	//	version
	CASN1EncInt* version = CASN1EncInt::NewLC(KPkcs8Version);
	seq->AddAndPopChildL(version);
	
	//	Algorithm sequence
	CASN1EncSequence* algSeq = CASN1EncSequence::NewLC();

	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KDSA);
	algSeq->AddAndPopChildL(oid);

	seq->AddAndPopChildL(algSeq);

	// DSA Parameters	
	CASN1EncSequence* keySeq = CASN1EncSequence::NewLC();

	//	The modulus	P
	const TInteger& modulus1 = aPrivateKey.P();

	CASN1EncBigInt* mod = CASN1EncBigInt::NewLC(modulus1);
	keySeq->AddAndPopChildL(mod);

	//	The modulus	Q
	const TInteger& modulus2 = aPrivateKey.Q();
	
	CASN1EncBigInt* mod2 = CASN1EncBigInt::NewLC(modulus2);
	keySeq->AddAndPopChildL(mod2);
	
	//	The base G 
	const TInteger& base = aPrivateKey.G();

	CASN1EncBigInt* b = CASN1EncBigInt::NewLC(base);
	keySeq->AddAndPopChildL(b);
	
	algSeq->AddAndPopChildL(keySeq);
	
	//	The private key X
	const TInteger& priv = aPrivateKey.X();	
	CASN1EncBigInt* x = CASN1EncBigInt::NewLC(priv);

	CASN1EncOctetString* octet = ElementToOctetL(*x);

	CleanupStack::PopAndDestroy(x);
	CleanupStack::PushL(octet);

	seq->AddAndPopChildL(octet);

	AddAttributesL(*seq, aAttributes);
	
	CleanupStack::Pop(seq);

	return seq;	
	}


/**
 * Encodes a DSA key in encrypted format.
 * 
 * @param aPrivateKey The private key to be encoded.
 * @param aEncryptor The object used to encrypt the data.
 * @param aData The encryption parameters of the given encryptor.
 *              These parameters are stored in the resulting sequence.
 *
 * @return An ASN1 Sequence encoding the encrypted key.
 */
/*static*/ EXPORT_C CASN1EncSequence* TASN1EncPKCS8::EncodeEncryptedL(const CDSAPrivateKey& aPrivateKey, CPBEncryptor& aEncryptor, 
                                                                      CPBEncryptParms& aData, const TDesC8& aAttributes)
	{
	// Now generate a PrivateKeyInfo type 
	CASN1EncSequence* privKeySeq = EncodeL(aPrivateKey, aAttributes);
	CleanupStack::PushL(privKeySeq);
	
	CASN1EncSequence* seq = EncryptedSequenceL(*privKeySeq, aEncryptor, aData);
	
	CleanupStack::PopAndDestroy(privKeySeq);
		
	return seq;
	}

