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
* header file for NCM notification message class 
*
*/



#ifndef NCMNOTIFICATIONDESCRIPTOR_H
#define NCMNOTIFICATIONDESCRIPTOR_H

#include <e32std.h>


const TUint KUSBNotificationNetworkConnection = 8;

/**
 * USB NetworkConnection Notification
 */
 
NONSHARABLE_CLASS(TUSBNotificationNetworkConnection)
    {
public:
    TUint8    iRequestType;   ///< Request type
    TUint8    iNotification;  ///< Notification number
    TUint16   iValue;         ///< Notification value
    TUint16   iIndex;         ///< Notification index
    TUint16   iLength;        ///< Notification length
public:
    TDes8&    Des();

private:
    TBuf8<KUSBNotificationNetworkConnection> iBuffer;
    };

const TUint KUSBNotificationConnectionSpeedChange = 16;

/**
 * USB ConnectionSpeedChange Notification
 */

NONSHARABLE_CLASS(TUSBNotificationConnectionSpeedChange)
    {
public:
    TUint8    iRequestType;   ///< Request type
    TUint8    iNotification;  ///< Notification number
    TUint16   iValue;         ///< Notification value
    TUint16   iIndex;         ///< Notification index
    TUint16   iLength;        ///< Notification length
    TUint32   iUSBitRate;     ///< upstream bit rate, in bits per second 
    TUint32   iDSBitRate;     ///< downstream bit rate, in bits per second
public:
    TDes8&    Des();

private:
    TBuf8<KUSBNotificationConnectionSpeedChange> iBuffer;
    };

#endif // NCMNOTIFICATIONDESCRIPTOR_H

