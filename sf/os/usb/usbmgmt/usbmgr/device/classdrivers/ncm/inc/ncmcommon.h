/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
@internalTechnology
 */

#ifndef NCMCOMMON_H
#define NCMCOMMON_H

#ifndef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
#error "NCM needs the 3-plane comms environment!"
#endif

#include <usb/usbncm.h>

#ifndef __OVER_DUMMYUSBSCLDD__
#include <comms-infras/nifprvar.h>
#include <d32ethernet.h> //FOR KEthernetAddressLength
#else
#include <usb/testncm/ethinter.h>
#endif

//property's category UID.
//This Property is used to indicate the Packet Driver that it's started by NCM Class Controller.
#ifdef OVERDUMMY_NCMCC
static const TUid KUsbmanSvrUid = {0x2002EA88}; //
#else
static const TUid KUsbmanSvrUid = {0x101fe1db};
#endif // OVERDUMMY_NCMCC

//property's sub-key.
static const TUint KKeyNcmSharedState = 0x2002C340;

//Platform Security for the property.
static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_C1(KNetworkControlPolicy, ECapabilityNetworkControl);
static _LIT_SECURITY_POLICY_C1(KCommDDPolicy, ECapabilityCommDD);


/**
NCM network device MAC address.
*/
typedef TBuf8<KEthernetAddressLength> TNcmMacAddress;

/**
NCM SharedState between NCM Class Controller and Packet Driver.
*/
enum TNcmSharedState
    {
    ENcmStateIntial = 0,
    ENcmStartRequested = 1, // NCM class controller is request NCM packet driver
    ENcmStateEndMark
    };

/**
 Packet driver progress to Comms
 */
enum TNcmNifProgress
    {
    ENcmPktDrvFatalError    = KMinNifProgress   // NCM packet driver fatal error
    };

#endif // NCMCOMMON_H
