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
* Data shared between KMS client API and server, but not with the
* client.
*
*/




/**
 @file
 @internalComponent
*/


#ifndef KMSSERVERCOMMON_H
#define KMSSERVERCOMMON_H

#include "kmsldd.h"


namespace KeyMgmtSvrImpl
/**
	The values and structures in this namespace are shared between
	the client API DLL and server, but are not visible to clients.
 */
	{
	/** Key management server name, which the client uses to connect to it. */
	_LIT(KKeyMgmtSrvName, "KeyMgmtSvr");
	
	/** KMS major version number. */
	const TInt KMgmtServerVerMajor = 1;
	/** KMS minor version number. */
	const TInt KMgmtServerVerMinor = 0;
	/** KMS build version number.  Not used. */
	const TInt KMgmtServerVerBuild = 0;

	inline TVersion Version();
	
	enum TServerFunction
	/** Function codes identify requests sent from the client to the server. */
		{
		EGenerateKey,
		EStoreKey,
		EDeleteKey,
		EAddUsage,
		EDeleteUsage,
		ESetHeapFail,
		EEndHeapFail
		};
	}

#include "kmsservercommon.inl"

#endif	// #ifndef KMSSERVERCOMMON_H

