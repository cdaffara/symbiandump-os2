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

 
#include <cmssignedobject.h>
#include <x509cert.h>
#include <x509certext.h>
#include <asymmetrickeys.h>
#include <hash.h>
#include <asn1enc.h>
#include <asn1dec.h>
#include <pkcs7excert.h>
#include <cmssigneridentifier.h>
#include <cmscontentinfo.h>
#include <cmssignerinfo.h>
#include "cmsutils.h"
#include "pkcs7asn1.h"

const TInt KSignedDataCertificates = 0;
const TInt KSignedDataRevocationLists = 1;

const TInt KCmsMinSignedDataElements = 4;
//
// Implementation of CMS Signed object
//
EXPORT_C CCmsSignedObject* CCmsSignedObject::NewLC(TCmsContentInfoType aType, TBool aIsDetached, const TDesC8& aContentData)
	{
	CCmsSignedObject* self = new (ELeave) CCmsSignedObject();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aIsDetached, aContentData);
	return self;
	}

EXPORT_C CCmsSignedObject* CCmsSignedObject::NewL(TCmsContentInfoType aType, TBool aIsDetached, const TDesC8& aContentData)
	{
	CCmsSignedObject* self = NewLC(aType, aIsDetached, aContentData);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CCmsSignedObject* CCmsSignedObject::NewLC(TCmsContentInfoType aType,
												const TDesC8& aHashValue,
												TAlgorithmId aDigestAlgorithm,
												const CDSAPrivateKey& aKey,
												const CX509Certificate& aCert,
												TBool aAddCertificate)
	{
	CCmsSignedObject* self = new (ELeave) CCmsSignedObject();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aHashValue, aDigestAlgorithm, aKey, aCert, aAddCertificate);
	return self;
	}

EXPORT_C CCmsSignedObject* CCmsSignedObject::NewL(TCmsContentInfoType aType,
												const TDesC8& aHashValue,
												TAlgorithmId aDigestAlgorithm,
												const CDSAPrivateKey& aKey,
												const CX509Certificate& aCert,
												TBool aAddCertificate)
	{
	CCmsSignedObject* self = NewLC(aType, aHashValue, aDigestAlgorithm, aKey, aCert, aAddCertificate);
	CleanupStack::Pop(self);
	return self;
	}


EXPORT_C CCmsSignedObject* CCmsSignedObject::NewLC(TCmsContentInfoType aType,
												const TDesC8& aHashValue,
												TAlgorithmId aDigestAlgorithm,
												const CRSAPrivateKey& aKey,
												const CX509Certificate& aCert,
												TBool aAddCertificate)
	{
	CCmsSignedObject* self = new (ELeave) CCmsSignedObject();
	CleanupStack::PushL(self);
	self->ConstructL(aType, aHashValue, aDigestAlgorithm, aKey, aCert, aAddCertificate);
	return self;
	}

EXPORT_C CCmsSignedObject* CCmsSignedObject::NewL(TCmsContentInfoType aType,
												const TDesC8& aHashValue,
												TAlgorithmId aDigestAlgorithm,
												const CRSAPrivateKey& aKey,
												const CX509Certificate& aCert,
												TBool aAddCertificate)
	{
	CCmsSignedObject* self = NewLC(aType, aHashValue, aDigestAlgorithm, aKey, aCert, aAddCertificate);
	CleanupStack::Pop(self);
	return self;
	}

	
EXPORT_C CCmsSignedObject* CCmsSignedObject::NewL(const CCmsContentInfo& aContentInfo)
	{
	CCmsSignedObject* self = NewLC(aContentInfo);
	CleanupStack::Pop(self);
	return self;		
	}
	
EXPORT_C CCmsSignedObject* CCmsSignedObject::NewLC(const CCmsContentInfo& aContentInfo)
	{
	CCmsSignedObject* self = new (ELeave) CCmsSignedObject();
	CleanupStack::PushL(self);
	self->ConstructL(aContentInfo);
	return self;				
	}

CCmsSignedObject::CCmsSignedObject() : iVersion(EVersion_1)
	{
	}

EXPORT_C CCmsSignedObject::~CCmsSignedObject()
	{
	delete iContentInfo;
	iDigestAlgorithms.ResetAndDestroy();
	iCertificates.ResetAndDestroy();
	iSignerInfo.ResetAndDestroy();	
	for(TInt i = 0; i < KCmsMaxSignedDataElements; i++)
		{
		delete iDataElements.At(i);
		}		
	}
	

