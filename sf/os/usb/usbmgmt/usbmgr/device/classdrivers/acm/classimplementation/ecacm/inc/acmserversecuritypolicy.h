/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Server Security Policy definitions.
*
*/

/**
 @file 
 @internalComponent
*/
 
#ifndef ACMSERVERSECURITYPOLICY_H
#define ACMSERVERSECURITYPOLICY_H

#include <e32base.h>
#include "acmserverconsts.h"

/** ACM Server Security Policy Definition */
const TUint KAcmServerRangeCount = 2;

const TInt KAcmServerRanges[KAcmServerRangeCount] = 
	{
	EAcmCreateAcmFunctions, 			/** pass */
	EAcmDestroyAcmFunctions + 1,		/** fail (to KMaxTInt) */
	};

/** Index numbers into KAcmServerElements[] */
const TInt KPolicyUsbsvr = 0;

/** Mapping IPCs to policy element */
const TUint8 KAcmServerElementsIndex[KAcmServerRangeCount] = 
	{
	KPolicyUsbsvr,					/** All (valid) APIs */
	CPolicyServer::ENotSupported,	/** remainder of possible IPCs */
	};

/** Individual policy elements */
const CPolicyServer::TPolicyElement KAcmServerElements[] = 
	{
		{ 
		// These caps are those of USBSVR's process. This is the recommended 
		// way to check that an API is called by a specific process only. [The 
		// alternative would be an SID check.]
		_INIT_SECURITY_POLICY_C5(ECapabilityCommDD, ECapabilityNetworkControl, ECapabilityNetworkServices, ECapabilityLocalServices, ECapabilityProtServ), CPolicyServer::EFailClient 
		},
	};

/** Main policy */
const CPolicyServer::TPolicy KAcmServerPolicy = 
	{
	CPolicyServer::EAlwaysPass, /** Specifies all connect attempts should pass */
	KAcmServerRangeCount,
	KAcmServerRanges,
	KAcmServerElementsIndex,
	KAcmServerElements,
	};
	
#endif // ACMSERVERSECURITYPOLICY_H
