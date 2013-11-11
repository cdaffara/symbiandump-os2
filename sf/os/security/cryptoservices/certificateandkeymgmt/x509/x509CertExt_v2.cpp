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
* X509CERTEXT.CPP
*
*/


#include <s32strm.h>
#include <x509certext.h>
#include <asn1dec.h>
#include "x509bitstring.h"

void CX509ExtensionBase::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric dec(aBinaryData.Right(aBinaryData.Length() - aPos));
	dec.InitL();
	if (dec.Tag() != EASN1OctetString)
		{
		User::Leave(KErrArgument);
		}
	TInt end = aPos + dec.LengthDER();
	aPos += dec.LengthDERHeader();//add on header for octet string here
//pass in binary data with aPos set to start of contents octets of octet string
//we cheat a little here; since we know an octet string is just the contents octets, 
//we just pass in a reference to the contents octets, and save alloc'ing the whole thang
	DoConstructL(aBinaryData, aPos);	
	if (aPos != end)
		{
		User::Leave(KErrArgument);
		}
	}

//1) basic constraints...
EXPORT_C CX509BasicConstraintsExt* CX509BasicConstraintsExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509BasicConstraintsExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509BasicConstraintsExt* CX509BasicConstraintsExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509BasicConstraintsExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509BasicConstraintsExt* CX509BasicConstraintsExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509BasicConstraintsExt* self = CX509BasicConstraintsExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509BasicConstraintsExt* CX509BasicConstraintsExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509BasicConstraintsExt* self = new(ELeave) CX509BasicConstraintsExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509BasicConstraintsExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
//sequence of 2 optional components, a bool and an int
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	TInt count = seq->Count();
	TInt pos = 0;
	if (pos < count)
		{
		TBool doneInt = EFalse;
		TASN1DecGeneric* curr = seq->At(pos);
		pos++;
		if (curr->Tag() == EASN1Boolean)
			{
			TASN1DecBoolean encBool;
			iIsCA = encBool.DecodeDERL(*curr);
			}
		else
			{
			TASN1DecInteger encInt;
			iMaxChainLen = encInt.DecodeDERShortL(*curr);
			doneInt = ETrue;
			}
		if (pos < count)
			{
			if (doneInt)
				{
				User::Leave(KErrArgument);
				}
			curr = seq->At(pos);
			pos++;
			TASN1DecInteger encInt;
			iMaxChainLen = encInt.DecodeDERShortL(*curr);
			if (iMaxChainLen < 0)
				{
				User::Leave(KErrArgument);
				}
			}
		}
	if (pos != count)
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy();//seq
	}

CX509BasicConstraintsExt::CX509BasicConstraintsExt()
	:iIsCA(EFalse), iMaxChainLen(KMaxTInt)
	{
	}

CX509BasicConstraintsExt::~CX509BasicConstraintsExt()
	{
	}

EXPORT_C TBool CX509BasicConstraintsExt::IsCA() const
	{
	return iIsCA;
	}

EXPORT_C TInt CX509BasicConstraintsExt::MaxChainLength() const
	{
	return iMaxChainLen;
	}
	
//2) alt name
//#pragma message ("creating empty CX509AltNameExt and destroying it kills process")
EXPORT_C CX509AltNameExt* CX509AltNameExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509AltNameExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509AltNameExt* CX509AltNameExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509AltNameExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509AltNameExt* CX509AltNameExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AltNameExt* self = CX509AltNameExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509AltNameExt* CX509AltNameExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AltNameExt* self = new(ELeave) CX509AltNameExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509AltNameExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	// The sequence should have at least 1 name here, previously this was checked
	// but now zero lengths sequences are tolerated.
	//
	// RFC 3280 requires that CAs ensure that the SubjectAltName is not empty if it exists. The 
	// behaviour of the client is undefined if this condition occurs. Since this code will 
	// normally be used as a client (i.e. not the CA) and there should be no need to validate 
	// the SubjectAltName we do not enfore a minimum sequence length.
	// This avoids TLS connections being dropped unecessarily.

	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 0, KMaxTInt);
	TInt count = seq->Count();
	iAuthorityName = new(ELeave) CArrayPtrFlat<CX509GeneralName> (1);
	TASN1DecGeneric* gen;
	for (TInt i = 0; i < count; i++)
		{
		gen = seq->At(i);
		CX509GeneralName* gn = CX509GeneralName::NewLC(gen->Encoding());
		iAuthorityName->AppendL(gn);
		CleanupStack::Pop();//gn
		}
	CleanupStack::PopAndDestroy();
	}

CX509AltNameExt::~CX509AltNameExt()
	{
	if (iAuthorityName != NULL)
		{
		iAuthorityName->ResetAndDestroy();
		delete iAuthorityName;
		}
	}

EXPORT_C const CArrayPtrFlat<CX509GeneralName>& CX509AltNameExt::AltName() const
	{
	return *iAuthorityName;
	}

