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
* User-side classes which provide access to the KMS LDD.
*
*/




/**
 @file
 @internalComponent
*/


#ifndef KMSLDD_H
#define KMSLDD_H

#include <e32cmn.h>

#ifndef __KERNEL_MODE__
#include <e32std.h>
#endif

#include "keyhandle.h"

/** Hardware store LDD filename, as supplied to LoadLogicalDevice. */
_LIT(KKmsLddFileName, "KMSLDDK");


class RKmsChannel : public RBusLogicalChannel
/**
	User-side channel used to control the KMS LDD.
	This can only be used by the key management server.
	
	@internalTechnology
 */
	{
public:
	IMPORT_C TInt Open();
	inline static const TDesC& FactoryName();
	
#ifndef __KERNEL_MODE__
	IMPORT_C void GenerateKey(TInt aLength, RProcess aOwner, TKeyHandle& aHandle, TRequestStatus& aStatus);
	IMPORT_C void StoreKey(const TDesC8& aData, RProcess aOwner, TKeyHandle& aHandle, TRequestStatus& aStatus);
	IMPORT_C void DeleteKey(RProcess aClaimedOwner, TKeyHandle aHandle, TRequestStatus& aStatus);
	IMPORT_C void AddUsage(RProcess aClaimedOwner, TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol, TRequestStatus& aStatus);
	IMPORT_C void DeleteUsage(RProcess aDeletor, TKeyHandle aHandle, TInt aOperation, TRequestStatus& aStatus);
	IMPORT_C void UsageAllowed(RProcess aUserProc, TKeyHandle aHandle, TInt aOperation, TBool& aResult, TRequestStatus& aStatus);
#endif
	};


#include "kmsldd.inl"


#endif	// KMSLDD_H

