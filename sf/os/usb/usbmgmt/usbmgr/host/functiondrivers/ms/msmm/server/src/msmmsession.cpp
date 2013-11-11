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

#include "msmmsession.h"
#include "msmmserver.h"
#include "msmmengine.h"
#include "eventqueue.h"
#include "msmmnodebase.h"
#include <usb/hostms/srverr.h>
#include <usb/hostms/msmmpolicypluginbase.h>

#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmsessionTraces.h"
#endif



CMsmmSession::~CMsmmSession()
    {
    OstTraceFunctionEntry0( CMSMMSESSION_CMSMMSESSION_DES_ENTRY );
    
    delete iErrData;
    iServer.RemoveSession();
    OstTraceFunctionExit0( CMSMMSESSION_CMSMMSESSION_DES_EXIT );
    }

CMsmmSession* CMsmmSession::NewL(CMsmmServer& aServer, 
        CDeviceEventQueue& anEventQueue)
    {
    OstTraceFunctionEntry0( CMSMMSESSION_NEWL_ENTRY );
    
    CMsmmSession* self = new(ELeave) CMsmmSession(aServer, anEventQueue);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    OstTraceFunctionExit0( CMSMMSESSION_NEWL_EXIT );
    return self;
    }

void CMsmmSession::ServiceL(const RMessage2& aMessage)
    {
    OstTraceFunctionEntry0( CMSMMSESSION_SERVICEL_ENTRY );
    
    TInt ret(KErrNone);

#ifdef _DEBUG
    TInt* heapObj= NULL;
#endif // _DEBUG
        
    switch (aMessage.Function())
        {
    case EHostMsmmServerAddFunction:
        AddUsbMsInterfaceL(aMessage);
        break;

    case EHostMsmmServerRemoveDevice:
        RemoveUsbMsDeviceL(aMessage);
        break;

        // Supporting for server side OOM testing  
    case EHostMsmmServerDbgFailNext:
        ret = KErrNone;
#ifdef _DEBUG
        if (aMessage.Int0() == 0 )
            {
            __UHEAP_RESET;
            }
        else
            {
            __UHEAP_FAILNEXT(aMessage.Int0());
            }
#endif // _DEBUG
        break;

    case EHostMsmmServerDbgAlloc:
        ret = KErrNone;
#ifdef _DEBUG
        TRAP(ret, heapObj = new (ELeave) TInt);
        delete heapObj;
#endif // _DEBUG
        break;
        
    case EHostMsmmServerEjectUsbDrives:
        iServer.DismountUsbDrivesL(iDevicePkg());
        break;
    default:
        // Unsupported function number - panic the client
        PanicClient(aMessage, EBadRequest);
        }
        
    // Complete the request
    aMessage.Complete(ret);
    OstTraceFunctionExit0( CMSMMSESSION_SERVICEL_EXIT );
    }

void CMsmmSession::ServiceError(const RMessage2 &aMessage, TInt aError)
    {
    OstTraceFunctionEntry0( CMSMMSESSION_SERVICEERROR_ENTRY );
    
    CMsmmPolicyPluginBase* plugin = iServer.PolicyPlugin();    
    TUSBMSDeviceDescription& device = iDevicePkg();
       
    switch (aError)
        {
    case KErrNoMemory:
        iErrData->iError = EHostMsErrOutOfMemory;
        break;
    case KErrArgument:
        iErrData->iError = EHostMsErrInvalidParameter;
        break;
    case KErrNotFound:
        iErrData->iError = EHostMsErrInvalidParameter;
        break;
    default:
        iErrData->iError = EHostMsErrGeneral;
        }
    
    iErrData->iE32Error = aError;
    iErrData->iManufacturerString = device.iManufacturerString;
    iErrData->iProductString = device.iProductString;
    iErrData->iDriveName = 0x0;
   
    OstTrace1( TRACE_DUMP, CMSMMSESSION_SERVICEERROR, "iErrData->iE32Error = %d", aError );
    OstTraceExt1( TRACE_DUMP, CMSMMSESSION_SERVICEERROR_DUP1, "iErrData->iManufacturerString=\"%S\"", device.iManufacturerString );
    OstTraceExt1( TRACE_DUMP, CMSMMSESSION_SERVICEERROR_DUP2, "iErrData->iProductString=\"%S\"", device.iProductString );

        
    TInt err(KErrNone);
    TRAP(err, plugin->SendErrorNotificationL(*iErrData));
    aMessage.Complete(aError);
    OstTraceFunctionExit0( CMSMMSESSION_SERVICEERROR_EXIT );
    }

CMsmmSession::CMsmmSession(CMsmmServer& aServer, 
        CDeviceEventQueue& anEventQueue) :
iServer(aServer),
iEngine(aServer.Engine()),
iEventQueue(anEventQueue)
    {
    OstTraceFunctionEntry0( CMSMMSESSION_CMSMMSESSION_CONS_ENTRY );
    
    aServer.AddSession();
    OstTraceFunctionExit0( CMSMMSESSION_CMSMMSESSION_CONS_EXIT );
    }

void CMsmmSession::ConstructL()
    {
    OstTraceFunctionEntry0( CMSMMSESSION_CONSTRUCTL_ENTRY );
    
    iErrData = new (ELeave) THostMsErrData;
    OstTraceFunctionExit0( CMSMMSESSION_CONSTRUCTL_EXIT );
    }

void CMsmmSession::AddUsbMsInterfaceL(const RMessage2& aMessage)
    {
    OstTraceFunctionEntry0( CMSMMSESSION_ADDUSBMSINTERFACEL_ENTRY );
    
    aMessage.Read(0, iDevicePkg);
    iInterfaceNumber = aMessage.Int1();
    iInterfaceToken = static_cast<TInt32>(aMessage.Int2());
    TUSBMSDeviceDescription& device = iDevicePkg();
    
    // Put currently adding USB MS function related device 
    // information into engine
    iEngine.AddUsbMsDeviceL(device);
    
    // Put device event into queue
    TDeviceEvent event(EDeviceEventAddFunction, 
            device.iDeviceId, iInterfaceNumber, iInterfaceToken);
    iEventQueue.PushL(event);
    OstTraceFunctionExit0( CMSMMSESSION_ADDUSBMSINTERFACEL_EXIT );
    }

void CMsmmSession::RemoveUsbMsDeviceL(const RMessage2& aMessage)
    {
    OstTraceFunctionEntry0( CMSMMSESSION_REMOVEUSBMSDEVICEL_ENTRY );
    
    iDeviceID = aMessage.Int0();
       
    // Put device event into queue
    TDeviceEvent event(EDeviceEventRemoveDevice, iDeviceID, 0, 0);
    iEventQueue.PushL(event);
    OstTraceFunctionExit0( CMSMMSESSION_REMOVEUSBMSDEVICEL_EXIT );
    }

// End of file
