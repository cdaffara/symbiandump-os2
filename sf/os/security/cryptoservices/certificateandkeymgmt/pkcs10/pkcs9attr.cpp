/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CPKCS9ChallengePasswordAttr and CPKCS9ExtensionRequestAttr class implementation.
*
*/


#include <e32std.h>
#include <e32def.h>
#include <asn1enc.h>
#include <pkcs9attr.h>

// CPKCS9ChallengePasswordAttr ////////////////////////////////////////////////////////////

EXPORT_C CPKCS9ChallengePasswordAttr* CPKCS9ChallengePasswordAttr::NewLC(const TDesC8& aPassword)
	{
	CPKCS9ChallengePasswordAttr* self = new (ELeave) CPKCS9ChallengePasswordAttr;
	CleanupStack::PushL(self);
	self->ConstructL(aPassword);
	return self;
	}

EXPORT_C CPKCS9ChallengePasswordAttr* CPKCS9ChallengePasswordAttr::NewL(const TDesC8& aPassword)
	{
	CPKCS9ChallengePasswordAttr* self = NewLC(aPassword);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CPKCS9ChallengePasswordAttr::ResetL(const TDesC8& aPassword)
	{
	delete iRoot;
	iRoot = NULL;
	ConstructL(aPassword);
	}

CPKCS9ChallengePasswordAttr::CPKCS9ChallengePasswordAttr()
	{
	}

CPKCS9ChallengePasswordAttr::~CPKCS9ChallengePasswordAttr()
	{
	}

CASN1EncBase* CPKCS9ChallengePasswordAttr::GetEncodingLC()
	{
	return CPKCSAttributeBase::GetEncodingLC();
	}

void CPKCS9ChallengePasswordAttr::ConstructL(const TDesC8& aPassword)
	{
	// Size of password string should be between bounds
	if ((aPassword.Length() < KPkcs9StringLB) || (aPassword.Length() > KPkcs9StringUB))
		{
		User::Leave(KErrArgument);
		}

	iRoot = CASN1EncSequence::NewL();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KPkcs9ChallengePasswordAttrOID);
	iRoot->AddAndPopChildL(oid);
	iValueSet = CASN1EncSet::NewLC();
	iRoot->AddAndPopChildL(iValueSet);		// Takes ownership
	CASN1EncPrintableString* value = CASN1EncPrintableString::NewLC(aPassword);
	iValueSet->AddAndPopChildL(value);
	}

// CPKCS9ExtensionRequestAttr ////////////////////////////////////////////////////////////

EXPORT_C CPKCS9ExtensionRequestAttr* CPKCS9ExtensionRequestAttr::NewLC(const CX509CertExtension& aExtension)
	{
	CPKCS9ExtensionRequestAttr* self = new (ELeave) CPKCS9ExtensionRequestAttr;
	CleanupStack::PushL(self);
	self->ConstructL(aExtension);
	return self;
	}

EXPORT_C CPKCS9ExtensionRequestAttr* CPKCS9ExtensionRequestAttr::NewL(const CX509CertExtension& aExtension)
	{
	CPKCS9ExtensionRequestAttr* self = NewLC(aExtension);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CPKCS9ExtensionRequestAttr::AddExtensionL(const CX509CertExtension& aExtension)
	{
	CASN1EncSequence *extension = aExtension.EncodeASN1DERLC();
	iExtSeq->AddAndPopChildL(extension);
	}

EXPORT_C void CPKCS9ExtensionRequestAttr::ResetL(const CX509CertExtension& aExtension)
	{
	delete iRoot;
	iRoot = NULL;
	ConstructL(aExtension);
	}

CPKCS9ExtensionRequestAttr::CPKCS9ExtensionRequestAttr()
	{
	}

CPKCS9ExtensionRequestAttr::~CPKCS9ExtensionRequestAttr()
	{
	}

CASN1EncBase* CPKCS9ExtensionRequestAttr::GetEncodingLC()
	{
	return CPKCSAttributeBase::GetEncodingLC();
	}

void CPKCS9ExtensionRequestAttr::ConstructL(const CX509CertExtension& aExtension)
	{
	iRoot = CASN1EncSequence::NewL();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(KPkcs9ExtensionRequestAttrOID);
	iRoot->AddAndPopChildL(oid);
	iValueSet = CASN1EncSet::NewLC();
	iRoot->AddAndPopChildL(iValueSet);		// Takes ownership
	iExtSeq = CASN1EncSequence::NewLC();
	iValueSet->AddAndPopChildL(iExtSeq);
	CASN1EncSequence *extension = aExtension.EncodeASN1DERLC();
	iExtSeq->AddAndPopChildL(extension);
	}

