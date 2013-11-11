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


#include <asn1dec.h>
#include "x509constraintext.h"
#include "x509bitstring.h"

// Integer list methods
EXPORT_C CX509IntListExt* CX509IntListExt::NewL(const TDesC8& aBinaryData)
	{	
	CX509IntListExt* self = CX509IntListExt::NewLC(aBinaryData);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CX509IntListExt* CX509IntListExt::NewLC(const TDesC8& aBinaryData)
	{
	CX509IntListExt* self = new(ELeave) CX509IntListExt;
	CleanupStack::PushL(self);
	
	TInt pos = 0;
	self->ConstructL(aBinaryData, pos);
	return self;
	}

CX509IntListExt::~CX509IntListExt() 
	{
	iIntArray.Close();
	}
	
EXPORT_C const RArray<TInt>& CX509IntListExt::IntArray() const 
	{
	return iIntArray;	
	}		

void CX509IntListExt::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ExtensionBase::ConstructL(aBinaryData, aPos);
	}

void CX509IntListExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;	
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	TInt count = seq->Count();
	
	TASN1DecInteger encInt;
	for (TInt i = 0; i < count; i++) 
		{
		TASN1DecGeneric* curr = seq->At(i);
		
		if (curr->Tag() == EASN1Integer)		
			{
			User::LeaveIfError(iIntArray.Append(encInt.DecodeDERShortL(*curr)));
			}		
		else
			{
			User::Leave(KErrArgument);		
			}
		}		
	CleanupStack::PopAndDestroy(seq);
	}
	
//CX509IntListExt::CX509IntListExt() 
//	{
//	}
	
// UTF-8 String list methods	
EXPORT_C CX509Utf8StringListExt* CX509Utf8StringListExt::NewL(const TDesC8& aBinaryData)
	{	
	CX509Utf8StringListExt* self = CX509Utf8StringListExt::NewLC(aBinaryData);
	CleanupStack::Pop(self);
	return self;
	}
	
EXPORT_C CX509Utf8StringListExt* CX509Utf8StringListExt::NewLC(const TDesC8& aBinaryData)
	{
	CX509Utf8StringListExt* self = new(ELeave) CX509Utf8StringListExt;
	CleanupStack::PushL(self);
	
	TInt pos = 0;
	self->ConstructL(aBinaryData, pos);
	return self;
	}

EXPORT_C const RPointerArray<HBufC>& CX509Utf8StringListExt::StringArray() const
	{
	return iStringArray;
	}	

void CX509Utf8StringListExt::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ExtensionBase::ConstructL(aBinaryData, aPos);
	}
	
void CX509Utf8StringListExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	TASN1DecSequence encSeq;	
	CArrayPtrFlat<TASN1DecGeneric>* seq = encSeq.DecodeDERLC(aBinaryData, aPos);
	TInt count = seq->Count();
	
	TASN1DecUTF8String encStr;
	for (TInt i = 0; i < count; i++) 
		{
		TASN1DecGeneric* curr = seq->At(i);
		if (curr->Tag() == EASN1UTF8String)		
			{
			HBufC *str = encStr.DecodeDERL(*curr);
			CleanupStack::PushL(str);
			User::LeaveIfError(iStringArray.Append(str));
			CleanupStack::Pop(str);
			}		
		else
			{
			User::Leave(KErrArgument);		
			}
		}		
	CleanupStack::PopAndDestroy(seq);
	}

CX509Utf8StringListExt::~CX509Utf8StringListExt() 
	{
	iStringArray.ResetAndDestroy();
	}
	
// Capability set methods
EXPORT_C CX509CapabilitySetExt* CX509CapabilitySetExt::NewL(const TDesC8& aBinaryData) 
	{
	CX509CapabilitySetExt* self = CX509CapabilitySetExt::NewLC(aBinaryData);
	CleanupStack::Pop(self);
	return self;
	}

EXPORT_C CX509CapabilitySetExt* CX509CapabilitySetExt::NewLC(const TDesC8& aBinaryData) 
	{
	CX509CapabilitySetExt* self = new (ELeave) CX509CapabilitySetExt;
	CleanupStack::PushL(self);
	
	TInt pos = 0;
	self->ConstructL(aBinaryData, pos);
	return self;
	}

CX509CapabilitySetExt::~CX509CapabilitySetExt() 
	{	
	}

EXPORT_C const TCapabilitySet& CX509CapabilitySetExt::CapabilitySet() const
	{
	return iCapabilitySet;
	}

void CX509CapabilitySetExt::ConstructL(const TDesC8& aBinaryData, TInt& aPos)
	{
	CX509ExtensionBase::ConstructL(aBinaryData, aPos);
	}
			
void CX509CapabilitySetExt::DoConstructL(const TDesC8& aBinaryData, TInt& aPos) 
	{
	TASN1DecBitString encBit;	
	
	// Decode the padding bits
	HBufC8* bitBuffer = encBit.ExtractOctetStringL(aBinaryData, aPos);
	CleanupStack::PushL(bitBuffer);
	
	// Create bitString for easy access of individual bits.
	// This transfers ownership of bitBuffer
	CX509BitString* bitString = new (ELeave) CX509BitString(bitBuffer, bitBuffer->Length() * 8);	
	CleanupStack::Pop(bitBuffer);
	
	// Start off with an empty capability set and attempt to add each capability in turn
	// making sure we don't go past the limit of the supported capabilities.
	iCapabilitySet.SetEmpty();
	for (TInt i = 0; i < ECapability_Limit; i++) 
		{
		if (bitString->IsSet(i))		
			{
			iCapabilitySet.AddCapability(static_cast<TCapability>(i));		
			}
		}	
	delete bitString;
	}	