EXPORT_C TBool CX509AltNameExt::Match(const CX509AltNameExt& aExt) const
	{
	TBool res = EFalse;
	const CArrayPtrFlat<CX509GeneralName>& otherGNs = aExt.AltName();
	TInt otherGNCount = otherGNs.Count();
	TInt thisGNCount = iAuthorityName->Count();
	if (otherGNCount != thisGNCount)
		{
		}
	else
		{
		res = ETrue;
		for (TInt j = 0; j < otherGNCount; j++)
				{
				const CX509GeneralName* otherGN = otherGNs.At(j);
				const CX509GeneralName* thisGN = iAuthorityName->At(j);
				if (!thisGN->ExactMatch(*otherGN))
					{
					res = EFalse;
					}
				}
		}
	return res;
	}

CX509AltNameExt::CX509AltNameExt()
	{
	}

//3) key usage
CX509BitString::~CX509BitString()
	{
	delete iData;
	}

TBool CX509BitString::IsSet(TInt aBit) const
	{
	if (aBit < iLength)//offset from zero
		{
		TPtrC8 d(iData->Des());
		TUint8 oct = d[(aBit/8)];
		TUint mask = (1 << (7-(aBit % 8)));
		return (oct & mask);
		}
	return EFalse;
	}

CX509BitString::CX509BitString(HBufC8* aData, TInt aLength)
	:iData(aData), iLength(aLength)
	{
	}

EXPORT_C CX509KeyUsageExt* CX509KeyUsageExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509KeyUsageExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509KeyUsageExt* CX509KeyUsageExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509KeyUsageExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509KeyUsageExt* CX509KeyUsageExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509KeyUsageExt* self = CX509KeyUsageExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509KeyUsageExt* CX509KeyUsageExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509KeyUsageExt* self = new(ELeave) CX509KeyUsageExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509KeyUsageExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecGeneric gen(aBinaryData.Right(aBinaryData.Length() - aPos));
	gen.InitL();
	if (gen.Tag() != EASN1BitString)
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 p(gen.GetContentDER());
	if (p.Length() < 2)
		{
		User::Leave(KErrArgument);
		}
	TPtrC8 pData(p.Right(p.Length() - 1));
	TInt paddingLength = p[0];
	TInt bitStringLength = (pData.Length() * 8) - paddingLength;
	HBufC8* bitString = pData.AllocL();
	CleanupStack::PushL(bitString);
	iData = new(ELeave) CX509BitString(bitString, bitStringLength);
	CleanupStack::Pop();//bitstring
	aPos += gen.LengthDER();
	}

CX509KeyUsageExt::CX509KeyUsageExt()
	{
	}

CX509KeyUsageExt::~CX509KeyUsageExt()
	{
	delete iData;
	}

EXPORT_C TBool CX509KeyUsageExt::IsSet(TX509KeyUsage aUsage) const
	{
	return iData->IsSet(aUsage);
	}

//4) name constraints
CX509GeneralSubtree* CX509GeneralSubtree::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509GeneralSubtree::NewL(aBinaryData, pos);
	}

CX509GeneralSubtree* CX509GeneralSubtree::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509GeneralSubtree::NewLC(aBinaryData, pos);
	}

CX509GeneralSubtree* CX509GeneralSubtree::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509GeneralSubtree* self = CX509GeneralSubtree::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

CX509GeneralSubtree* CX509GeneralSubtree::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509GeneralSubtree* self = new(ELeave) CX509GeneralSubtree;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509GeneralSubtree::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);//(seq here must have at least 1 name)
	TInt count = seq->Count();
	TInt pos = 0;
	TASN1DecGeneric* curr = seq->At(pos);
	pos++;
	iName = CX509GeneralName::NewL(curr->Encoding());
	if (pos < count)
		{
		TBool doneMax = EFalse;
		curr = seq->At(pos);
		pos++;
		if (curr->Class() != EContextSpecific)
				{
				User::Leave(KErrArgument);
				}
			switch(curr->Tag())
				{
				case 0:
					{
					TASN1DecInteger encInt;
					iMinDist = encInt.DecodeDERShortL(*curr);
					break;
					}
				case 1:
					{
					TASN1DecInteger encInt;
					iMaxDist = encInt.DecodeDERShortL(*curr);
					doneMax = ETrue;
					break;
					}
				}
			if (pos < count)
				{
				curr = seq->At(pos);
				if ((doneMax) || (curr->Class() != EContextSpecific) || (curr->Tag() != 1))
					{
					User::Leave(KErrArgument);
					}
				TASN1DecInteger encInt;
				iMaxDist = encInt.DecodeDERShortL(*curr);
				}
		}
	if (pos != count)
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy();
	}

CX509GeneralSubtree::~CX509GeneralSubtree()
	{
	delete iName;
	}

EXPORT_C const CX509GeneralName& CX509GeneralSubtree::Name() const
	{
	return *iName;
	}

EXPORT_C TInt CX509GeneralSubtree::MinDistance() const
	{
	return iMinDist;
	}

EXPORT_C TInt CX509GeneralSubtree::MaxDistance() const
	{
	return iMaxDist;
	}

CX509GeneralSubtree::CX509GeneralSubtree()
	:iMaxDist(KMaxTInt), iMinDist(0)
	{
	}

