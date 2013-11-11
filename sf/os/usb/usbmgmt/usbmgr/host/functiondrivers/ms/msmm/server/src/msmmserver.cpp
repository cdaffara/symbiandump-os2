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

#include "msmmserver.h"
#include "msmm_internal_def.h"
#include "msmmsession.h"
#include "msmmengine.h"
#include "eventqueue.h"
#include "msmmterminator.h"
#include "msmmdismountusbdrives.h"

#include <usb/hostms/msmmpolicypluginbase.h>
#include <usb/usblogger.h>
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "msmmserverTraces.h"
#endif


//  Static public functions
TInt CMsmmServer::ThreadFunction()
    {
    TInt ret = KErrNone;
    
    __UHEAP_MARK;
    // Create the cleanup stack
    CTrapCleanup* cleanupStack = CTrapCleanup::New();
    if (cleanupStack)
        {
        TRAP(ret, ThreadFunctionL());
        
        delete cleanupStack;
        }
    else
        {
        ret = KErrNoMemory;
        }
    __UHEAP_MARKEND;
    
    return ret;
    }

void CMsmmServer::ThreadFunctionL()
    {
    OstTraceFunctionEntry0( CMSMMSERVER_THREADFUNCTIONL_ENTRY );
    
    TSecureId creatorSID = User::CreatorSecureId();
    if (KFDFWSecureId != creatorSID)
        {
        // Only FDF process can be the creator of the MSMM server
        User::Leave(KErrPermissionDenied);
        }
    
    // Create and install the active scheduler
    CActiveScheduler* scheduler = new(ELeave) CActiveScheduler;
    CleanupStack::PushL(scheduler);
    CActiveScheduler::Install(scheduler);

    // Create the server and leave it on cleanup stack
    CMsmmServer::NewLC();

    // Signal the client that the server is up and running
    RProcess::Rendezvous(KErrNone);
    
    RThread().SetPriority(EPriorityAbsoluteHigh);

    // Start the active scheduler, the server will now service 
    // request messages from clients.
    CActiveScheduler::Start();

    // Free the server and active scheduler.
    CleanupStack::PopAndDestroy(2, scheduler);
    OstTraceFunctionExit0( CMSMMSERVER_THREADFUNCTIONL_EXIT );
    }

CPolicyServer::TCustomResult CMsmmServer::CustomSecurityCheckL(
    const RMessage2&  aMsg,
     TInt&  /*aAction*/,
     TSecurityInfo&  /*aMissing*/)
 {
     CPolicyServer::TCustomResult returnValue = CPolicyServer::EFail;    
     
     TSecureId ClientSID = aMsg.SecureId();
 
     if (KFDFWSecureId == ClientSID)
         {
         returnValue = CPolicyServer::EPass;
         }     
     else if ((KSidHbDeviceDialogAppServer == ClientSID) && SessionNumber() > 0)
         {
         returnValue = CPolicyServer::EPass;
         }
     return returnValue;
 }

// Public functions
// Construction and destruction
CMsmmServer* CMsmmServer::NewLC()
    {
    OstTraceFunctionEntry0( CMSMMSERVER_NEWLC_ENTRY );
    
    CMsmmServer* self = new (ELeave) CMsmmServer(EPriorityHigh);
    CleanupStack::PushL(self);
    
    // Create a server with unique server name
    self->StartL(KMsmmServerName);
    self->ConstructL();
    
    OstTraceFunctionExit0( CMSMMSERVER_NEWLC_EXIT );
    return self;
    }

CMsmmServer::~CMsmmServer()
    {
    OstTraceFunctionEntry0( CMSMMSERVER_CMSMMSERVER_DES_ENTRY );
    
    delete iPolicyPlugin;
    delete iEventQueue;
    delete iEngine;
    delete iTerminator;
    delete iDismountErrData;
    delete iDismountManager;
    REComSession::FinalClose();

#ifndef __OVER_DUMMYCOMPONENT__
    iFs.RemoveProxyDrive(KPROXYDRIVENAME);
    iFs.Close();
#endif
    OstTraceFunctionExit0( CMSMMSERVER_CMSMMSERVER_DES_EXIT );
    }
    
    // CMsmmServer APIs
