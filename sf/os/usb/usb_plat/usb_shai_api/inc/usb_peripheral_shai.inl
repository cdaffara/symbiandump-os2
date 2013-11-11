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

#ifndef USB_PERIPHERAL_SHAI_INL
#define USB_PERIPHERAL_SHAI_INL

//
// inline constructor for TPeripheralControllerProperties.
// The responsibility of this constructor is to reset all
// data member of TPeripheralControllerProperties to zero.
//
inline UsbShai::TPeripheralControllerProperties::TPeripheralControllerProperties():
    iControllerCaps(0),
    iDfcQueue(NULL),
    iDeviceEndpointCaps(NULL),
    iDeviceTotalEndpoints(0),
    iMaxEp0Size(0),
    iDeviceRelease(0)
    {
    }
 
#endif
