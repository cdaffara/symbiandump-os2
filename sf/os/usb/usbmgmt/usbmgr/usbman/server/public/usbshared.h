/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Shared client/server definitions
*
*/

/**
 @file
 @publishedPartner
*/

#ifndef __USBSHARED_H__
#define __USBSHARED_H__

#include <e32std.h>
#include <usbhostdefs.h>
#include <usbotgdefs.h>
	
/** KUsbMessageBase
	
	Not used as a message value. 'Base' value for USB OTG/Host related messages which are NOT errors. 
	Numerical value: 6600.

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageBase	= 6600;
	

/** KUsbMessageSrpInitiated

	Used to inform the observer about SRP request 

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageSrpInitiated	= (KUsbMessageBase+1);

/** KUsbMessageSrpReceived

	Used to inform the observer about SRP request 

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageSrpReceived	= (KUsbMessageBase+2);

/** KUsbMessageHnpDisabled

	Used to inform the observer about HNP status after RESET

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageHnpDisabled	= (KUsbMessageBase+3);

/** KUsbMessageHnpEnabled

	Used to inform the observer about HNP status

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageHnpEnabled	= (KUsbMessageBase+4);

/** KUsbMessageVbusRaised

	Used to inform the observer about VBUS going up

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageVbusRaised	= (KUsbMessageBase+5);

/** KUsbMessageVbusDropped

	Used to inform the observer about VBUS going down

	@publishedPartner 
	@released 
*/
const TInt KUsbMessageVbusDropped	= (KUsbMessageBase+6);

/** KUsbMessageRequestSession

	Used to inform the observer about USB session request  

	@publishedPartner 
	@released
*/
const TInt KUsbMessageRequestSession = (KUsbMessageBase+7);

	
#endif //__USBSHARED_H__
