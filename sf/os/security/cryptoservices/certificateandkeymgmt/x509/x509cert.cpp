/*
* Copyright (c) 1998-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* -- FingerPrint Note:
* Developers have to be aware that if they are going to change the fingerprint for this certificate
* for a different hash, then there are other places that need to reflect this change
* -- Location
* void CX509Certificate::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
* EXPORT_C void CX509Certificate::InternalizeL(RReadStream& aStream)
* Also change the CWTLSCertificate and search for other occurences of the current
* hash.
* -- TeletexString type support Note:
* If the certificate has decoded the members from TeletexString then the return value 
* may be incorrect because TeletexString type is not fully supported by this library.
* Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
* null characters in the second byte of each character. This will work as expected 
* for cases where the string contains ASCII data.
*
*/




#include <x509cert.h>
#include <x509certext.h>
#include "X509time.h"
#include <x509keys.h>
#include <keyidentifierutil.h>
#include <asn1enc.h>
#include <asn1dec.h>

EXPORT_C CPKCS1SignatureResult* CPKCS1SignatureResult::NewL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest)
	{
	CPKCS1SignatureResult* self = CPKCS1SignatureResult::NewLC(aDigestAlgorithm, aDigest);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CPKCS1SignatureResult* CPKCS1SignatureResult::NewLC(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest)
	{
	CPKCS1SignatureResult* self = new(ELeave) CPKCS1SignatureResult;
	CleanupStack::PushL(self);
	self->ConstructL(aDigestAlgorithm, aDigest);
	return self;
	}

void CPKCS1SignatureResult::ConstructL(const CAlgorithmIdentifier& aDigestAlgorithm, const TDesC8& aDigest)
	{
	iDigestAlgorithm = CAlgorithmIdentifier::NewL(aDigestAlgorithm);
	iDigest = aDigest.AllocL();
	}

EXPORT_C TBool CPKCS1SignatureResult::VerifyL(const TDesC8& aResult)
	{

	TBool res = EFalse;
	TRAPD(err, res = DoVerifyL(aResult));
	if ((err != KErrNone) && (err != KErrArgument))
		{
		User::Leave(err);
		}
	return res;
	}

TBool CPKCS1SignatureResult::DoVerifyL(const TDesC8& aResult)
	{
	TBool res = EFalse;
	TASN1DecSequence decSeq;
	TInt pos = 0;
	TInt len = aResult.Length();

	const CArrayPtrFlat<TASN1DecGeneric>* seq = decSeq.DecodeDERLC(aResult, pos);
	if (seq->Count() == 2)
		{
		const TASN1DecGeneric* gen1 = seq->At(0);
		const TASN1DecGeneric* gen2 = seq->At(1);
		CAlgorithmIdentifier* algId = CX509AlgorithmIdentifier::NewLC(gen1->Encoding());
		HBufC8* digest = gen2->GetContentDER().AllocL();
		if ((*algId == *iDigestAlgorithm) && (*digest == *iDigest)  && (pos == len))
			{
			res = ETrue;
			}
		delete digest;
		CleanupStack::PopAndDestroy();
		}
	CleanupStack::PopAndDestroy();
	return res;
	}


EXPORT_C CRSAPublicKey* TX509KeyFactory::RSAPublicKeyL(const TDesC8& aEncoding) const
	{
	return CX509RSAPublicKey::NewL(aEncoding);
	}

EXPORT_C CRSASignatureResult* TX509KeyFactory::RSASignatureResultL(const CAlgorithmIdentifier& aDigestAlgorithm, TDesC8& aDigest) const
	{
	return CPKCS1SignatureResult::NewL(aDigestAlgorithm, aDigest);
	}

EXPORT_C CDSAPublicKey* TX509KeyFactory::DSAPublicKeyL(const TDesC8& aParamsEncoding, const TDesC8& aEncoding) const
	{
	return CX509DSAPublicKey::NewL(aParamsEncoding, aEncoding);
	}

EXPORT_C CDSAPublicKey* TX509KeyFactory::DSAPublicKeyL(const CDSAParameters& aParams, const TDesC8& aEncoding) const
	{
	return CX509DSAPublicKey::NewL(aParams, aEncoding);
	}


EXPORT_C CDSASignature* TX509KeyFactory::DSASignatureL(const TDesC8& aEncoding) const
	{
	return CX509DSASignature::NewL(aEncoding);
	}

EXPORT_C CDSAParameters* TX509KeyFactory::DSAParametersL(const TDesC8& aParamsEncoding) const
{
	return (CX509DSAPublicKey::DSAParametersL(aParamsEncoding));
}

//CX509ValidityPeriod
EXPORT_C CX509ValidityPeriod* CX509ValidityPeriod::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509ValidityPeriod::NewL(aBinaryData, pos);
	}

EXPORT_C CX509ValidityPeriod* CX509ValidityPeriod::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509ValidityPeriod::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509ValidityPeriod* CX509ValidityPeriod::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ValidityPeriod* self = CX509ValidityPeriod::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509ValidityPeriod* CX509ValidityPeriod::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ValidityPeriod* self = new(ELeave) CX509ValidityPeriod;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CX509ValidityPeriod::CX509ValidityPeriod()
	{
	}

void CX509ValidityPeriod::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	if (seq->Count() != 2)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecX509Time decTime;
	iStart = decTime.DecodeDERL(*(seq->At(0)));
	iFinish = decTime.DecodeDERL(*(seq->At(1)));
	CleanupStack::PopAndDestroy();//seq + contents
	}

