/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

 
#include "pkcs7digestinfo.h"
#include "pkcs7asn1.h"

EXPORT_C CPKCS7DigestInfo* CPKCS7DigestInfo::NewL(const TDesC8& aRawData)
	{
	CPKCS7DigestInfo* self = new (ELeave) CPKCS7DigestInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	CleanupStack::Pop(self);
	return self;	
	}
	
CPKCS7DigestInfo::~CPKCS7DigestInfo()
	{
    }

CPKCS7DigestInfo::CPKCS7DigestInfo()
	{
	}
	
EXPORT_C const TDesC8& CPKCS7DigestInfo::Digest() const
	{
	return iDigest;
	}

EXPORT_C TAlgorithmId CPKCS7DigestInfo::Algorithm() const
	{
	return iAlgorithmId;
	}

EXPORT_C const TPtrC8& CPKCS7DigestInfo::EncodedParams() const
	{
	return iEncodedParams;
	}
	
void CPKCS7DigestInfo::ConstructL(const TDesC8& aDigestData)
	{
    CArrayPtr<TASN1DecGeneric>* digestInfo = PKCS7ASN1::DecodeSequenceLC(aDigestData);

	// Check if both the digestAlgorithm and the Digest are present.
	if ( digestInfo->Count() != 2 )
		{
	    User::Leave(KErrArgument);
		}

	// DIGEST ALGORITHM
	// Get the algorithm identifier and the encoded parameters present in the sequence.
	const TASN1DecGeneric* digestInfoAt0 = digestInfo->At(0);
	if(digestInfoAt0->Tag() != EASN1Sequence || digestInfoAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	CX509AlgorithmIdentifier* digestAlg = CX509AlgorithmIdentifier::NewLC(digestInfo->At(0)->Encoding());
	TAlgorithmId algorithmId = digestAlg->Algorithm();
	switch(algorithmId)
		{
		case EMD2:
			{
			iAlgorithmId = EMD2;
			}
			break;
		case EMD5:
			{
			iAlgorithmId = EMD5;
			}
			break;
		case ESHA1:
			{
			iAlgorithmId = ESHA1;	
			}
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	iEncodedParams.Set(digestAlg->EncodedParams()); 
		
	// DIGEST  
	const TASN1DecGeneric* digestInfoAt1 = digestInfo->At(1);
	if(digestInfoAt1->Tag() != EASN1OctetString || digestInfoAt1->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecGeneric octetStringDecoder(*(digestInfo->At(1)));
	octetStringDecoder.InitL();
	iDigest.Set(octetStringDecoder.GetContentDER());
	
	CleanupStack::PopAndDestroy(2,digestInfo);// digestInfo, digestAlg
    }
