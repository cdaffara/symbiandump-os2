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

#ifndef NCMIAPCONNECTIONOBSERVER_H
#define NCMIAPCONNECTIONOBSERVER_H

#include <e32std.h>

/** MNcmConnectionObserver defines the interface to handle the Notification of 
 * NCM connection established.
 */
class MNcmConnectionObserver
    {
public:
    /**
     * Called by connection watcher to handle NCM connection building 
     * completion.
     */
    virtual void MicoHandleConnectionComplete(TInt aError) = 0;
    };

#endif // NCMIAPCONNECTIONOBSERVER_H
