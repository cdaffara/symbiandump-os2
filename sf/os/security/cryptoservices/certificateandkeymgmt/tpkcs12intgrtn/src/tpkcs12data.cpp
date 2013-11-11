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
* This file contains the implementation of the CPfxHeader,CSafeBagAttribute,
* CSafeContentBag,CSafeBagData.
*
*/


/**
 @file 
 @internalTechnology
*/
//User Include 
#include "tpkcs12data.h"

// Implementation of CPfxHeader

/**
Description:constructor
@internalTechnology:
@test
*/
CPfxHeader::CPfxHeader()
	{
	}
/**
Description:Function is intended to create the  CPfxHeader object
@internalTechnology:
@test
*/
 CPfxHeader* CPfxHeader::NewL(const CDecPkcs12& aDecPkcs12 , TInt aError)
	{
	CPfxHeader* self = NewLC(aDecPkcs12 , aError);
	CleanupStack::Pop(self);
	return self;
	}
/**
Description:Function is intended to pcreate the  CPfxHeader object and push it to cleanupstack
@internalTechnology:
@test
*/
 CPfxHeader* CPfxHeader::NewLC(const CDecPkcs12& aDecPkcs12 , TInt aError)
	{
	CPfxHeader* self = new (ELeave) CPfxHeader();
	CleanupStack::PushL(self);
	self->ConstructL(aDecPkcs12 , aError);
	return self;
	}

/**
Description:Function is intended to construct the CPfxHeader object
@param-CDecPkcs12&: reference to the CDecPkcs12 object
@param-aError: Error to be set in case, the CDecPkcs12 object creation fails
@internalTechnology:
@test
*/	
void CPfxHeader::ConstructL(const CDecPkcs12& aDecPkcs12 , TInt aError)
	{	
	if(aError != KErrNone)
		{
		iPkcs12ActualError = aError;
		return;
		}
	//get the version
	iVersion = 	aDecPkcs12.Version();
	// put the integrity mode in the Header
	iIntegrityMode =  aDecPkcs12.IntegrityMode();
	
	// Get the macData 
	const CDecPkcs12MacData* macData = aDecPkcs12.MacData();
	
	// set the macData member in the Header

	iIsMacDataPresent = ( macData != NULL) ;
	
	if (macData  && iIntegrityMode ==  CDecPkcs12::EPasswordIntegrityMode)
		{
		// MacL method returns the DigestInfo structure.
		const CPKCS7DigestInfo& digestInfo = macData->DigestInfo();		
		iMac = digestInfo.Digest().AllocL();
		// MacSaltL method returns the macSalt
		iMacSalt = macData->MacSalt().AllocL();
		// IterationCountL method returns the iteration Count.
		iIterationCount= macData->IterationCount();
		}
	}
/**
Description:destructor
@internalTechnology:
@test
*/
CPfxHeader::~CPfxHeader()
	{
	delete iMac;
	delete iMacSalt;		
	}
		
// Implementation of CSafeBagAttribute
/**
Description:constructor
@internalTechnology:
@test
*/
CSafeBagAttribute::CSafeBagAttribute()
	{
	}
/**
Description:destructor
@internalTechnology:
@test
*/
CSafeBagAttribute::~CSafeBagAttribute()
	{
	delete iAttrId;
	}
/**
Description:Function is intended create CSafeBagAttribute object
@param-aAttribute: constant reference to the CDecPkcs12Attribute
@internalTechnology:
@test
*/
 CSafeBagAttribute* CSafeBagAttribute::NewL(const CDecPkcs12Attribute& aAttribute)
	{
	CSafeBagAttribute* self = NewLC(aAttribute);
	CleanupStack::Pop(self);
	return self;
	}
/**
Description:Function is intended to construct CSafeBagAttribute object, push it to cleanupstack
@param-aAttribute: constant reference to the CDecPkcs12Attribute
@internalTechnology:
@test
*/
 CSafeBagAttribute* CSafeBagAttribute::NewLC(const CDecPkcs12Attribute& aAttribute)
	{
	CSafeBagAttribute* self = new (ELeave) CSafeBagAttribute();
	CleanupStack::PushL(self);
	self->ConstructL(aAttribute);
	return self;
	}
/**
Description:Function is intended to construct the CSafeBagAttribute object
@param-CDecPkcs12Attribute: pointer to the CDecPkcs12ShroudedKeyBag
@internalTechnology:
@test
*/
void CSafeBagAttribute::ConstructL(const CDecPkcs12Attribute& aAttribute)	
	{
	iAttrId = aAttribute.AttributeId().AllocL();
	iAttrValCount = aAttribute.AttributeValue().Count();
	}
	
//Implementation of CSafeContentBag

/**
Description:Function is intended to create the  CSafeContentBag
@internalTechnology:
@test
*/
 CSafeContentBag* CSafeContentBag::NewL()
	{
	CSafeContentBag* self = NewLC();
	CleanupStack::Pop(self);
	return self;	
	}
/**
Description:Function is intended to create the  CSafeContentBag, push it to cleanupstack
@internalTechnology:
@test
*/
 CSafeContentBag* CSafeContentBag::NewLC()
	{
	CSafeContentBag* self = new (ELeave) CSafeContentBag();
	CleanupStack::PushL(self);
	return self;	
	}
/**
Description:constructor
@internalTechnology:
@test
*/
CSafeContentBag::CSafeContentBag()
	{	
	}
	
// Implementation of CSafeBagData

/**
Description:destructor
@internalTechnology:
@test
*/
CSafeBagData::~CSafeBagData()
	{
	delete iBagValue;
	delete iAlgorithmID;
	delete iCertificateID;	
	iAttributeIDs.ResetAndDestroy();
	iAttributeValues.ResetAndDestroy();
	}	
/**
Description:Function is intended to create  the  CSafeBagData object
@internalTechnology:
@test
*/
 CSafeBagData* CSafeBagData::NewL()
	{
	CSafeBagData* self = NewLC();
	CleanupStack::Pop(self);
	return self;	
	}
/**
Description:Function is intended to create  the  CSafeBagData object, push it to cleanupstack
@internalTechnology:
@test
*/
 CSafeBagData* CSafeBagData::NewLC()
	{
	CSafeBagData* self = new (ELeave) CSafeBagData;
	CleanupStack::PushL(self);
	return self;	
	}
/**
Description:constructor
@internalTechnology:
@test
*/		
CSafeBagData::CSafeBagData()
	{		
	}
