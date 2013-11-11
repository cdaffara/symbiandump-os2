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


#include <bigint.h>
#include <asymmetric.h>
#include <hash.h>
#include <padding.h>
#include <keyidentifierutil.h>
#include <signed.h>

//RSA signature result
EXPORT_C TBool CRSASignatureResult::operator== (const CRSASignatureResult& aResult) const 
	{
	return ((*(iDigestAlgorithm) == *(aResult.iDigestAlgorithm)) &&
			(*(iDigest)==*(aResult.iDigest)));
	}

EXPORT_C CRSASignatureResult::~CRSASignatureResult()
	{
	delete iDigestAlgorithm;
	delete iDigest;
	}

//validity period
EXPORT_C CValidityPeriod::CValidityPeriod(const CValidityPeriod& aValidityPeriod)
	:iStart(aValidityPeriod.iStart), iFinish(aValidityPeriod.iFinish)
	{
	}

EXPORT_C CValidityPeriod::CValidityPeriod()
	{
	}

EXPORT_C TBool CValidityPeriod::Valid(const TTime& aTime) const
	{
	return ((iStart < aTime) && (iFinish > aTime));
	}

EXPORT_C const TTime& CValidityPeriod::Start() const
	{
	return iStart;
	}

EXPORT_C const TTime& CValidityPeriod::Finish() const
	{
	return iFinish;
	}

