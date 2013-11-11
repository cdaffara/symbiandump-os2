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

 
#include "pkcs7signedobject.h"
#include <pkcs7contentinfo_v2.h>
#include "pkcs7excert.h"
#include "pkcs7signerinfo.h"
#include "pkcs7issuerserial.h"
#include "pkcs7asn1.h"
#include <asn1dec.h>
#include <x509cert.h>
#include <hash.h>

const TInt KSignedDataCertificates = 0;
const TInt KSignedDataRevocationLists = 1;

EXPORT_C CPKCS7SignedObject::~CPKCS7SignedObject(void)
	{
	iDigestAlgorithms.ResetAndDestroy();
	iCertificates.ResetAndDestroy();
	delete iContentInfo;
	iSignerInfo.ResetAndDestroy();
	for(TInt i = 0; i < KPKCS7MaxDataElements; i++)
		{
		delete iDataElements.At(i);
		}
	}

EXPORT_C CPKCS7SignedObject::CPKCS7SignedObject(void)
	{
	}


EXPORT_C CPKCS7SignedObject* CPKCS7SignedObject::NewLC(const CPKCS7ContentInfo& aContentInfo)
	{
	CPKCS7SignedObject* self = new (ELeave) CPKCS7SignedObject();
	CleanupStack::PushL(self);
	self->ConstructL(aContentInfo);
	return self;
	}

EXPORT_C CPKCS7SignedObject* CPKCS7SignedObject::NewL(const CPKCS7ContentInfo& aContentInfo)
	{
	CPKCS7SignedObject* self = NewLC(aContentInfo);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CPKCS7SignedObject::ConstructL(const CPKCS7ContentInfo& aContentInfo)
	{
	if(aContentInfo.ContentType() != KPkcs7SignedData)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecGeneric decGen(aContentInfo.ContentData());
	decGen.InitL();

	if(decGen.Tag() == EASN1Sequence)
		{
		InitSignedObjectL(decGen.Encoding());
	    DecodeSignedDataL(*iEncoding);
		}
	else
		{
	    User::Leave(KErrArgument);
		}
	}

EXPORT_C const TPtrC8 CPKCS7SignedObject::SignedDataL() const
	{
	return iContentInfo->ContentData();
	}

EXPORT_C void CPKCS7SignedObject::InternalizeL(RReadStream& /*aStream*/) 
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C const TPtrC8* CPKCS7SignedObject::DataElementEncoding(const TUint aIndex) const
	{
	return iDataElements.At(aIndex);
	}


EXPORT_C const RPointerArray<CX509AlgorithmIdentifier>& CPKCS7SignedObject::DigestAlgorithms() const
	{
	return iDigestAlgorithms;
	}

EXPORT_C TInt CPKCS7SignedObject::Version() const
	{
	return iVersion;
	}

EXPORT_C const CPKCS7ContentInfo& CPKCS7SignedObject::ContentInfo() const
	{
	return *iContentInfo;
	}

EXPORT_C const RPointerArray<CPKCS7ExtendedCertificateOrCertificate>& CPKCS7SignedObject::Certificates() const
	{
	return iCertificates;
	}

EXPORT_C const RPointerArray<CPKCS7SignerInfo>& CPKCS7SignedObject::SignerInfo() const
	{
	return iSignerInfo;
	}

void CPKCS7SignedObject::InitSignedObjectL(const TDesC8& aRawData)
	{
	// Populate CSignedObject data members
	iKeyFactory = new (ELeave) TX509KeyFactory;
	iEncoding = aRawData.AllocL();

	CSHA1* hash = CSHA1::NewL();
	CleanupStack::PushL(hash);
	iFingerprint = hash->Hash(Encoding()).AllocL();
	CleanupStack::PopAndDestroy(hash);
	}

void CPKCS7SignedObject::DecodeSignedDataL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* signedData = PKCS7ASN1::DecodeSequenceLC(aRawData, 4, KPKCS7MaxDataElements);
	TInt totalItems = signedData->Count();
	TASN1DecInteger decInt;

	// decodes version
	iDataElements.At(EVersionNumber) = new(ELeave) TPtrC8(signedData->At(0)->GetContentDER());
	iVersion = decInt.DecodeDERShortL(*signedData->At(0));	
	// decodes algorithms
	iDataElements.At(EDigestAlgorithms) = new(ELeave) TPtrC8(signedData->At(1)->GetContentDER());
	DecodeDigestAlgorithmsL(signedData->At(1)->Encoding());
	// decodes contentinfo
	iDataElements.At(EContentInfo) = new(ELeave) TPtrC8(signedData->At(2)->GetContentDER());
	iContentInfo = CPKCS7ContentInfo::NewL(signedData->At(2)->Encoding());

	// Checks for optional fields
	TInt pos = 3;	// Skip first non-optional fields
	do
		{
		const TASN1DecGeneric& currentItem = *signedData->At(pos);
		switch(currentItem.Tag())
			{
			case KSignedDataCertificates:
				{
				iDataElements.At(ECertificates) = new(ELeave) TPtrC8(currentItem.GetContentDER());
				DecodeCertificatesL(currentItem.Encoding());
				break;
				}
			case KSignedDataRevocationLists:
				{
				
				iDataElements.At(ERevocationLists) = new(ELeave) TPtrC8(currentItem.GetContentDER());
				DecodeRevocationListsL(currentItem.Encoding());
				break;
				}
			default:	// Non-optional field
				{
				break;
				}
			}
		pos++;
		}
	while(pos < totalItems);

	iDataElements.At(ESignedInfo) = new(ELeave) TPtrC8(signedData->At(totalItems-1)->GetContentDER());
	DecodeSignerInfoL(signedData->At(totalItems-1)->Encoding());

	CleanupStack::PopAndDestroy(signedData);
	}


