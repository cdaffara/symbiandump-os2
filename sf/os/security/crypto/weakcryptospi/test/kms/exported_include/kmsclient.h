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
* API which clients use to talk to the key management server.
*
*/




/**
 @file 
 @internalTechnology
*/

#ifndef KMSCLIENT_H
#define KMSCLIENT_H

#include <e32std.h>

#include "keyhandle.h"


/** The server will panic clients with the category if it detects an error. */
_LIT(KKmsClientPanicCat, "KmsClientPanic");

enum TKmsClientPanic
/** Reasons why the KMS might panic a client. */
	{
	/**
		Client supplied an invalid descriptor.  The descriptor may have been
		created by the client DLL when the client passed a bad object reference.
	 */
    //  Unfortunately 0xBADDE5C causes the techview
    //  EikSrvs.exe::ViewServerThread to panic with bad descriptor
	//	EBadClientDescriptor = 0xBADDE5C
	EBadClientDescriptor = -38
	};

class RKeyMgmtSession : public RSessionBase
	{
public:
	IMPORT_C TInt Connect();
	IMPORT_C TInt Connect(const TVersion& aReqVer);
	
	IMPORT_C TInt GenerateKey(TInt aLength, TKeyHandle& aHandle);
	IMPORT_C TInt StoreKey(const TDesC8& aData, TKeyHandle& aHandle);
	IMPORT_C TInt DeleteKey(TKeyHandle aHandle);
	IMPORT_C TInt AddUsage(TKeyHandle aHandle, TInt aOperation, const TSecurityPolicy& aSecPol);
	IMPORT_C TInt DeleteUsage(TKeyHandle aHandle, TInt aOperation);

	IMPORT_C void SetHeapFail(TInt aValue);
	IMPORT_C void EndHeapFail();
	};

#endif	// #ifndef KMSCLIENT_H

