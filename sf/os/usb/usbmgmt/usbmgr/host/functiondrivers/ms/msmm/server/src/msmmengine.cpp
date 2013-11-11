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

#include "msmmengine.h"
#include "msmmnodebase.h"

#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmengineTraces.h"
#endif


CMsmmEngine::~CMsmmEngine()
    {
    OstTraceFunctionEntry0( CMSMMENGINE_CMSMMENGINE_DES_ENTRY );
    
    if (iDataEntrys)
        {
        delete iDataEntrys;
        }
    OstTraceFunctionExit0( CMSMMENGINE_CMSMMENGINE_DES_EXIT );
    }

CMsmmEngine* CMsmmEngine::NewL()
    {
    OstTraceFunctionEntry0( CMSMMENGINE_NEWL_ENTRY );
    
    CMsmmEngine* self = CMsmmEngine::NewLC();
    CleanupStack::Pop(self);
    
    OstTraceFunctionExit0( CMSMMENGINE_NEWL_EXIT );
    return self;
    }

CMsmmEngine* CMsmmEngine::NewLC()
    {
    OstTraceFunctionEntry0( CMSMMENGINE_NEWLC_ENTRY );
    
    CMsmmEngine* self = new (ELeave) CMsmmEngine();
    CleanupStack::PushL(self);
    self->ConstructL();
    
    OstTraceFunctionExit0( CMSMMENGINE_NEWLC_EXIT );
    return self;
    }

void CMsmmEngine::AddUsbMsDeviceL(const TUSBMSDeviceDescription& aDevice)
    {
    OstTraceFunctionEntry0( CMSMMENGINE_ADDUSBMSDEVICEL_ENTRY );
    
    TUsbMsDevice* device = SearchDevice(aDevice.iDeviceId);
    if (!device)
        {
        device = new (ELeave) TUsbMsDevice(aDevice);
        iDataEntrys->AddChild(device);
        }
    OstTraceFunctionExit0( CMSMMENGINE_ADDUSBMSDEVICEL_EXIT );
    }

TUsbMsInterface* CMsmmEngine::AddUsbMsInterfaceL(TInt aDeviceId, TUint8 aInterfaceNumber,
        TInt32 aInterfaceToken)
    {
    OstTraceFunctionEntry0( CMSMMENGINE_ADDUSBMSINTERFACEL_ENTRY );
    
    TUsbMsDevice* device = SearchDevice(aDeviceId);
    if (!device)
        {
        User::Leave(KErrArgument);
        }
    TUsbMsInterface* interface = 
        SearchInterface(device, aInterfaceNumber);
    if (interface)
        {
        User::Leave(KErrAlreadyExists);
        }
    else
        {
        interface = AddUsbMsInterfaceNodeL(device, aInterfaceNumber, aInterfaceToken);
        }
    OstTraceFunctionExit0( CMSMMENGINE_ADDUSBMSINTERFACEL_EXIT );
    return interface;
    }

TUsbMsLogicalUnit* CMsmmEngine::AddUsbMsLogicalUnitL(TInt aDeviceId,
        TInt aInterfaceNumber, TInt aLogicalUnitNumber, TText aDrive)
    {
    OstTraceFunctionEntry0( CMSMMENGINE_ADDUSBMSLOGICALUNITL_ENTRY );
    
    TUsbMsDevice* device = SearchDevice(aDeviceId);
    if (!device)
        {
        User::Leave(KErrArgument); // A proper device node can't be found
        }
    
    TUsbMsInterface* interface = SearchInterface(device, aInterfaceNumber);
    TUsbMsLogicalUnit* logicalUnit = NULL;
    if (interface)
        {
        logicalUnit = AddUsbMsLogicalUnitNodeL(interface, aLogicalUnitNumber, aDrive);
        }
    else
        {
        User::Leave(KErrArgument); // A proper interface node can't be found
        }
    OstTraceFunctionExit0( CMSMMENGINE_ADDUSBMSLOGICALUNITL_EXIT );
    return logicalUnit;
    }

void CMsmmEngine::RemoveUsbMsNode(TMsmmNodeBase* aNodeToBeRemoved)
    {
    OstTraceFunctionEntry0( CMSMMENGINE_REMOVEUSBMSNODE_ENTRY );
    
    delete aNodeToBeRemoved;
    OstTraceFunctionExit0( CMSMMENGINE_REMOVEUSBMSNODE_EXIT );
    }

TUsbMsDevice* CMsmmEngine::SearchDevice(TInt aDeviceId) const
    {
    OstTraceFunctionEntry0( CMSMMENGINE_SEARCHDEVICE_ENTRY );
    
    return static_cast<TUsbMsDevice*>(
            iDataEntrys->SearchInChildren(aDeviceId));
    }

TUsbMsInterface* CMsmmEngine::SearchInterface(TMsmmNodeBase* aDevice, 
        TInt aInterfaceNumber) const
    {
    OstTraceFunctionEntry0( CMSMMENGINE_SEARCHINTERFACE_ENTRY );
    
    return static_cast<TUsbMsInterface*>(
            aDevice->SearchInChildren(aInterfaceNumber));
    }

CMsmmEngine::CMsmmEngine()
    {
    OstTraceFunctionEntry0( CMSMMENGINE_CMSMMENGINE_CONS_ENTRY );

    }

void CMsmmEngine::ConstructL()
    {
    OstTraceFunctionEntry0( CMSMMENGINE_CONSTRUCTL_ENTRY );
    
    // Create the root of the whole node tree
    iDataEntrys = new (ELeave) TMsmmNodeBase(0x0);
    OstTraceFunctionExit0( CMSMMENGINE_CONSTRUCTL_EXIT );
    }

TUsbMsInterface* CMsmmEngine::AddUsbMsInterfaceNodeL(TUsbMsDevice* iParent,
        TInt aInterfaceNumber, TInt aInterfaceToken)
    {
       OstTraceFunctionEntry0( CMSMMENGINE_ADDUSBMSINTERFACENODEL_ENTRY );
       
    TUsbMsInterface* interface = new (ELeave) TUsbMsInterface(
            aInterfaceNumber, aInterfaceToken);
    iParent->AddChild(interface);
    
    OstTraceFunctionExit0( CMSMMENGINE_ADDUSBMSINTERFACENODEL_EXIT );
    return interface;
    }

TUsbMsLogicalUnit* CMsmmEngine::AddUsbMsLogicalUnitNodeL(
        TUsbMsInterface* iParent, TInt aLogicalUnitNumber, 
        TText aDrive)
    {
    OstTraceFunctionEntry0( CMSMMENGINE_ADDUSBMSLOGICALUNITNODEL_ENTRY );
    
    TUsbMsLogicalUnit* logicalUnit = new (ELeave) TUsbMsLogicalUnit(
            aLogicalUnitNumber, aDrive);
    iParent->AddChild(logicalUnit);
    
    OstTraceFunctionExit0( CMSMMENGINE_ADDUSBMSLOGICALUNITNODEL_EXIT );
    return logicalUnit;
    }

// End of file