//algorithm id
EXPORT_C CX509AlgorithmIdentifier* CX509AlgorithmIdentifier::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509AlgorithmIdentifier::NewL(aBinaryData, pos);
	}

EXPORT_C CX509AlgorithmIdentifier* CX509AlgorithmIdentifier::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509AlgorithmIdentifier::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509AlgorithmIdentifier* CX509AlgorithmIdentifier::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AlgorithmIdentifier* self = CX509AlgorithmIdentifier::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509AlgorithmIdentifier* CX509AlgorithmIdentifier::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AlgorithmIdentifier* self = new(ELeave) CX509AlgorithmIdentifier;
	CleanupStack::PushL(self);
	self->InitializeL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509AlgorithmIdentifier* CX509AlgorithmIdentifier::NewL(TAlgorithmId aAlgorithmId, const TDesC8& aEncodedParams)
 	{
	CX509AlgorithmIdentifier* self = CX509AlgorithmIdentifier::NewLC(aAlgorithmId, aEncodedParams);
 	CleanupStack::Pop();
 	return self;	
 	}
 	
EXPORT_C CX509AlgorithmIdentifier* CX509AlgorithmIdentifier::NewLC(TAlgorithmId aAlgorithmId, const TDesC8& aEncodedParams)
 	{
 	CX509AlgorithmIdentifier* self = new(ELeave) CX509AlgorithmIdentifier(aAlgorithmId);
 	CleanupStack::PushL(self);
 	self->ConstructL(aEncodedParams);
 	return self;				
 	}

CX509AlgorithmIdentifier::CX509AlgorithmIdentifier(TAlgorithmId& aAlgorithmId):CAlgorithmIdentifier(aAlgorithmId)
 	{
 	}


CX509AlgorithmIdentifier::CX509AlgorithmIdentifier()
	{
	}

void CX509AlgorithmIdentifier::InitializeL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();
	TASN1DecObjectIdentifier encOID;
	HBufC* oid = encOID.DecodeDERL(*(seq->At(0)));
	CleanupStack::PushL(oid);
	TPtrC oidDes(oid->Des()); 
	if (oidDes == KDSA)
		{
		//optional params
		if (count > 1)//if we still have stuff left
			{
			TASN1DecGeneric* gen = seq->At(1);
			iEncodedParams = gen->Encoding().AllocL();
			}
		else
			{
			iEncodedParams = HBufC8::NewL(1);
			*iEncodedParams = KNullDesC8;
			}
		iAlgorithmId = EDSA;
		CleanupStack::PopAndDestroy(2);//seq, oid
		return;
		}
	if (count > 1)
		{
		TASN1DecGeneric* gen = seq->At(1);
		if (oidDes == KDH)
			{
			iEncodedParams = gen->Encoding().AllocL();
			iAlgorithmId = EDH;
			CleanupStack::PopAndDestroy(2);//seq, oid
			return;
			}
		if (oidDes == KRSA)
			{
			iAlgorithmId = ERSA;
			TASN1DecNull null;
			null.DecodeDERL(*gen);//just to check the syntax is OK
			iEncodedParams = HBufC8::NewL(1);
			*iEncodedParams = KNullDesC8;
			CleanupStack::PopAndDestroy(2);//seq, oid
			return;
			}
		if (oidDes == KMD5)
			{
			iAlgorithmId = EMD5;
			TASN1DecNull null;
			null.DecodeDERL(*gen);//just to check the syntax is OK
			iEncodedParams = HBufC8::NewL(1);
			*iEncodedParams = KNullDesC8;
			CleanupStack::PopAndDestroy(2);//seq, oid
			return;
			}
		if (oidDes == KMD2)
			{
			iAlgorithmId = EMD2;
			TASN1DecNull null;
			null.DecodeDERL(*gen);//just to check the syntax is OK
			iEncodedParams = HBufC8::NewL(1);
			*iEncodedParams = KNullDesC8;
			CleanupStack::PopAndDestroy(2);//seq, oid
			return;
			}
		if (oidDes == KSHA1)
			{
			iAlgorithmId = ESHA1;
			TASN1DecNull null;
			null.DecodeDERL(*gen);//just to check the syntax is OK
			iEncodedParams = HBufC8::NewL(1);
			*iEncodedParams = KNullDesC8;
			CleanupStack::PopAndDestroy(2);//seq, oid
			return;
			}

        if (oidDes == KSHA224)
            {
            iAlgorithmId = ESHA224;
            TASN1DecNull null;
            null.DecodeDERL(*gen);//just to check the syntax is OK
            iEncodedParams = HBufC8::NewL(1);
            *iEncodedParams = KNullDesC8;
            CleanupStack::PopAndDestroy(2);//seq, oid
            return;
            }
        if (oidDes == KSHA256)
            {
            iAlgorithmId = ESHA256;
            TASN1DecNull null;
            null.DecodeDERL(*gen);//just to check the syntax is OK
            iEncodedParams = HBufC8::NewL(1);
            *iEncodedParams = KNullDesC8;
            CleanupStack::PopAndDestroy(2);//seq, oid
            return;
            }      
        if (oidDes == KSHA384)
            {
            iAlgorithmId = ESHA384;
            TASN1DecNull null;
            null.DecodeDERL(*gen);//just to check the syntax is OK
            iEncodedParams = HBufC8::NewL(1);
            *iEncodedParams = KNullDesC8;
            CleanupStack::PopAndDestroy(2);//seq, oid
            return;
            }        
        if (oidDes == KSHA512)
            {
            iAlgorithmId = ESHA512;
            TASN1DecNull null;
            null.DecodeDERL(*gen);//just to check the syntax is OK
            iEncodedParams = HBufC8::NewL(1);
            *iEncodedParams = KNullDesC8;
            CleanupStack::PopAndDestroy(2);//seq, oid
            return;
            }         
	
		}
	User::Leave(KErrNotSupported);
	}
	
