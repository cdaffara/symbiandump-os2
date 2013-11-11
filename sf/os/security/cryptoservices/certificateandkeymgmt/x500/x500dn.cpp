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


#include <x500dn.h>
#include <x520ava.h>
#include "X500dec.h"
#include <asn1enc.h>


EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewL(const CArrayPtr<CX520AttributeTypeAndValue>& aElements)
	{
	CX500DistinguishedName* self = CX500DistinguishedName::NewLC(aElements);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewLC(const CArrayPtr<CX520AttributeTypeAndValue>& aElements)
	{
	CX500DistinguishedName* self = new(ELeave) CX500DistinguishedName;
	CleanupStack::PushL(self);
	self->ConstructL(aElements);
	return self;
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewL(const CX500DistinguishedName& aName)
	{
	CX500DistinguishedName* self = CX500DistinguishedName::NewLC(aName);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewLC(const CX500DistinguishedName& aName)
	{
	CX500DistinguishedName* self = new(ELeave) CX500DistinguishedName;
	CleanupStack::PushL(self);
	self->ConstructL(static_cast<CArrayPtr<CX520AttributeTypeAndValue> &>(*(aName.iElements)));
	return self;
	}	

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX500DistinguishedName::NewL(aBinaryData, pos);
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX500DistinguishedName::NewLC(aBinaryData, pos);
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX500DistinguishedName* self = CX500DistinguishedName::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX500DistinguishedName* self = new(ELeave) CX500DistinguishedName;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}


EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewL(RReadStream& aStream)
	{
	CX500DistinguishedName* self = CX500DistinguishedName::NewLC(aStream);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX500DistinguishedName* CX500DistinguishedName::NewLC(RReadStream& aStream)
	{
	CX500DistinguishedName* self = new(ELeave) CX500DistinguishedName;
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

CX500DistinguishedName::CX500DistinguishedName()
	{
	}

void CX500DistinguishedName::ConstructL(const CArrayPtr<CX520AttributeTypeAndValue>& aElements)
	{
	iElements = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>(1);
	TInt count = aElements.Count();
	for (TInt i=0;i < count; i++)
		{
		CX520AttributeTypeAndValue* ava = CX520AttributeTypeAndValue::NewLC(*(aElements[i]));
		iElements->AppendL(ava);
		CleanupStack::Pop();//ava
		}
	}

void CX500DistinguishedName::ConstructL(RReadStream& aStream)
	{
	iElements = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue>(1);
	InternalizeL(aStream);
	}

void CX500DistinguishedName::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric dec(aBinaryData.Right(aBinaryData.Length() - aPos));
	dec.InitL();
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();

	iElements = new(ELeave) CArrayPtrFlat<CX520AttributeTypeAndValue> (1);
	if (dec.Tag() != EASN1Sequence)
		{
		User::Leave(KErrArgument);
		}
	while (aPos < end)
		{
		TASN1DecGeneric rdn(aBinaryData.Right(aBinaryData.Length() - aPos));
		rdn.InitL();
		if (rdn.Tag() != EASN1Set)
			{
			User::Leave(KErrArgument);
			}
		TInt rdnEnd = rdn.LengthDER();
		TInt rdnPos = rdn.LengthDERHeader();//add on header
		while (rdnPos < rdnEnd)
			{
			CX520AttributeTypeAndValue* ava = CX520AttributeTypeAndValue::NewLC(rdn.Encoding(), rdnPos);
			iElements->AppendL(ava);
			CleanupStack::Pop();//ava
			}
		aPos += rdnEnd;
		}
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}


EXPORT_C CX500DistinguishedName::~CX500DistinguishedName()
	{
	if (iElements != NULL)
		{
		iElements->ResetAndDestroy();
		delete iElements;
		}
	}

EXPORT_C TBool CX500DistinguishedName::ExactMatchL(const CX500DistinguishedName& aName) const
	{
	if (iElements->Count()!=aName.Count())
		return EFalse;
	return IsWithinSubtreeL(aName);
	}

EXPORT_C HBufC* CX500DistinguishedName::ExtractFieldL(const TDesC& aFieldName) const
	{
	TInt count = Count();
	for (TInt i = 0; i < count; i++)
		{
		const CX520AttributeTypeAndValue& ava = Element(i);
		if (ava.Type() == aFieldName)
			{
			return ava.ValueL();
			}
		}
	return NULL;
	}

EXPORT_C HBufC* CX500DistinguishedName::DisplayNameL() const
	{
	HBufC* res = ExtractFieldL(KX520CommonName);
	if (res == NULL)
		{
		res = ExtractFieldL(KX520OrganizationName);
		if (res == NULL)
			{
			res = HBufC::NewL(0);
			*res = KNullDesC;
			}
		}
	return res;		
	}

EXPORT_C TBool CX500DistinguishedName::IsWithinSubtreeL(const CX500DistinguishedName& aName) const
	{
	TInt elementsCount = aName.Count();
	for (TInt i = 0; i < elementsCount; ++i)
		{
		if (!MatchElementL(*(aName.iElements->At(i))))
			return EFalse;	
		}
	return ETrue;
	}

EXPORT_C TBool CX500DistinguishedName::MatchElementL(const CX520AttributeTypeAndValue& aElement) const
	{
	TInt count = iElements->Count();
	for (TInt i = 0; i < count; i++)
		{
		if (aElement.ExactMatchL(*iElements->At(i)))
			return ETrue;
		}
	return EFalse;
	}

EXPORT_C TInt CX500DistinguishedName::Count() const
	{
	return iElements->Count();
	}

EXPORT_C const CX520AttributeTypeAndValue& CX500DistinguishedName::Element(TInt aIndex) const
	{
	return *(iElements->At(aIndex));
	}

EXPORT_C void CX500DistinguishedName::ExternalizeL(RWriteStream& aStream) const
	{
	TInt count = iElements->Count();
	aStream.WriteInt32L(count);
	for (TInt i = 0; i < count; i++)
		{
		CX520AttributeTypeAndValue* ava = iElements->At(i);
		ava->ExternalizeL(aStream);
		}
	}

void CX500DistinguishedName::InternalizeL(RReadStream& aStream)
	{
	TInt count = aStream.ReadInt32L();
	for (TInt i = 0; i < count; i++)
		{
		CX520AttributeTypeAndValue* ava = CX520AttributeTypeAndValue::NewLC(aStream);
		iElements->AppendL(ava);
		CleanupStack::Pop();
		}
	}

EXPORT_C CASN1EncSequence* CX500DistinguishedName::EncodeASN1LC() const
	{
	CASN1EncSequence* nameSeq = CASN1EncSequence::NewLC();
	TInt numEntries = Count();
	for(TInt i = 0; i < numEntries; i++)
		{
		// Each attribute pair has to be enclosed in a SET-OF 
		// and SEQUENCE-OF
		CASN1EncSequence* set = CASN1EncSequence::NewLC();
		// This conversion of a sequence into a set presumably 
		// works if the set is going to contain just one child 
		// item, because otherwise order of child items in a 
		// SET-OF becomes important.
		set->SetTag(EASN1Set, EUniversal);
		CASN1EncSequence* attr = Element(i).EncodeASN1LC();
		set->AddAndPopChildL(attr);
		nameSeq->AddAndPopChildL(set);
		}
	return nameSeq;
	}

EXPORT_C CASN1EncSequence* CX500DistinguishedName::EncodeASN1L() const
	{
	CASN1EncSequence* nameSeq = EncodeASN1LC();
	CleanupStack::Pop(nameSeq);
	return nameSeq;
	}
