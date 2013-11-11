/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include "eventqueue.h"
#include "eventhandler.h"
#include "msmmserver.h"
#include "msmmnodebase.h"
#include "msmmengine.h"
#include <usb/hostms/msmmpolicypluginbase.h>
#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eventqueueTraces.h"
#endif


// Public member functions
CDeviceEventQueue::~CDeviceEventQueue( )
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_CDEVICEEVENTQUEUE_DES_ENTRY );
    
    Cancel();
    delete iHandler;
    iEventArray.Close();
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_CDEVICEEVENTQUEUE_DES_EXIT );
    }

CDeviceEventQueue* CDeviceEventQueue::NewL(MMsmmSrvProxy& aServer)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_NEWL_ENTRY );
    
    CDeviceEventQueue* self = CDeviceEventQueue::NewLC(aServer);
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_NEWL_EXIT );
    return self;
    }
CDeviceEventQueue* CDeviceEventQueue::NewLC(MMsmmSrvProxy& aServer)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_NEWLC_ENTRY );
    
    CDeviceEventQueue* self = new (ELeave) CDeviceEventQueue(aServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_NEWLC_EXIT );
    return self;
    }

void CDeviceEventQueue::PushL(const TDeviceEvent& aEvent)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_PUSHL_ENTRY );

    // Perform optimization for remove device event
    AppendAndOptimizeL(aEvent);
    
    // Start handling first event in queue
    StartL();
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_PUSHL_EXIT );
    }

void CDeviceEventQueue::Finalize()
    {
    TInt index(0);
    while(index < iEventArray.Count())
        {
        if (EDeviceEventAddFunction == iEventArray[index].iEvent)
            {
            iEventArray.Remove(index);
            }
        else
            {
            index ++;
            }
        };
    
    if (EDeviceEventAddFunction == iHandler->Event().iEvent)
        {
        iHandler->Cancel();
        }
    }


// Protected member functions
void CDeviceEventQueue::DoCancel()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_DOCANCEL_ENTRY );
    
    iEventArray.Reset();
    iHandler->Cancel();
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_DOCANCEL_EXIT );
    }

void CDeviceEventQueue::RunL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_RUNL_ENTRY );
    
    // Check the completion code from CDeviceEventHandler. If there
    // is some error occured. We need issue error notification here.
    TInt err = iStatus.Int();
    if ((KErrNone != err) && (KErrCancel != err))
        {
        iServer.PolicyPlugin()->
            SendErrorNotificationL(iHandler->ErrNotiData());
        }
    iHandler->ResetHandler();
    if (IsEventAvailable())
        {
        SendEventL();
        }
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_RUNL_EXIT );
    }

TInt CDeviceEventQueue::RunError(TInt aError)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_RUNERROR_ENTRY );
    
    THostMsErrData errData;
    switch (aError)
        {
    case KErrNoMemory:
        errData.iError = EHostMsErrOutOfMemory;
        break;
    case KErrArgument:
        errData.iError = EHostMsErrInvalidParameter;
        break;
    default:
        errData.iError = EHostMsErrGeneral;
        }
    errData.iE32Error = aError;
    TUsbMsDevice* deviceNode = 
        iServer.Engine().SearchDevice(iHandler->Event().iDeviceId);
    if (deviceNode)
        {
        errData.iManufacturerString.Copy(deviceNode->iDevice.iManufacturerString);
        errData.iProductString.Copy(deviceNode->iDevice.iProductString);
        }
    errData.iDriveName = 0x0;
    TInt err(KErrNone);
    TRAP(err, iServer.PolicyPlugin()->SendErrorNotificationL(errData));
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_RUNERROR_EXIT );
    return KErrNone;
    }

// Private member functions
CDeviceEventQueue::CDeviceEventQueue(MMsmmSrvProxy& aServer):
CActive(EPriorityStandard),
iServer(aServer)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_CDEVICEEVENTQUEUE_CONS_ENTRY );
    
    CActiveScheduler::Add(this);
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_CDEVICEEVENTQUEUE_CONS_EXIT );
    }

void CDeviceEventQueue::ConstructL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_CONSTRUCTL_ENTRY );
    
    iHandler = CDeviceEventHandler::NewL(iServer);
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_CONSTRUCTL_EXIT );
    }

void CDeviceEventQueue::AppendAndOptimizeL(const TDeviceEvent& aEvent)
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_APPENDANDOPTIMIZEL_ENTRY );
    
    if (EDeviceEventRemoveDevice == aEvent.iEvent)
        {
        // Scan the event queue to discard all pending related adding 
        // function events.
        TInt index(0);
        while(index < iEventArray.Count())
            {
        	if (aEvent.iDeviceId == iEventArray[index].iDeviceId)
        	    {
        	    iEventArray.Remove(index);
        	    }
        	else
        	    {
        	    index ++;
        	    }
            };
        
        switch (iHandler->Event().iEvent)
            {
        case EDeviceEventAddFunction:
            // If a related adding interface event is being handled currently,
            // CDeviceEventQueue shall cancel it first.
            if (aEvent.iDeviceId == iHandler->Event().iDeviceId)
                {
                iHandler->Cancel();
                }
            break;
        case EDeviceEventRemoveDevice:
            if (aEvent.iDeviceId == iHandler->Event().iDeviceId && IsActive())
                {
                // Discard duplicated removing event.
                OstTraceFunctionExit0( CDEVICEEVENTQUEUE_APPENDANDOPTIMIZEL_EXIT );
                return;
                }
            break;
            }
        }
        iEventArray.AppendL(aEvent);
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_APPENDANDOPTIMIZEL_EXIT_DUP1 );
    }

void CDeviceEventQueue::StartL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_STARTL_ENTRY );
    
    if (IsActive())
        {
        OstTraceFunctionExit0( CDEVICEEVENTQUEUE_STARTL_EXIT );
        return;
        }

    if (IsEventAvailable())
        {
        SendEventL();
        }
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_STARTL_EXIT_DUP1 );
    }

void CDeviceEventQueue::SendEventL()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_SENDEVENTL_ENTRY );
         
    // If the handler is available, sending oldest event to it
    iHandler->HandleEventL(iStatus, Pop());
        
    // Activiate the manager again to wait for the handler 
    // finish current event
    SetActive();
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_SENDEVENTL_EXIT );
    }

TDeviceEvent CDeviceEventQueue::Pop()
    {
    OstTraceFunctionEntry0( CDEVICEEVENTQUEUE_POP_ENTRY );
    
    TDeviceEvent event = iEventArray[0];
    iEventArray.Remove(0);
    OstTraceFunctionExit0( CDEVICEEVENTQUEUE_POP_EXIT );
    return event;
    }

// End of file
