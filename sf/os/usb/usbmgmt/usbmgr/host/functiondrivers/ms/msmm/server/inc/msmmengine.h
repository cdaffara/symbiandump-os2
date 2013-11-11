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

#ifndef MSMMENGINE_H
#define MSMMENGINE_H

#include <e32base.h>
class TUSBMSDeviceDescription;
class TMsmmNodeBase;
class TUsbMsInterface;
class TUsbMsLogicalUnit;
class TUsbMsDevice;

NONSHARABLE_CLASS(CMsmmEngine) : public CBase
    {
public:
    virtual ~CMsmmEngine();
    static CMsmmEngine* NewL();
    static CMsmmEngine* NewLC();

public:

    /**
    Called to add a Usb MS device node
    @param aDevice The device currently attched
    */
    void AddUsbMsDeviceL(const TUSBMSDeviceDescription& aDevice);
        
    /**
    Called to add a Usb MS interface node
    @param aDevice The device to which current interface belongs
    @param aInterfaceNode Currently adding interface node
    */
    TUsbMsInterface* AddUsbMsInterfaceL(TInt aDeviceId, 
            TUint8 aInterfaceNumber, TInt32 aInterface);
    
    /**
    Called to add a Usb MS LU node
    @param aDeviceId The device identifier of the device by which current 
        logical unit supported
    @param aInterfaceNumber The interface number value of the interface
        to which current logical unit belongs
    @param aLogicalUnitNumber Current LU number value
    @param aDrive The drive on which current LU mounted
    */
    TUsbMsLogicalUnit* AddUsbMsLogicalUnitL(TInt aDeviceId, TInt aInterfaceNumber, 
            TInt aLogicalUnitNumber, TText aDrive);
    
    /**
    Called to remove a Usb MS node
    @param aNodeToBeRemoved The node to be removed
    */
    void RemoveUsbMsNode(TMsmmNodeBase* aNodeToBeRemoved);
    
    /**
    Called to search a particular device in engine
    @param aDeviceId The related device identifier of the device node
    @return Pointer to the device node found out in the data engine
    */
    TUsbMsDevice* SearchDevice(TInt aDeviceId) const;
        
    /**
    Called to search a particular device in engine. Intends to be used
    when a parent device node is available.
    @param aDevice The parent device node of the interface node 
        currently searched 
    @param aInterfaceNumber The related interface number value of the 
        interface node
    @return Pointer to the device node found out in the data engine
    */
    TUsbMsInterface* SearchInterface(TMsmmNodeBase* aDevice, 
            TInt aInterfaceNumber) const;
    
protected:
    CMsmmEngine();
    void ConstructL();

private:

    // Called to add a new USB MS interface node into data engine.
    TUsbMsInterface* AddUsbMsInterfaceNodeL(TUsbMsDevice* iParent,
            TInt aInterfaceNumber, TInt aInterfaceToken);
    
    // Called to add a new USB MS logical unit node into data engine.
    TUsbMsLogicalUnit* AddUsbMsLogicalUnitNodeL(TUsbMsInterface* iParent,
            TInt aLogicalUnitNumber, TText aDrive);

private:
    TMsmmNodeBase* iDataEntrys; // Usb Ms device tree
    };

#endif /*MSMMENGINE_H*/
