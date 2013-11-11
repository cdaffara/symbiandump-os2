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


#include "cmsutils.h"
#include <asymmetric.h>
#include <asn1enc.h>
#include <asn1dec.h> 
#include <cmsdefs.h>
#include <pkcs7excert.h>
#include <x509cert.h>
#include "pkcs7asn1.h"


TInt CmsUtils::DecodeContentTypeL(const TASN1DecGeneric* aASN1DecGeneric)
	{
	TInt type(0);
	if(aASN1DecGeneric->Tag()==EASN1ObjectIdentifier || aASN1DecGeneric->Class() == EUniversal)
		{
		TASN1DecObjectIdentifier oidDec;
		HBufC* oidVal = oidDec.DecodeDERL(*aASN1DecGeneric);
		CleanupStack::PushL(oidVal);		
		
		// Checks if it is data OID.
		if(*oidVal == KCmsDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to Data then,it is represented by 1
			type = EContentTypeData;
			}
		else if(*oidVal == KCmsSignedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to SignedData then,it is represented by 2
			type = EContentTypeSignedData;
			}
		else if(*oidVal == KCmsEnvelopedDataOID) 
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to EnvelopedData then,it is represented by 3.
			type = EContentTypeEnvelopedData;
			}
		else if(*oidVal == KCmsDigestedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to DigestedData then,it is represented by 5.
			type = EContentTypeDigestedData;
			}
		else if(*oidVal == KCmsEncryptedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to EncryptedData then,it is represented by 6
			type = EContentTypeEncryptedData;
			}
		else if(*oidVal == KCmsAuthenticatedDataOID)
			{
			// The Content Type is indicated by an Integer.
			// Here if Content Type is equal to EncryptedData then,it is represented by 7
			type = EContentTypeAuthenticatedData;
			}
			
		else
			{
			User::Leave(KErrArgument);
			}
		CleanupStack::PopAndDestroy(oidVal);
		}
	else
		{
		User::Leave(KErrArgument);
		}	
	return type;
	}

CASN1EncObjectIdentifier* CmsUtils::EncodeContentTypeLC(TInt aContentType)
	{
	TPtrC oidBuf;
	switch (aContentType)
		{
	case EContentTypeData:
		oidBuf.Set(KCmsDataOID());
		break;
		
	case EContentTypeSignedData:
		oidBuf.Set(KCmsSignedDataOID());
		break;
		
	case EContentTypeEnvelopedData:
		oidBuf.Set(KCmsEnvelopedDataOID());
		break;
	
	case EContentTypeDigestedData:
		oidBuf.Set(KCmsDigestedDataOID());
		break;
	
	case EContentTypeEncryptedData:
		oidBuf.Set(KCmsEncryptedDataOID());
		break;
	
	default:
		User::Leave(KErrArgument);
		}
	//Encode the OID		
	CASN1EncObjectIdentifier* oid=CASN1EncObjectIdentifier::NewLC(oidBuf);
	return oid;	
	}

void CmsUtils::AddCertificateL(RPointerArray<CCmsCertificateChoice>& aCertList, const TDesC8& aCert, CCmsCertificateChoice::TCertificateType aType)
	{
	TInt found(EFalse);
	TInt count=aCertList.Count();
	for (TInt i=0;i<count;i++)
		{
		if (aCertList[i]->CertificateType()==CCmsCertificateChoice::ECertificateAttribute
				&& aCert.Compare(*aCertList[i]->AttributeCertificate())==0)
			{
			found=ETrue;
			break;
			}
		}
		
	if (!found)
		{
		CCmsCertificateChoice* cert=CCmsCertificateChoice::NewL(aType, aCert);
		CleanupStack::PushL(cert);
		aCertList.AppendL(cert);
		CleanupStack::Pop(cert);					
		}
	}
	
void CmsUtils::AddCertificateL(RPointerArray<CCmsCertificateChoice>& aCertList, const CX509Certificate& aCert)
	{
	
	TInt found(EFalse);
	TInt count=aCertList.Count();
	for (TInt i=0;i<count;i++)
		{
		if (aCertList[i]->CertificateType()==CCmsCertificateChoice::ECertificateX509
				&& aCert.IsEqualL(aCertList[i]->Certificate()))
			{
			found=ETrue;
			break;
			}
		}
	
	if (!found)
		{
		CCmsCertificateChoice* cert=CCmsCertificateChoice::NewL(aCert);
		CleanupStack::PushL(cert);
		aCertList.AppendL(cert);
		CleanupStack::Pop(cert);					
		}
		
	}
	
