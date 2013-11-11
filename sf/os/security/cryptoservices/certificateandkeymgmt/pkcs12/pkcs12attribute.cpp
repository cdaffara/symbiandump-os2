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


#include "pkcs12attribute.h"

using namespace PKCS12;

CDecPkcs12Attribute::CDecPkcs12Attribute()
	{
	}
	
EXPORT_C CDecPkcs12Attribute* CDecPkcs12Attribute::NewL(const TDesC8& aBagAttributes)
	{
	CDecPkcs12Attribute* self = new (ELeave) CDecPkcs12Attribute;
    CleanupStack::PushL(self);
	self->ConstructL(aBagAttributes);
	CleanupStack::Pop(self);
	return self;	
	}

CDecPkcs12Attribute::~CDecPkcs12Attribute()
	{
	delete iAttributeId;	
	iAttributeValue.ResetAndDestroy();
	iAttributeValue.Close();
	}
	
void CDecPkcs12Attribute::ConstructL(const TDesC8& aBagAttributes)
	{
	TASN1DecGeneric seqGen(aBagAttributes);
	seqGen.InitL();
	
	// Check if this is a Sequence
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence seq;
	CArrayPtrFlat<TASN1DecGeneric>* attributeSet = seq.DecodeDERLC(seqGen);
	const TASN1DecGeneric* attributeSetAt0 = attributeSet->At(0);
	if(attributeSetAt0->Tag() != EASN1ObjectIdentifier || attributeSetAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
		
	// Decode the ObjectIdentifier
	TASN1DecObjectIdentifier oid;
	iAttributeId = oid.DecodeDERL(*attributeSetAt0); 
	 
	const TASN1DecGeneric* attributeSetAt1 = attributeSet->At(1);
	if(attributeSetAt1->Tag() != EASN1Set || attributeSetAt1->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
			
	// Attribute Set
	TASN1DecSet decSet;
    CArrayPtrFlat<TASN1DecGeneric>* attributeValues = decSet.NewDERLC(attributeSetAt1->Encoding());
    
    TInt attributeCount = attributeValues->Count();
		    
    for(TInt index = 0; index < attributeCount; ++index)         
	   	{
	   	const TASN1DecGeneric* attributeValuesAt = attributeValues->At(index);
		TASN1DecGeneric seqGen(*attributeValuesAt);
		seqGen.InitL();
		TPtrC8 attrValue = seqGen.Encoding();
		TDesC8* attributeVal = attrValue.AllocL();
		CleanupStack::PushL(attributeVal);
		iAttributeValue.AppendL(attributeVal);
		CleanupStack::Pop(attributeVal);
	   	}
    CleanupStack::PopAndDestroy(2,attributeSet); // attributeSet,attributeValues    
	}	
		
	
EXPORT_C const TDesC& CDecPkcs12Attribute::AttributeId() const
	{
	return *iAttributeId;	
	}
	
EXPORT_C const RPointerArray<TDesC8>& CDecPkcs12Attribute::AttributeValue() const
	{
	return iAttributeValue;	
	}