//
EXPORT_C CX509NameConstraintsExt* CX509NameConstraintsExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509NameConstraintsExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509NameConstraintsExt* CX509NameConstraintsExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509NameConstraintsExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509NameConstraintsExt* CX509NameConstraintsExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509NameConstraintsExt* self = CX509NameConstraintsExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509NameConstraintsExt* CX509NameConstraintsExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509NameConstraintsExt* self = new(ELeave) CX509NameConstraintsExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509NameConstraintsExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	TInt count = seq->Count();
	TInt pos = 0;

	iExcludedSubtrees = new(ELeave) CArrayPtrFlat<CX509GeneralSubtree> (1);
	iPermittedSubtrees = new(ELeave) CArrayPtrFlat<CX509GeneralSubtree> (1);
	TBool doneExcluded = EFalse;
	
	if (pos < count)
		{
		TASN1DecGeneric* curr = seq->At(pos);
		pos++;
		if (curr->Class() != EContextSpecific)
			{
			User::Leave(KErrArgument);
			}
		switch (curr->Tag())
			{
			case 0:
				{
				AddSubtreesL(*iPermittedSubtrees, curr->Encoding());
				break;
				}
			case 1:
				{
				AddSubtreesL(*iExcludedSubtrees, curr->Encoding());
				doneExcluded = ETrue;
				break;
				}
			default:
				{
				User::Leave(KErrArgument);
				}
			}
		if (pos < count)
			{
			curr = seq->At(pos);
			pos++;
			if ((curr->Class() != EContextSpecific) || (curr->Tag() != 1) || (doneExcluded))
				{
				User::Leave(KErrArgument);
				}
			AddSubtreesL(*iExcludedSubtrees, curr->Encoding());
			}
		}
	CleanupStack::PopAndDestroy();//seq
	}

void CX509NameConstraintsExt::AddSubtreesL(	CArrayPtrFlat<CX509GeneralSubtree>& aSubtrees,
											const TDesC8& aBinaryData)
	{
	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, pos, 1, KMaxTInt);
	TInt count = seq->Count();
	TASN1DecGeneric* gen;
	for(TInt i = 0; i < count; i++)
		{
		gen = seq->At(i);
		CX509GeneralSubtree* subtree = CX509GeneralSubtree::NewLC(gen->Encoding());
		aSubtrees.AppendL(subtree);
		CleanupStack::Pop();
		}
	CleanupStack::PopAndDestroy();
	}

EXPORT_C CX509NameConstraintsExt::~CX509NameConstraintsExt()
	{
	if (iExcludedSubtrees != NULL)
		{
		iExcludedSubtrees->ResetAndDestroy();
		delete iExcludedSubtrees;
		}
	if (iPermittedSubtrees != NULL)
		{
		iPermittedSubtrees->ResetAndDestroy();
		delete iPermittedSubtrees;
		}
	}

EXPORT_C const CArrayPtrFlat<CX509GeneralSubtree>& CX509NameConstraintsExt::ExcludedSubtrees() const
	{
	return *iExcludedSubtrees;
	}

EXPORT_C const CArrayPtrFlat<CX509GeneralSubtree>& CX509NameConstraintsExt::PermittedSubtrees() const
	{
	return *iPermittedSubtrees;
	}

CX509NameConstraintsExt::CX509NameConstraintsExt()
	{
	}

//5) policy constraints
TX509PolicyConstraint::TX509PolicyConstraint(TBool aRequired, TInt aCountdown)
	:iRequired(aRequired), iCountdown(aCountdown)
	{
	}

TX509PolicyConstraint::TX509PolicyConstraint()
	:iRequired(EFalse), iCountdown(0)
	{
	}

//
EXPORT_C CX509PolicyConstraintsExt* CX509PolicyConstraintsExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyConstraintsExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509PolicyConstraintsExt* CX509PolicyConstraintsExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyConstraintsExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509PolicyConstraintsExt* CX509PolicyConstraintsExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyConstraintsExt* self = CX509PolicyConstraintsExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509PolicyConstraintsExt* CX509PolicyConstraintsExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyConstraintsExt* self = new(ELeave) CX509PolicyConstraintsExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509PolicyConstraintsExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	TInt count = seq->Count();
	TInt pos = 0;
	TASN1DecGeneric* curr;
	if (pos < count)
		{
		curr = seq->At(pos);
		pos++;
		if (curr->Class() != EContextSpecific)
			{
			User::Leave(KErrArgument);
			}
		switch (curr->Tag())
			{
			case 0:
				{
				iRequirePolicy.iRequired = ETrue;
				TASN1DecInteger encInt;
				iRequirePolicy.iCountdown = encInt.DecodeDERShortL(*curr);
				break;
				}
			case 1:
				{
				iInhibitPolicyMapping.iRequired = ETrue;
				TASN1DecInteger encInt;
				iInhibitPolicyMapping.iCountdown = encInt.DecodeDERShortL(*curr);
				break;
				}
			default:
				{
				User::Leave(KErrArgument);
				}
			}
		if(pos < count)
			{
			curr = seq->At(pos);
			pos++;
			if ((iInhibitPolicyMapping.iRequired) || (curr->Class() != EContextSpecific) || (curr->Tag() != 1))
				{
				User::Leave(KErrArgument);
				}
			iInhibitPolicyMapping.iRequired = ETrue;
			TASN1DecInteger encInt;
			iInhibitPolicyMapping.iCountdown = encInt.DecodeDERShortL(*curr);
			}
		}
	if (pos != count)
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy();
	}