void CmsUtils::AddAlgorithmIdentifierL(RPointerArray<CX509AlgorithmIdentifier>& aAlgorithmIdList, TAlgorithmId aDigestAlgorithm)
	{
	TInt found(EFalse);
	TInt count=aAlgorithmIdList.Count();
	for (TInt i=0;i<count;i++)
		{
		if (aAlgorithmIdList[i]->Algorithm()==aDigestAlgorithm)
			{
			found=ETrue;
			break;
			}
		}
	if (!found)
		{
		CX509AlgorithmIdentifier* digAlg=CX509AlgorithmIdentifier::NewLC(aDigestAlgorithm, KNullDesC8());
		aAlgorithmIdList.AppendL(digAlg);
		CleanupStack::Pop(digAlg);					
		}
	}

void CmsUtils::DecodeDigestAlgorithmsL(RPointerArray<CX509AlgorithmIdentifier>& aDigestAlgorithms, const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* algsData = PKCS7ASN1::DecodeSequenceLC(aRawData);
	TInt count = algsData->Count();
	CX509AlgorithmIdentifier* alIdent;

	for(TInt item = 0; item < count; item++)
		{
 		alIdent = CX509AlgorithmIdentifier::NewLC(algsData->At(item)->Encoding());
		aDigestAlgorithms.AppendL(alIdent);
		CleanupStack::Pop(alIdent);
		}
	CleanupStack::PopAndDestroy(algsData);			
	}

void CmsUtils::DecodeCertificatesL(RPointerArray<CCmsCertificateChoice>& aCertificates, const TDesC8& aRawData)
	{
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();
	TASN1DecSequence decSeq;
	// have to do manual decoding of sequence because field is optional
	CArrayPtr<TASN1DecGeneric>* items = NULL;
	items = decSeq.DecodeDERLC(decGen);
	TInt count = items->Count();

	CCmsCertificateChoice* certificate;

	for(TInt item = 0; item < count; item++)
		{
 		certificate = CCmsCertificateChoice::NewL(items->At(item)->Encoding());
		CleanupStack::PushL(certificate);
		aCertificates.AppendL(certificate);
		CleanupStack::Pop(certificate);
		}

	CleanupStack::PopAndDestroy(items);			
	}

CASN1EncBase* CmsUtils::EncodeCertificatesLC(const RPointerArray<CCmsCertificateChoice>& aCertificates)
	{
	CASN1EncSet* certSet(NULL);
	TInt count=aCertificates.Count();
	if (count>0)
		{
		certSet = CASN1EncSet::NewLC();
		for (TInt i=0;i<count;i++)
			{
			CASN1EncEncoding* cert=aCertificates[i]->EncodeASN1DERLC();
			certSet->AddAndPopChildL(cert);
			}
		// [0] implicit
		certSet->SetTag(0);	
		}
	return certSet;
	}

CASN1EncBase* CmsUtils::EncodeDigestAlgorithmsLC(const RPointerArray<CX509AlgorithmIdentifier>& aDigestAlgorithms)
	{
	CASN1EncSet* algorithmSet = CASN1EncSet::NewLC();
	TInt count=aDigestAlgorithms.Count();
	
	for (TInt i=0;i<count;i++)
		{
		CASN1EncSequence* tmp=aDigestAlgorithms[i]->EncodeASN1DERLC();
		algorithmSet->AddAndPopChildL(tmp);
		}
		
	return algorithmSet;
	}

