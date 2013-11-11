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


/**
@file
@internalComponent
*/

#include "omxthreadrequest.h"

COmxThreadRequest* COmxThreadRequest::NewL()
    {
    COmxThreadRequest* self = new (ELeave) COmxThreadRequest();
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

COmxThreadRequest::COmxThreadRequest()
    {
    }

void COmxThreadRequest::ConstructL()
    {
    //Parameters to send to the sub thread
    TThreadParam param;
    param.iThreadRequestObj = this;
    
    //Get a reference to this thread's heap
    RHeap& thisHeap = User::Heap();
            
    //Get the id of this thread
    RThread thisThread;
    TThreadId thisThreadId = thisThread.Id();
    param.iMainThreadId = thisThreadId;
    
    //Get a request to signal for setup completion
    TRequestStatus setupComplete = KRequestPending;
    param.iSetupComplete = &setupComplete;
    
    //current time and the "this" pointer for a unique key
    _LIT(KFormatString,"%S.%020Lu.%08X");
    TName threadName;
    TTime now;
    now.UniversalTime();
    threadName.Format(KFormatString, &KXMLClientOMXThreadName, now.Int64(), reinterpret_cast<TUint>(this));

    //Create a new thread using the same heap as this thread
    TInt result = iRequestThread.Create(threadName,
                                    ThreadCreateFunction,
                                    KDefaultStackSize,
                                    &thisHeap,
                                    &param);
    User::LeaveIfError(result);

    //Run the thread under high priority
    iRequestThread.SetPriority(EPriorityNormal); 

    //Wait for thread startup to complete
    TRequestStatus threadStatus = KRequestPending;
    iRequestThread.Logon(threadStatus);

    //Start the thread
    iRequestThread.Resume();
    User::WaitForRequest(threadStatus, setupComplete);
    if(threadStatus != KRequestPending)
        {
        //Thread creation failed
        TInt reason = iRequestThread.ExitReason();
        User::Leave(reason);
        }

    // Thread creation was successfull
    TInt error = iRequestThread.LogonCancel(threadStatus);
    User::LeaveIfError(error); // There is no outstanding request
    User::WaitForRequest(threadStatus); // Consume the signal

    iThreadCreated = ETrue;
    User::LeaveIfError(setupComplete.Int());
    }

/**
Main thread entry point for the omx sub-thread.
Create a cleanup stack for the thread and process the commands
inside a trap for cleanup behaviour.
@internalComponent

@param aPtr Parameters to be used for creating the thread.
@return The error code for thread termination.
*/
TInt COmxThreadRequest::ThreadCreateFunction(TAny* aPtr)
    {
    TInt error = KErrNone;

    // Create a cleanup stack for the thread
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack)
        {
        if(error == KErrNone)
            {
            TRAP(error, ThreadTrapFunctionL(aPtr));
            }
        }
    else
        {
        error = KErrNoMemory;
        }

    delete cleanupStack;
    return error;
    }

/**
Function for thread execution. If the thread is successfully created signal
the main thread that the thread creation was successfull. Then enter OMX
request loop.

@internalComponent

@param aPtr A pointer to a TThreadParam object containing the startup parameters
            for the thread.
*/
void COmxThreadRequest::ThreadTrapFunctionL(TAny* aPtr)
    {
    TThreadParam* param = static_cast<TThreadParam*>(aPtr);
    COmxThreadRequest* iThreadRequestObj = param->iThreadRequestObj;
    
    // tell main thread that setup is complete
    RThread mainThread;
    mainThread.Open(param->iMainThreadId);
    mainThread.RequestComplete(param->iSetupComplete, KErrNone);
    mainThread.Close();
    
    // loop to execute OMX commands in own thread
    while(iThreadRequestObj->iFunction != EOMXFunctionCodeTerminate)
        {
        iThreadRequestObj->iRequestThreadStatus = KRequestPending;
        
        // block until client request is made
        User::WaitForRequest(iThreadRequestObj->iRequestThreadStatus);
        switch(iThreadRequestObj->iFunction)
            {
            case EOMXFunctionCodeGetHandle:
                iThreadRequestObj->iErrorType = OMX_GetHandle(
                        iThreadRequestObj->ipHandle,
                        iThreadRequestObj->icComponentName,
                        iThreadRequestObj->ipAppData,
                        iThreadRequestObj->ipCallBacks);
                break;
                
            case EOMXFunctionCodeFreeHandle:
                iThreadRequestObj->iErrorType = OMX_FreeHandle(
                        iThreadRequestObj->ihComponent);
                break;
                
            case EOMXFunctionCodeTerminate:
                break;
                
            default:
                // should never be reached
                User::Panic(_L("COmxThreadRequest"), KErrNotSupported);
            }
        
        // complete client request
        TRequestStatus *status = iThreadRequestObj->iCallingStatus;
        RThread callingThread;
        callingThread.Open(iThreadRequestObj->iCallingThreadId);
        callingThread.RequestComplete(status, KErrNone);
        callingThread.Close();
        }
    }

COmxThreadRequest::~COmxThreadRequest()
    {
    if(iThreadCreated)
        {
        TRequestStatus status;
        // Logon to ensure thread has completely terminated, not just broken out of message handling loop.
        // In particular this ensures the cleanup stack is destroyed before the heap marks are inspected.
        iRequestThread.Logon(status);
        Terminate();
        User::WaitForRequest(status);
        iThreadCreated = EFalse;
        iRequestThread.Close();
        }
    }

OMX_ERRORTYPE COmxThreadRequest::GetHandle(
        OMX_OUT OMX_HANDLETYPE* pHandle,
        OMX_IN  OMX_STRING cComponentName,
        OMX_IN  OMX_PTR pAppData,
        OMX_IN  OMX_CALLBACKTYPE* pCallBacks)
    {
    ipHandle = pHandle;
    icComponentName = cComponentName;
    ipAppData = pAppData;
    ipCallBacks = pCallBacks;
    
    iFunction = EOMXFunctionCodeGetHandle; 
    DoRequest();
    
    return iErrorType;
    }

OMX_ERRORTYPE COmxThreadRequest::FreeHandle(
    OMX_IN  OMX_HANDLETYPE hComponent)
    {
    ihComponent = hComponent;
    
    iFunction = EOMXFunctionCodeFreeHandle; 
    DoRequest();
    
    return iErrorType;
    }

void COmxThreadRequest::Terminate()
    {
    iFunction = EOMXFunctionCodeTerminate; 
    DoRequest();
    }

void COmxThreadRequest::DoRequest()
    {
    RThread me;
    iCallingThreadId = me.Id();

    TRequestStatus callingRequestStatus = KRequestPending;
    iCallingStatus = &callingRequestStatus;
      
    // send request to request thread
    RThread requestThread;
    requestThread.Open(iRequestThread.Id());
    TRequestStatus* requestStatus = &iRequestThreadStatus;
    requestThread.RequestComplete(requestStatus, KErrNone);
    requestThread.Close();
     
    // block calling request until OMX request completes
    User::WaitForRequest(callingRequestStatus);
    }
