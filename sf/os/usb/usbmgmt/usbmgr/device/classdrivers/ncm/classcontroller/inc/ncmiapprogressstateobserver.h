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

#ifndef NCMIAPPROGRESSSTATEOBSERVER_H
#define NCMIAPPROGRESSSTATEOBSERVER_H

#include <e32std.h>

/**
 * MNcmIapProgressStateObserver defines the interface for events 
 * on IAP progress stage changing.
 */
class MNcmIapProgressStateObserver
    {
public:
    /**
     * Called by connection watcher when it detects the class fatal error.
     */
    virtual void MipsoHandleClassFatalError(TInt aError) = 0;
    };

#endif // NCMIAPPROGRESSSTATEOBSERVER_H