EXPORT_C TBool CCmsSignedObject::IsCertificateSetPresent() const
	{
	return iIsCertificateSetPresent;
	}
	
EXPORT_C TBool CCmsSignedObject::IsCertificateRevocationListsPresent() const
	{
	return iIsCertificateRevocationListsPresent;
	}

EXPORT_C TInt CCmsSignedObject::Version() const
	{
	return iVersion;	
	}

EXPORT_C const RPointerArray<CCmsCertificateChoice>& CCmsSignedObject::Certificates() const
	{
	return iCertificates;
	}

EXPORT_C const RPointerArray<CX509AlgorithmIdentifier>& CCmsSignedObject::DigestAlgorithms() const
	{
	return iDigestAlgorithms;
	}

EXPORT_C const CEncapsulatedContentInfo& CCmsSignedObject::ContentInfo() const
	{
	return *iContentInfo;
	}
		
EXPORT_C const RPointerArray<CCmsSignerInfo>& CCmsSignedObject::SignerInfo() const
	{
	return iSignerInfo;
	}

EXPORT_C void CCmsSignedObject::AddCertificateL(const CX509Certificate& aCert)
	{	
	CmsUtils::AddCertificateL(iCertificates, aCert);
	}

void CCmsSignedObject::AddDigestAlgorithmL(TAlgorithmId aDigestAlgorithm)
	{
	CmsUtils::AddAlgorithmIdentifierL(iDigestAlgorithms, aDigestAlgorithm);	
	}

EXPORT_C void CCmsSignedObject::AddCertificateL(const TDesC8& aCert, CCmsCertificateChoice::TCertificateType aType)
	{
	if (aType==CCmsCertificateChoice::ECertificateAttribute)
		{
		iVersion=EVersion_3;
		}
	CmsUtils::AddCertificateL(iCertificates, aCert, aType);
	}

void CCmsSignedObject::DecodeSignerInfoL(const TDesC8& aRawData)
	{	
	CArrayPtr<TASN1DecGeneric>* signerInfo = PKCS7ASN1::DecodeSequenceLC(aRawData);
	TInt total = signerInfo->Count();
	CCmsSignerInfo* signer(NULL);

	for(TInt item = 0; item < total; item ++)
		{
		signer = CCmsSignerInfo::NewL(signerInfo->At(item)->Encoding());
		CleanupStack::PushL(signer);
		User::LeaveIfError(iSignerInfo.Append(signer));
		CleanupStack::Pop(signer);
		}
	CleanupStack::PopAndDestroy(signerInfo);
	}

void CCmsSignedObject::DecodeEncapsulatedContentInfoL(const TDesC8& aRawData)
	{
	iContentInfo = CEncapsulatedContentInfo::NewL(aRawData);
	}

void CCmsSignedObject::ConstructL(const CCmsContentInfo& aContentInfo)
	{
	if(aContentInfo.ContentType() != EContentTypeSignedData)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecGeneric decGen(aContentInfo.ContentData());
	decGen.InitL();

	if(decGen.Tag() == EASN1Sequence && decGen.Class()==EUniversal)
		{
		InitSignedObjectL(decGen.Encoding());
	    DecodeSignedDataL(*iEncoding);
		}
	else
		{
	    User::Leave(KErrArgument);
		}			
	}

void CCmsSignedObject::ConstructL(TCmsContentInfoType aType, TBool aIsDetached, const TDesC8& aContentData)
	{
	if (aContentData==KNullDesC8() && !aIsDetached)
		{
		User::Leave(KErrArgument);	
		}
	iContentInfo=CEncapsulatedContentInfo::NewL(aType, !aIsDetached, aContentData);	
	//For later use to create hash if detached and hash not provided
	iContentData.Set(aContentData);			
	}