void CPKCS7SignedObject::DecodeDigestAlgorithmsL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* algsData = PKCS7ASN1::DecodeSequenceLC(aRawData);
	TInt count = algsData->Count();
	CX509AlgorithmIdentifier* alIdent;

	for(TInt item = 0; item < count; item++)
		{
 		alIdent = CX509AlgorithmIdentifier::NewLC(algsData->At(item)->Encoding());
		User::LeaveIfError(iDigestAlgorithms.Append(alIdent));
		CleanupStack::Pop(alIdent);
		}
	CleanupStack::PopAndDestroy(algsData);
	}

void CPKCS7SignedObject::DecodeCertificatesL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* items = NULL;
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
	TASN1DecSequence decSeq;
	// have to do manual decoding of sequence because field is optional
	items = decSeq.DecodeDERLC(decGen);
	TInt count = items->Count();

	CPKCS7ExtendedCertificateOrCertificate* certificate;

	for(TInt item = 0; item < count; item++)
		{
 		certificate = CPKCS7ExtendedCertificateOrCertificate::NewL(items->At(item)->Encoding());
		CleanupStack::PushL(certificate);
		User::LeaveIfError(iCertificates.Append(certificate));
		CleanupStack::Pop(certificate);
		}

	CleanupStack::PopAndDestroy(items);
	}

void CPKCS7SignedObject::DecodeRevocationListsL(const TDesC8& /*aRawData*/)
	{
	// not yet supported
	User::Leave(KErrNotSupported);
	}

void CPKCS7SignedObject::DecodeSignerInfoL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* signerInfo = PKCS7ASN1::DecodeSequenceLC(aRawData);
	TInt total = signerInfo->Count();
	CPKCS7SignerInfo* signer;

	for(TInt item = 0; item < total; item ++)
		{
		signer = CPKCS7SignerInfo::NewL(signerInfo->At(item)->Encoding());
		CleanupStack::PushL(signer);
		User::LeaveIfError(iSignerInfo.Append(signer));
		CleanupStack::Pop(signer);
		}
	CleanupStack::PopAndDestroy(signerInfo);
	}

EXPORT_C TBool CPKCS7SignedObject::ValidateSignerL(const CPKCS7SignerInfo& aSignerInfo, HBufC8*& aCertChainEncoding)
	{
	TInt certCount = iCertificates.Count();
	TInt endEntityPos = -1;
	TInt endEncodingSize = 0;
	TPtrC8 endEntityEncoding;
	TInt cert;
	TBool valid = EFalse;

	// looks for end entity certificate
	for(cert = 0; cert < certCount; cert++)
		{
		const CX509Certificate& certificate = iCertificates[cert]->Certificate();

		endEncodingSize+= certificate.Encoding().Length();
		if(certificate.IssuerName().ExactMatchL(aSignerInfo.IssuerAndSerialNumber().IssuerName()))
			{
			endEntityPos = cert;
			endEntityEncoding.Set(certificate.Encoding());
			valid = ValidateSignatureL(aSignerInfo, certificate);
			}
		}

	// checks if end entity was found
	if(endEntityPos != -1)
		{
		// builds the cert chain encoding by putting the end entity first then all remaining
		// certs
		aCertChainEncoding = HBufC8::NewLC(endEncodingSize);
		TPtr8 encodingPtr(aCertChainEncoding->Des());
		encodingPtr.Copy(endEntityEncoding);
		for(cert = 0; cert < certCount; cert++)
			{
			const CX509Certificate& certificate = iCertificates[cert]->Certificate();
	
			if(cert != endEntityPos)
				{
				encodingPtr.Append(certificate.Encoding());
				}
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	return valid;
	}

TBool CPKCS7SignedObject::ValidateSignatureL(const CPKCS7SignerInfo& aSignerInfo, const CX509Certificate& aEndEntityCert)
	{
	iSigningAlgorithm = CX509SigningAlgorithmIdentifier::NewL(aSignerInfo.DigestEncryptionAlgorithm(), aSignerInfo.DigestAlgorithm());
	if(iSignature)
		{
		delete iSignature;
		iSignature = NULL;
		}
	iSignature = aSignerInfo.EncryptedDigest().AllocL();
	return VerifySignatureL(aEndEntityCert.PublicKey().KeyData());
	}