CX509PolicyConstraintsExt::CX509PolicyConstraintsExt()
	{
	}

EXPORT_C CX509PolicyConstraintsExt::~CX509PolicyConstraintsExt()
	{
	}

EXPORT_C TX509PolicyConstraint CX509PolicyConstraintsExt::ExplicitPolicyRequired() const
	{
	return iRequirePolicy;
	}

EXPORT_C TX509PolicyConstraint CX509PolicyConstraintsExt::InhibitPolicyMapping() const
	{
	return iInhibitPolicyMapping;
	}

//6) policies
CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyQualifierInfo::NewL(aBinaryData, pos);
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyQualifierInfo::NewLC(aBinaryData, pos);
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyQualifierInfo* self = CX509PolicyQualifierInfo::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyQualifierInfo* self = new(ELeave) CX509PolicyQualifierInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewL(const CX509PolicyQualifierInfo& aQualifier)
	{
	CX509PolicyQualifierInfo* self = CX509PolicyQualifierInfo::NewLC(aQualifier);
	CleanupStack::Pop();//self
	return self;
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewLC(const CX509PolicyQualifierInfo& aQualifier)
	{
	CX509PolicyQualifierInfo* self = new(ELeave) CX509PolicyQualifierInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aQualifier);
	return self;
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewL(RReadStream& aStream)
	{
	CX509PolicyQualifierInfo* self = CX509PolicyQualifierInfo::NewLC(aStream);
	CleanupStack::Pop();
	return self;
	}

CX509PolicyQualifierInfo* CX509PolicyQualifierInfo::NewLC(RReadStream& aStream)
	{
	CX509PolicyQualifierInfo* self = new(ELeave) CX509PolicyQualifierInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

void CX509PolicyQualifierInfo::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

void CX509PolicyQualifierInfo::ConstructL(const CX509PolicyQualifierInfo& aQualifier)
	{
	iPolicyQualifierId = aQualifier.iPolicyQualifierId->Des().AllocL();//must be a better way to do this!!
	iData = aQualifier.iData->Des().AllocL();//must be a better way to do this!!
	}

void CX509PolicyQualifierInfo::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();
	TInt pos = 0;
	TASN1DecGeneric* curr = seq->At(pos);
	pos++;
	TASN1DecObjectIdentifier encOID;
	iPolicyQualifierId = encOID.DecodeDERL(*curr);
	if (pos < count)
		{
		iData = curr->Encoding().AllocL();
		pos++;
		}
	else
		{
		iData = HBufC8::NewL(1);
		*iData = KNullDesC8;
		}
	if (pos != count)
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy();
	}

CX509PolicyQualifierInfo::CX509PolicyQualifierInfo()
	{
	}

CX509PolicyQualifierInfo::~CX509PolicyQualifierInfo()
	{
	delete iPolicyQualifierId;
	delete iData;
	}

EXPORT_C TPtrC CX509PolicyQualifierInfo::Id() const
	{
	return iPolicyQualifierId->Des();
	}

EXPORT_C TPtrC8 CX509PolicyQualifierInfo::Data() const
	{
	return iData->Des();
	}

void CX509PolicyQualifierInfo::ExternalizeL(RWriteStream& aStream) const
	{
	//iPolicyQualifierId
	aStream << *iPolicyQualifierId;

	//iData
	aStream << *iData;
	}

void CX509PolicyQualifierInfo::InternalizeL(RReadStream& aStream)
	{
	//iPolicyQualifierId
	delete iPolicyQualifierId;
	iPolicyQualifierId=0;
	iPolicyQualifierId=HBufC::NewL(aStream, KMaxTInt);

	//iData
	delete iData;
	iData=0;
	iData=HBufC8::NewL(aStream, KMaxTInt);
	}


CX509CertPolicyInfo* CX509CertPolicyInfo::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509CertPolicyInfo::NewL(aBinaryData, pos);
	}

CX509CertPolicyInfo* CX509CertPolicyInfo::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509CertPolicyInfo::NewLC(aBinaryData, pos);
	}

CX509CertPolicyInfo* CX509CertPolicyInfo::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509CertPolicyInfo* self = CX509CertPolicyInfo::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

CX509CertPolicyInfo* CX509CertPolicyInfo::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509CertPolicyInfo* self = new(ELeave) CX509CertPolicyInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}


EXPORT_C CX509CertPolicyInfo* CX509CertPolicyInfo::NewL(RReadStream& aStream)
	{
	CX509CertPolicyInfo* self = CX509CertPolicyInfo::NewLC(aStream);
	CleanupStack::Pop();
	return self;
	}
 
EXPORT_C CX509CertPolicyInfo* CX509CertPolicyInfo::NewLC(RReadStream& aStream)
	{
	CX509CertPolicyInfo* self = new(ELeave) CX509CertPolicyInfo();
	CleanupStack::PushL(self);
	self->ConstructL(aStream);
	return self;
	}