CCmsSignerIdentifier* CCmsSignedObject::BuildSignerIdentifierLC(const CX509Certificate& aCert)
	{
	CCmsSignerIdentifier* sid(NULL);
	const CX509CertExtension* certExt = aCert.Extension(KSubjectKeyId);
	if (certExt)
		{
		CX509SubjectKeyIdExt* ext=CX509SubjectKeyIdExt::NewLC(certExt->Data());
		HBufC8* subKeyId=ext->KeyId().AllocL();
		CleanupStack::PopAndDestroy(ext);
		CleanupStack::PushL(subKeyId);
		sid=CCmsSignerIdentifier::NewL(subKeyId);
		CleanupStack::Pop(subKeyId);
		CleanupStack::PushL(sid);
		iVersion=EVersion_3;
		}
	else
		{
		CX500DistinguishedName* distinguishedName=CX500DistinguishedName::NewLC(aCert.IssuerName());
		CPKCS7IssuerAndSerialNumber* issuerAndSN=CPKCS7IssuerAndSerialNumber::NewL(distinguishedName, aCert.SerialNumber());
		CleanupStack::Pop(distinguishedName);
		CleanupStack::PushL(issuerAndSN);
		sid=CCmsSignerIdentifier::NewL(issuerAndSN);
		CleanupStack::Pop(issuerAndSN);
		CleanupStack::PushL(sid);
		}
	return sid;	
	}


void CCmsSignedObject::BuildSignerInfoCertListAndAlgoritmListL(TAlgorithmId aDigestAlgorithm,
																TBool aIsHash,
																const TDesC8& aValue,
																const CDSAPrivateKey& aKey,
																const CX509Certificate& aCert,
																TBool aAddCertificate)
	{		
	//build Signer Identifier
	CCmsSignerIdentifier* sid=BuildSignerIdentifierLC(aCert);
	
	//build digest algorithm and signing algorithm
	CX509AlgorithmIdentifier* digAlg=CX509AlgorithmIdentifier::NewLC(aDigestAlgorithm, KNullDesC8());
	const CSubjectPublicKeyInfo& publicKeyInfo=aCert.PublicKey();
	CX509AlgorithmIdentifier* signingAlg=CX509AlgorithmIdentifier::NewLC(publicKeyInfo.AlgorithmId(), KNullDesC8());

	//build signer info
	CCmsSignerInfo* signerInfo=CCmsSignerInfo::NewL(aValue,
													aIsHash,
													aKey,
													sid,
													digAlg,
													signingAlg);
	CleanupStack::Pop(3, sid);
	CleanupStack::PushL(signerInfo);
	//add to the signer info list
	iSignerInfo.AppendL(signerInfo);
	CleanupStack::Pop();
	
	//Add the certificate to the list if needed
	if (aAddCertificate)
		{
		AddCertificateL(aCert);
		}
		
	//Add the digest algorithm the list if needed
	AddDigestAlgorithmL(aDigestAlgorithm);	
	}



void CCmsSignedObject::BuildSignerInfoCertListAndAlgoritmListL(TAlgorithmId aDigestAlgorithm,
																TBool aIsHash,
																const TDesC8& aValue,
																const CRSAPrivateKey& aKey,
																const CX509Certificate& aCert,
																TBool aAddCertificate)

	{		
	//build Signer Identifier
	CCmsSignerIdentifier* sid=BuildSignerIdentifierLC(aCert);
	
	//build digest algorithm and signing algorithm
	CX509AlgorithmIdentifier* digAlg=CX509AlgorithmIdentifier::NewLC(aDigestAlgorithm, KNullDesC8());	
	const CSubjectPublicKeyInfo& publicKeyInfo=aCert.PublicKey();
	CX509AlgorithmIdentifier* signingAlg=CX509AlgorithmIdentifier::NewLC(publicKeyInfo.AlgorithmId(), publicKeyInfo.EncodedParams());

	//build signer info
	CCmsSignerInfo* signerInfo=CCmsSignerInfo::NewL(aValue,
													aIsHash,
													aKey,
													sid,
													digAlg,
													signingAlg);
	CleanupStack::Pop(3, sid);
	CleanupStack::PushL(signerInfo);
	//add to the signer info list
	iSignerInfo.AppendL(signerInfo);
	CleanupStack::Pop();
	
	//Add the certificate to the list if needed
	if (aAddCertificate)
		{
		AddCertificateL(aCert);
		}
		
	//Add the digest algorithm the list if needed
	AddDigestAlgorithmL(aDigestAlgorithm);
	}

