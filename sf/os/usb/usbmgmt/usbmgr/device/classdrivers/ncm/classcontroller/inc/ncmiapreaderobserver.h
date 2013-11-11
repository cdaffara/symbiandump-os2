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

#ifndef NCMIAPREADEROBSERVER_H
#define NCMIAPREADEROBSERVER_H

#include <e32std.h>

/**
MNcmIAPReaderObserver defines the interface for handling IAP reading completion
*/
class MNcmIAPReaderObserver
    {
public:
    /**
     * Called by IAP reader when IAP reading completed 
     */
    virtual void MicoHandleIAPReadingComplete(TInt aError) = 0;
    };

#endif // NCMIAPREADEROBSERVER_H
