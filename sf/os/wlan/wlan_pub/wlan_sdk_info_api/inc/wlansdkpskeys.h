/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  WLAN event enumerations and uid:s for Publish And Subscribe.
*                PubSub clients can include this file and listen to these events.
*                These events will be routed through Publish And Subscribe.
*
*/

/*
* %version: 3 %
*/

#ifndef WLANSDKPSKEYS_H
#define WLANSDKPSKEYS_H

#include <e32property.h>


/**
* P&S category WLAN information
*/
const TUid KPSUidWlan = { 0x101f8ec5 };


/**
* WLAN MAC address
* The MAC address array consists of 6 consecutive bytes which can be used 
* to produce the address in format XX:XX:XX:XX:XX:XX  
*/
const TUint KPSWlanMacAddress = 0x00000001;
const RProperty::TType KPSWlanMacAddressType = RProperty::EByteArray;

/**
* Length of the MAC address array
*/
const TUint KPSWlanMacAddressLength = 6;

#endif // WLANSDKPSKEYS_H
           
//  End of File