EXPORT_C CX509CertPolicyInfo* CX509CertPolicyInfo::NewL(const CX509CertPolicyInfo& aInfo)
	{
	CX509CertPolicyInfo* self = CX509CertPolicyInfo::NewLC(aInfo);
	CleanupStack::Pop();//self
	return self;
	}

EXPORT_C CX509CertPolicyInfo* CX509CertPolicyInfo::NewLC(const CX509CertPolicyInfo& aInfo)
	{
	CX509CertPolicyInfo* self = new(ELeave) CX509CertPolicyInfo;
	CleanupStack::PushL(self);
	self->ConstructL(aInfo);
	return self;
	}

void CX509CertPolicyInfo::ConstructL(RReadStream& aStream)
	{
	InternalizeL(aStream);
	}

void CX509CertPolicyInfo::ConstructL(const CX509CertPolicyInfo& aInfo)
	{
	iCertPolicyId = aInfo.iCertPolicyId->Des().AllocL();//must be a better way to do this!!
	iQualifiers = new(ELeave) CArrayPtrFlat<CX509PolicyQualifierInfo> (1);
	TInt count = aInfo.iQualifiers->Count();
	for (TInt i = 0; i < count; i++)
		{
		CX509PolicyQualifierInfo* q = CX509PolicyQualifierInfo::NewLC(*(aInfo.iQualifiers->At(i)));
		iQualifiers->AppendL(q);
		CleanupStack::Pop();//q
		}
	}

void CX509CertPolicyInfo::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();
	TInt pos = 0;
	TASN1DecGeneric* curr = seq->At(pos);
	pos++;

	TASN1DecObjectIdentifier encOID;
	iCertPolicyId = encOID.DecodeDERL(*curr);
	iQualifiers = new(ELeave) CArrayPtrFlat<CX509PolicyQualifierInfo> (1);
	if (pos < count)
		{
		curr = seq->At(pos);
		pos++;
		TASN1DecSequence encSeqQualifier;
		CArrayPtrFlat<TASN1DecGeneric>* seqQualifier = encSeqQualifier.DecodeDERLC(*curr);
		TInt qCount = seqQualifier->Count();
		for(TInt i = 0; i < qCount; i++)
			{
			TASN1DecGeneric* qGen = seqQualifier->At(i);
			CX509PolicyQualifierInfo* qualifier = CX509PolicyQualifierInfo::NewLC(qGen->Encoding());
			iQualifiers->AppendL(qualifier);
			CleanupStack::Pop();//qualifier
			}
		CleanupStack::PopAndDestroy();
		}
	if (pos != count)
		{
		User::Leave(KErrArgument);
		}
	CleanupStack::PopAndDestroy();
	}

CX509CertPolicyInfo::~CX509CertPolicyInfo()
	{
	if (iQualifiers != NULL)
		{
		iQualifiers->ResetAndDestroy();
		delete iQualifiers;
		}
	delete iCertPolicyId;
	}

EXPORT_C const CArrayPtrFlat<CX509PolicyQualifierInfo>& CX509CertPolicyInfo::Qualifiers() const
	{
	return *iQualifiers;
	}

EXPORT_C TPtrC CX509CertPolicyInfo::Id() const
	{
	return iCertPolicyId->Des();
	}

CX509CertPolicyInfo::CX509CertPolicyInfo()
	{
	}

EXPORT_C void CX509CertPolicyInfo::ExternalizeL(RWriteStream& aStream) const
	{
	//iCertPolicyId
 	aStream << *iCertPolicyId;

 	// iQualifiers
	aStream.WriteInt32L(iQualifiers->Count());
	for (TInt32 i=0;i < iQualifiers->Count(); ++i)
		{
		(*iQualifiers)[i]->ExternalizeL(aStream);
		}
	}

EXPORT_C void CX509CertPolicyInfo::InternalizeL(RReadStream& aStream)
	{
	//iCertPolicyId
	delete iCertPolicyId;
	iCertPolicyId=0;
	iCertPolicyId=HBufC::NewL(aStream, KMaxTInt);

	// iQualifiers
	if (iQualifiers != NULL)
		{
		iQualifiers->ResetAndDestroy();
		}
	else
		{
		iQualifiers = new(ELeave) CArrayPtrFlat<CX509PolicyQualifierInfo> (1);
		}

	TInt32 count=aStream.ReadInt32L();
	for (TInt32 i=0;i < count; ++i)
		{
		CX509PolicyQualifierInfo* policyQualifierInfo=CX509PolicyQualifierInfo::NewLC(aStream);
		iQualifiers->AppendL(policyQualifierInfo);
		CleanupStack::Pop(policyQualifierInfo);
		}
	}

EXPORT_C CX509CertPoliciesExt* CX509CertPoliciesExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509CertPoliciesExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509CertPoliciesExt* CX509CertPoliciesExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509CertPoliciesExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509CertPoliciesExt* CX509CertPoliciesExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509CertPoliciesExt* self = CX509CertPoliciesExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509CertPoliciesExt* CX509CertPoliciesExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509CertPoliciesExt* self = new(ELeave) CX509CertPoliciesExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509CertPoliciesExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();
	iPolicies = new(ELeave) CArrayPtrFlat<CX509CertPolicyInfo> (1);
	TASN1DecGeneric* gen;
	for (TInt i = 0; i < count; i++)
		{
		gen = seq->At(i);
		CX509CertPolicyInfo* policy = CX509CertPolicyInfo::NewLC(gen->Encoding());
		iPolicies->AppendL(policy);
		CleanupStack::Pop();//policy
		}
	CleanupStack::PopAndDestroy();
	}

