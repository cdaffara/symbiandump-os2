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
* Extraction LDD clients use these functions to access the channel
* functionality.
*
*/


/**
 @file
*/


#include "kmsextrlddcommon.h"
#include "kmsextrldd.h"


EXPORT_C TInt RKmsExtrChannel::Open()
/**
	Open a channel with the key extraction LDD.
	
	@return					KErrNone on success, any other error code on failure.
 */
	{
	const TDesC& name = RKmsExtrChannel::FactoryName();
	const TVersion reqVer = KmsExtrLddImpl::Version();
	
	return DoCreate(
		name,
		reqVer,
		KNullUnit,			// aUnit: units not supported (KDeviceAllowUnit flag not set)
		NULL,				// aDriver: PDD name, not supported (KDeviceAllocPhysicalDevice flag not set)
		NULL,				// anInfo: allow info not supported (KDeviceAllowInfo flag not set)
		EOwnerThread,		// aType: handle is owned by thread, not process
		EFalse);			// aProtected: channel cannot be shared with other processes
	}

EXPORT_C TInt RKmsExtrChannel::Extract(TKeyHandle aHandle, TInt aOperation, HBufC8*& aData)
/**
	Extract a key from the store.  This is only allowed if the current thread satisfies
	the usage policy for the handle and operation.
	
	@param	aHandle			Identifies key to extract.
	@param	aOperation		What the current thread wants to do with the key.  The key can
							only be extracted if the key defines a usage policy for this key
							and operation, which the current thread satisfies.
	@param	aData			On success, this points to a client-side descriptor which contains
							the key data.  Ownership is transferred to the caller.  Its value
							is undefined on failure.
	@return					KErrNone on success, any other error code on failure.
 */
	{
	// Because the driver cannot allocate memory on the client's heap, this is a three-stage
	// process.
	//
	// 1. get the key length
	// 2. allocate a buffer on the client heap
	// 3. read the key data into the buffer
	
	KmsExtrLddImpl::TKeyDataArgs kda = {aHandle, aOperation};
	TPckg<KmsExtrLddImpl::TKeyDataArgs> kdaPckg(kda);
	TInt length;
	TInt r = DoControl(KmsExtrLddImpl::EGetKeySize, (TAny*) &kdaPckg, (TAny*) &length);
	if (r != KErrNone)
		return r;
	
	aData = HBufC8::New(length);	
	if (aData == 0)
		return KErrNoMemory;
	
	TPtr8 dataPtr = aData->Des();
	r = DoControl(KmsExtrLddImpl::EGetKeyData, (TAny*) &kdaPckg, &dataPtr);
	if (r != KErrNone)
		delete aData;
	return r;
	}