void CmsUtils::DecodeOctetStringL(const TDesC8& aRawData, HBufC8*& aBuf)
	{
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();

	if(decGen.Tag()==EASN1OctetString && decGen.Class()==EUniversal)
		{
		TASN1DecOctetString decOct;
		aBuf = decOct.DecodeDERL(decGen);
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}


CMessageDigest* CmsUtils::CreateHashLC(TAlgorithmId aAlgorithm)
	{	
	CMessageDigest* hash(NULL);
	switch (aAlgorithm)
		{
	case EMD2:
		hash=CMD2::NewL();
		break;
		
	case EMD5:
		hash=CMD5::NewL();
		break;
		
	case ESHA1:
		hash=CSHA1::NewL();
		break;
		
	default:
		User::Leave(KErrNotSupported);
		}
	CleanupStack::PushL(hash);
	return hash;	
	}


HBufC8* CmsUtils::CreateSignatureL(const TDesC8& aDataToBeSigned, TBool aIsHash, TAlgorithmId aAlgorithm, const CDSAPrivateKey& aKey)
	{
	HBufC8* signature(NULL);
	
	if (!aIsHash)
		{
		TPtrC8 hashValue;
		// Create hash first
		CMessageDigest* hash=CreateHashLC(aAlgorithm);
		hashValue.Set(hash->Hash(aDataToBeSigned));
		signature=CreateSignatureL(hashValue, aKey);
		CleanupStack::PopAndDestroy(); // hash
		}
	else
		{
		signature=CreateSignatureL(aDataToBeSigned, aKey);
		}
		
	return signature;
	}

HBufC8* CmsUtils::CreateSignatureL(const TDesC8& aDataToBeSigned, TBool aIsHash, TAlgorithmId aAlgorithm, const CRSAPrivateKey& aKey)
	{
	HBufC8* signature(NULL);
	TPtrC8 hashValue;
	if (!aIsHash)
		{
		// Create hash first
		CMessageDigest* hash=CreateHashLC(aAlgorithm);
		hashValue.Set(hash->Hash(aDataToBeSigned));
		}
	else
		{
		hashValue.Set(aDataToBeSigned);
		}
		
	//Build the digestInfo Sequence
	CASN1EncSequence* digestInfoSeq = CASN1EncSequence::NewLC();
	
	//Encode the Algorithm
	CX509AlgorithmIdentifier* digAlg=CX509AlgorithmIdentifier::NewLC(aAlgorithm, KNullDesC8());
	CASN1EncSequence* sigAlg=digAlg->EncodeASN1DERLC();
	digestInfoSeq->AddAndPopChildL(sigAlg);
	CleanupStack::PopAndDestroy(digAlg);
	
	//Encode the digest itself
	CASN1EncOctetString* sigEnc=CASN1EncOctetString::NewLC(hashValue);
	digestInfoSeq->AddAndPopChildL(sigEnc);

	//Get the Encoding
	HBufC8* digestInfo=CreateDEREncodingLC(*digestInfoSeq);
	signature=CreateSignatureL(digestInfo->Des(), aKey);
	
	CleanupStack::PopAndDestroy(2, digestInfoSeq); //digestInfo, digestInfoSeq
	if (!aIsHash)
		{
		CleanupStack::PopAndDestroy(); // hash			
		}
	return signature;
	}
	
HBufC8* CmsUtils::CreateSignatureL(const TDesC8& aHash, const CDSAPrivateKey& aKey)
	{	
	//Create Signer and sign the hash
	CDSASigner* signer=CDSASigner::NewLC(aKey);
	const CDSASignature* sig=signer->SignL(aHash);
	CDSASignature* signonc=const_cast<CDSASignature*>(sig);
	
	CleanupStack::PushL(signonc);
	
	//The sequence
	CASN1EncSequence* sigSeq = CASN1EncSequence::NewLC();	
	CASN1EncBigInt* r = CASN1EncBigInt::NewLC(static_cast<const RInteger&>(sig->R()));
	sigSeq->AddAndPopChildL(r);
	CASN1EncBigInt* s = CASN1EncBigInt::NewLC(static_cast<const RInteger&>(sig->S()));
	sigSeq->AddAndPopChildL(s);
		
	//Write the buffer
	TUint len=sigSeq->LengthDER();
	HBufC8* buf=HBufC8::NewMaxLC(len);
	TPtr8 des=buf->Des();
	TUint pos=0;
	sigSeq->WriteDERL(des, pos);
	
	//Clean up
	CleanupStack::Pop(buf); //buf
	CleanupStack::PopAndDestroy(3, signer); //sigSeq, signonc, signer
	return buf;
	}

HBufC8* CmsUtils::CreateSignatureL(const TDesC8& aHash, const CRSAPrivateKey& aKey)
	{
	//Create Signer and sign the hash
	CRSAPKCS1v15Signer* signer=CRSAPKCS1v15Signer::NewLC(aKey);
	const CRSASignature* sig=signer->SignL(aHash);
	CRSASignature* signonc=const_cast<CRSASignature*>(sig);
	
	CleanupStack::PushL(signonc);
	
	HBufC8* sigData = sig->S().BufferWithNoTruncationLC();
	
	//Clean up
	CleanupStack::Pop(sigData);
	CleanupStack::PopAndDestroy(2, signer);//signonc signer
	return sigData;
	}

HBufC8* CmsUtils::CreateDEREncodingLC(const CASN1EncBase& aEncoding)
	{	
	TUint len = aEncoding.LengthDER();
	HBufC8* buf = HBufC8::NewMaxLC(len);
	TUint pos = 0;
	TPtr8 bufptr(buf->Des());
	aEncoding.WriteDERL(bufptr, pos);
	return buf;
	}
