void CCmsSignedObject::ConstructL(TCmsContentInfoType aType,
								const TDesC8& aHashValue,
								TAlgorithmId aDigestAlgorithm,
								const CDSAPrivateKey& aKey,
								const CX509Certificate& aCert,
								TBool aAddCertificate)
	{
	//Set the CMS object version to version 3 if the encapsulatedconetent data type is not data
	if (aType != EContentTypeData)
		{
		iVersion=EVersion_3;
		}
		
	//build EncapsulatedContentInfo	
	iContentInfo=CEncapsulatedContentInfo::NewL(aType, EFalse, KNullDesC8());
	
	BuildSignerInfoCertListAndAlgoritmListL(aDigestAlgorithm,
											ETrue,
											aHashValue,
											aKey,
											aCert,
											aAddCertificate);		
		
	}
void CCmsSignedObject::ConstructL(TCmsContentInfoType aType,
								const TDesC8& aHashValue,
								TAlgorithmId aDigestAlgorithm,
								const CRSAPrivateKey& aKey,
								const CX509Certificate& aCert,
								TBool aAddCertificate)
	{
	//Set the CMS object version to version 3 if the encapsulatedconetent data type is not data
	if (aType != EContentTypeData)
		{
		iVersion=EVersion_3;
		}

	//build EncapsulatedContentInfo	
	iContentInfo=CEncapsulatedContentInfo::NewL(aType, EFalse, KNullDesC8());

	BuildSignerInfoCertListAndAlgoritmListL(aDigestAlgorithm,
											ETrue,
											aHashValue,
											aKey,
											aCert,
											aAddCertificate);		
	}


EXPORT_C void CCmsSignedObject::SignL(const TDesC8& aHashValue,
										TAlgorithmId aDigestAlgorithm,
										const CDSAPrivateKey& aKey,
										const CX509Certificate& aCert,
										TBool aAddCertificate)
										
	{
	TBool isHash=(aHashValue!=KNullDesC8())? ETrue:EFalse;
	if (isHash)
		{
		BuildSignerInfoCertListAndAlgoritmListL(aDigestAlgorithm,
												isHash,
												aHashValue,
												aKey,
												aCert,
												aAddCertificate);			
		}
	else
		{
		if (iContentData!=KNullDesC8())
			{
			BuildSignerInfoCertListAndAlgoritmListL(aDigestAlgorithm,
													isHash,
													iContentData,
													aKey,
													aCert,
													aAddCertificate);							
			}
		else
			{
			//No way to sign if no data content nor its hash.
			User::Leave(KErrArgument);	
			}
		}
	}
										
EXPORT_C void CCmsSignedObject::SignL(const TDesC8& aHashValue,
										TAlgorithmId aDigestAlgorithm,
										const CRSAPrivateKey& aKey,
										const CX509Certificate& aCert,
										TBool aAddCertificate)
	{
	TBool isHash=(aHashValue!=KNullDesC8())? ETrue:EFalse;
	if (isHash)
		{
		BuildSignerInfoCertListAndAlgoritmListL(aDigestAlgorithm,
												isHash,
												aHashValue,
												aKey,
												aCert,
												aAddCertificate);			
		}
	else
		{
		if (iContentData!=KNullDesC8())
			{
			BuildSignerInfoCertListAndAlgoritmListL(aDigestAlgorithm,
													isHash,
													iContentData,
													aKey,
													aCert,
													aAddCertificate);
			}
		else
			{
			//No way to sign if no data content nor its hash.
			User::Leave(KErrArgument);	
			}
		}
	}


EXPORT_C CASN1EncSequence* CCmsSignedObject::EncodeASN1DERLC() const
	{
	// the root sequence contains the signed object
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	
	// Encode version
	CASN1EncInt* version=CASN1EncInt::NewLC(iVersion);	
	root->AddAndPopChildL(version);

	// Encode Algorithm
	CASN1EncBase* algorithm=EncodeAlgorithmsLC();
	root->AddAndPopChildL(algorithm);			

			
	// Encode EncapsulatedContentInfo	
	CASN1EncSequence* contentInfo=iContentInfo->EncodeASN1DERLC();
	root->AddAndPopChildL(contentInfo);
	
	// Encode option fields certificates SET
	CASN1EncBase* cert=EncodeCertificatesLC();
	if (cert)
		{
		root->AddAndPopChildL(cert);			
		}
		
	// Encode signerinfo
	CASN1EncBase* signerInfo=EncodeSignerInfoLC();
	root->AddAndPopChildL(signerInfo);			
	
	return root;
	}


