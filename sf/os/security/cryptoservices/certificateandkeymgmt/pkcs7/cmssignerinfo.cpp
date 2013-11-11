/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include <cmssignerinfo.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <x509cert.h>
#include <cmssigneridentifier.h>
#include <cmsdefs.h>
#include "cmsutils.h"
#include "pkcs7asn1.h"

//
// Implementation of CCmsSignerInfo
//
CCmsSignerInfo* CCmsSignerInfo::NewL(const TDesC8& aDataToBeSigned,
									TBool aIsHash,
									const CDSAPrivateKey& aKey,
									CCmsSignerIdentifier* aSignerIdentifier,
									CX509AlgorithmIdentifier* aDigestAlgorithm,
									CX509AlgorithmIdentifier* aSignatureAlgorithm)
	{
	CCmsSignerInfo* self = NewLC(aDataToBeSigned,
								 aIsHash,
								 aKey,
								 aSignerIdentifier,
								 aDigestAlgorithm,
								 aSignatureAlgorithm);
	CleanupStack::Pop(self);
	return self;
	}
										
CCmsSignerInfo* CCmsSignerInfo::NewLC(const TDesC8& aDataToBeSigned,
									TBool aIsHash,
									const CDSAPrivateKey& aKey,
									CCmsSignerIdentifier* aSignerIdentifier,
									CX509AlgorithmIdentifier* aDigestAlgorithm,
									CX509AlgorithmIdentifier* aSignatureAlgorithm)
	{
	if (!aSignerIdentifier||!aDigestAlgorithm||!aSignatureAlgorithm)
		{
		User::Leave(KErrArgument);	
		}
	CCmsSignerInfo* self = new (ELeave) CCmsSignerInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aDataToBeSigned, aIsHash, aKey, aSignerIdentifier, aDigestAlgorithm, aSignatureAlgorithm);
	return self;
	}

CCmsSignerInfo* CCmsSignerInfo::NewL(const TDesC8& aDataToBeSigned,
									TBool aIsHash,
									const CRSAPrivateKey& aKey,
									CCmsSignerIdentifier* aSignerIdentifier,
									CX509AlgorithmIdentifier* aDigestAlgorithm,
									CX509AlgorithmIdentifier* aSignatureAlgorithm)
	{
	CCmsSignerInfo* self = NewLC(aDataToBeSigned,
								 aIsHash,
								 aKey,
								 aSignerIdentifier,
								 aDigestAlgorithm,
								 aSignatureAlgorithm);
	CleanupStack::Pop(self);
	return self;		
	}

CCmsSignerInfo* CCmsSignerInfo::NewLC(const TDesC8& aDataToBeSigned,
									TBool aIsHash,
									const CRSAPrivateKey& aKey,
									CCmsSignerIdentifier* aSignerIdentifier,
									CX509AlgorithmIdentifier* aDigestAlgorithm,
									CX509AlgorithmIdentifier* aSignatureAlgorithm)
	{
	if (!aSignerIdentifier||!aDigestAlgorithm||!aSignatureAlgorithm)
		{
		User::Leave(KErrArgument);	
		}

	CCmsSignerInfo* self = new (ELeave) CCmsSignerInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aDataToBeSigned, aIsHash, aKey, aSignerIdentifier, aDigestAlgorithm, aSignatureAlgorithm);
	return self;
	}

CCmsSignerInfo* CCmsSignerInfo::NewL(const TDesC8& aRawData)
	{
	CCmsSignerInfo* self = NewLC(aRawData);
	CleanupStack::Pop(self);
	return self;
	}
	
CCmsSignerInfo* CCmsSignerInfo::NewLC(const TDesC8& aRawData)
	{
	CCmsSignerInfo* self = new (ELeave) CCmsSignerInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	return self;
	}

CCmsSignerInfo::CCmsSignerInfo()
	{	
	}
	
CCmsSignerInfo::~CCmsSignerInfo()
	{
	delete iDigestAlgorithm;
	delete iSignatureAlgorithm;
	delete iSignatureValue;	
	delete iSignerIdentifier;
	}

void CCmsSignerInfo::ConstructL(const TDesC8& aDataToBeSigned, 	
								TBool aIsHash, 
								const CDSAPrivateKey& aKey,
								CCmsSignerIdentifier* aSignerIdentifier,
								CX509AlgorithmIdentifier* aDigestAlgorithm,
								CX509AlgorithmIdentifier* aSignatureAlgorithm)
								
	{
	if (aSignatureAlgorithm->Algorithm()!=EDSA)
		{
		User::Leave(KErrArgument);	
		}
	iSignatureValue=CmsUtils::CreateSignatureL(aDataToBeSigned, aIsHash, aDigestAlgorithm->Algorithm(), aKey);
	
	iSignerIdentifier=aSignerIdentifier;
	iDigestAlgorithm=aDigestAlgorithm;
	iSignatureAlgorithm=aSignatureAlgorithm;
	//find out the CMS signer info version
	if (iSignerIdentifier->SignerIdentifierType() == CCmsSignerIdentifier::EIssuerAndSerialNumber)
		{
		iVersion=EVersion_1;
		}
	else if (iSignerIdentifier->SignerIdentifierType() == CCmsSignerIdentifier::ESubjectKeyIdentifier)
		{
		iVersion=EVersion_3;	
		}	
	
	}