CX509CertPoliciesExt::~CX509CertPoliciesExt()
	{
	if (iPolicies != NULL)
		{
		iPolicies->ResetAndDestroy();
		delete iPolicies;
		}
	}

EXPORT_C const CArrayPtrFlat<CX509CertPolicyInfo>& CX509CertPoliciesExt::Policies() const
	{
	return *iPolicies;
	}

CX509CertPoliciesExt::CX509CertPoliciesExt()
	{
	}

//7) policy mapping
CX509PolicyMapping* CX509PolicyMapping::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyMapping::NewL(aBinaryData, pos);
	}

CX509PolicyMapping* CX509PolicyMapping::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyMapping::NewLC(aBinaryData, pos);
	}

CX509PolicyMapping* CX509PolicyMapping::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyMapping* self = CX509PolicyMapping::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

CX509PolicyMapping* CX509PolicyMapping::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyMapping* self = new(ELeave) CX509PolicyMapping;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

EXPORT_C CX509PolicyMapping* CX509PolicyMapping::NewL(const CX509PolicyMapping& aMapping)
	{
	CX509PolicyMapping* self = CX509PolicyMapping::NewLC(aMapping);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509PolicyMapping* CX509PolicyMapping::NewLC(const CX509PolicyMapping& aMapping)
	{
	CX509PolicyMapping* self = new(ELeave) CX509PolicyMapping;
	CleanupStack::PushL(self);
	self->ConstructL(aMapping);
	return self;
	}

void CX509PolicyMapping::ConstructL(const CX509PolicyMapping& aMapping)
	{
	iIssuerPolicy = aMapping.iIssuerPolicy->AllocL();
	iSubjectPolicy = aMapping.iSubjectPolicy->AllocL();
	}

void CX509PolicyMapping::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	if (seq->Count() < 2)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecObjectIdentifier encOID;
	iIssuerPolicy = encOID.DecodeDERL(*(seq->At(0)));
	iSubjectPolicy = encOID.DecodeDERL(*(seq->At(1)));
	CleanupStack::PopAndDestroy();
	}

CX509PolicyMapping::~CX509PolicyMapping()
	{
	delete iIssuerPolicy;
	delete iSubjectPolicy;
	}

EXPORT_C TPtrC CX509PolicyMapping::IssuerPolicy() const
	{
	return iIssuerPolicy->Des();
	}

EXPORT_C TPtrC CX509PolicyMapping::SubjectPolicy() const
	{
	return iSubjectPolicy->Des();
	}

CX509PolicyMapping::CX509PolicyMapping()
	{
	}

EXPORT_C CX509PolicyMappingExt* CX509PolicyMappingExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyMappingExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509PolicyMappingExt* CX509PolicyMappingExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509PolicyMappingExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509PolicyMappingExt* CX509PolicyMappingExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyMappingExt* self = CX509PolicyMappingExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509PolicyMappingExt* CX509PolicyMappingExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509PolicyMappingExt* self = new(ELeave) CX509PolicyMappingExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509PolicyMappingExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();

	iPolicies = new(ELeave) CArrayPtrFlat<CX509PolicyMapping> (1);
	TASN1DecGeneric* gen;
	for (TInt i = 0; i < count; i++)
		{
		gen = seq->At(i);
		CX509PolicyMapping* policy = CX509PolicyMapping::NewLC(gen->Encoding());
		iPolicies->AppendL(policy);
		CleanupStack::Pop();//policy
		}
	CleanupStack::PopAndDestroy();
	}

CX509PolicyMappingExt::~CX509PolicyMappingExt()
	{
	if (iPolicies != NULL)
		{
		iPolicies->ResetAndDestroy();
		delete iPolicies;
		}
	}

EXPORT_C const CArrayPtrFlat<CX509PolicyMapping>& CX509PolicyMappingExt::Mappings() const
	{
	return *iPolicies;
	}

CX509PolicyMappingExt::CX509PolicyMappingExt()
	{
	}

