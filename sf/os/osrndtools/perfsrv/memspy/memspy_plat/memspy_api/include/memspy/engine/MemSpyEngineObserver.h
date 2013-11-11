/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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

#ifndef MEMSPYENGINEOBSERVER_H
#define MEMSPYENGINEOBSERVER_H

// System includes
#include <e32base.h>



class MMemSpyEngineObserver
    {
public: // Enumerations
    enum TEvent
        {
        EHandleThreadsOrProcessesChanged,
        EHandleClientServerOperationRequest
        };

public: // From MMemSpyEngineObserver
    virtual void HandleMemSpyEngineEventL( TEvent aEvent, TAny* aContext = NULL ) = 0;
    };



#endif
