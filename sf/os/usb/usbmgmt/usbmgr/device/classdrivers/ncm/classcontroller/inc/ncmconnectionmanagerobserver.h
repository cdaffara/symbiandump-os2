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

#ifndef NCMCONNECTIONMANAGEROBSERVER_H
#define NCMCONNECTIONMANAGEROBSERVER_H

#include <e32std.h>

/**
 * MNcmConnectionManagerObserver defines the interface for connection manager
 * callback.
 */
class MNcmConnectionManagerObserver
    {
public:
    /*
     * Called by NCM connection manager to report error to NCM connection 
     * observer.
     * @param   aError, error code reports to NCM connection observer
     */
    virtual void McmoErrorIndication(TInt aError) = 0;
    };

#endif // NCMCONNECTIONMANAGEROBSERVER_H
