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
* API which drivers use to access the emulated hardware key store.
*
*/




/**
 @file 
 @internalComponent
*/

#ifndef HWKEYSTORE_H
#define HWKEYSTORE_H

#include <e32cmn.h>
#include <e32ver.h>

#include "keyhandle.h"


class HwKeyStore
/**
	The variant layer must implement this interface to provide access
	to the hardware key store.
 */
	{
public:
	IMPORT_C static TDfcQue& DfcQ();
	
	IMPORT_C static TInt GenerateKey(TInt aLength, TSecureId aOwner, TKeyHandle& aHandle);
	IMPORT_C static TInt StoreKey(const TDesC8& aData, TSecureId aOwner, TKeyHandle& aHandle);
	IMPORT_C static TInt DeleteKey(TSecureId aClaimedOwner, TKeyHandle aHandle);
	
	IMPORT_C static TInt AddUsage(TSecureId aClaimedOwner, TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol);
	IMPORT_C static TInt DeleteUsage(TSecureId aClaimedOwner, TKeyHandle aHandle, TInt aOperation);
	IMPORT_C static TInt UsageAllowed(DProcess* aProcess, TKeyHandle aHandle, TInt aOperation, TBool& aAllowed);
	
	IMPORT_C static TInt ExtractKey(DProcess* aProcess, TKeyHandle aHandle, TInt aOperation, HBuf8*& aData);
	
private:
	static void RunExtractDfc(TAny* aPtr);
	
public:
#ifdef _DEBUG
	enum TPanic
	/**
		Reasons why threads calling this class may be panicked.
		@see Panic
	 */
		{
		EGKWrongThread = 0x00,
		ESKWrongThread = 0x10,
		EDKWrongThread = 0x20,
		EAUWrongThread = 0x30,
		EDUWrongThread = 0x40,
		EUAWrongThread = 0x50
		};
	
	static void ConfirmCurrentThread(HwKeyStore::TPanic aPanic);
	static void Panic(TPanic aPanic);
#endif
	};


#endif	// #ifndef HWKEYSTORE_H