void CCmsSignerInfo::ConstructL(const TDesC8& aDataToBeSigned, 	
								TBool aIsHash, 
								const CRSAPrivateKey& aKey,
								CCmsSignerIdentifier* aSignerIdentifier,
								CX509AlgorithmIdentifier* aDigestAlgorithm,
								CX509AlgorithmIdentifier* aSignatureAlgorithm)								
	{
	if (aSignatureAlgorithm->Algorithm()!=ERSA)
		{
		User::Leave(KErrArgument);	
		}
	iSignatureValue=CmsUtils::CreateSignatureL(aDataToBeSigned, aIsHash, aDigestAlgorithm->Algorithm(), aKey);
	iSignerIdentifier=aSignerIdentifier;
	iDigestAlgorithm=aDigestAlgorithm;
	iSignatureAlgorithm=aSignatureAlgorithm;
	//find out the CMS signer info version
	if (iSignerIdentifier->SignerIdentifierType() == CCmsSignerIdentifier::EIssuerAndSerialNumber)
		{
		iVersion=EVersion_1;
		}
	else if (iSignerIdentifier->SignerIdentifierType() == CCmsSignerIdentifier::ESubjectKeyIdentifier)
		{
		iVersion=EVersion_3;	
		}
	}

void CCmsSignerInfo::ConstructL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* signerInfo = PKCS7ASN1::DecodeSequenceLC(aRawData, 5, 7);
	TASN1DecInteger decInt;
	TInt pos = 0;
	// decodes version
	iVersion = decInt.DecodeDERShortL(*signerInfo->At(pos++));
	if (iVersion<0 || iVersion>4)
		{
		User::Leave(KErrArgument);	
		}

	DecodeSignerIdentifierL(signerInfo->At(pos++)->Encoding());
	
	iDigestAlgorithm = CX509AlgorithmIdentifier::NewL(signerInfo->At(pos++)->Encoding());

	if(signerInfo->At(pos)->Tag() == 0 && signerInfo->At(pos)->Class() == EContextSpecific)
		{
		// authenticated attributes not supported at this time
		iSignedAttributesPresent=ETrue;
		pos++;
		}
	iSignatureAlgorithm = CX509AlgorithmIdentifier::NewL(signerInfo->At(pos++)->Encoding());
	DecodeEncryptedDigestL(signerInfo->At(pos++)->Encoding());

	if(pos < signerInfo->Count() && signerInfo->At(pos)->Tag() == 1 && signerInfo->At(pos)->Class() == EContextSpecific)
		{
		// unauthenticated attributes not supported at this time
		iUnsignedAttributesPresent=ETrue;
		pos++;		
		}

	CleanupStack::PopAndDestroy(signerInfo);
	}
	
CASN1EncSequence* CCmsSignerInfo::EncodeASN1DERLC() const
	{
	// the root sequence contains the signed object
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	
	//Encode the version
	CASN1EncInt* version=CASN1EncInt::NewLC(iVersion);	
	root->AddAndPopChildL(version);
	
	//Encode sid
	CASN1EncBase* sid=EncodeSignerIdentifierLC();
	root->AddAndPopChildL(sid);
	
	//Encode Digest Algoritm
	CASN1EncSequence* digAlg=iDigestAlgorithm->EncodeASN1DERLC();
	root->AddAndPopChildL(digAlg);
	
	//Encode signature Algoritm
	CASN1EncSequence* sigAlg=iSignatureAlgorithm->EncodeASN1DERLC();
	root->AddAndPopChildL(sigAlg);
	
	//Encode signature value
	CASN1EncOctetString* sigEnc=CASN1EncOctetString::NewLC(iSignatureValue->Des());
	root->AddAndPopChildL(sigEnc);
	
	return root;	
	}
	
void CCmsSignerInfo::DecodeEncryptedDigestL(const TDesC8& aRawData)
	{
	CmsUtils::DecodeOctetStringL(aRawData, iSignatureValue);
	}

void CCmsSignerInfo::DecodeSignerIdentifierL(const TDesC8& aRawData)
	{
	iSignerIdentifier=CCmsSignerIdentifier::NewL(aRawData);
	}

CASN1EncBase* CCmsSignerInfo::EncodeSignerIdentifierLC() const
	{
	return iSignerIdentifier->EncodeASN1DERLC();	
	}

EXPORT_C TInt CCmsSignerInfo::Version() const
	{
	return iVersion;
	}

EXPORT_C TBool CCmsSignerInfo::IsSignedAttributesPresent() const
	{
	return iSignedAttributesPresent;	
	}

EXPORT_C TBool CCmsSignerInfo::IsUnsignedAttributesPresent() const
	{
	return iUnsignedAttributesPresent;	
	}
	
EXPORT_C const CX509AlgorithmIdentifier& CCmsSignerInfo::DigestAlgorithm() const
	{
	return *iDigestAlgorithm;	
	}

EXPORT_C const CX509AlgorithmIdentifier& CCmsSignerInfo::SignatureAlgorithm() const	
	{
	return *iSignatureAlgorithm;	
	}

EXPORT_C const TPtrC8 CCmsSignerInfo::SignatureValue() const
	{
	return *iSignatureValue;	
	}
	
EXPORT_C const CCmsSignerIdentifier& CCmsSignerInfo::SignerIdentifier() const
	{
	return *iSignerIdentifier;
	}





	
	
	
	





