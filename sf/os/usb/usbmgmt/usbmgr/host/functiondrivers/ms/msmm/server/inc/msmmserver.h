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

#ifndef MSMMSERVER_H
#define MSMMSERVER_H

//  Include Files
#include <e32base.h> // for CPolicyServer
#include <ecom/implementationinformation.h>

// Project includes
#include "srvdef.h"
#include "srvpanic.h"
#include "srvsec.h"

// Forward declaration
class CMsmmEngine;
class CMsmmTerminator;
class CDeviceEventQueue;
class CMsmmPolicyPluginBase;
class THostMsErrData;
class CMsmmDismountUsbDrives;
class TUSBMSDeviceDescription;

// Server side resource container class
// Intends to be used by any internal objects (such as sub-command objects) 
// needs access to engine, policy plugin, MSC or RFs. 
class MMsmmSrvProxy
    {
public:
    virtual CMsmmEngine& Engine() const = 0;
    virtual RFs& FileServerSession() const = 0;
    virtual CMsmmPolicyPluginBase* PolicyPlugin() const = 0;
    };

// The MSMM server class 
NONSHARABLE_CLASS (CMsmmServer) : public CPolicyServer, public MMsmmSrvProxy
    {
public: // Static
    static TInt ThreadFunction();
    static void ThreadFunctionL();
        
public:
   
    // Construction and destruction
    static CMsmmServer* NewLC();
    ~CMsmmServer();

public:
    // CMsmmServer API
    virtual CSession2* NewSessionL(const TVersion& aVersion, 
            const RMessage2& aMessage) const;
    CPolicyServer::TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, 
                                                        TInt& aAction,  
                                                        TSecurityInfo& aMissing);

    TInt SessionNumber() const;
    void AddSession();
    void RemoveSession();

    void DismountUsbDrivesL(TUSBMSDeviceDescription& aDevice);
    
    // From MMsmmSrvProxy
    inline CMsmmEngine& Engine() const;
    inline RFs& FileServerSession() const;
    inline CMsmmPolicyPluginBase* PolicyPlugin() const;

private: // CMsmmServer Construction
    CMsmmServer(TInt aPriority);
    void ConstructL();

private: // Data members
    TInt                iNumSessions;
    CMsmmTerminator*    iTerminator; // Owned
    CMsmmEngine*        iEngine; // Owned
    RFs                 iFs;
    CMsmmPolicyPluginBase*  iPolicyPlugin; // Owned
    CDeviceEventQueue* iEventQueue; // Owned
    CMsmmDismountUsbDrives* iDismountManager; //Singleton to request dismounting usb drives via RFs
    THostMsErrData* iDismountErrData; // The data nodes try, Owned
    };
#include "msmmserver.inl" 
#endif  // MSMMSERVER_H
