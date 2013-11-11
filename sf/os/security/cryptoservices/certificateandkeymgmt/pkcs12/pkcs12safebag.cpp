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


#include "pkcs12safebag.h"
#include <pkcs12bags.h>

using namespace PKCS12;

CDecPkcs12SafeBag::CDecPkcs12SafeBag()
	{
	}

EXPORT_C CDecPkcs12SafeBag* CDecPkcs12SafeBag::NewL(const TDesC8& aSafeBagData)
	{
	TASN1DecGeneric seqGen(aSafeBagData);
	seqGen.InitL();
	
	// Check if this is a Sequence
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence seq;
	CArrayPtrFlat<TASN1DecGeneric>* safeBagSequence = seq.DecodeDERLC(seqGen);

	// Check for BagId, BagId is an ObjectIdentifier
	const TASN1DecGeneric* safeBagSequenceAt0 = safeBagSequence->At(0);
	if (safeBagSequenceAt0->Tag() != EASN1ObjectIdentifier || safeBagSequenceAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	TASN1DecObjectIdentifier oid;
  	HBufC* bagId = oid.DecodeDERL(*safeBagSequenceAt0);
	CleanupStack::PushL(bagId);
	CDecPkcs12SafeBag* safeBag = NULL;
	
	// If BagType is a KeyBag.	
	if( *bagId == KPkcs12KeyBagOID )
		{
		safeBag = CDecPkcs12KeyBag::NewL(aSafeBagData);
		}
	// If BagType is a Shrouded KeyBag.
	else if( *bagId == KPkcs12ShroudedKeyBagOID )
		{
		safeBag = CDecPkcs12ShroudedKeyBag::NewL(aSafeBagData);
		}
	// If bagType is a CertBag.
	else if( *bagId == KPkcs12CertBagOID )
		{
		safeBag = CDecPkcs12CertBag::NewL(aSafeBagData);
		}
	// If Bag Type is a SafeContentsBag.
	else if( *bagId == KPkcs12SafeContentsBagOID )
		{
		safeBag = CDecPkcs12SafeContentsBag::NewL(aSafeBagData);
		}
	// If Bag Type is a CrlBag.
	else if ( *bagId == KPkcs12CrlBagOID )
		{
		safeBag = new(ELeave) CDecPkcs12SafeBag();
		CleanupStack::PushL(safeBag);
		safeBag->ConstructL(aSafeBagData);
		CleanupStack::Pop(safeBag);
		}
	// If Bag Type is a Secret Bag.
	else if ( *bagId == KPkcs12SecretBagOID )
		{
		safeBag = new(ELeave) CDecPkcs12SafeBag();
		CleanupStack::PushL(safeBag);
		safeBag->ConstructL(aSafeBagData);
		CleanupStack::Pop(safeBag);
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	CleanupStack::PopAndDestroy(2,safeBagSequence); // safeBagSequence, bagId.
	return safeBag;
	}

EXPORT_C CDecPkcs12SafeBag::~CDecPkcs12SafeBag()
	{
	iBagAttributes.ResetAndDestroy();
	iBagAttributes.Close();
	}
	
void CDecPkcs12SafeBag::ConstructL(const TDesC8& aSafeBagData)
	{
	// This is SafeBag Sequence
	TASN1DecGeneric seqGen(aSafeBagData);
	seqGen.InitL();
	
	// Check if this is a Sequence
	if (seqGen.Tag() != EASN1Sequence || seqGen.Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
	
	TASN1DecSequence seq;
	CArrayPtrFlat<TASN1DecGeneric>* safeBagSequence = seq.DecodeDERLC(seqGen);
	const TASN1DecGeneric* safeBagSequenceAt0 = safeBagSequence->At(0);
	// Obtain the BagId from the SafeBag Sequence
	 if (safeBagSequenceAt0->Tag() != EASN1ObjectIdentifier || safeBagSequenceAt0->Class() != EUniversal)
		{
		User::Leave(KErrArgument);
		}
		
	TASN1DecObjectIdentifier oid;
	HBufC* bagId = oid.DecodeDERL(*(safeBagSequence->At(0)));

	// If BagType is a KeyBag	
	if(*bagId == KPkcs12KeyBagOID)
		{
		iBagId = EKeyBag;
		}
	// If BagType is a Shrouded KeyBag	
	else if( *bagId == KPkcs12ShroudedKeyBagOID )
		{
		iBagId = EShroudedKeyBag;
		}
	// If bagType is a CertBag
	else if( *bagId == KPkcs12CertBagOID )
		{
		iBagId = ECertBag;
		}
	else if ( *bagId == KPkcs12CrlBagOID )
		{
		iBagId = ECrlBag;
		}
	else if ( *bagId == KPkcs12SecretBagOID )
		{
		iBagId = ESecretBag;
		}
	// If Bag Type is a SafeContentsBag
	else if( *bagId == KPkcs12SafeContentsBagOID )
		{
		iBagId = ESafeContentsBag;
		}
	else
		{
		User::Leave(KErrNotSupported);	
		}	
	delete bagId;	 
	// Obtain the BagValue from the SafeBag Sequence. The SafeBag contains a KeyBag 
	// or a ShroudedKeyBag.
	const TASN1DecGeneric* safeBagSequenceAt1 = safeBagSequence->At(1);
	if (safeBagSequenceAt1->Tag() == EASN1EOC || safeBagSequenceAt1->Class() == EContextSpecific)
		{
		TASN1DecGeneric seqGen(safeBagSequence->At(1)->GetContentDER());
		seqGen.InitL();
		
		iBagValue.Set(seqGen.Encoding());
		}
   	else
		{
		User::Leave(KErrArgument);
		}
	// Obtain the BagAttribute from the SafeBag Sequence. ATTRIBUTES ARE OPTIONAL
	if (safeBagSequence->Count() == 3)
		{
		TASN1DecGeneric seqGen(*(safeBagSequence->At(2)));
	    seqGen.InitL();
	    
		// Check if this is a Set
		if (seqGen.Tag() != EASN1Set || seqGen.Class() != EUniversal)
			{
			User::Leave(KErrArgument);
			}   
		// Set
		TASN1DecSet decSet;
        CArrayPtrFlat<TASN1DecGeneric>* attributeSet = decSet.NewDERLC((safeBagSequence->At(2)->Encoding()));
                
        TInt attributeSetCount = attributeSet->Count();
        for ( TInt index = 0; index < attributeSetCount; index++ )
        	{
        	const TDesC8& attribute(attributeSet->At(index)->Encoding());
        	CDecPkcs12Attribute* bagAttribute = CDecPkcs12Attribute::NewL(attribute);
        	CleanupStack::PushL(bagAttribute);
         	iBagAttributes.AppendL(bagAttribute);
         	CleanupStack::Pop(bagAttribute);
         	}
        CleanupStack::PopAndDestroy(attributeSet); // attributeSet
        }
	CleanupStack::PopAndDestroy(safeBagSequence); // safeBagSequence
	}
	
EXPORT_C CDecPkcs12SafeBag::TBagId CDecPkcs12SafeBag::BagID() const
	{
	return iBagId;
	}
 
EXPORT_C const TDesC8& CDecPkcs12SafeBag::BagValue() const
	{
	return iBagValue;
	}

EXPORT_C const RPointerArray<CDecPkcs12Attribute>& CDecPkcs12SafeBag::BagAttributes() const
	{
	return iBagAttributes;
	}

