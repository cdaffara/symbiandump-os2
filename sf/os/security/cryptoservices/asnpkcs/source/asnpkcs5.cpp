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
#include <asn1dec.h>
#include <pbedata.h>
#include <rc2.h>

#include "asnpkcs.h"

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS

/** OpenSSL PKCS8 Effective Key Length Compatibility.*/
const TUint KPkcs8CompatibilityBits = 128;

#endif


_LIT(Kpkcs5PBES2, "1.2.840.113549.1.5.13");
_LIT(Kpkcs5PBKDF2, "1.2.840.113549.1.5.12");
_LIT(KDESCBC, "1.3.14.3.2.7");
_LIT(K3DESCBC, "1.2.840.113549.3.7");
_LIT(KRC2CBC, "1.2.840.113549.3.2");

// pbe12Algorithm Ids
_LIT(KPbeWithSHA1And128BitRC4, "1.2.840.113549.1.12.1.1");
_LIT(KPbeWithSHA1And40BitRC4, "1.2.840.113549.1.12.1.2");
_LIT(KPbeWithSHA1And3_KeyTripleDES_CBC, "1.2.840.113549.1.12.1.3");
_LIT(KPbeWithSHA1And2_KeyTripleDES_CBC, "1.2.840.113549.1.12.1.4");
_LIT(KPbeWithSHA1And128BitRC2_CBC, "1.2.840.113549.1.12.1.5");
_LIT(KPbeWithSHA1And40BitRC2_CBC, "1.2.840.113549.1.12.1.6");
//The size of the Initialization vector
const TInt KIvSize = 8;


/*
* //For RC2
* SEQUENCE
* 	OID -- pkcs5PBES2
*	SEQUENCE
*		SEQUENCE
*		OID -- pkcs5PBKDF2
*		SEQUENCE
*			OCTET STRING -- salt
*			INTEGER -- iteration count
*			INTEGER -- effective key length in octets
* 	SEQUENCE
*		OID -- algorithm id (rc2)
*		SEQUENCE
*			INTEGER -- RC2 parameter version 58 = 128, 160 = 40
*			OCTET STRING -- iv
*
* //For DES and 3DES
* SEQUENCE
* 	OID -- pkcs5PBES2
*	SEQUENCE
*		SEQUENCE
*		OID -- pkcs5PBKDF2
*		SEQUENCE
*			OCTET STRING -- salt
*			INTEGER -- iteration count
* 	SEQUENCE
*		OID -- algorithm id (des, 3des)
*		OCTET STRING -- iv
*/

