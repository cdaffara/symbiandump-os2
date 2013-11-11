/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#ifndef EVENTMAN_H
#define EVENTMAN_H

#include <e32base.h>
#include "msmm_internal_def.h"

class MMsmmSrvProxy;
class CDeviceEventHandler;

NONSHARABLE_CLASS (CDeviceEventQueue) : public CActive
    {
public:
    ~CDeviceEventQueue();
    static CDeviceEventQueue* NewL(MMsmmSrvProxy& aServer);
    static CDeviceEventQueue* NewLC(MMsmmSrvProxy& aServer);

    /** Queue a device event 
    * Intends to be used by CMsmmSession class
    */
    void PushL(const TDeviceEvent& aEvent);
    
    /** Finalize events in queue
    * Intends to be used by CMsmmServer when last session has been closed.
    * This function will remove all pending events of adding a interface
    * from the queue. It also cancels adding events that currently is being
    * handled in CDeviceEventHandler. 
    */
    void Finalize();

    /** Return count of events in queue
    */
    inline TUint8 Count() const;
    
protected:
    // Derived from CActive    
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:
    CDeviceEventQueue(MMsmmSrvProxy& aServer);
    void ConstructL();

    void AppendAndOptimizeL(const TDeviceEvent& aEvent);
    void StartL();
    
    void SendEventL();
    
    inline TBool IsEventAvailable() const;
    TDeviceEvent Pop();

private:
    MMsmmSrvProxy& iServer;
    CDeviceEventHandler* iHandler; // Owned
    RArray<TDeviceEvent> iEventArray;
    };

#include "eventqueue.inl"

#endif /*EVENTMAN_H*/
