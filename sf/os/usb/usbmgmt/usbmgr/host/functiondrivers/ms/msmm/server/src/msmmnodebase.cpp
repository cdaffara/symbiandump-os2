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

#include "msmmnodebase.h"

#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmnodebaseTraces.h"
#endif



TMsmmNodeBase::TMsmmNodeBase(TInt aIdentifier):
iIdentifier(aIdentifier),
iNextPeer(NULL),
iFirstChild(NULL),
iLastChild(NULL),
iParent(NULL)
    {
    OstTraceFunctionEntry0( TMSMMNODEBASE_TMSMMNODEBASE_CONS_ENTRY );
    }

TMsmmNodeBase::~TMsmmNodeBase()
    {
    OstTraceFunctionEntry0( TMSMMNODEBASE_TMSMMNODEBASE_DES_ENTRY );
    
    // Remove current node from the parent node and destroy it.
    DestroyNode(); 
    OstTraceFunctionExit0( TMSMMNODEBASE_TMSMMNODEBASE_DES_EXIT );
    }

void TMsmmNodeBase::DestroyNode()
    {
    OstTraceFunctionEntry0( TMSMMNODEBASE_DESTROYNODE_ENTRY );
    
    TMsmmNodeBase* parentNode = iParent; 
    TMsmmNodeBase* iterator(this);
    TMsmmNodeBase* iteratorPrev(NULL);
    TMsmmNodeBase* iteratorNext(NULL);
    
    if (parentNode)
        {
        // A parent node exists
        iterator = parentNode->iFirstChild;
        if (iterator)
            {
            // iteratorPrev equal NULL at beginning;
            iteratorNext= iterator->iNextPeer;
            }
        // Go through each child node to find the node to be destroyed
        while (iterator && (iterator != this))
            {
            iteratorPrev = iterator;
            iterator = iteratorNext;
            if(iteratorNext)
                {
                iteratorNext = iteratorNext->iNextPeer;
                }
            }
        if (iterator)
            {
            // Matched node found
            if (parentNode->iLastChild == iterator)
                {
                parentNode->iLastChild = iteratorPrev;
                }
            if (iteratorPrev)
                {
                iteratorPrev->iNextPeer = iteratorNext;
                }
            else
                {
                parentNode->iFirstChild = iteratorNext;
                }
            }
        else
            {
            // No matched node
            OstTraceFunctionExit0( TMSMMNODEBASE_DESTROYNODE_EXIT );
            return;
            }
        }
        
    // Remove all children node
    if (iFirstChild)
        {
        // Current node isn't a leaf node
        iterator = iFirstChild;
        iteratorNext= iterator->iNextPeer;
        while (iterator)
            {
            delete iterator;
            iterator = iteratorNext;
            if (iteratorNext)
                {
                iteratorNext = iterator->iNextPeer;
                }
            }
        }
    OstTraceFunctionExit0( TMSMMNODEBASE_DESTROYNODE_EXIT_DUP1 );
    }

void TMsmmNodeBase::AddChild(TMsmmNodeBase* aChild)
    {
    OstTraceFunctionEntry0( TMSMMNODEBASE_ADDCHILD_ENTRY );
    
    if (!iFirstChild)
        {
        iFirstChild = aChild;
        }
    else
        {
        iLastChild->iNextPeer = aChild;
        }
    iLastChild = aChild;
    aChild->iParent = this;
    OstTraceFunctionExit0( TMSMMNODEBASE_ADDCHILD_EXIT );
    }

TMsmmNodeBase* TMsmmNodeBase::SearchInChildren(TInt aIdentifier)
    {
    OstTraceFunctionEntry0( TMSMMNODEBASE_SEARCHINCHILDREN_ENTRY );
    
    TMsmmNodeBase* iterator(iFirstChild);
    
    while (iterator)
        {
        if (iterator->iIdentifier == aIdentifier)
            {
            break;
            }
        iterator = iterator->iNextPeer;
        }
    
    OstTraceFunctionExit0( TMSMMNODEBASE_SEARCHINCHILDREN_EXIT );
    return iterator;
    }

// TUsbMsDevice
// Function memeber
TUsbMsDevice::TUsbMsDevice(const TUSBMSDeviceDescription& aDevice):
TMsmmNodeBase(aDevice.iDeviceId),
iDevice(aDevice)
    {
    OstTraceFunctionEntry0( TUSBMSDEVICE_TUSBMSDEVICE_CONS_ENTRY );
    }

// TUsbMsInterface
// Function memeber
TUsbMsInterface::TUsbMsInterface(TUint8 aInterfaceNumber, 
        TUint32 aInterfaceToken):
TMsmmNodeBase(aInterfaceNumber), 
iInterfaceNumber(aInterfaceNumber),
iInterfaceToken(aInterfaceToken)
    {
    OstTraceFunctionEntry0( TUSBMSINTERFACE_TUSBMSINTERFACE_CONS_ENTRY );
    }

TUsbMsInterface::~TUsbMsInterface()
    {
    OstTraceFunctionEntry0( TUSBMSINTERFACE_TUSBMSINTERFACE_DES_ENTRY );
    
    iUsbMsDevice.Close();
    OstTraceFunctionExit0( TUSBMSINTERFACE_TUSBMSINTERFACE_DES_EXIT );
    }

// TUsbMsLogicalUnit
// Function memeber
TUsbMsLogicalUnit::TUsbMsLogicalUnit(TUint8 aLogicalUnitNumber, TText aDrive):
TMsmmNodeBase(aLogicalUnitNumber),
iLogicalUnitNumber(aLogicalUnitNumber),
iDrive(aDrive)
    {
    OstTraceFunctionEntry0( TUSBMSLOGICALUNIT_TUSBMSLOGICALUNIT_CONS_ENTRY );
    }

// End of file