EXPORT_C CASN1EncSequence* CX509AlgorithmIdentifier::EncodeASN1DERLC() const
	{
	// the root sequence contains the signed object
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	
	//encode the oid
	CASN1EncObjectIdentifier* oid(NULL);
	switch (iAlgorithmId)
		{
	case EDSA:
		oid=CASN1EncObjectIdentifier::NewLC(KDSA);
		break;
		
	case EDH:
		oid=CASN1EncObjectIdentifier::NewLC(KDH);
		break;
		
	case ERSA:
		oid=CASN1EncObjectIdentifier::NewLC(KRSA);
		break;
		
	case EMD5:
		oid=CASN1EncObjectIdentifier::NewLC(KMD5);
		break;
		
	case EMD2:
		oid=CASN1EncObjectIdentifier::NewLC(KMD2);
		break;
		
	case ESHA1:
		oid=CASN1EncObjectIdentifier::NewLC(KSHA1);
		break;

	case ESHA224:
		oid=CASN1EncObjectIdentifier::NewLC(KSHA224);
		break;

	case ESHA256:
		oid=CASN1EncObjectIdentifier::NewLC(KSHA256);
		break;

	case ESHA384:
		oid=CASN1EncObjectIdentifier::NewLC(KSHA384);
		break;

	case ESHA512:
		oid=CASN1EncObjectIdentifier::NewLC(KSHA512);
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
	
	root->AddAndPopChildL(oid);
	
	//Encode parameters
	if (*iEncodedParams!=KNullDesC8)
		{
		CASN1EncEncoding* enc=CASN1EncEncoding::NewLC(*iEncodedParams);
		root->AddAndPopChildL(enc);
		}
	else
		{
		if (iAlgorithmId!=EDSA)
			{
			CASN1EncNull* enc=CASN1EncNull::NewLC();
			root->AddAndPopChildL(enc);				
			}
		}
		
	return root;		
	}
	

//signing algorithm id
EXPORT_C CX509SigningAlgorithmIdentifier* CX509SigningAlgorithmIdentifier::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509SigningAlgorithmIdentifier::NewL(aBinaryData, pos);
	}

EXPORT_C CX509SigningAlgorithmIdentifier* CX509SigningAlgorithmIdentifier::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509SigningAlgorithmIdentifier::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509SigningAlgorithmIdentifier* CX509SigningAlgorithmIdentifier::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509SigningAlgorithmIdentifier* self = CX509SigningAlgorithmIdentifier::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509SigningAlgorithmIdentifier* CX509SigningAlgorithmIdentifier::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509SigningAlgorithmIdentifier* self = new(ELeave) CX509SigningAlgorithmIdentifier;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509SigningAlgorithmIdentifier* CX509SigningAlgorithmIdentifier::NewL(const CAlgorithmIdentifier& aAsymmetricAlgorithm, const CAlgorithmIdentifier& aDigestAlgorithm)
 	{
 	CX509SigningAlgorithmIdentifier* self = CX509SigningAlgorithmIdentifier::NewLC(aAsymmetricAlgorithm, aDigestAlgorithm);
 	CleanupStack::Pop();
 	return self;
 	}
 
EXPORT_C CX509SigningAlgorithmIdentifier* CX509SigningAlgorithmIdentifier::NewLC(const CAlgorithmIdentifier& aAsymmetricAlgorithm, const CAlgorithmIdentifier& aDigestAlgorithm)
 	{
 	CX509SigningAlgorithmIdentifier* self = new(ELeave) CX509SigningAlgorithmIdentifier;
 	CleanupStack::PushL(self);
 	self->ConstructL(aAsymmetricAlgorithm, aDigestAlgorithm);
 	return self;
 	}

CX509SigningAlgorithmIdentifier::CX509SigningAlgorithmIdentifier()
	{
	}

