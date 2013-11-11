/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#include "pkcs7signerinfo.h"
#include "pkcs7issuerserial.h"
#include "pkcs7asn1.h"
#include "signed.h"
#include <asn1dec.h>
#include <x509cert.h>

CPKCS7SignerInfo* CPKCS7SignerInfo::NewL(const TDesC8& aRawData)
	{
	CPKCS7SignerInfo* self = new (ELeave) CPKCS7SignerInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	CleanupStack::Pop(self);
	return self;
	}

CPKCS7SignerInfo::~CPKCS7SignerInfo(void)
	{
	delete iIssuerAndSerialNumber;
	delete iDigestAlgorithm;
	delete iDigestEncryptionAlgorithm;
	delete iEncryptedDigest;
	}

CPKCS7SignerInfo::CPKCS7SignerInfo(void)
	{
	}

EXPORT_C TInt CPKCS7SignerInfo::Version() const
	{
	return iVersion;
	}

EXPORT_C const CPKCS7IssuerAndSerialNumber& CPKCS7SignerInfo::IssuerAndSerialNumber() const
	{
	return *iIssuerAndSerialNumber;
	}

EXPORT_C const CX509AlgorithmIdentifier& CPKCS7SignerInfo::DigestAlgorithm() const
	{
	return *iDigestAlgorithm;
	}

EXPORT_C const CX509AlgorithmIdentifier& CPKCS7SignerInfo::DigestEncryptionAlgorithm() const
	{
	return *iDigestEncryptionAlgorithm;
	}

EXPORT_C const TPtrC8 CPKCS7SignerInfo::EncryptedDigest() const
	{
	return *iEncryptedDigest;
	}


void CPKCS7SignerInfo::ConstructL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* signerInfo = PKCS7ASN1::DecodeSequenceLC(aRawData, 5, 7);
	TASN1DecInteger decInt;
	TInt pos = 3;

	// decodes version
	iVersion = decInt.DecodeDERShortL(*signerInfo->At(0));	

	iIssuerAndSerialNumber = CPKCS7IssuerAndSerialNumber::NewL(signerInfo->At(1)->Encoding());

	iDigestAlgorithm = CX509AlgorithmIdentifier::NewL(signerInfo->At(2)->Encoding());

	if(signerInfo->At(pos)->Tag() == 0)
		{
		// authenticated attributes not supported at this time
		pos++;
		}
	iDigestEncryptionAlgorithm = CX509AlgorithmIdentifier::NewL(signerInfo->At(pos++)->Encoding());
	DecodeEncryptedDigestL(signerInfo->At(pos++)->Encoding());

	if(pos < signerInfo->Count() && (signerInfo->At(pos)->Tag() == 0))
		{
		// unauthenticated attributes not supported at this time
		pos++;		
		}

	CleanupStack::PopAndDestroy(signerInfo);
	}

void CPKCS7SignerInfo::DecodeEncryptedDigestL(const TDesC8& aRawData)
	{
	TASN1DecGeneric decGen(aRawData);
	decGen.InitL();

	if(decGen.Tag() == EASN1OctetString)
		{
		TASN1DecOctetString decOct;
		iEncryptedDigest = decOct.DecodeDERL(decGen);
		}
	else
		{
		User::Leave(KErrArgument);
		}
	}