//******************************************************************************//
//algorithm id
EXPORT_C CAlgorithmIdentifier* CAlgorithmIdentifier::NewL(const CAlgorithmIdentifier& aAlgorithmIdentifier)
	{
	CAlgorithmIdentifier* self = CAlgorithmIdentifier::NewLC(aAlgorithmIdentifier);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CAlgorithmIdentifier* CAlgorithmIdentifier::NewLC(const CAlgorithmIdentifier& aAlgorithmIdentifier)
	{
	CAlgorithmIdentifier* self = new(ELeave) CAlgorithmIdentifier;
	CleanupStack::PushL(self);
	self->ConstructL(aAlgorithmIdentifier);
	return self;
	}

EXPORT_C CAlgorithmIdentifier* CAlgorithmIdentifier::NewL(TAlgorithmId& aAlgorithmId, const TDesC8& aEncodedParams)
	{
	CAlgorithmIdentifier* self = CAlgorithmIdentifier::NewLC(aAlgorithmId, aEncodedParams);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CAlgorithmIdentifier* CAlgorithmIdentifier::NewLC(TAlgorithmId& aAlgorithmId, const TDesC8& aEncodedParams)
	{
	CAlgorithmIdentifier* self = new(ELeave) CAlgorithmIdentifier(aAlgorithmId);
	CleanupStack::PushL(self);
	self->ConstructL(aEncodedParams);
	return self;
	}

EXPORT_C CAlgorithmIdentifier::CAlgorithmIdentifier(TAlgorithmId& aAlgorithmId)
	:iAlgorithmId(aAlgorithmId)
	{
	}

EXPORT_C CAlgorithmIdentifier::CAlgorithmIdentifier()
	{
	}

EXPORT_C void CAlgorithmIdentifier::ConstructL(const TDesC8& aEncodedParams)
	{
	iEncodedParams = aEncodedParams.AllocL();
	}


EXPORT_C TBool CAlgorithmIdentifier::operator==(const CAlgorithmIdentifier& aAlgorithmIdentifier) const 
	{
	return ((iAlgorithmId == aAlgorithmIdentifier.iAlgorithmId) &&
			(*(iEncodedParams) == *(aAlgorithmIdentifier.iEncodedParams)));
	}

EXPORT_C TAlgorithmId CAlgorithmIdentifier::Algorithm() const
	{
	return iAlgorithmId;
	}

EXPORT_C TPtrC8 CAlgorithmIdentifier::EncodedParams() const
	{
	return iEncodedParams->Des();
	}

EXPORT_C CAlgorithmIdentifier::~CAlgorithmIdentifier()
	{
	delete iEncodedParams;
	}

EXPORT_C void CAlgorithmIdentifier::ConstructL(const CAlgorithmIdentifier& aAlgorithmIdentifier)
	{
	iAlgorithmId = aAlgorithmIdentifier.iAlgorithmId;
	iEncodedParams = aAlgorithmIdentifier.iEncodedParams->AllocL();
	}

//***********************************************************************//
//signing algorithm id
EXPORT_C CSigningAlgorithmIdentifier* CSigningAlgorithmIdentifier::NewL(const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier)
	{
	CSigningAlgorithmIdentifier* self = CSigningAlgorithmIdentifier::NewLC(aSigningAlgorithmIdentifier);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CSigningAlgorithmIdentifier* CSigningAlgorithmIdentifier::NewLC(const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier)
	{
	CSigningAlgorithmIdentifier* self = new(ELeave) CSigningAlgorithmIdentifier;
	CleanupStack::PushL(self);
	self->ConstructL(aSigningAlgorithmIdentifier);
	return self;
	}

EXPORT_C TBool CSigningAlgorithmIdentifier::operator == (const CSigningAlgorithmIdentifier& aSigningAlgorithmIdentifier) const
	{
	return ((*(iAsymmetricAlgorithm) == *(aSigningAlgorithmIdentifier.iAsymmetricAlgorithm)) &&
			(*(iDigestAlgorithm) == *(aSigningAlgorithmIdentifier.iDigestAlgorithm)));
	}

void CSigningAlgorithmIdentifier::ConstructL(const CSigningAlgorithmIdentifier& aAlgorithmIdentifier)
	{
	iAsymmetricAlgorithm = CAlgorithmIdentifier::NewL(*(aAlgorithmIdentifier.iAsymmetricAlgorithm));
	iDigestAlgorithm = CAlgorithmIdentifier::NewL(*(aAlgorithmIdentifier.iDigestAlgorithm));
	}

EXPORT_C const CAlgorithmIdentifier& CSigningAlgorithmIdentifier::AsymmetricAlgorithm() const
	{
	return *iAsymmetricAlgorithm;
	}

EXPORT_C const CAlgorithmIdentifier& CSigningAlgorithmIdentifier::DigestAlgorithm() const
	{
	return *iDigestAlgorithm;
	}

EXPORT_C CSigningAlgorithmIdentifier::~CSigningAlgorithmIdentifier()
	{
	delete iAsymmetricAlgorithm;
	delete iDigestAlgorithm;
	}

//************************************************************************//
//subject public key info
EXPORT_C CSubjectPublicKeyInfo* CSubjectPublicKeyInfo::NewL(const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo)
	{
	CSubjectPublicKeyInfo* self = CSubjectPublicKeyInfo::NewLC(aSubjectPublicKeyInfo);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CSubjectPublicKeyInfo* CSubjectPublicKeyInfo::NewLC(const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo)
	{
	CSubjectPublicKeyInfo* self = new(ELeave) CSubjectPublicKeyInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aSubjectPublicKeyInfo);
	return self;
	}

EXPORT_C TAlgorithmId CSubjectPublicKeyInfo::AlgorithmId() const
	{
	return iAlgId->Algorithm();
	}

EXPORT_C const TPtrC8 CSubjectPublicKeyInfo::EncodedParams() const
	{
	return iAlgId->EncodedParams();
	}

EXPORT_C const TPtrC8 CSubjectPublicKeyInfo::KeyData() const
	{
	return iEncodedKeyData->Des();
	}

EXPORT_C CSubjectPublicKeyInfo::~CSubjectPublicKeyInfo()
	{
	delete iAlgId;
	delete iEncodedKeyData;
	}

EXPORT_C void CSubjectPublicKeyInfo::ConstructL(const CSubjectPublicKeyInfo& aSubjectPublicKeyInfo)
	{
	iAlgId = CAlgorithmIdentifier::NewL(*(aSubjectPublicKeyInfo.iAlgId));
	iEncodedKeyData = aSubjectPublicKeyInfo.iEncodedKeyData->AllocL();
	}

//******************************************************************//
//signed object parameters

	//ctors

EXPORT_C CSigningKeyParameters* CSigningKeyParameters::NewL()
	{
	CSigningKeyParameters* self = new(ELeave) CSigningKeyParameters;
	return self;
	}

EXPORT_C CSigningKeyParameters* CSigningKeyParameters::NewLC()
	{
	CSigningKeyParameters* self = CSigningKeyParameters::NewL();
	CleanupStack::PushL(self);
	return self;
	}

	//copy ctors
EXPORT_C CSigningKeyParameters* CSigningKeyParameters::NewL(const CSigningKeyParameters& aParameters)
	{
	CSigningKeyParameters* self = CSigningKeyParameters::NewLC(aParameters);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CSigningKeyParameters* CSigningKeyParameters::NewLC(const CSigningKeyParameters& aParameters)
	{
	CSigningKeyParameters* self = new(ELeave) CSigningKeyParameters;
	CleanupStack::PushL(self);
	self->ConstructL(aParameters);
	return self;
	}

void CSigningKeyParameters::ConstructL(const CSigningKeyParameters& aParameters)
	{
	SetDSAParamsL(*(aParameters.iDSAParams));
	}

	//dtor
EXPORT_C CSigningKeyParameters::~CSigningKeyParameters()
	{
	delete iDSAParams;
	}

EXPORT_C void CSigningKeyParameters::SetDSAParamsL(const CDSAParameters& aParams)
	{
	delete iDSAParams;
	iDSAParams = NULL;	//	In case the next fails
	
//	Copy the TIntegers
	RInteger p = RInteger::NewL(aParams.P());
	CleanupStack::PushL(p);
	
	RInteger q = RInteger::NewL(aParams.Q());
	CleanupStack::PushL(q);

	RInteger g = RInteger::NewL(aParams.G());
	CleanupStack::PushL(g);

	iDSAParams = CDSAParameters::NewL(p, q, g);
	CleanupStack::Pop(3, &p);
	}

const CDSAParameters* CSigningKeyParameters::DSAParams() const
	{
	return iDSAParams;
	}

CSigningKeyParameters::CSigningKeyParameters()
	{
	}

//******************************************************************//
//signed object
EXPORT_C TBool CSignedObject::VerifySignatureL(const TDesC8& aEncodedKey) const
	{
	const CAlgorithmIdentifier& algId = iSigningAlgorithm->AsymmetricAlgorithm();
	const CAlgorithmIdentifier& digestId = iSigningAlgorithm->DigestAlgorithm();
	
	TBool verifyResult = EFalse;
	if (algId.Algorithm() == EDSA)
		{
		if (digestId.Algorithm() != ESHA1)
			{
			User::Leave(KErrArgument);
			}
			
		CSHA1* sha1 = CSHA1::NewL();
		CleanupStack::PushL(sha1);
		TPtrC8 signedData = SignedDataL();
		TPtrC8 hash = sha1->Final(signedData);
		verifyResult=VerifySignatureL(aEncodedKey, hash);
		CleanupStack::PopAndDestroy(sha1);//hash
		}
	else
		{
		TRAPD(result, verifyResult = VerifyRSASignatureL(aEncodedKey));
		if (result==KErrNoMemory)	//	Leave if OOM, anything else is a verify
			User::Leave(result);	//	error => verifyResult = EFalse

		}

	return verifyResult;
	}

EXPORT_C TBool CSignedObject::VerifySignatureL(const TDesC8& aEncodedKey, const TDesC8& aHash) const
	{
	const CAlgorithmIdentifier& algId = iSigningAlgorithm->AsymmetricAlgorithm();
	const CAlgorithmIdentifier& digestId = iSigningAlgorithm->DigestAlgorithm();
	
	TBool verifyResult = EFalse;
	if (algId.Algorithm() == EDSA)
		{
		if (digestId.Algorithm() != ESHA1)
			{
			User::Leave(KErrArgument);
			}

		CDSAPublicKey* key = NULL;
		if (algId.EncodedParams().Length() != 0)
			{
			key = iKeyFactory->DSAPublicKeyL(algId.EncodedParams(), aEncodedKey);
			}
		else
			{
			if (iParameters != NULL)
				{
				const CDSAParameters* params = iParameters->DSAParams();
				if (!params)
					{					
					User::Leave(KErrArgument);//no params
					}
				else
					{
					key = iKeyFactory->DSAPublicKeyL(*params, aEncodedKey);
					}
				}
			}
		if (key == NULL)
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::PushL(key);
		CDSAVerifier* verifier = CDSAVerifier::NewLC(*key);
		CDSASignature* sig = iKeyFactory->DSASignatureL(Signature());
		CleanupStack::PushL(sig);
		verifyResult = verifier->VerifyL(aHash, *sig);
		CleanupStack::PopAndDestroy(3);//	key, verifier, sig
		}
	else
		{
		TRAPD(result, verifyResult = VerifyRSASignatureL(aEncodedKey, aHash));
		if (result==KErrNoMemory)	//	Leave if OOM, anything else is a verify
			User::Leave(result);	//	error => verifyResult = EFalse

		}
	return verifyResult;
		
	}

TBool CSignedObject::VerifyRSASignatureL(const TDesC8& aEncodedKey, const TDesC8& aHash) const
	{
	__UHEAP_MARK;
	const CAlgorithmIdentifier& algId = iSigningAlgorithm->AsymmetricAlgorithm();
	const CAlgorithmIdentifier& digestId = iSigningAlgorithm->DigestAlgorithm();
	if (algId.Algorithm() != ERSA)
		{
		User::Leave(KErrArgument);
		}
	
	//Get the public key	
	CRSAPublicKey* key = iKeyFactory->RSAPublicKeyL(aEncodedKey);
	CleanupStack::PushL(key);

	CRSAPKCS1v15Verifier* verifier = CRSAPKCS1v15Verifier::NewLC(*key);

	RInteger sig = RInteger::NewL(Signature());
	CleanupStack::PushL(sig);
	CRSASignature* theSignature = CRSASignature::NewL(sig);
	CleanupStack::Pop(&sig);
	CleanupStack::PushL(theSignature);

	HBufC8* publicDecryptOutput = verifier->InverseSignLC(*theSignature);
	
	//Now verify it
	TPtrC8 hash(aHash);
	CRSASignatureResult* decoder = iKeyFactory->RSASignatureResultL(digestId, hash);
	CleanupStack::PushL(decoder);
	
	TPtr8 outputPtr(publicDecryptOutput->Des());
	TBool verified = decoder->VerifyL(outputPtr);
	CleanupStack::PopAndDestroy(5, key);	//	decoder, publicDecryptOutput, theSignature
											//	verifier, key,
	__UHEAP_MARKEND;
	return (verified);		
	}

//	x509 signature comprises of an AlgID and the signed data itself, so a simple
//	verify is not possible (the data returned from public decrypt will not match
//	the original data signed because of the extra algID appendage).
//
TBool CSignedObject::VerifyRSASignatureL(const TDesC8& aEncodedKey) const
	{
	__UHEAP_MARK;
	const CAlgorithmIdentifier& algId = iSigningAlgorithm->AsymmetricAlgorithm();
	const CAlgorithmIdentifier& digestId = iSigningAlgorithm->DigestAlgorithm();
	if (algId.Algorithm() != ERSA)
		{
		User::Leave(KErrArgument);
		}	
	CMessageDigest* digest = NULL;
	switch(digestId.Algorithm())
		{
	case EMD2:
		{
		digest = CMD2::NewL();
		break;			
		}
	case EMD5:
		{
		digest = CMD5::NewL();
		break;
		}
	case ESHA1:
		{
		digest = CSHA1::NewL();
		break;
		}
	case ESHA224:
	    {
	    digest = CSHA2::NewL(E224Bit);
	    break;
	    }
	case ESHA256:
	    {
	    digest = CSHA2::NewL(E256Bit);
	    break;
	    }
	case ESHA384:
	    {
	    digest = CSHA2::NewL(E384Bit);
	    break;
	    }
	case ESHA512:
	    {
	    digest = CSHA2::NewL(E512Bit);
	    break;
	    }

	default:
		User::Leave(KErrArgument);
		}

	//Hash the data	
	CleanupStack::PushL(digest);
	TPtrC8 hash = digest->Final(SignedDataL());
	TBool verifyResult=VerifyRSASignatureL(aEncodedKey, hash);
	CleanupStack::PopAndDestroy(digest);
	__UHEAP_MARKEND;
	return verifyResult;
	}

EXPORT_C void CSignedObject::SetParametersL(const CSigningKeyParameters& aParameters)
	{
		delete iParameters;
		iParameters = NULL;	//	In case next fails
		iParameters = CSigningKeyParameters::NewL(aParameters);
	}

EXPORT_C const TPtrC8 CSignedObject::Signature() const
	{
	return iSignature->Des();
	}

EXPORT_C const TPtrC8 CSignedObject::Encoding() const
	{
	return iEncoding->Des();
	}

EXPORT_C const CSigningAlgorithmIdentifier& CSignedObject::SigningAlgorithm() const
	{
	return *iSigningAlgorithm;
	}

EXPORT_C const TPtrC8 CSignedObject::Fingerprint() const
	{
	return iFingerprint->Des();
	}

EXPORT_C CSignedObject::~CSignedObject()
	{
	delete iParameters;
	delete iKeyFactory;
	delete iFingerprint;
	delete iEncoding;
	delete iSignature;
	delete iSigningAlgorithm;
	}

EXPORT_C void CSignedObject::ExternalizeL(RWriteStream& aStream) const 
	{
	aStream.WriteInt32L(iEncoding->Length());
	aStream.WriteL(*iEncoding);
	}


//****************************************************************************************//
//certificate base
EXPORT_C CCertificate::~CCertificate()
	{
	delete iSerialNumber;
	delete iValidityPeriod;
	delete iSubjectPublicKeyInfo;
	}

EXPORT_C const TPtrC8 CCertificate::SerialNumber() const
	{
	return iSerialNumber->Des();
	}

EXPORT_C const CValidityPeriod& CCertificate::ValidityPeriod() const
	{
	return *iValidityPeriod;
	}

EXPORT_C const CSubjectPublicKeyInfo& CCertificate::PublicKey() const
	{
	return *iSubjectPublicKeyInfo;
	}

EXPORT_C TKeyIdentifier CCertificate::KeyIdentifierL() const	
	{
	if (iSubjectPublicKeyInfo->AlgorithmId() != ERSA)
		{
		User::Leave(KErrNotSupported);
		}

	CRSAPublicKey* rsaKey = iKeyFactory->RSAPublicKeyL(iSubjectPublicKeyInfo->KeyData());
	CleanupStack::PushL(rsaKey);
	TKeyIdentifier retVal;
	KeyIdentifierUtil::RSAKeyIdentifierL(*rsaKey, retVal);
	CleanupStack::PopAndDestroy(rsaKey);
	return retVal;
	}