CASN1EncBase* CCmsSignedObject::EncodeCertificatesLC() const
	{
	return CmsUtils::EncodeCertificatesLC(iCertificates);
	}

CASN1EncBase* CCmsSignedObject::EncodeAlgorithmsLC() const
	{
	return CmsUtils::EncodeDigestAlgorithmsLC(iDigestAlgorithms);
	}

CASN1EncBase* CCmsSignedObject::EncodeSignerInfoLC() const
	{
	CASN1EncSet* signerInfoSet = CASN1EncSet::NewLC();
	TInt count=iSignerInfo.Count();
	
	for (TInt i=0;i<count;i++)
		{
		CASN1EncSequence* signerInfo=iSignerInfo[i]->EncodeASN1DERLC();
		signerInfoSet->AddAndPopChildL(signerInfo);	
		}
	return signerInfoSet;	
	}
	
EXPORT_C TBool CCmsSignedObject::ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, HBufC8*& aCertChainEncoding)
	{
	TInt certCount = iCertificates.Count();
	TInt endEntityPos = -1;
	TInt endEncodingSize = 0;
	TPtrC8 endEntityEncoding;
	TInt cert;
	TBool valid = EFalse;

	const CCmsSignerIdentifier& signerId = aSignerInfo.SignerIdentifier();
	
	// looks for end entity certificate
	for(cert = 0; cert < certCount; cert++)
		{
		if (iCertificates[cert]->CertificateType()==CCmsCertificateChoice::ECertificateX509)
			{
			const CX509Certificate& certificate = iCertificates[cert]->Certificate();

			endEncodingSize+= certificate.Encoding().Length();
			
			if(endEntityPos == -1)
				{
				if (signerId.SignerIdentifierType()==CCmsSignerIdentifier::EIssuerAndSerialNumber)
					{
					if (certificate.IssuerName().ExactMatchL(signerId.IssuerAndSerialNumber()->IssuerName()))
						{
						RInteger sn1=RInteger::NewL(certificate.SerialNumber());
						CleanupClosePushL(sn1);
						RInteger sn2=RInteger::NewL(signerId.IssuerAndSerialNumber()->SerialNumber());
						CleanupClosePushL(sn2);
						if (sn1==sn2)
							{
							endEntityPos = cert;
							endEntityEncoding.Set(certificate.Encoding());
							valid = ValidateSignatureL(aSignerInfo, certificate);					
							}
						CleanupStack::PopAndDestroy(2, &sn1);//sn2, sn1
						}
					}
				else
					{
					const CX509CertExtension* certExt = certificate.Extension(KSubjectKeyId);
					if (certExt)
						{
						CX509SubjectKeyIdExt* ext=CX509SubjectKeyIdExt::NewLC(certExt->Data());
						if (signerId.SubjectKeyIdentifier().Compare(ext->KeyId())==0)
							{
							endEntityPos = cert;
							endEntityEncoding.Set(certificate.Encoding());
							valid = ValidateSignatureL(aSignerInfo, certificate);						
							}
						CleanupStack::PopAndDestroy(ext);					
						}
					}											
				}
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
			if (iCertificates[cert]->CertificateType()==CCmsCertificateChoice::ECertificateX509)
				{
				const CX509Certificate& certificate = iCertificates[cert]->Certificate();
				if(cert != endEntityPos)
					{
					encodingPtr.Append(certificate.Encoding());
					}					
				}
			}
		}
	else
		{
		User::Leave(KErrNotFound);
		}
	return valid;
	}


