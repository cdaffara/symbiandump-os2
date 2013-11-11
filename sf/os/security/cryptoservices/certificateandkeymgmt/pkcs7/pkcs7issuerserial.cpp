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


#include <pkcs7issuerserial.h> 
#include <x500dn.h>
#include <asn1dec.h>
#include <asn1enc.h>
#include <bigint.h>
#include "pkcs7asn1.h"

//
// Implementation of CPKCS7IssuerAndSerialNumber
//
CPKCS7IssuerAndSerialNumber* CPKCS7IssuerAndSerialNumber::NewL(CX500DistinguishedName* aIssuerName, const TDesC8& aSerialNumber)
	{
	CPKCS7IssuerAndSerialNumber* self = NewLC(aIssuerName, aSerialNumber);
	CleanupStack::Pop(self);
	return self;		
	}

CPKCS7IssuerAndSerialNumber* CPKCS7IssuerAndSerialNumber::NewLC(CX500DistinguishedName* aIssuerName, const TDesC8& aSerialNumber)
	{
	if (!aIssuerName)
		{
		User::Leave(KErrArgument);	
		}
		
	CPKCS7IssuerAndSerialNumber* self = new (ELeave) CPKCS7IssuerAndSerialNumber();
	CleanupStack::PushL(self);
	self->ConstructL(aIssuerName, aSerialNumber);
	return self;		
	}

CPKCS7IssuerAndSerialNumber* CPKCS7IssuerAndSerialNumber::NewL(const TDesC8& aRawData)
	{
	CPKCS7IssuerAndSerialNumber* self = new (ELeave) CPKCS7IssuerAndSerialNumber();
	CleanupStack::PushL(self);
	self->ConstructL(aRawData);
	CleanupStack::Pop(self);
	return self;	
	}

EXPORT_C CPKCS7IssuerAndSerialNumber::~CPKCS7IssuerAndSerialNumber()
	{
	delete iIssuerName;
	delete iSerialNumber;
	}

CPKCS7IssuerAndSerialNumber::CPKCS7IssuerAndSerialNumber(void)
	{
	}

void CPKCS7IssuerAndSerialNumber::ConstructL(const TDesC8& aRawData)
	{
	CArrayPtr<TASN1DecGeneric>* itemsData = PKCS7ASN1::DecodeSequenceLC(aRawData, 2, 2);
	TASN1DecInteger decInt;
	iIssuerName = CX500DistinguishedName::NewL(itemsData->At(0)->Encoding());
	RInteger sn = decInt.DecodeDERLongL(*itemsData->At(1));
	CleanupClosePushL(sn);
	iSerialNumber = sn.BufferLC();
	CleanupStack::Pop(iSerialNumber);
	CleanupStack::PopAndDestroy(2, itemsData);
	}

void CPKCS7IssuerAndSerialNumber::ConstructL(CX500DistinguishedName* aIssuerName, const TDesC8& aSerialNumber)
	{
	iSerialNumber=aSerialNumber.AllocL();
	iIssuerName=aIssuerName;
	}

EXPORT_C const CX500DistinguishedName& CPKCS7IssuerAndSerialNumber::IssuerName() const
	{
	return *iIssuerName;
	}

EXPORT_C const TDesC8& CPKCS7IssuerAndSerialNumber::SerialNumber() const
	{
	return *iSerialNumber;
	}


CASN1EncSequence* CPKCS7IssuerAndSerialNumber::EncodeASN1DERLC() const
	{
	CASN1EncSequence* root = CASN1EncSequence::NewLC();
	CASN1EncSequence* issuerName=iIssuerName->EncodeASN1LC();
	root->AddAndPopChildL(issuerName);
	RInteger snBInt=RInteger::NewL(iSerialNumber->Des());
	CleanupClosePushL(snBInt);
	CASN1EncBigInt* sn=CASN1EncBigInt::NewLC(snBInt);
	root->AddAndPopChildL(sn);
	CleanupStack::PopAndDestroy(&snBInt);
	return root;
	}




