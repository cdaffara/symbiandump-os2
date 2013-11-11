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
*
*/

/**
 @file
 @internalComponent
*/

#ifndef ACMSERVERCONSTS_H
#define ACMSERVERCONSTS_H

#include <e32base.h>

_LIT(KAcmServerName, "!AcmSrv");

/** Version numbers. */
const TInt8 KAcmSrvMajorVersionNumber = 1;
const TInt8 KAcmSrvMinorVersionNumber = 1;
const TInt16 KAcmSrvBuildNumber = 0;

/** IPC messages supported by the server. */
enum TAcmIpc
	{
	EAcmCreateAcmFunctions				= 0, // the 1st message must be 0 for CPolicyServer to start up
	EAcmDestroyAcmFunctions				= 1,		
	};
	
/** Panic category with which the ACM server panics the client. */
_LIT(KAcmSrvPanic,"ACMServer");

/** Panic codes with which the ACM server panics the client. */
enum TAcmSvrPanic
	{
	/** The client has sent a bad IPC message number. */
	EAcmBadAcmMessage		 = 0,
	};

/**
Default protocol number for ACM ports. Taken from USBCDC 1.1 Table 17
@internalTechnology
*/	
const TUint8 KDefaultAcmProtocolNum = 0x01; // Hayes compatible modem

/**
Default control interface name for ACM ports.
@internalTechnology
*/	
_LIT16(KControlIfcName, "CDC Comms Interface");

/**
Default data interface name for ACM ports.
@internalTechnology
*/	
_LIT16(KDataIfcName, "CDC Data Interface");

#endif // ACMSERVERCONSTS_H
