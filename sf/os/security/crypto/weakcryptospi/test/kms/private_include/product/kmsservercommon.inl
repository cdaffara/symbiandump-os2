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
* Inline function support structures and values which KMS LDD client
* uses to pass data to the LDD, but which should not be visible to clients.
* The functions in this file are defined as inline because they must be
* accessible to both the client and the driver.
*
*/




/**
 @file
 @internalComponent
*/


#ifndef KMSSERVERCOMMON_INL
#define KMSSERVERCOMMON_INL

#include "kmsservercommon.h"


inline TVersion KeyMgmtSvrImpl::Version()
/**
	This function is defined because it is not possible to construct a literal TVersion.
	It is compiled into both the client and server implementations.  On connection, the
	client ensures that the server supports at least this version.
 */
	{
	const TVersion v = TVersion(KMgmtServerVerMajor, KMgmtServerVerMinor, KMgmtServerVerBuild);
	return v;
	}

#endif	// #ifndef KMSSERVERCOMMON_INL

