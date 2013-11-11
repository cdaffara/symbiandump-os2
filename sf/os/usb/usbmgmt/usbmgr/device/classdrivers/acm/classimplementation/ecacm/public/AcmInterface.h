/*
* Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __ACMINTERFACE_H__
#define __ACMINTERFACE_H__

/** @file
Public interface to ECACM CSY.
@publishedPartner
@released
*/

#include <e32std.h>

/**
The root of the CSY filename. Use this in LoadCommModule.
*/
_LIT(KAcmCsyName, "ECACM"); 

/**
The name of the CSerial, and consequently the root of the ACM port name 
ACM::0. Use this in Open (and UnloadCommModule, if you're not closing your 
session anyway).
*/
_LIT(KAcmSerialName, "ACM"); 

/**
Lowest valid value of %d in calls to Open(_L("ACM::%d")).
For the highest valid value of %d in calls to Open(_L("ACM::%d")), use RCommServ::GetPortInfo.
Note that if USB services are stopped/idle (see RUsb::GetServiceState) the high unit value will 
be incorrect. Under these circumstances, no ACM ports are may be opened, but TSerialInfo cannot
express this.
When USB services are starting/started/stopping, the given high unit will accurately reflect 
the highest available port number. Note that this numerical value will depend on the current 
USB personality (see RUsb) and also potentially whether USB is fully started or in a transitional
state (starting or stopping).
*/
const TUint KAcmLowUnit = 0;

/**
@deprecated RCommServ::GetPortInfo is the preferred method for determining the number of available ports.
@see RCommServ::GetPortInfo
*/	
const TUint KAcmHighUnit = 0; 

#endif // __ACMINTERFACE_H__
