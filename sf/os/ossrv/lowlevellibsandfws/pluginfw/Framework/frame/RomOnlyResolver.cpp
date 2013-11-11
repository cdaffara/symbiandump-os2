// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Implements the CRomOnlyResolver class.
// 
//

/**
 @internalComponent
 @file
*/

#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>

#include "TestUtilities.h"	// For __FILE__LINE__
#include "RomOnlyResolver.h"

CRomOnlyResolver* CRomOnlyResolver::NewL(MPublicRegistry& aRegistry)
	{
	return new(ELeave) CRomOnlyResolver(aRegistry);
	}

CRomOnlyResolver::~CRomOnlyResolver()
	{
	}

CRomOnlyResolver::CRomOnlyResolver(MPublicRegistry& aRegistry)
: CDefaultResolver(aRegistry)
	{
	// Do nothing here
	}

TUid CRomOnlyResolver::IdentifyImplementationL(TUid aInterfaceUid, 
											   const TEComResolverParams& aAdditionalParameters) const
	{
	RImplInfoArray* implementationsInfo = ListAllL(aInterfaceUid, aAdditionalParameters);
	TUid found = KNullUid;
	if(implementationsInfo->Count())
		{
		found = Resolve(*implementationsInfo, aAdditionalParameters);
		}
	implementationsInfo->Reset();
	delete implementationsInfo;
	return found;
	}

RImplInfoArray* CRomOnlyResolver::ListAllL(TUid aInterfaceUid, 
										   const TEComResolverParams& aAdditionalParameters) const
	{
	// Use the base class version to get the list of matches
	RImplInfoArray* retList = CDefaultResolver::ListAllL(aInterfaceUid, aAdditionalParameters);

	// Now remove any from the list that aren't in ROM (or are upgrades of ROM)
	const TInt count = retList->Count();
	for(TInt index = count-1; index >=0; --index)
		if(!(*retList)[index]->RomBased())
			retList->Remove(index);

	return retList;
	}

