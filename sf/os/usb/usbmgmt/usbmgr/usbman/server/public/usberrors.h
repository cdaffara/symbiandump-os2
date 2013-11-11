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
*
*/

#ifndef __USBERRORS_H__
#define __USBERRORS_H__

#include <e32def.h>

/** @file
 Errors used by the USB Manager.
*/

/** KErrUsbBase
	
	Not used as an error. 'Base' value for USB Manager errors. 
	Numerical value: -6600.

	@publishedPartner 
	@released 
*/
const TInt KErrUsbBase				= (-6600);

/** KErrUsbServiceStopped
	
	Used to complete an RUsb::Start request when the service ends up Idle 
	because another client did an RUsb::Stop. 
	Numerical value: -6601
	
	@publishedPartner 
	@released 
*/
const TInt KErrUsbServiceStopped	= (KErrUsbBase-1); // -6601

/** KErrUsbServiceStarted

	Used to complete an RUsb::Stop request when the service ends up Started 
	because another client did an RUsb::Start. 
	Numerical value: -6602

	@publishedPartner 
	@released 
*/
const TInt KErrUsbServiceStarted	= (KErrUsbBase-2); // -6602

#endif //__USBERRORS_H__
