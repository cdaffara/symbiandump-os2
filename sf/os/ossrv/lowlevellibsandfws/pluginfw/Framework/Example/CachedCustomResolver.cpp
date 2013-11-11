// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 @file
 @internalComponent
*/

#include "CachedCustomResolver.h"
#include <ecom/implementationproxy.h>
#include <ecom/ecomerrorcodes.h>

/**
This function is used for cleanup support of ListAllL call.
*/
LOCAL_C void CleanupEComPtrArray(TAny* aArray)
	{
	RImplInfoArray* self = static_cast<RImplInfoArray*>(aArray);
	self->Reset();
	delete self;
	}

// static factory method to instantiate the resolver.
CExampleResolver* CExampleResolver::NewL(MPublicRegistry& aRegistry)
	{
	CExampleResolver* self = new(ELeave) CExampleResolver(aRegistry);
	(void)self->GetSignature();
	return self;
	}

CExampleResolver::~CExampleResolver()
	{
	// nothing
	}

CExampleResolver::CExampleResolver(MPublicRegistry& aRegistry)
: CResolver(aRegistry)
	{
	// nothing to do
	}

// stub for the IdentifyImplementationL pure virtual
TUid CExampleResolver::IdentifyImplementationL(TUid aInterfaceUid, 
											   const TEComResolverParams&) const
	{
	RImplInfoArray& implementationsInfo = iRegistry.ListImplementationsL(aInterfaceUid);
	if (implementationsInfo.Count())
		{
		return implementationsInfo[0]->ImplementationUid();
		}
	return KNullUid;
	}

// stub for the ListAllL pure virtual
RImplInfoArray* CExampleResolver::ListAllL(TUid aInterfaceUid, 
										   const TEComResolverParams&) const
	{
	TBool checkMyVersion = (aInterfaceUid == KCustomResolverInterfaceUid);
	TBool foundMyself = EFalse;

	// Ownership of the returned pointer is passed back to the caller.
	// Hence need to new it.
	RImplInfoArray* retList = new(ELeave) RImplInfoArray;
	CleanupStack::PushL(TCleanupItem(CleanupEComPtrArray, retList));

	RImplInfoArray& fullList = iRegistry.ListImplementationsL(aInterfaceUid);
	const TInt numImps = fullList.Count();
	for(TInt index = 0; index < numImps; ++index)
		{
		if (checkMyVersion)
			{
			VerifyVersionL(fullList[index], foundMyself);
			}

		User::LeaveIfError(retList->Append(fullList[index]));
		}

	if (checkMyVersion && !foundMyself)
		{
		RDebug::Print(_L("ECOM: error cachedcustomresolver not in list\n"));
		User::Leave(KEComErrMismatchedTags);
		}

	// Reset the member variable because we are passing ownership back
	CleanupStack::Pop(retList);
	return retList;
	}

//
// Used by test code in list request where the i/f UID is KCustomResolverInterfaceUid.
// This gives the custom resolver object a chance to check that the version listed
// in ECOM registry matches the current running object.
//
// param aImplInfo - the implementation object to check
// param aMe - output parameter to tell the call if 'this' is found
void CExampleResolver::VerifyVersionL(const CImplementationInformation* aImplInfo, TBool& aMe) const
	{
	const TUid KMyImplUid = {0x10009E12};
	if (aImplInfo->ImplementationUid() == KMyImplUid)
		{
		if (GetSignature() != aImplInfo->Version())
			{
			RDebug::Print(_L("ECOM: error registry has ver %d, binary has %d\n"), aImplInfo->Version(), GetSignature());
			// leave with un-used cwcode. But even if ECOM will use this in
			// the future, the code will not occur in a list request.
			User::Leave(KEComErrMismatchedTags);
			}
		aMe = ETrue; // found myself
		}
	}

// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10009E12, CExampleResolver::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
