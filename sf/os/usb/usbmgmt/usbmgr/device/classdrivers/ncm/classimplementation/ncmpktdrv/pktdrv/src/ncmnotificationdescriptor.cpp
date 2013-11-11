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
* implementation for NCM notification message class 
*
*/


/**
@file
@internalComponent
*/


#include <es_sock.h>
#include "ncmnotificationdescriptor.h"


// ======== MEMBER FUNCTIONS ========
//


//
// This function packs the TUSBNotificationNetworkConnection class into a descriptor with 
// the correct byte alignment for transmission on the USB bus.


TDes8& TUSBNotificationNetworkConnection::Des()
    {
    TUint index = 0;

    iBuffer.SetLength(KUSBNotificationNetworkConnection);
    iBuffer[index++] = iRequestType;
    iBuffer[index++] = iNotification;
    LittleEndian::Put16(&iBuffer[index], iValue);
    index += 2;
    LittleEndian::Put16(&iBuffer[index], iIndex);
    index += 2;
    LittleEndian::Put16(&iBuffer[index], iLength);
    return iBuffer;
    }

//
//This function packs the TUSBNotificationConnectionSpeedChange class into a descriptor with 
//the correct byte alignment for transmission on the USB bus.

TDes8& TUSBNotificationConnectionSpeedChange::Des()
    {
    TUint index = 0;

    iBuffer.SetLength(KUSBNotificationConnectionSpeedChange);
    iBuffer[index++] = iRequestType;
    iBuffer[index++] = iNotification;
    LittleEndian::Put16(&iBuffer[index], iValue);
    index += 2;
    LittleEndian::Put16(&iBuffer[index], iIndex);
    index += 2;
    LittleEndian::Put16(&iBuffer[index], iLength);
    index += 2;
    LittleEndian::Put32(&iBuffer[index], iUSBitRate);
    index += 4;
    LittleEndian::Put32(&iBuffer[index], iDSBitRate);
    return iBuffer;
    }


