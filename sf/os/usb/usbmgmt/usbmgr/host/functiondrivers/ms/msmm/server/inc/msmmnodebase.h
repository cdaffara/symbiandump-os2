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

#ifndef MSMMNODEBASE_H
#define MSMMNODEBASE_H

#include <e32base.h>
#include "msmm_pub_def.h"

#ifdef __OVER_DUMMYCOMPONENT__
#include <usb/hostms/dummycomponent/rusbhostmsdevice.h>
#else
#include <rusbhostmsdevice.h> // For RUsbHostMsDevice
#endif

class TMsmmNodeBase
    {    
public:
	TMsmmNodeBase(TInt aIdentifier);
    virtual ~TMsmmNodeBase();

    void AddChild(TMsmmNodeBase* aChild);
    TMsmmNodeBase* SearchInChildren(TInt aIdentifier);
    
private:
    void DestroyNode();
    
private:
    TInt    iIdentifier;
    
protected:
    /**
     A pointer to the next peer of this entry, or NULL.
     */
    TMsmmNodeBase* iNextPeer;

    /**
     A pointer to the first child of this entry, or NULL.
     */
    TMsmmNodeBase* iFirstChild;
    
    /**
     A pointer to the last child of this entry, or NULL.
     */
    TMsmmNodeBase* iLastChild;

    /**
     A pointer to the parent to this entry, or NULL.
     */
    TMsmmNodeBase* iParent;
    };

class TUsbMsInterface;
// The USB MS device node class
NONSHARABLE_CLASS(TUsbMsDevice) : public TMsmmNodeBase
    {
public:
    TUsbMsDevice(const TUSBMSDeviceDescription& aDevice);

    inline TUsbMsInterface* FirstChild() const;
    
public:
    TUSBMSDeviceDescription iDevice;
    };

class TUsbMsLogicalUnit;
// The USB MS interface node class
NONSHARABLE_CLASS(TUsbMsInterface) : public TMsmmNodeBase
    {
public:
    TUsbMsInterface(TUint8 aInterfaceNumber, 
            TUint32 aInterfaceToken);
    ~TUsbMsInterface();

    inline TUsbMsInterface* NextPeer() const;
    inline TUsbMsLogicalUnit* FirstChild() const;
    
public:
    TUint8 iInterfaceNumber;
    TUint32 iInterfaceToken;
    RUsbHostMsDevice iUsbMsDevice;
    };

// The USB MS logical unit node class
NONSHARABLE_CLASS(TUsbMsLogicalUnit) : public TMsmmNodeBase
    {
public:
    TUsbMsLogicalUnit(TUint8 aLogicalUnitNumber, 
            TText aDrive);

    inline TUsbMsLogicalUnit* NextPeer() const;
    inline TUsbMsInterface* Parent() const;
    
public:
    TUint8 iLogicalUnitNumber; // Logic Unit Number
    TText iDrive;
    };

inline TUsbMsInterface* TUsbMsDevice::FirstChild() const
    {return static_cast<TUsbMsInterface*>(iFirstChild);}

inline TUsbMsLogicalUnit* TUsbMsInterface::FirstChild() const
    {return static_cast<TUsbMsLogicalUnit*>(iFirstChild);}
inline TUsbMsInterface* TUsbMsInterface::NextPeer() const
    {return static_cast<TUsbMsInterface*>(iNextPeer);}

inline TUsbMsLogicalUnit* TUsbMsLogicalUnit::NextPeer() const
    {return static_cast<TUsbMsLogicalUnit*>(iNextPeer);}
inline TUsbMsInterface* TUsbMsLogicalUnit::Parent() const
    {return static_cast<TUsbMsInterface*>(iParent);}

#endif /*MSMMNODEBASE_H*/
