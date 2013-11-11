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

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include <e32base.h>
#include <e32msgqueue.h>
#include "msmm_internal_def.h"
#include "subcommandbase.h"
#include "handlerinterface.h"

class MMsmmSrvProxy;
class THostMsErrData;

// USB MS sub-commands queue class
// Intend to be used in event handler internally.
NONSHARABLE_CLASS(RSubCommandQueue)
    {
public:
    
    inline TInt Count() const; 

    // Push a sub-command into the queue and transfer the owership 
    // to the queue
    void PushL(TSubCommandBase* aCommand);
    
    // Pop the head entity from the queue and destroy it  
    void Pop();
    
    // Insert a sub-command sequence after head entities    
    void InsertAfterHeadL(TSubCommandBase* aCommand);
    
    // Execute the head sub-comment
    void ExecuteHeadL();
    
    // Get a reference of head sub-command in queue
    TSubCommandBase& Head();
    
    // Destory all entities and release the memory of queue
    void Release();
    
private:
    RPointerArray<TSubCommandBase> iQueue;
    };

NONSHARABLE_CLASS (CDeviceEventHandler) : 
    public CActive, 
    public MUsbMsEventHandler,
    public MUsbMsSubCommandCreator
    {
public:
    virtual ~CDeviceEventHandler();
    static CDeviceEventHandler* NewL(MMsmmSrvProxy& aServer);
    static CDeviceEventHandler* NewLC(MMsmmSrvProxy& aServer);

    inline const TDeviceEvent& Event() const;

    // From MUsbMsSubCommandCreator
    void CreateSubCmdForRetrieveDriveLetterL(TInt aLogicalUnitCount);
    void CreateSubCmdForMountingLogicalUnitL(TText aDrive, TInt aLuNumber);
    void CreateSubCmdForSaveLatestMountInfoL(TText aDrive, TInt aLuNumber);
    
    // From MUsbMsEventHandler
    void Start();
    void Complete(TInt aError = KErrNone);
    TRequestStatus& Status() const;

    /** Send an event to handler and start to secure handler. 
    * Intends to be used by CDeviceEventQueue class.
    */
    void HandleEventL(TRequestStatus& aStatus, const TDeviceEvent& aEvent);

    /** Reset event handler to be ready for the next event
     * 
     */
    void ResetHandler();
    inline const THostMsErrData& ErrNotiData() const;
    
private:
    // CActive implementation
    void DoCancel();
    void RunL();
    TInt RunError(TInt aError);

private:
    CDeviceEventHandler(MMsmmSrvProxy& aServer);
    void ConstructL();

    // Create sub-commands based on a particular USB MS event
    void CreateSubCmdForDeviceEventL();
    void CreateSubCmdForAddingUsbMsFunctionL();
    void CreateSubCmdForRemovingUsbMsDeviceL();

    void ResetHandlerData();
    void ResetHandlerError();
    void CompleteClient(TInt aError = KErrNone);
    
private:
    MMsmmSrvProxy& iServer;
    TDeviceEvent iIncomingEvent;
    RSubCommandQueue iSubCommandQueue;
    TRequestStatus* iEvtQueueStatus; // No ownership

    THostMsErrData* iErrNotiData; // Error notification - Owned
    };

#include "eventhandler.inl"

#endif // EVENTHANDLER_H

// End of file