EXPORT_C TBool CCmsSignedObject::ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, const RPointerArray<CX509Certificate>& aCertificates, HBufC8*& aCertChainEncoding)
	{
	TInt certCount = aCertificates.Count();
	TInt endEntityPos = -1;
	TInt endEncodingSize = 0;
	TPtrC8 endEntityEncoding;
	TInt cert;
	TBool valid = EFalse;
	const CCmsSignerIdentifier& signerId = aSignerInfo.SignerIdentifier();
	
	// looks for end entity certificate
	for(cert = 0; cert < certCount; cert++)
		{
		const CX509Certificate& certificate = *aCertificates[cert];
		endEncodingSize+= certificate.Encoding().Length();
		if(endEntityPos == -1)
			{
			if (signerId.SignerIdentifierType()==CCmsSignerIdentifier::EIssuerAndSerialNumber)
				{
				if (certificate.IssuerName().ExactMatchL(signerId.IssuerAndSerialNumber()->IssuerName()))
					{					
					RInteger sn1=RInteger::NewL(certificate.SerialNumber());
					CleanupClosePushL(sn1);
					RInteger sn2=RInteger::NewL(signerId.IssuerAndSerialNumber()->SerialNumber());
					CleanupClosePushL(sn2);
					if (sn1==sn2)
						{
						endEntityPos = cert;
						endEntityEncoding.Set(certificate.Encoding());
						valid = ValidateSignatureL(aSignerInfo, certificate);					
						}
					CleanupStack::PopAndDestroy(2, &sn1);//sn2, sn1
					}
				}
			else
				{
				const CX509CertExtension* certExt = certificate.Extension(KSubjectKeyId);
				if (certExt)
					{
					CX509SubjectKeyIdExt* ext=CX509SubjectKeyIdExt::NewLC(certExt->Data());
					if (signerId.SubjectKeyIdentifier().Compare(ext->KeyId())==0)
						{
						endEntityPos = cert;
						endEntityEncoding.Set(certificate.Encoding());
						valid = ValidateSignatureL(aSignerInfo, certificate);						
						}
					CleanupStack::PopAndDestroy(ext);
					}
				}				
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
			const CX509Certificate& certificate = *aCertificates[cert];
	
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


EXPORT_C TBool CCmsSignedObject::ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, HBufC8*& aCertChainEncoding, TBool aIsHash, const TDesC8& aContentDataOrHash)
	{
	if (aIsHash)
		{
		SetHash(aContentDataOrHash);	
		}
	else
		{
		SetContentData(aContentDataOrHash);
		}
	return ValidateSignerLC(aSignerInfo, aCertChainEncoding);
	}

EXPORT_C TBool CCmsSignedObject::ValidateSignerLC(const CCmsSignerInfo& aSignerInfo, const RPointerArray<CX509Certificate>& aCertificates, HBufC8*& aCertChainEncoding, TBool aIsHash, const TDesC8& aContentDataOrHash)
	{
	if (aIsHash)
		{
		SetHash(aContentDataOrHash);	
		}
	else
		{
		SetContentData(aContentDataOrHash);
		}
	return ValidateSignerLC(aSignerInfo, aCertificates, aCertChainEncoding);	
	}

TBool CCmsSignedObject::ValidateSignatureL(const CCmsSignerInfo& aSignerInfo, const CX509Certificate& aEndEntityCert)
	{
	delete iSigningAlgorithm;
	iSigningAlgorithm = NULL;			
	iSigningAlgorithm = CX509SigningAlgorithmIdentifier::NewL(aSignerInfo.SignatureAlgorithm(), aSignerInfo.DigestAlgorithm());
	
	delete iSignature;
	iSignature = NULL;
	iSignature = aSignerInfo.SignatureValue().AllocL();
	
	if (aSignerInfo.SignatureAlgorithm().Algorithm()==EDSA)
		{
		delete iParameters;
		iParameters = NULL;
		CDSAParameters* theDSAParams = iKeyFactory->DSAParametersL(aEndEntityCert.PublicKey().EncodedParams());
		CleanupStack::PushL(theDSAParams);
		CSigningKeyParameters* params = CSigningKeyParameters::NewLC();
		params->SetDSAParamsL(*theDSAParams);
		SetParametersL(*params);
		CleanupStack::PopAndDestroy(2, theDSAParams);
		}
	
	if (iContentInfo->IsContentDataPresent() || iContentData != KNullDesC8)
		{
		return VerifySignatureL(aEndEntityCert.PublicKey().KeyData());			
		}
	else if (iHash!=KNullDesC8)
			{
			return VerifySignatureL(aEndEntityCert.PublicKey().KeyData(), iHash);	
			}
		 else
			 {
			 User::Leave(KErrArgument);
			 return EFalse;
			 }			
	
	}

void CCmsSignedObject::InitSignedObjectL(const TDesC8& aRawData)
	{	
	// Populate CSignedObject data members
	iKeyFactory = new (ELeave) TX509KeyFactory;
	iEncoding = aRawData.AllocL();

	CSHA1* hash = CSHA1::NewL();
	CleanupStack::PushL(hash);
	iFingerprint = hash->Hash(Encoding()).AllocL();
	CleanupStack::PopAndDestroy(hash);	
	}

void CCmsSignedObject::DecodeSignedDataL(const TDesC8& aRawData)
	{		
	CArrayPtr<TASN1DecGeneric>* signedData = PKCS7ASN1::DecodeSequenceLC(aRawData, KCmsMinSignedDataElements, KCmsMaxSignedDataElements);
	TInt totalItems = signedData->Count();
	TASN1DecInteger decInt;

	// decodes version
	iDataElements.At(EVersionNumber) = new(ELeave) TPtrC8(signedData->At(0)->GetContentDER());
	iVersion = decInt.DecodeDERShortL(*signedData->At(0));
	if (iVersion>4 || iVersion<0)
		{
		User::Leave(KErrArgument);
		}
	// decodes algorithms
	iDataElements.At(EDigestAlgorithms) = new(ELeave) TPtrC8(signedData->At(1)->GetContentDER());
	DecodeDigestAlgorithmsL(signedData->At(1)->Encoding());
	// decodes contentinfo
	iDataElements.At(EEncapsulatedContentInfo) = new(ELeave) TPtrC8(signedData->At(2)->GetContentDER());
	DecodeEncapsulatedContentInfoL(signedData->At(2)->Encoding());

	// Checks for optional fields
	TInt pos = 3;	// Skip first non-optional fields
	do
		{
		const TASN1DecGeneric& currentItem = *signedData->At(pos);
		switch(currentItem.Tag())
			{
			case KSignedDataCertificates:
				{
				if (currentItem.Class()!=EContextSpecific)
					{
					User::Leave(KErrArgument);	
					}				
				iIsCertificateSetPresent=ETrue;
				iDataElements.At(ECertificates) = new(ELeave) TPtrC8(currentItem.GetContentDER());
				DecodeCertificatesL(currentItem.Encoding());
				break;
				}
			case KSignedDataRevocationLists:
				{
				if (currentItem.Class()!=EContextSpecific)
					{
					User::Leave(KErrArgument);	
					}				
				iIsCertificateRevocationListsPresent=ETrue;
				iDataElements.At(ERevocationLists) = new(ELeave) TPtrC8(currentItem.GetContentDER());
				DecodeRevocationListsL(currentItem.Encoding());
				break;
				}
			default:	
				{
				//Optional field with wrong tag and class
				if (pos<totalItems-1)
					{
					User::Leave(KErrArgument);
					}
				// else Non-optional field	
				}
			}
		pos++;
		}
	while(pos < totalItems);

	iDataElements.At(ESignedInfo) = new(ELeave) TPtrC8(signedData->At(totalItems-1)->GetContentDER());
	DecodeSignerInfoL(signedData->At(totalItems-1)->Encoding());

	CleanupStack::PopAndDestroy(signedData);
	}

void CCmsSignedObject::DecodeDigestAlgorithmsL(const TDesC8& aRawData)
	{	
	CmsUtils::DecodeDigestAlgorithmsL(iDigestAlgorithms, aRawData);
	}

void CCmsSignedObject::DecodeCertificatesL(const TDesC8& aRawData)
	{
	CmsUtils::DecodeCertificatesL(iCertificates, aRawData);	
	}

void CCmsSignedObject::DecodeRevocationListsL(const TDesC8& /*aRawData*/)
	{	
	}
	
EXPORT_C const TPtrC8 CCmsSignedObject::SignedDataL() const
	{
	if (iContentInfo->IsContentDataPresent())
		{
		return iContentInfo->ContentData();	
		}
	else
		{
		if (iContentData!=KNullDesC8)
			{
			return iContentData;	
			}
		else
			{
			User::Leave(KErrArgument);
			}
		}
	return KNullDesC8();
	}

EXPORT_C void CCmsSignedObject::InternalizeL(RReadStream& /*aStream*/) 
	{
	User::Leave(KErrNotSupported);
	}

EXPORT_C const TPtrC8* CCmsSignedObject::DataElementEncoding(const TUint aIndex) const
	{
	return iDataElements.At(aIndex);
	}

void CCmsSignedObject::SetContentData(const TDesC8& aContentData)
	{
	iContentData.Set(aContentData);	
	}

void CCmsSignedObject::SetHash(const TDesC8& aHash)
	{
	iHash.Set(aHash);
	}


	

