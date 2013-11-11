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

/** @file
@internalComponent
*/

#ifndef NCMDHCPNOTIFOBSERVER_H
#define NCMDHCPNOTIFOBSERVER_H

#include <e32std.h>

/**
 * MdnoHandleDhcpNotification defines the interface for notification of Dhcp
 * provisioning request
 */
class MNcmDhcpNotificationObserver
    {
public:
    /**
     * Called by DHCP prvisioning requestion watcher to handle DHCP 
     * provisioning request notification from NCM internal server.
     * @param   aError, completion code of DHCP provisioning request
     */
    virtual void MdnoHandleDhcpNotification(TInt aError) = 0;
    };

#endif // NCMDHCPNOTIFOBSERVER_H