void CX509SigningAlgorithmIdentifier::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();
	TASN1DecObjectIdentifier encOID;
	HBufC* oid = encOID.DecodeDERL(*(seq->At(0)));
	CleanupStack::PushL(oid);
	TPtrC oidDes(oid->Des()); 
	//none of the signing algorithms we support have parameters here...
	HBufC8* encodedParams = HBufC8::NewLC(1);
	*encodedParams = KNullDesC8;

	if (oidDes == KDSAWithSHA1)
		{
		//should be no params, but we allow params encoded as NULL for interop reasons
		TAlgorithmId algId = EDSA;
		TAlgorithmId digestId = ESHA1;
		iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
		iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
		if (count == 1)
			{			
			CleanupStack::PopAndDestroy(3);//seq, oid, encodedParams
			return;
			}
		}

	if (oidDes == KMD2WithRSA)
		{
		TAlgorithmId algId = ERSA;
		TAlgorithmId digestId = EMD2;
		iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
		iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
		}

	if (oidDes == KMD5WithRSA)
		{
		TAlgorithmId algId = ERSA;
		TAlgorithmId digestId = EMD5;
		iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
		iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
		}

	if (oidDes == KSHA1WithRSA  || oidDes == KSHA1WithRSASignature)
		{
		TAlgorithmId algId = ERSA;
		TAlgorithmId digestId = ESHA1;
		iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
		iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
		}

	if (oidDes == KSHA224WithRSA)
         {
         TAlgorithmId algId = ERSA;
         TAlgorithmId digestId = ESHA224;
         iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
         iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
         }
	
	if (oidDes == KSHA256WithRSA)
         {
         TAlgorithmId algId = ERSA;
         TAlgorithmId digestId = ESHA256;
         iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
         iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
         }

	if (oidDes == KSHA384WithRSA)
         {
         TAlgorithmId algId = ERSA;
         TAlgorithmId digestId = ESHA384;
         iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
         iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
         }

	if (oidDes == KSHA512WithRSA)
         {
         TAlgorithmId algId = ERSA;
         TAlgorithmId digestId = ESHA512;
         iDigestAlgorithm = CAlgorithmIdentifier::NewL(digestId, encodedParams->Des());
         iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(algId, encodedParams->Des());
         }

		//???not sure if we should just leave here...
	if (iDigestAlgorithm == NULL)
		{	
		User::Leave(KErrNotSupported);
		}
	else
		{
		if (count != 2)
			{
			// Shouldn't ever get here, since RFC2459 mandates having 2
			// data items.  However, some people miss out the second
			// when it's NULL, so we'll not report and error here
			}
		else
			{
			TASN1DecGeneric* gen = seq->At(1);
			TASN1DecNull null;
			null.DecodeDERL(*gen);//just to check the syntax is OK
			}
		}
	CleanupStack::PopAndDestroy(3);//seq, oid, encodedParams
	}

void CX509SigningAlgorithmIdentifier::ConstructL(const CAlgorithmIdentifier& aAsymmetricAlgorithm, const CAlgorithmIdentifier& aDigestAlgorithm)
 	{
 	iDigestAlgorithm = CAlgorithmIdentifier::NewL(aDigestAlgorithm);
 	iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(aAsymmetricAlgorithm);
 	};

//subject public key info
EXPORT_C CX509SubjectPublicKeyInfo* CX509SubjectPublicKeyInfo::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509SubjectPublicKeyInfo* self = CX509SubjectPublicKeyInfo::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509SubjectPublicKeyInfo* CX509SubjectPublicKeyInfo::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509SubjectPublicKeyInfo* self = new(ELeave) CX509SubjectPublicKeyInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509SubjectPublicKeyInfo* CX509SubjectPublicKeyInfo::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509SubjectPublicKeyInfo::NewL(aBinaryData, pos);
	}

EXPORT_C CX509SubjectPublicKeyInfo* CX509SubjectPublicKeyInfo::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509SubjectPublicKeyInfo::NewLC(aBinaryData, pos);
	}

CX509SubjectPublicKeyInfo::CX509SubjectPublicKeyInfo()
	{
	}

void CX509SubjectPublicKeyInfo::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 2, KMaxTInt);
	iAlgId = CX509AlgorithmIdentifier::NewL(seq->At(0)->Encoding());
	TASN1DecBitString encBS;
	iEncodedKeyData = encBS.ExtractOctetStringL(*(seq->At(1)));
	CleanupStack::PopAndDestroy();//seq
	}

