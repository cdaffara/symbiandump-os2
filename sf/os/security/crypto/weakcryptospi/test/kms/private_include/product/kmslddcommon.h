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
* Data shared between KMS LDD client API and server, but not with the
* client.
*
*/




/**
 @file
 @internalComponent
*/

#ifndef KMSLDDCOMMON_H
#define KMSLDDCOMMON_H

#include <e32cmn.h>

#include "keyhandle.h"

namespace KmsLddImpl
/**
	Bundles constants and structures used to transfer data from the
	KMS LDD client and the driver.
 */
	{
	inline TVersion Version();
	
	/** Numeric value of KMS process ID.  Only this process can open a channel. */
	const TUint32 KKmsServerUidValue = 0x102833BF;
	/**
		Wraps server process identifier as a UID.
		@see KKmsServerUidValue
	 */
	const TUid KKmsServerUid = {KKmsServerUidValue};
	
	/** LDD version major number. */
	const TInt KMajorVersionNumber = 1;
	/** LDD version minor number. */
	const TInt KMinorVersionNumber = 0;
	/** LDD version build number. */
	const TInt KBuildVersionNumber = 0;

	// structures used to transfer requests from user- to kernel-side
	
	class TGenKeyArgs
	/** Bundles generate key args to send to kernel side. */
		{
	public:
		/** Length of key in bytes. */
		TInt iLength;
		/** Process which will own the newly-generated key. */
		TSecureId iOwner;
		/** Address of user-side key handle object. */
		TKeyHandle* iUserHandle;
		};
	/** Descriptor wraps an existing instance of TGenKeyArgs. */
	typedef TPckg<TGenKeyArgs> TGenKeyArgsPckg;
	
	class TStoreKeyArgs
	/** Bundles store key args to send to kernel side. */
		{
	public:
		/** Process which will own the key. */
		TSecureId iOwner;
		/** User-side handle to populate. */
		TKeyHandle* iUserHandle;
		};
	/** Descriptor wraps an existing instance of TGenKeyArgs. */
	typedef TPckg<TStoreKeyArgs> TStoreKeyArgsPckg;
	
	class TUsageIdentifierArgs
	/** Bundles arguments which identify a usage to add or delete. */
		{
	public:
		/** Process which claims to own this key. */
		TSecureId iClaimedOwner;
		/** Target key. */
		TKeyHandle iHandle;
		/** Operation for which it is granting privileges. */
		TInt iOperation;
		};
	/** Descriptor wraps an existing instance of TUsageIdentifierArgs. */
	typedef TPckg<TUsageIdentifierArgs> TUsageIdentifierArgsPckg;	

	class TUsageAllowedArgs
	/** Bundles arguments which identify a usage allowed request. */
		{
	public:
		/** Process which wants to use the key. */
		TInt iUserProcHandle;
		/** The key which the process wants to use. */
		TKeyHandle iKeyHandle;
		/** Operation which the process wants to perform. */
		TInt iOperation;
		};
	/** Descriptor wraps an existing instance of TUsageAllowedArgsPckg. */
	typedef TPckg<TUsageAllowedArgs> TUsageAllowedArgsPckg;
	
	enum TRequest
	/** Asynchronous function identifiers. */
		{
		EGenerateKey,
		EStoreKey,
		EDeleteKey,
		EAddUsage,
		EDeleteUsage,
		EUsageAllowed,
		ERequestCount,
		EAllRequests = (1 << ERequestCount) - 1
		};

	/**
		The LDD client is panicked with this category when it supplies a bad descriptor,
		or an invalid pointer to user-side data.

		@see TClientPanic
	 */
	_LIT(KKmsLddPanicCat, "KmsLddK");

	enum TClientPanic
	/**
		These values are used as exit reasons when the client is panicked.

		@see KKmsLddPanicCat
	 */
		{
		EGenKeyBadArgDescriptor = 0x00, EGenKeyInvalidHandleAddr,
		EStoreKeyBadArgDescriptor = 0x10, EStoreKeyInvalidDataDesc, EStoreKeyInvalidHandleAddr,
		EAddUsageBadArgDescriptor = 0x20, EAddUsageBadSecPolDescriptor,
		EUsageAllowedBadArgDescriptor = 0x30, EUsageAllowedBadAllowedPtr
		};
	}

#include "kmslddcommon.inl"

#endif	// #ifndef KMSLDDCOMMON_H