//8) authority key ID
EXPORT_C CX509AuthorityKeyIdExt* CX509AuthorityKeyIdExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509AuthorityKeyIdExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509AuthorityKeyIdExt* CX509AuthorityKeyIdExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509AuthorityKeyIdExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509AuthorityKeyIdExt* CX509AuthorityKeyIdExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AuthorityKeyIdExt* self = CX509AuthorityKeyIdExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509AuthorityKeyIdExt* CX509AuthorityKeyIdExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AuthorityKeyIdExt* self = new(ELeave) CX509AuthorityKeyIdExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509AuthorityKeyIdExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	TInt count = seq->Count();
	TInt pos = 0;

	iAuthorityName = new(ELeave) CArrayPtrFlat<CX509GeneralName> (1);
	TASN1DecGeneric* curr;
	if (pos < count)
		{
		curr = seq->At(pos);
		pos++;
		if (curr->Class() != EContextSpecific)
			{
			User::Leave(KErrArgument);
			}
		switch(curr->Tag())
			{
			case 0:
				{
				DecodeKeyIdL(curr->Encoding());
				break;
				}
			case 1:
				{
				DecodeNameL(curr->Encoding());
				break;
				}
			case 2:
				{
				DecodeSerialNoL(curr->Encoding());
				break;
				}
			default:
				{
				User::Leave(KErrArgument);
				}
			}
		if (pos < count)
			{
			curr = seq->At(pos);
			pos++;
			if (curr->Class() != EContextSpecific)
				{
				User::Leave(KErrArgument);
				}
			switch(curr->Tag())
				{
				case 1:
					{
					DecodeNameL(curr->Encoding());
					break;
					}
				case 2:
					{
					DecodeSerialNoL(curr->Encoding());
					break;
					}
				default:
					{
					User::Leave(KErrArgument);
					}
				}
			if (pos < count)
				{
				curr = seq->At(pos);
				pos++;
				if ((curr->Class() != EContextSpecific) || (curr->Tag() != 2))
					{
					User::Leave(KErrArgument);
					}
				DecodeSerialNoL(curr->Encoding());
				}
			}
		}
	if (!iKeyIdentifier)
		{
		iKeyIdentifier = HBufC8::NewL(1);
		*iKeyIdentifier = KNullDesC8;
		}
	if (!iAuthorityCertSerialNumber)
		{
		iAuthorityCertSerialNumber = HBufC8::NewL(1);
		*iAuthorityCertSerialNumber = KNullDesC8;
		}
	CleanupStack::PopAndDestroy();
	}

void CX509AuthorityKeyIdExt::DecodeNameL(const TDesC8& aBinaryData)
	{
	TASN1DecSequence encSeq;
	TInt pos = 0;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, pos, 1, KMaxTInt);
	TInt count = seq->Count();
	TASN1DecGeneric* gen;
	for (TInt i = 0; i < count; i++)
		{
		gen = seq->At(i);
		CX509GeneralName* gn = CX509GeneralName::NewLC(gen->Encoding());
		iAuthorityName->AppendL(gn);
		CleanupStack::Pop();//gn
		}
	CleanupStack::PopAndDestroy();
	}

void CX509AuthorityKeyIdExt::DecodeKeyIdL(const TDesC8& aBinaryData)
	{
	TASN1DecGeneric gen(aBinaryData);
	gen.InitL();
	if (iKeyIdentifier != NULL)
		{
		User::Leave(KErrArgument);
		}
	iKeyIdentifier = gen.GetContentDER().AllocL();
	}

void CX509AuthorityKeyIdExt::DecodeSerialNoL(const TDesC8& aBinaryData)
	{
	TASN1DecGeneric gen(aBinaryData);
	gen.InitL();
	if (iAuthorityCertSerialNumber != NULL)
		{
		User::Leave(KErrArgument);
		}
	iAuthorityCertSerialNumber = gen.GetContentDER().AllocL();
	}

CX509AuthorityKeyIdExt::~CX509AuthorityKeyIdExt()
	{
	if (iAuthorityName != NULL)
		{
		iAuthorityName->ResetAndDestroy();
		}
	delete iAuthorityName;
	delete iAuthorityCertSerialNumber;
	delete iKeyIdentifier;
	}

EXPORT_C const CArrayPtrFlat<CX509GeneralName>& CX509AuthorityKeyIdExt::AuthorityName() const
	{
	return *iAuthorityName;
	}

EXPORT_C TPtrC8 CX509AuthorityKeyIdExt::AuthorityCertSerialNumber() const
	{
	return iAuthorityCertSerialNumber->Des();
	}

EXPORT_C TPtrC8 CX509AuthorityKeyIdExt::KeyId() const
	{
	return iKeyIdentifier->Des();
	}

CX509AuthorityKeyIdExt::CX509AuthorityKeyIdExt()
	{
	}

//9) subject key ID
EXPORT_C CX509SubjectKeyIdExt* CX509SubjectKeyIdExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509SubjectKeyIdExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509SubjectKeyIdExt* CX509SubjectKeyIdExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509SubjectKeyIdExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509SubjectKeyIdExt* CX509SubjectKeyIdExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509SubjectKeyIdExt* self = CX509SubjectKeyIdExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509SubjectKeyIdExt* CX509SubjectKeyIdExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509SubjectKeyIdExt* self = new(ELeave) CX509SubjectKeyIdExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509SubjectKeyIdExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecOctetString octetStr;
	iKeyIdentifier = octetStr.DecodeDERL(aBinaryData, aPos);
	}

EXPORT_C CX509SubjectKeyIdExt::~CX509SubjectKeyIdExt()
	{
	delete iKeyIdentifier;
	}

EXPORT_C TPtrC8 CX509SubjectKeyIdExt::KeyId() const
	{
	return iKeyIdentifier->Des();
	}

CX509SubjectKeyIdExt::CX509SubjectKeyIdExt()
	{
	}

//10) extended key usage
EXPORT_C CX509ExtendedKeyUsageExt* CX509ExtendedKeyUsageExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509ExtendedKeyUsageExt::NewL(aBinaryData, pos);
	}