//generic X 509 extension syntax
EXPORT_C CX509CertExtension* CX509CertExtension::NewL(const CX509CertExtension& aExtension)
	{
	CX509CertExtension* self = CX509CertExtension::NewLC(aExtension);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewLC(const CX509CertExtension& aExtension)
	{
	CX509CertExtension* self = new(ELeave) CX509CertExtension;
	CleanupStack::PushL(self);
	self->ConstructL(aExtension);
	return self;
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509CertExtension* self = CX509CertExtension::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509CertExtension* self = new(ELeave) CX509CertExtension;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509CertExtension::NewL(aBinaryData, pos);
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509CertExtension::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewL(const TDesC& aCertExtOID, 
							const TBool aCritical,
							const TDesC8& aCertExtValue)
	{
	CX509CertExtension* self = CX509CertExtension::NewLC(aCertExtOID, aCritical, aCertExtValue);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509CertExtension* CX509CertExtension::NewLC(const TDesC& aCertExtOID, 
							const TBool aCritical,
							const TDesC8& aCertExtValue)
	{
	CX509CertExtension* self = new(ELeave) CX509CertExtension;
	CleanupStack::PushL(self);
	self->ConstructL(aCertExtOID, aCritical, aCertExtValue);
	return self;
	}

void CX509CertExtension::ConstructL(const CX509CertExtension& aExtension)
	{
	iCritical = aExtension.iCritical;
	iId = aExtension.iId->Des().AllocL();
	iData = aExtension.iData->Des().AllocL();
	}

void CX509CertExtension::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 2, KMaxTInt);

	TASN1DecObjectIdentifier encOID;
	iId = encOID.DecodeDERL(*(seq->At(0)));
	//second is either critical flag, or the ext
	TASN1DecGeneric* second = seq->At(1);
	if (second->Tag() != EASN1Boolean)
		{
		iData = second->Encoding().AllocL();
		aPos += second->LengthDER();
		}
	else
		{
		TASN1DecBoolean encBool;
		iCritical = encBool.DecodeDERL(*second);
		if (seq->Count() != 3)
			{
			User::Leave(KErrArgument);
			}

		TASN1DecGeneric* third = seq->At(2);
		iData = third->Encoding().AllocL();
		}
	CleanupStack::PopAndDestroy();//seq
	}

void CX509CertExtension::ConstructL(const TDesC& aCertExtOID, 
									const TBool aCritical,
									const TDesC8& aCertExtValue)
	{
	iId = aCertExtOID.AllocL();
	iCritical = aCritical;
	iData = aCertExtValue.AllocL();
	}

EXPORT_C CX509CertExtension::~CX509CertExtension()
	{
	delete iData;
	delete iId;
	}

EXPORT_C TBool CX509CertExtension::Critical() const
	{
	return iCritical;
	}

EXPORT_C TPtrC CX509CertExtension::Id() const	//OID for the extension
	{
	return iId->Des();
	}

EXPORT_C TPtrC8 CX509CertExtension::Data() const	 //the extension itself
	{
	return iData->Des();
	}

EXPORT_C CASN1EncSequence* CX509CertExtension::EncodeASN1DERLC() const
	{
	CASN1EncSequence *result = CASN1EncSequence::NewLC();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(*iId);
	result->AddAndPopChildL(oid);
	// Encode critical flag only if true
	if (iCritical)
		{
		CASN1EncBoolean *critical = CASN1EncBoolean::NewLC(iCritical);
		result->AddAndPopChildL(critical);
		}	
	CASN1EncOctetString *data = CASN1EncOctetString::NewLC(*iData);
	result->AddAndPopChildL(data);
	return result;
	}

CX509CertExtension::CX509CertExtension()
	:iCritical(EFalse)
	{
	}

//CX509Certificate
EXPORT_C CX509Certificate* CX509Certificate::NewL(const CX509Certificate& aCert)
	{
	CX509Certificate* self = CX509Certificate::NewLC(aCert);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509Certificate* CX509Certificate::NewLC(const CX509Certificate& aCert)
	{
	CX509Certificate* self = new(ELeave) CX509Certificate;
	CleanupStack::PushL(self);
	self->ConstructL(aCert);
	return self;
	}

EXPORT_C CX509Certificate* CX509Certificate::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509Certificate::NewL(aBinaryData, pos);
	}

EXPORT_C CX509Certificate* CX509Certificate::NewLC(const TDesC8& aBinaryData)
	{	
	TInt pos = 0;
	return CX509Certificate::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509Certificate* CX509Certificate::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509Certificate* self = CX509Certificate::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509Certificate* CX509Certificate::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{	
	CX509Certificate* self = new(ELeave) CX509Certificate();
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509Certificate* CX509Certificate::NewL(RReadStream& aStream)
	{
	CX509Certificate* self = CX509Certificate::NewLC(aStream);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509Certificate* CX509Certificate::NewLC(RReadStream& aStream)
	{
	CX509Certificate* self = new(ELeave) CX509Certificate;
	CleanupStack::PushL(self);
	self->InternalizeL(aStream);
	return self;
	}

void CX509Certificate::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric gen(aBinaryData.Right(aBinaryData.Length() - aPos));
	gen.InitL();
	
	// The outermost tag for X509 certificates is always a sequence.
	// Since this tag does not form part of the signed data it is possible
	// to corrupt the tag by changing it to any other ASN.1 tag and process
	// the rest of the certificate as normal.
	// However, we still reject the certificate anyway to avoid 
	// confusion because the data does not match the X.509 specification.	
	if (gen.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	
	aPos += gen.LengthDER();
	iKeyFactory = new(ELeave) TX509KeyFactory;

	iEncoding = gen.Encoding().AllocL();

	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(*iEncoding, pos, 3, 3);	
	TASN1DecGeneric* encSigAlg = seq->At(1);
	iSigningAlgorithm = CX509SigningAlgorithmIdentifier::NewL(encSigAlg->Encoding());
	TASN1DecBitString encBS;
	iSignature = encBS.ExtractOctetStringL(*(seq->At(2)));
	CleanupStack::PopAndDestroy();//seq

	CSHA1* hash = CSHA1::NewL();
	CleanupStack::PushL(hash);
	iFingerprint = hash->Final(Encoding()).AllocL();
	CleanupStack::PopAndDestroy();//hash
	ConstructCertL();
	}

void CX509Certificate::ConstructL(const CX509Certificate& aCertificate)
	{
	iKeyFactory = new(ELeave) TX509KeyFactory;

	iEncoding = aCertificate.iEncoding->AllocL();
	iSignature = aCertificate.iSignature->AllocL();
	iFingerprint = aCertificate.iFingerprint->AllocL();
	iSigningAlgorithm = CSigningAlgorithmIdentifier::NewL(aCertificate.SigningAlgorithm());	
	iSerialNumber = aCertificate.iSerialNumber->Des().AllocL();
	iIssuerName = CX500DistinguishedName::NewL(*(aCertificate.iIssuerName));
	iValidityPeriod = new(ELeave) CValidityPeriod(*(aCertificate.iValidityPeriod));
	iSubjectName = CX500DistinguishedName::NewL(*(aCertificate.iSubjectName));
	iSubjectPublicKeyInfo = CSubjectPublicKeyInfo::NewL(*(aCertificate.iSubjectPublicKeyInfo));
	iIssuerUid = aCertificate.iIssuerUid->Des().AllocL();
	iSubjectUid = aCertificate.iSubjectUid->Des().AllocL();
	iExtensions = new(ELeave) CArrayPtrFlat<CX509CertExtension> (1);
	TInt count = aCertificate.iExtensions->Count();
	for (TInt i = 0; i < count; i++)
		{
		CX509CertExtension* ext = CX509CertExtension::NewLC(*aCertificate.iExtensions->At(i));
		iExtensions->AppendL(ext);
		CleanupStack::Pop();//ext
		}
	iVersion = aCertificate.Version();
	InitDataElementsL(aCertificate);
	}

void CX509Certificate::InitDataElementsL(const CX509Certificate& aCertificate)
	{
	iDataElements = new(ELeave) TFixedArray<TPtrC8*, KX509MaxDataElements>;
	iDataElements->Reset();
	TPtrC8 signedData = SignedDataL();
	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(signedData, pos, 6, KMaxTInt);//6 is the minimum number of elements in an x509 cert
	pos = 0;

	TPtrC8** pElement = iDataElements->Begin();
	*pElement++ = aCertificate.DataElementEncoding(CX509Certificate::EVersionNumber)? new(ELeave) TPtrC8(seq->At(pos++)->Encoding()):NULL;
	for (TInt i = 0; i < 6; i++)	//init all the non-optional elements
		{
		*pElement++ = new(ELeave) TPtrC8(seq->At(pos++)->Encoding());
		}
	*pElement++ = aCertificate.DataElementEncoding(CX509Certificate::EIssuerUID)? new(ELeave) TPtrC8(seq->At(pos++)->Encoding()):NULL;
	*pElement++ = aCertificate.DataElementEncoding(CX509Certificate::ESubjectUID)? new(ELeave) TPtrC8(seq->At(pos++)->Encoding()):NULL;
	*pElement++ = aCertificate.DataElementEncoding(CX509Certificate::EExtensionList)? new(ELeave) TPtrC8(seq->At(pos++)->Encoding()):NULL;
	CleanupStack::PopAndDestroy();
	}

void CX509Certificate::ConstructCertL()
	{
	TPtrC8 signedData = SignedDataL();
	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(signedData, pos, 6, KMaxTInt);//6 is the minimum number of elements in an x509 cert
	TInt count = seq->Count();
	pos = 0;
	TASN1DecGeneric* curr = seq->At(pos);
	pos++;
	iDataElements = new(ELeave) TFixedArray<TPtrC8*, KX509MaxDataElements>;
	iDataElements->Reset();
	TPtrC8** pElement = iDataElements->Begin();
	if ((curr->Class() == EContextSpecific) && (curr->Tag() == 0))
		{
		//version!
		TASN1DecGeneric ver(curr->GetContentDER());
		ver.InitL();
		TPtrC8 pVer8 = ver.GetContentDER();
		if(pVer8.Length() != 1)
			{
			User::Leave(KErrArgument);
			}
		iVersion = (pVer8[0]) + 1;
		if ((iVersion < 1) || (iVersion > 3) || (count < 7))
			{
			User::Leave(KErrArgument);
			}
		*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
		curr = seq->At(pos);
		pos++;
		}
	else
		{
		*pElement++ = NULL;
		}
	if (curr->Tag() != EASN1Integer)
		{
		User::Leave(KErrArgument);
		}
	iSerialNumber = (curr->GetContentDER()).AllocL();
	*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
	curr = seq->At(pos);
	pos++;
	CX509SigningAlgorithmIdentifier* algorithmId = CX509SigningAlgorithmIdentifier::NewLC(curr->Encoding());
	if (!(SigningAlgorithm() == *(algorithmId)))
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy();//algorithmId
	*pElement++ = new(ELeave) TPtrC8(curr->Encoding());

	curr = seq->At(pos);
	pos++;
	iIssuerName = CX500DistinguishedName::NewL(curr->Encoding());
	*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
	curr = seq->At(pos);
	pos++;
	iValidityPeriod = CX509ValidityPeriod::NewL(curr->Encoding());
	*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
	curr = seq->At(pos);
	pos++;
	iSubjectName = CX500DistinguishedName::NewL(curr->Encoding());
	*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
	curr = seq->At(pos);
	pos++;
	iSubjectPublicKeyInfo = CX509SubjectPublicKeyInfo::NewL(curr->Encoding());
	*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
	//do issuer uid, subject uid, exts
	//these are all optional
	TBool hasIssuerUid = EFalse;
	TBool hasSubjectUid = EFalse;
	TBool hasExts = EFalse;
	iExtensions = new(ELeave)CArrayPtrFlat<CX509CertExtension> (1);
	if (pos < count)//otherwise there aren't any of 'em
		{
		curr = seq->At(pos);
		pos++;
		if (curr->Class() != EContextSpecific)
			{
			User::Leave(KErrArgument);
			}
		switch(curr->Tag())
			{
			case 1:
				{
				iIssuerUid = DecodeUidL(curr->GetContentDER(), hasIssuerUid);
				*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
				break;
				}
			case 2:
				{
				iSubjectUid = DecodeUidL(curr->GetContentDER(), hasSubjectUid);
				*pElement++ = NULL;
				*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
				break;
				}
			case 3:
				{
				DecodeExtsL(curr->GetContentDER(), hasExts);
				*pElement++ = NULL;
				*pElement++ = NULL;
				*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
				break;
				}
			default:
				{
				User::Leave(KErrArgument);
				}
			}
		if (pos < count)
			{
			curr = seq->At(pos);
			pos++;
			switch(curr->Tag())
				{
				case 2:
					{
					iSubjectUid = DecodeUidL(curr->GetContentDER(), hasSubjectUid);
					*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
					break;
					}
				case 3:
					{
					DecodeExtsL(curr->GetContentDER(), hasExts);
					*pElement++ = NULL;
					*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
					break;
					}
				default:
					{
					User::Leave(KErrArgument);
					}
				}
			if (pos < count)
				{
				curr = seq->At(pos);
				pos++;
				if (curr->Tag() == 3)
					{
					DecodeExtsL(curr->GetContentDER(), hasExts);
					*pElement++ = new(ELeave) TPtrC8(curr->Encoding());
					}
				else
					{
					User::Leave(KErrArgument);
					}
				}
			}
		}
	if (pos != count)
		{
		User::Leave(KErrArgument);
		}
	if (!iIssuerUid)
		{
		iIssuerUid = HBufC8::NewL(1);
		*iIssuerUid = KNullDesC8;
		}
	if (!iSubjectUid)
		{
		iSubjectUid = HBufC8::NewL(1);
		*iSubjectUid = KNullDesC8;
		}
		
	// we have not checked for the certificate version number based on 
	// the certificate contents. This is primarily done to avoid BC for 
	// clients who are still using malformed certificates.
	
	CleanupStack::PopAndDestroy();//seq
	}

CX509Certificate::CX509Certificate()
	:iVersion(1)
	{
	}

EXPORT_C CX509Certificate::~CX509Certificate()
	{
	delete iIssuerName;
	delete iSubjectName;
	delete iIssuerUid;
	delete iSubjectUid;
	
	if (iDataElements != NULL)
		{
		for (TInt i = 0; i < KX509MaxDataElements; i++)
			{
			delete iDataElements->At(i);
			}
		delete iDataElements;
		}
	if (iExtensions != NULL)
		{
		iExtensions->ResetAndDestroy();
		}
	delete iExtensions;
	}

EXPORT_C TBool CX509Certificate::IsEqualL(const CX509Certificate& aCert) const
	{
	return	(	(*(iSerialNumber) == *(aCert.iSerialNumber)) && 
				(iIssuerName->ExactMatchL(*(aCert.iIssuerName)))	);
	}

EXPORT_C void CX509Certificate::InternalizeL(RReadStream& aStream)
	{
	if (iIssuerName != NULL) //just to check cert is uninitialised
		{
		User::Leave(KErrArgument);
		}
	iKeyFactory = new(ELeave) TX509KeyFactory;
	
	TInt len = aStream.ReadInt32L(); //Read the length of the streamed encoding
	HBufC8* temp= HBufC8::NewLC(len);	
	TPtr8 ptr=temp->Des();
	aStream.ReadL(ptr,len);
	iEncoding=temp->AllocL();
	CleanupStack::PopAndDestroy(); // temp

	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(*iEncoding, pos, 3, 3);	
	TASN1DecGeneric* encSigAlg = seq->At(1);
	iSigningAlgorithm = CX509SigningAlgorithmIdentifier::NewL(encSigAlg->Encoding());
	TASN1DecBitString encBS;
	iSignature = encBS.ExtractOctetStringL(*(seq->At(2)));
	CleanupStack::PopAndDestroy();//seq	

	CSHA1* hash = CSHA1::NewL();
	CleanupStack::PushL(hash);
	iFingerprint = hash->Final(Encoding()).AllocL();
	CleanupStack::PopAndDestroy();//hash

	ConstructCertL();
	}

void CX509Certificate::DecodeExtsL(const TDesC8& aBinaryData, TBool& aHasElementAlready)
	{
	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, pos);
	TInt count = seq->Count();
	for (TInt i = 0; i < count; i++)
		{
		TASN1DecGeneric* gen = seq->At(i);
		CX509CertExtension* ext = CX509CertExtension::NewLC(gen->Encoding());
		iExtensions->AppendL(ext);
		CleanupStack::Pop();//ext
		}
	CleanupStack::PopAndDestroy();//
	aHasElementAlready = ETrue;
	}

HBufC8* CX509Certificate::DecodeUidL(const TDesC8& aBinaryData, TBool& aHasElementAlready)
	{
	if ((aHasElementAlready) || (iVersion ==1))
		{
		User::Leave(KErrArgument);
		}
	aHasElementAlready = ETrue;
	return aBinaryData.AllocL();
	}

//***************************************************************************************//
//extra accessors
EXPORT_C const TPtrC8 CX509Certificate::SignedDataL() const
	{
	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(*iEncoding, pos, 3, 3);
	TASN1DecGeneric* gen = seq->At(0);
	TPtrC8 res = gen->Encoding();
	CleanupStack::PopAndDestroy();
	return res;
	}

EXPORT_C TInt CX509Certificate::Version() const
	{
	return iVersion;
	}

/**
* If the certificate has decoded the members from TeletexString then the return value 
* may be incorrect because TeletexString type is not fully supported by this library.
* Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
* null characters in the second byte of each character. This will work as expected 
* for cases where the string contains ASCII data.
*/
EXPORT_C const CX500DistinguishedName& CX509Certificate::IssuerName() const
	{
	return *iIssuerName;
	}

/**
* If the certificate has decoded the members from TeletexString then the return value 
* may be incorrect because TeletexString type is not fully supported by this library.
* Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
* null characters in the second byte of each character. This will work as expected 
* for cases where the string contains ASCII data.
*/
EXPORT_C const CX500DistinguishedName& CX509Certificate::SubjectName() const
	{
	return *iSubjectName;
	}

EXPORT_C const CArrayPtrFlat<CX509CertExtension>& CX509Certificate::Extensions() const
	{
	return *iExtensions;
	}

EXPORT_C const CX509CertExtension* CX509Certificate::Extension(const TDesC& aExtensionName) const
	{
	TInt count = iExtensions->Count();
	for (TInt i = 0; i < count; i++)
		{
		CX509CertExtension* ext = iExtensions->At(i);
		if (ext->Id() == aExtensionName)
			return ext;
		}
	return NULL;
	}

EXPORT_C const TPtrC8* CX509Certificate::DataElementEncoding(const TUint aIndex) const
	{
	return iDataElements->At(aIndex);
	}

/**
* If the certificate has decoded the members from TeletexString then the return value 
* may be incorrect because TeletexString type is not fully supported by this library.
* Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
* null characters in the second byte of each character. This will work as expected 
* for cases where the string contains ASCII data.
*/
EXPORT_C HBufC* CX509Certificate::IssuerL() const
	{
	return iIssuerName->DisplayNameL();
	}

/**
* If the certificate has decoded the members from TeletexString then the return value 
* may be incorrect because TeletexString type is not fully supported by this library.
* Instead the decode methods perform a direct conversion from 8 to 16bits by adding 
* null characters in the second byte of each character. This will work as expected 
* for cases where the string contains ASCII data.
*/
EXPORT_C HBufC* CX509Certificate::SubjectL() const
	{
	return iSubjectName->DisplayNameL();
	}

EXPORT_C TBool CX509Certificate::IsSelfSignedL() const
	{
	if (iSubjectName->Count() > 0)
		{
		return iSubjectName->ExactMatchL(*iIssuerName);
		}
	else
		{
		TBool res = EFalse;
		const CX509CertExtension* subjectExt = Extension(KIssuerAltName);
		const CX509CertExtension* issuerExt = Extension(KSubjectAltName);
		if ((subjectExt) && (issuerExt))
			{
			const CX509AltNameExt* issuerAltName = CX509AltNameExt::NewLC(subjectExt->Data());
			const CX509AltNameExt* subjectAltName = CX509AltNameExt::NewLC(issuerExt->Data());
			if (subjectAltName->Match(*issuerAltName))
				{
				res = ETrue;
				}
			CleanupStack::PopAndDestroy(2);//subjectAltName, issuerAltName
			}
		return res;
		} 
	}


EXPORT_C TKeyIdentifier CX509Certificate::KeyIdentifierL() const	
	{
	if (iSubjectPublicKeyInfo->AlgorithmId() != EDSA)
		{
		// The base class handles the RSA case and leaves with KErrNotSupported if
		// it is not RSA.
		return CCertificate::KeyIdentifierL();
		}
	else
		{
		// This handles the DSA case
		CDSAPublicKey* dsaKey = 
						iKeyFactory->DSAPublicKeyL(iSubjectPublicKeyInfo->EncodedParams(), 
													iSubjectPublicKeyInfo->KeyData());
		CleanupStack::PushL(dsaKey);
		TKeyIdentifier retVal;
		KeyIdentifierUtil::DSAKeyIdentifierL(*dsaKey, retVal);
		CleanupStack::PopAndDestroy(dsaKey);
		return retVal;
		}
	}

EXPORT_C TKeyIdentifier CX509Certificate::SubjectKeyIdentifierL() const
	{	
	const CX509CertExtension* subjectKeyIdExtStr = this->Extension(KSubjectKeyId);
	if (subjectKeyIdExtStr)
		{
		TKeyIdentifier ret;
		CX509SubjectKeyIdExt *subjectKeyIdExt = CX509SubjectKeyIdExt::NewLC(subjectKeyIdExtStr->Data());
		// Currently, we do not support key identifiers larger than 160 bits - make sure not to overflow the descriptor
		if (subjectKeyIdExt->KeyId().Length() <= ret.MaxLength())
			{
			ret.Copy(subjectKeyIdExt->KeyId());
			CleanupStack::PopAndDestroy(subjectKeyIdExt);
			return ret;
			}
		CleanupStack::PopAndDestroy(subjectKeyIdExt);
		}
		
	return KeyIdentifierL();
	}


EXPORT_C TKeyIdentifier CX509Certificate::SubjectKeyIdL() const
	{
	// if it is a v1 or v2 type then there is no way of knowing which is a CA, treat all certs as CA as done in the certificate recognizer.
	if (Version() < 3 )
		{
		return SubjectKeyIdentifierL();
		}
	
	// if it is x509 v3 certificate then check for the basic constraint extension.
	const CX509CertExtension* ext = Extension(KBasicConstraints);
	if (ext)
		{
		CX509BasicConstraintsExt* basic = CX509BasicConstraintsExt::NewLC(ext->Data());
		TBool markedAsCA = basic->IsCA();
		CleanupStack::PopAndDestroy(basic);
		// it can be an intermediate as well as root CA
		if ( markedAsCA )
			{
			return SubjectKeyIdentifierL();
			}
		}
	// For non-CA certs, use the recommended method of computing it from RFC5280, section 4.2.1.2
	return KeyIdentifierL();									
		
	}