EXPORT_C CASN1EncSequence* TASN1EncPKCS5::EncodeDERL(const CPBEncryptParms& aParms)
	{
	CASN1EncSequence* seq = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* pbes2 = CASN1EncObjectIdentifier::NewLC(Kpkcs5PBES2);
	seq->AddChildL(pbes2);
	CleanupStack::Pop(pbes2);

	CASN1EncSequence* seq1 = CASN1EncSequence::NewLC();
	seq->AddChildL(seq1);
	CleanupStack::Pop(seq1);

	CASN1EncSequence* seq2 = CASN1EncSequence::NewLC();
	seq1->AddChildL(seq2);
	CleanupStack::Pop(seq2);

	CASN1EncObjectIdentifier* pbkdf2 = CASN1EncObjectIdentifier::NewLC(Kpkcs5PBKDF2);
	seq2->AddChildL(pbkdf2);
	CleanupStack::Pop(pbkdf2);

	CASN1EncSequence* seq3 = CASN1EncSequence::NewLC();
	seq2->AddChildL(seq3);
	CleanupStack::Pop(seq3);

	CASN1EncOctetString* salt = CASN1EncOctetString::NewLC(aParms.Salt());
	seq3->AddChildL(salt);
	CleanupStack::Pop(salt);

	CASN1EncInt* iterations = CASN1EncInt::NewLC(aParms.Iterations());
	seq3->AddChildL(iterations);
	CleanupStack::Pop(iterations);

	CASN1EncInt* keysize = 0;
	switch(aParms.Cipher())
		{
		case ECipherDES_CBC:
		case ECipher3DES_CBC: 
			break;
		case ECipherRC2_CBC_40:
 			keysize = CASN1EncInt::NewLC(KSSLCompatibilityBits/8);  // effective key length in *octets*
			seq3->AddChildL(keysize);
			CleanupStack::Pop(keysize);
			break;
		case ECipherRC2_CBC_128:
 			keysize = CASN1EncInt::NewLC(KSSLCompatibilityBits/8);  // effective key length in *octets*
			seq3->AddChildL(keysize);
			CleanupStack::Pop(keysize);
			break;
		case ECipherRC2_CBC_40_16:
 			keysize = CASN1EncInt::NewLC(KPkcs8CompatibilityBits/8);  // effective key length in *octets*
			seq3->AddChildL(keysize);
			CleanupStack::Pop(keysize);
			break;
		case ECipherRC2_CBC_128_16:
 			keysize = CASN1EncInt::NewLC(KPkcs8CompatibilityBits/8);  // effective key length in *octets*
			seq3->AddChildL(keysize);
			CleanupStack::Pop(keysize);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}

	CASN1EncSequence* seq4 = CASN1EncSequence::NewLC();
	seq1->AddChildL(seq4);
	CleanupStack::Pop(seq4);

	CASN1EncObjectIdentifier* algid = 0;
	switch(aParms.Cipher())
		{
		case ECipherDES_CBC:
			algid = CASN1EncObjectIdentifier::NewLC(KDESCBC);
			break;
		case ECipher3DES_CBC:
			algid = CASN1EncObjectIdentifier::NewLC(K3DESCBC);
			break;
		case ECipherRC2_CBC_40:
		case ECipherRC2_CBC_128:
		case ECipherRC2_CBC_40_16:
		case ECipherRC2_CBC_128_16:
			algid = CASN1EncObjectIdentifier::NewLC(KRC2CBC);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	seq4->AddChildL(algid);
	CleanupStack::Pop(algid);

	CASN1EncSequence* seq5 = 0;
	CASN1EncInt* keysize1 = 0;
	CASN1EncOctetString* iv = 0;
	switch(aParms.Cipher())
		{
		case ECipherDES_CBC:
		case ECipher3DES_CBC:
			iv = CASN1EncOctetString::NewLC(aParms.IV());
			seq4->AddChildL(iv);
			CleanupStack::Pop(iv);
			break;
		case ECipherRC2_CBC_40:
		case ECipherRC2_CBC_40_16:
			seq5 = CASN1EncSequence::NewLC();
			seq4->AddChildL(seq5);
			CleanupStack::Pop(seq5);

			keysize1 = CASN1EncInt::NewLC(160); //encoding for 40 bit
			seq5->AddChildL(keysize1);
			CleanupStack::Pop(keysize1);

			iv = CASN1EncOctetString::NewLC(aParms.IV());
			seq5->AddChildL(iv);
			CleanupStack::Pop(iv);
			break;
		case ECipherRC2_CBC_128:
		case ECipherRC2_CBC_128_16:
			seq5 = CASN1EncSequence::NewLC();
			seq4->AddChildL(seq5);
			CleanupStack::Pop(seq5);
			
			keysize1 = CASN1EncInt::NewLC(58); //encoding for 128 bit
			seq5->AddChildL(keysize1);
			CleanupStack::Pop(keysize1);

			iv = CASN1EncOctetString::NewLC(aParms.IV());
			seq5->AddChildL(iv);
			CleanupStack::Pop(iv);
			break;
		default:
			User::Leave(KErrNotSupported);
			break;
		}
	CleanupStack::Pop(seq);
	return seq;
	}

EXPORT_C CPBEncryptParms* TASN1DecPKCS5::DecodeDERL(const TDesC8& aBinaryData)
	{
	TASN1DecGeneric seqGen(aBinaryData);
	seqGen.InitL();
	if (seqGen.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	
	//Decode the Algorithm Identifier Sequence
	TASN1DecSequence seq;
	CArrayPtrFlat<TASN1DecGeneric>* seqContents = seq.DecodeDERLC(seqGen);

	//PbeAlgorithm Id
	if (seqContents->At(0)->Tag() != EASN1ObjectIdentifier)
		{
		User::Leave(KErrArgument);
		}	
	CPBEncryptParms* params = NULL;	
	TASN1DecObjectIdentifier oid;
	HBufC* oiddes = oid.DecodeDERL(*(seqContents->At(0)));
	CleanupStack::PushL(oiddes);
	//Algorithm Id is a pkcs-12Pbe Algorithm Id.
	if(*oiddes != Kpkcs5PBES2)
		{
		// Initialise to impossible value
		TPBECipher cipher = (TPBECipher) -1; 
		// Pbe12Algorithm Ids
		if(*oiddes == KPbeWithSHA1And128BitRC4)
			{
			cipher = ECipherARC4_128;
			}
		else if(*oiddes == KPbeWithSHA1And40BitRC4)
			{
			cipher = ECipherARC4_40;
			}
		else if(*oiddes == KPbeWithSHA1And3_KeyTripleDES_CBC)
			{
			cipher = ECipher3DES_CBC;
			}
		else if(*oiddes == KPbeWithSHA1And2_KeyTripleDES_CBC)
			{
			cipher = ECipher2Key3DES_CBC;
			}
		else if(*oiddes == KPbeWithSHA1And128BitRC2_CBC)
			{
			cipher = ECipherRC2_CBC_128_16; 
			}
		else if(*oiddes == KPbeWithSHA1And40BitRC2_CBC)
			{
			cipher = ECipherRC2_CBC_40_5; 
			}
	    else
        	{
        	User::Leave(KErrNotSupported);
        	}	 
        
        TInt seqContentsCount = seqContents->Count();
		
		//All pkcs-12Pbe algorithms require the Algorithm Parameters.
		//Algorithm Parameters are not OPTIONAL for pkcs-12Pbe algorithms.
		
		//seqContentsCount should be equal to 2.That is, the Algorithm Id 
		//and associated Algorithm Parameters have to be present.
		if(seqContentsCount != 2)
			{
			User::Leave(KErrArgument);	
			}
		//This if statement checks if the pkcs-12PbeParams Sequence is present in the 
		//AlgorithmIdentifier Sequence Since pkcs-12PbeParams are OPTIONAL
		else 
			{
			//Set the Initialization vector size to 8 bytes.
			TBuf8<KIvSize> iv(KIvSize);
			// Initialized to NULL, if salt is not present.
			TPtrC8 salt;
			TInt iterations; 

			const TASN1DecGeneric* seqContentsAt1 = seqContents->At(1);
			if (seqContentsAt1->Tag() != EASN1Sequence || seqContentsAt1->Class() != EUniversal)
				{
				User::Leave(KErrArgument);
				}
				
			CArrayPtrFlat<TASN1DecGeneric>* seq1Contents = seq.DecodeDERLC(*seqContentsAt1);
			const TASN1DecGeneric* seq1ContentsAt0 = seq1Contents->At(0);
			if (seq1ContentsAt0->Tag() != EASN1OctetString || seq1ContentsAt0->Class() != EUniversal)
				{
				User::Leave(KErrArgument);
				}
			salt.Set(seq1ContentsAt0->GetContentDER());
			const TASN1DecGeneric* seq1ContentsAt1 = seq1Contents->At(1);
			if (seq1ContentsAt1->Tag() != EASN1Integer || seq1ContentsAt1->Class() != EUniversal)
				{
				User::Leave(KErrArgument);
				}
			TASN1DecInteger integer;
			iterations = integer.DecodeDERShortL(*seq1ContentsAt1);
			if (iterations <= 0)
				{
				User::Leave(KErrArgument);
				}
			params = CPBEncryptParms::NewL(cipher, salt, iv, iterations);
			params->SetKdf(CPBEncryptParms::EKdfPkcs12);
			CleanupStack::PopAndDestroy(seq1Contents);
			}
		}
	//Algorithm Id is a pkcs-5Pbe Algorithm Id.
   	 else if (*oiddes == Kpkcs5PBES2)
    	{
    	if (seqContents->At(1)->Tag() != EASN1Sequence)
			{
			User::Leave(KErrArgument);
			}
		CArrayPtrFlat<TASN1DecGeneric>* seq1Contents = seq.DecodeDERLC(*(seqContents->At(1)));

		if (seq1Contents->At(0)->Tag() != EASN1Sequence)
			{
			User::Leave(KErrArgument);
			}
		CArrayPtrFlat<TASN1DecGeneric>* seq2Contents = seq.DecodeDERLC(*(seq1Contents->At(0)));

		if (seq2Contents->At(0)->Tag() != EASN1ObjectIdentifier)
			{
			User::Leave(KErrArgument);
			}
		HBufC* oid1des = oid.DecodeDERL(*(seq2Contents->At(0)));
		CleanupStack::PushL(oid1des);
		
		if(*oid1des != Kpkcs5PBKDF2)
			{
			User::Leave(KErrNotSupported);
			}
		if (seq2Contents->At(1)->Tag() != EASN1Sequence)
			{
			User::Leave(KErrArgument);
			}
		CArrayPtrFlat<TASN1DecGeneric>* seq3Contents = seq.DecodeDERLC(*(seq2Contents->At(1)));

		if (seq3Contents->At(0)->Tag() != EASN1OctetString)
			{
			User::Leave(KErrArgument);
			}
		TASN1DecOctetString octet;
		HBufC8* salt = octet.DecodeDERL(*(seq3Contents->At(0)));
		CleanupStack::PushL(salt);
		
		if (seq3Contents->At(1)->Tag() != EASN1Integer)
			{
			User::Leave(KErrArgument);
			}
		TASN1DecInteger integer;
		TInt iterations = integer.DecodeDERShortL(*(seq3Contents->At(1)));

		if (seq1Contents->At(1)->Tag() != EASN1Sequence)
			{
			User::Leave(KErrArgument);
			}
		CArrayPtrFlat<TASN1DecGeneric>* seq4Contents = seq.DecodeDERLC(*(seq1Contents->At(1)));
	
		TPBECipher cipher = (TPBECipher) -1; // Initialise to impossible value
		if (seq4Contents->At(0)->Tag() != EASN1ObjectIdentifier)
			{
			User::Leave(KErrArgument);
			}
		HBufC* oid2des = oid.DecodeDERL(*(seq4Contents->At(0)));
		CleanupStack::PushL(oid2des);
		
		CArrayPtrFlat<TASN1DecGeneric>* seq5Contents = 0; 

		if(*oid2des == K3DESCBC)
			{
			cipher = ECipher3DES_CBC;
		CleanupStack::PushL(seq5Contents);
			}
		else if(*oid2des == KDESCBC)
			{
			cipher = ECipherDES_CBC;
		CleanupStack::PushL(seq5Contents);
			}
		else if(*oid2des == KRC2CBC)
			{
			// RC2 has an additional parameter, the effective key lenght in octets.
			if (seq3Contents->At(2)->Tag() != EASN1Integer)
				{
				User::Leave(KErrArgument);
				}		
			TInt effectiveKeyLength =  integer.DecodeDERShortL(*(seq3Contents->At(2)));

			if (seq4Contents->At(1)->Tag() != EASN1Sequence)
				{
				User::Leave(KErrArgument);
				}
			seq5Contents = seq.DecodeDERLC(*(seq4Contents->At(1)));
			if (seq5Contents->At(0)->Tag() != EASN1Integer)
				{
				User::Leave(KErrArgument);
				}
			TInt keysize = integer.DecodeDERShortL(*(seq5Contents->At(0)));
			switch(keysize)
				{
				// These values come from the PKCS#5 v2 specs
				case 160:
					if (effectiveKeyLength == 16)
						{
						cipher = ECipherRC2_CBC_40_16;					
						}
					else 
						{
						if (effectiveKeyLength == 128)
							{
							cipher = ECipherRC2_CBC_40;
							}
						else 
							{
							User::Leave(KErrNotSupported); // Unsupported effective key length!						
							}
						}
					break;
				case 58:
					if (effectiveKeyLength == 16)
						{
						cipher = ECipherRC2_CBC_128_16;					
						}
					else 
						{
						if (effectiveKeyLength == 128)
							{
							cipher = ECipherRC2_CBC_128;
							}
						else 
							{
							User::Leave(KErrNotSupported); // Unsupported effective key length!						
							}
						}			
					break;
				case 120:
					//would be RC_CBC_64 but we don't support that
				default:
					User::Leave(KErrNotSupported);
					break;
				}
			}
		else 
			{
			User::Leave(KErrNotSupported);
			}	

		HBufC8* iv = 0;
		switch(cipher)
			{
			case ECipher3DES_CBC:
			case ECipherDES_CBC:
				if (seq4Contents->At(1)->Tag() != EASN1OctetString)
					{
					User::Leave(KErrArgument);
					}
				iv = octet.DecodeDERL(*(seq4Contents->At(1)));	
			CleanupStack::PushL(iv);
				break;
			case ECipherRC2_CBC_40:
			case ECipherRC2_CBC_128:
			case ECipherRC2_CBC_40_16:
			case ECipherRC2_CBC_128_16:	
				if (seq5Contents->At(1)->Tag() != EASN1OctetString)
					{
					User::Leave(KErrArgument);
					}
				iv = octet.DecodeDERL(*(seq5Contents->At(1)));
			CleanupStack::PushL(iv);
				break;
			default:
				User::Leave(KErrNotSupported);
				break;
			}

		params = CPBEncryptParms::NewL(cipher, *salt, *iv,
		iterations);
	CleanupStack::PopAndDestroy(9); //iv, seq5contents, oid2des, seq4Contents,
	//salt, seq3Contents, oid1des, seq2Contents, seq1Contents
    	}
	else
		{
		User::Leave(KErrNotSupported);
		}
	CleanupStack::PopAndDestroy(2, seqContents);
	return params;
	}