EXPORT_C CX509ExtendedKeyUsageExt* CX509ExtendedKeyUsageExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return CX509ExtendedKeyUsageExt::NewLC(aBinaryData, pos);
	}

EXPORT_C CX509ExtendedKeyUsageExt* CX509ExtendedKeyUsageExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ExtendedKeyUsageExt* self = CX509ExtendedKeyUsageExt::NewLC(aBinaryData, aPos);
	CleanupStack::Pop();
	return self;
	}

EXPORT_C CX509ExtendedKeyUsageExt* CX509ExtendedKeyUsageExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ExtendedKeyUsageExt* self = new(ELeave) CX509ExtendedKeyUsageExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

void CX509ExtendedKeyUsageExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();

	iKeyUsages = new(ELeave) CArrayPtrFlat<HBufC> (1);
	TASN1DecGeneric* gen;
	for (TInt i = 0; i < count; i++)
		{
		gen = seq->At(i);
		TASN1DecObjectIdentifier encOID;
		HBufC* usage = encOID.DecodeDERL(*gen);
		CleanupStack::PushL(usage);
		iKeyUsages->AppendL(usage);
		CleanupStack::Pop();
		}
	CleanupStack::PopAndDestroy();
	}

EXPORT_C CX509ExtendedKeyUsageExt::~CX509ExtendedKeyUsageExt()
	{
	if (iKeyUsages != NULL)
		{
		iKeyUsages->ResetAndDestroy();
		delete iKeyUsages;
		}
	}

EXPORT_C const CArrayPtrFlat<HBufC>& CX509ExtendedKeyUsageExt::KeyUsages() const
	{
	return *iKeyUsages;
	}

CX509ExtendedKeyUsageExt::CX509ExtendedKeyUsageExt()
	{
	}

//12) authority information access - CX509AccessDescription

CX509AccessDescription* CX509AccessDescription::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return NewL(aBinaryData, pos);
	}

CX509AccessDescription* CX509AccessDescription::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return NewLC(aBinaryData, pos);
	}

CX509AccessDescription* CX509AccessDescription::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AccessDescription* self = NewLC(aBinaryData, aPos);
	CleanupStack::Pop(self);
	return self;
	}

CX509AccessDescription* CX509AccessDescription::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AccessDescription* self = new (ELeave) CX509AccessDescription;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CX509AccessDescription::CX509AccessDescription()
	{
	// empty
	}

void CX509AccessDescription::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 2, 2);

	TASN1DecGeneric* curr = seq->At(0);
	TASN1DecObjectIdentifier encOID;
	iMethodId = encOID.DecodeDERL(*curr);

	curr = seq->At(1);
	iLocation = CX509GeneralName::NewL(curr->Encoding());	

	CleanupStack::PopAndDestroy(seq);
	}

CX509AccessDescription::~CX509AccessDescription()
	{
	delete iMethodId;
	delete iLocation;
	}

EXPORT_C TPtrC CX509AccessDescription::Method() const
	{
	return *iMethodId;
	}

EXPORT_C const CX509GeneralName& CX509AccessDescription::Location() const
	{
	return *iLocation;
	}

//12) authority information access - CX509AuthInfoAccessExt

EXPORT_C CX509AuthInfoAccessExt* CX509AuthInfoAccessExt::NewL(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return NewL(aBinaryData, pos);
	}

EXPORT_C CX509AuthInfoAccessExt* CX509AuthInfoAccessExt::NewLC(const TDesC8& aBinaryData)
	{
	TInt pos = 0;
	return NewLC(aBinaryData, pos);
	}

EXPORT_C CX509AuthInfoAccessExt* CX509AuthInfoAccessExt::NewL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AuthInfoAccessExt* self = NewLC(aBinaryData, aPos);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CX509AuthInfoAccessExt* CX509AuthInfoAccessExt::NewLC(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509AuthInfoAccessExt* self = new (ELeave) CX509AuthInfoAccessExt;
	CleanupStack::PushL(self);
	self->ConstructL(aBinaryData, aPos);
	return self;
	}

CX509AuthInfoAccessExt::CX509AuthInfoAccessExt()
	{
	// empty
	}

void CX509AuthInfoAccessExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
 	{
	TASN1DecSequence encSeq;
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos, 1, KMaxTInt);
	TInt count = seq->Count();

	iAccessDescs = new (ELeave) CArrayPtrFlat<CX509AccessDescription>(1);

	for (TInt i = 0 ; i < count ; ++i)
		{
		TASN1DecGeneric* curr = seq->At(i);
		CX509AccessDescription* desc = CX509AccessDescription::NewLC(curr->Encoding());
		iAccessDescs->AppendL(desc);
		CleanupStack::Pop(desc);
		}

	CleanupStack::PopAndDestroy(seq);
	}

EXPORT_C CX509AuthInfoAccessExt::~CX509AuthInfoAccessExt()
	{
	if (iAccessDescs)
		{
		iAccessDescs->ResetAndDestroy();
		delete iAccessDescs;
		}
	}

EXPORT_C const CArrayPtrFlat<CX509AccessDescription>& CX509AuthInfoAccessExt::AccessDescriptions() const
	{
	return *iAccessDescs;
	}
