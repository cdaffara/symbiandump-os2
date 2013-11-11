/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* CPKCS10Attributes class implementation.
*
*/


#include <e32std.h>
#include <e32def.h>
#include <asn1enc.h>
#include <pkcs10attr.h>


// CPKCS10AttributeBase /////////////////////////////////////////////////////////

CPKCSAttributeBase::CPKCSAttributeBase()
	{
	}

CPKCSAttributeBase::~CPKCSAttributeBase()
	{
	delete iRoot;
	}

CASN1EncBase* CPKCSAttributeBase::GetEncodingLC()
	{
	CASN1EncBase* result = iRoot;
	iRoot = NULL;
	iValueSet = NULL;
	CleanupStack::PushL(result);
	return result;
	}

// CPKCS10Attribute /////////////////////////////////////////////////////////

// This is being deprecated, will be removed
EXPORT_C CPKCS10Attribute* CPKCS10Attribute::NewLC(const TDesC& aAttrOID)
	{
	CPKCS10Attribute* self = new (ELeave) CPKCS10Attribute;
	CleanupStack::PushL(self);
	self->ConstructL(aAttrOID, NULL);
	return self;
	}

EXPORT_C CPKCS10Attribute* CPKCS10Attribute::NewLC(const TDesC& aAttrOID, CASN1EncBase* aASN1)
	{
	if (aASN1 == NULL)
		{
		User::Leave(KErrArgument);
		}
	CPKCS10Attribute* self = new (ELeave) CPKCS10Attribute;
	CleanupStack::PushL(self);
	self->ConstructL(aAttrOID, aASN1);
	return self;
	}

EXPORT_C CPKCS10Attribute* CPKCS10Attribute::NewL(const TDesC& aAttrOID, CASN1EncBase* aASN1)
	{
	CPKCS10Attribute* self = NewLC(aAttrOID, aASN1);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C void CPKCS10Attribute::ResetL(const TDesC& aAttrOID, CASN1EncBase* aASN1)
	{
	if (aASN1 == NULL)
		{
		User::Leave(KErrArgument);
		}
	delete iRoot;
	iRoot = NULL;
	ConstructL(aAttrOID, aASN1);
	}

EXPORT_C void CPKCS10Attribute::AddValueL(CASN1EncBase* aASN1)
	{
	if (aASN1 == NULL)
		{
		User::Leave(KErrArgument);
		}
	iValueSet->AddChildL(aASN1);
	}

CPKCS10Attribute::CPKCS10Attribute()
	{
	}

CPKCS10Attribute::~CPKCS10Attribute()
	{
	}

void CPKCS10Attribute::ConstructL(const TDesC& aAttrOID, CASN1EncBase* aASN1)
	{
	iRoot = CASN1EncSequence::NewL();
	CASN1EncObjectIdentifier* oid = CASN1EncObjectIdentifier::NewLC(aAttrOID);
	iRoot->AddAndPopChildL(oid);
	iValueSet = CASN1EncSet::NewLC();
	iRoot->AddAndPopChildL(iValueSet);	// Takes ownership

	// For now just to allow for deprecated NewLC() which allows for NULL aASN1
	// Replace with call to AddValueL() once the quirky NewLC() is removed
	if (aASN1)
		{
		iValueSet->AddChildL(aASN1);
		}
	}

// Deprecated
CASN1EncBase* CPKCS10Attribute::TakeEncodingC()
	{
	return CPKCSAttributeBase::GetEncodingLC();
	}

// Deprecated
CASN1EncBase* CPKCS10Attribute::TakeEncodingLC()
	{
	return CPKCSAttributeBase::GetEncodingLC();
	}

CASN1EncBase* CPKCS10Attribute::GetEncodingLC()
	{
	return CPKCSAttributeBase::GetEncodingLC();
	}


// CPKCS10Attributes ///////////////////////////////////////////////////////////

CPKCS10Attributes::CPKCS10Attributes()
	{
	}

EXPORT_C CPKCS10Attributes::~CPKCS10Attributes()
	{
	delete iRoot;
	}

EXPORT_C CPKCS10Attributes* CPKCS10Attributes::NewLC()
	{
	CPKCS10Attributes* self = new (ELeave) CPKCS10Attributes();
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

EXPORT_C CPKCS10Attributes* CPKCS10Attributes::NewL()
	{
	CPKCS10Attributes* self = NewLC();
	CleanupStack::Pop(self);
	return self;
	}

void CPKCS10Attributes::ConstructL()
	{
	// Create top level ASN1 sequence
	iRoot = CASN1EncSequence::NewL();
	iRoot->SetTag(0);
	}

// To be deprecated
EXPORT_C void CPKCS10Attributes::AddAttributeL(CPKCS10Attribute* aAttr)
	{
	ASSERT(iRoot != NULL);
	ASSERT(aAttr != NULL);
	iRoot->AddAndPopChildL(aAttr->TakeEncodingLC());
	delete aAttr;
	}

EXPORT_C void CPKCS10Attributes::AddPKCSAttributeL(CPKCSAttributeBase* aAttr)
	{
	ASSERT(iRoot != NULL);
	if (aAttr == NULL)
		{
		User::Leave(KErrArgument);
		}
	iRoot->AddAndPopChildL(aAttr->GetEncodingLC());
	delete aAttr;
	}

CASN1EncBase* CPKCS10Attributes::TakeEncodingLC()
	{
	CASN1EncBase* result = iRoot;
 	iRoot = NULL;
	CleanupStack::PushL(result);
	return result;
	}