CSession2* CMsmmServer::NewSessionL(const TVersion& aVersion, 
        const RMessage2& aMessage) const
    {
    OstTraceFunctionEntry0( CMSMMSERVER_NEWSESSIONL_ENTRY );
    
    if (KMaxClientCount <= SessionNumber())
        {
        // There is a connection to MSMM server already.
        // Currently design of MSMM can have two clients, one FDF and the other Indicator UI
        // at any time.
        User::Leave(KErrInUse);
        }
    
    // Check the client-side API version number against the server version 
    // number.
    TVersion serverVersion(KMsmmServMajorVersionNumber,
        KMsmmServMinorVersionNumber, KMsmmServBuildVersionNumber);
    
    if (!User::QueryVersionSupported(serverVersion, aVersion))
        {
        // Server version incompatible with client-side API
        PanicClient(aMessage, ENoSupportedVersion);
        User::Leave(KErrNotSupported);
        }

    // Version number is OK - create the session
    return CMsmmSession::NewL(*(const_cast<CMsmmServer*>(this)), *iEventQueue);
    }

TInt CMsmmServer::SessionNumber() const
    {
    OstTraceFunctionEntry0( CMSMMSERVER_SESSIONNUMBER_ENTRY );
    return iNumSessions;
    }

void CMsmmServer::AddSession()
    {
    OstTraceFunctionEntry0( CMSMMSERVER_ADDSESSION_ENTRY );
     
    ++iNumSessions;
    iTerminator->Cancel();
    OstTraceFunctionExit0( CMSMMSERVER_ADDSESSION_EXIT );
    }

void CMsmmServer::RemoveSession()
    {
    OstTraceFunctionEntry0( CMSMMSERVER_REMOVESESSION_ENTRY );
    
    --iNumSessions;
    if (iNumSessions == 0)
        {
        // Discard all pending adding interface events from queue
        // and cancel event handler if a adding events is being 
        // handled in it.
        iEventQueue->Finalize();
        iTerminator->Cancel();
        iTerminator->Start();
        }
		
    OstTraceFunctionExit0( CMSMMSERVER_REMOVESESSION_EXIT );
    }
	
	void CMsmmServer::DismountUsbDrivesL(TUSBMSDeviceDescription& aDevice)
    {
    OstTraceFunctionEntry0( CMSMMSERVER_DISMOUNTUSBDRIVERSL_ENTRY );
    delete iDismountManager;
    iDismountManager = NULL;
    iDismountManager= CMsmmDismountUsbDrives::NewL();
    
    //Also notify the MSMM plugin of beginning of dismounting     
    iDismountErrData->iError = EHostMsEjectInProgress;
    iDismountErrData->iE32Error = KErrNone;
    iDismountErrData->iManufacturerString = aDevice.iManufacturerString;
    iDismountErrData->iProductString = aDevice.iProductString;
    iDismountErrData->iDriveName = 0x0;
   
    TRAP_IGNORE(iPolicyPlugin->SendErrorNotificationL(*iDismountErrData));

    // Start dismounting
    iDismountManager->DismountUsbDrives(*iPolicyPlugin, aDevice);
	OstTraceFunctionExit0( CMSMMSERVER_DISMOUNTUSBDRIVERSL_EXIT );
    }


//  Private functions 
// CMsmmServer Construction
CMsmmServer::CMsmmServer(TInt aPriority)
    :CPolicyServer(aPriority, KMsmmServerSecurityPolicy, EUnsharableSessions)
    {
    OstTraceFunctionEntry0( CMSMMSERVER_CMSMMSERVER_CONS_ENTRY );
    }

void CMsmmServer::ConstructL()
    {
    OstTraceFunctionEntry0( CMSMMSERVER_CONSTRUCTL_ENTRY );
    
    iEngine = CMsmmEngine::NewL();
    iEventQueue = CDeviceEventQueue::NewL(*this);
    iTerminator = CMsmmTerminator::NewL(*iEventQueue);
    iPolicyPlugin = CMsmmPolicyPluginBase::NewL();
    iDismountErrData = new (ELeave) THostMsErrData;
    if (!iPolicyPlugin)
        {
        // Not any policy plugin implementation available
        PanicServer(ENoPolicyPlugin);
        }
    
    // Initalize RFs connection and add the ELOCAL file system to file server
    User::LeaveIfError(iFs.Connect());

#ifndef __OVER_DUMMYCOMPONENT__
    TInt ret(KErrNone);
    ret = iFs.AddProxyDrive(KFSEXTNAME);
    if ((KErrNone != ret) && (KErrAlreadyExists !=  ret))
        {
        User::Leave(ret);
        }
#endif
    
    // Start automatic shutdown timer
    iTerminator->Start();
    OstTraceFunctionExit0( CMSMMSERVER_CONSTRUCTL_EXIT );
    }

// End of file
