/*
* Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Implements a Symbian OS server that exposes the RUsb API
*
*/

/**
 @file
*/

#include <e32svr.h>
#include <usb/usblogger.h>
#include "UsbSettings.h"
#include "CUsbServer.h"
#include "CUsbSession.h"
#include "CUsbDevice.h"

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
#include <e32property.h> //Publish & Subscribe header
#include "CUsbOtg.h"
#include "cusbhost.h"
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

#include "UsbmanServerSecurityPolicy.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbServerTraces.h"
#endif



/**
 * The CUsbServer::NewL method
 *
 * Constructs a Usb Server
 *
 * @internalComponent
 *
 * @return	A new Usb Server object
 */
CUsbServer* CUsbServer::NewLC()
	{
	OstTraceFunctionEntry0( CUSBSERVER_NEWLC_ENTRY );

	CUsbServer* self = new(ELeave) CUsbServer;
	CleanupStack::PushL(self);
	self->StartL(KUsbServerName);
	self->ConstructL();
	OstTraceFunctionExit0( CUSBSERVER_NEWLC_EXIT );
	return self;
	}


/**
 * The CUsbServer::~CUsbServer method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbServer::~CUsbServer()
	{
	OstTraceFunctionEntry0( CUSBSERVER_CUSBSERVER_DES_ENTRY );

	delete iShutdownTimer;
	delete iUsbDevice;
	
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	delete iUsbHost;
	
	// Check that this is A-Device
	OstTrace0( TRACE_NORMAL, CUSBSERVER_CUSBSERVER, "CUsbServer::~CUsbServer; Checking Id-Pin state..." );
	TInt value = 0;
	TInt err = RProperty::Get(KUidUsbManCategory, KUsbOtgIdPinPresentProperty,value);
	if (err == 0 && value == 1)
		{
		// Ensure VBus is dropped when Usb server exits
		OstTrace0( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP1, "CUsbServer::~CUsbServer; Checking VBus state..." );
		err = RProperty::Get(KUidUsbManCategory, KUsbOtgVBusPoweredProperty,value);
		if ( err == KErrNone && value != 0 )
			{
			if ( iUsbOtg )
				{
				err = iUsbOtg->BusDrop();
				OstTrace1( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP2, "CUsbServer::~CUsbServer;BusDrop() returned err = %d", err );
				OstTrace0( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP3, "CUsbServer::~CUsbServer; USBMAN will wait until VBus is actually dropped" );
				// Wait 1 second for Hub driver to perform VBus drop
				RTimer timer;
				err = timer.CreateLocal();
				if ( err == KErrNone )
					{
					TRequestStatus tstatus;
					timer.After(tstatus, 1000000);
					User::WaitForRequest(tstatus);
					timer.Close();
					}
				else
					{
					OstTrace1( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP4, "CUsbServer::~CUsbServer;Failed to create local timer: err = %d", err );
					}
				}
			else
				{
				OstTrace0( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP5, "CUsbServer::~CUsbServer; Unexpected: OTG object is NULL" );
				}
			}
		else
			{
			OstTraceExt2( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP6, "CUsbServer::~CUsbServer;VBus is already dropped or an error occured: err = %d, value =%d", err, value );
			}
		}
	else
		{
		OstTraceExt2( TRACE_NORMAL, CUSBSERVER_CUSBSERVER_DUP7, "CUsbServer::~CUsbServer;No Id-Pin is found or an error occured: err = %d, value = %d", err, value );
		}

	delete iUsbOtg;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	OstTraceFunctionExit0( CUSBSERVER_CUSBSERVER_DES_EXIT );
	}


/**
 * The CUsbServer::CUsbServer method
 *
 * Constructor
 *
 * @internalComponent
 */
CUsbServer::CUsbServer()
     : CPolicyServer(EPriorityHigh,KUsbmanServerPolicy)
	{
	}

/**
 * The CUsbServer::ConstructL method
 *
 * 2nd Phase Construction
 *
 * @internalComponent
 */
void CUsbServer::ConstructL()
	{


	iShutdownTimer = new(ELeave) CShutdownTimer;
	iShutdownTimer->ConstructL(); 

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	iUsbOtg = CUsbOtg::NewL();
	iUsbOtg->StartL();
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

	iUsbDevice = CUsbDevice::NewL(*this);
	OstTrace0( TRACE_NORMAL, CUSBSERVER_CONSTRUCTL, "CUsbServer::ConstructL; About to load USB classes" );
	iUsbDevice->EnumerateClassControllersL();

#ifndef USE_DUMMY_CLASS_CONTROLLER	
	iUsbDevice->ReadPersonalitiesL();
	if (iUsbDevice->isPersonalityCfged())
		{
#ifndef __OVER_DUMMYUSBDI__
#ifdef _DEBUG
		iUsbDevice->ValidatePersonalities();
#endif
#endif
		iUsbDevice->SetDefaultPersonalityL();		
		}
	else  
		{
		OstTrace0( TRACE_NORMAL, CUSBSERVER_CONSTRUCTL_DUP1, "CUsbServer::ConstructL;Personalities unconfigured, so using fallback CCs" );
		iUsbDevice->LoadFallbackClassControllersL();
		}
#else // USE_DUMMY_CLASS_CONTROLLER
	OstTrace0( TRACE_NORMAL, CUSBSERVER_CONSTRUCTL_DUP2, "CUsbServer::ConstructL; Using Dummy Class Controller, so using fallback CCs" );
	iUsbDevice->LoadFallbackClassControllersL();
#endif // USE_DUMMY_CLASS_CONTROLLER		

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	iUsbHost = CUsbHost::NewL(*this);
	//previously this was moved to CUsbSession:StartDeviceL() and similar
	//But it will cause the loading of personality longer.
	//So it is moved back here.
	iUsbHost->StartL();
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

	OstTrace0( TRACE_NORMAL, CUSBSERVER_CONSTRUCTL_DUP3, "CUsbServer::ConstructL; CUsbServer constructed" );
	}


/**
 * The CUsbServer::NewSessionL method
 *
 * Create a new client on this server
 *
 * @internalComponent
 * @param	&aVersion	Vesion of client
 * @param  	&aMessage 	Client's IPC message
 *
 * @return	A new USB session to be used for the client
 */
CSession2* CUsbServer::NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const
	{
	OstTraceFunctionEntry0( CUSBSERVER_NEWSESSIONL_ENTRY );

	(void)aMessage;//Remove compiler warning
	
	TVersion v(KUsbSrvMajorVersionNumber,KUsbSrvMinorVersionNumber,KUsbSrvBuildVersionNumber);

	OstTrace0( TRACE_NORMAL, CUSBSERVER_NEWSESSIONL, "CUsbServer::NewSessionL; CUsbServer::NewSessionL - creating new session..." );
	if (!User::QueryVersionSupported(v, aVersion))
		{
        OstTrace1( TRACE_NORMAL, CUSBSERVER_NEWSESSIONL_DUP1, "CUsbServer::NewSessionL;leave reason=%d", KErrNotSupported );
		User::Leave(KErrNotSupported);
		}

	CUsbServer* ncThis = const_cast<CUsbServer*>(this);
	
	CUsbSession* sess = CUsbSession::NewL(ncThis);
		
	OstTraceFunctionExit0( CUSBSERVER_NEWSESSIONL_EXIT );
	return sess;
	}


/**
 * Inform the client there has been an error.
 *
 * @param	aError	The error that has occurred
 */
void CUsbServer::Error(TInt aError)
	{
	OstTrace1( TRACE_NORMAL, CUSBSERVER_ERROR, "CUsbServer::Error;aError=%d", aError );

	Message().Complete(aError);
	ReStart();
	}

/**
 * Increment the open session count (iSessionCount) by one.
 * 
 * @post	the number of open sessions is incremented by one
 */
void CUsbServer::IncrementSessionCount()
	{
	OstTraceFunctionEntry0( CUSBSERVER_INCREMENTSESSIONCOUNT_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBSERVER_INCREMENTSESSIONCOUNT, "CUsbServer::IncrementSessionCount;iSessionCount=%d", iSessionCount );
	if(iSessionCount < 0)
	    {
        OstTrace1( TRACE_FATAL, CUSBSERVER_INCREMENTSESSIONCOUNT_DUP1, "CUsbServer::IncrementSessionCount;panic code=%d", EICSInvalidCount );
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbSvrPncCat, EICSInvalidCount));
	    }
	
	++iSessionCount;
	iShutdownTimer->Cancel();

	OstTraceFunctionExit0( CUSBSERVER_INCREMENTSESSIONCOUNT_EXIT );
	}

/**
 * Decrement the open session count (iSessionCount) by one.
 * 
 * @post		the number of open sessions is decremented by one
 */
void CUsbServer::DecrementSessionCount()
	{
	OstTraceExt2( TRACE_NORMAL, CUSBSERVER_DECREMENTSESSIONCOUNT, "CUsbServer::DecrementSessionCount;iSessionCount=%d;Device().ServiceState()=%d", iSessionCount, Device().ServiceState() );
	if(iSessionCount <= 0)
	    {
        OstTrace1( TRACE_FATAL, CUSBSERVER_DECREMENTSESSIONCOUNT_DUP1, "CUsbServer::DecrementSessionCount;panic code=%d", EDCSInvalidCount );
        __ASSERT_DEBUG(EFalse, User::Panic(KUsbSvrPncCat, EDCSInvalidCount));
	    }
	
	--iSessionCount;
	
	if (iSessionCount == 0 && Device().ServiceState() == EUsbServiceIdle)
		{
		iShutdownTimer->After(KShutdownDelay);
		}
	}

/**
 * If there are no sessions then launch the shutdown timer.  This function
 * is provided for the case where the sole session stops the classes but dies
 * before they are completely stopped.  The server must then be shut down
 * from CUsbDevice::SetServiceState().
 *
 * @pre		the services have been stopped.
 * @see		CUsbDevice::SetServiceStateIdle
 */
void CUsbServer::LaunchShutdownTimerIfNoSessions()
	{
	OstTraceFunctionEntry0( CUSBSERVER_LAUNCHSHUTDOWNTIMERIFNOSESSIONS_ENTRY );
#ifdef _DEBUG
	if(Device().ServiceState() != EUsbServiceIdle)
	    {
        OstTrace1( TRACE_FATAL, CUSBSERVER_LAUNCHSHUTDOWNTIMERIFNOSESSIONS, "CUsbServer::LaunchShutdownTimerIfNoSessions;panic code=%d", ELSTNSNotIdle );
        User::Panic(KUsbSvrPncCat, ELSTNSNotIdle);
	    }
#endif
	if (iSessionCount == 0)
		iShutdownTimer->After(KShutdownDelay);
	OstTraceFunctionExit0( CUSBSERVER_LAUNCHSHUTDOWNTIMERIFNOSESSIONS_EXIT );
	}

/**
 * Initialize this shutdown timer as a normal-priority
 * (EPriorityStandard) active object.
 */
CUsbServer::CShutdownTimer::CShutdownTimer()
:	CTimer(EPriorityStandard)
	{
	CActiveScheduler::Add(this);
	}

/**
 * Forwarding function call's CTimer's ConstructL() to initialize the RTimer.
 */
void CUsbServer::CShutdownTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

/**
 * Server shutdown callback.  This stops the active scheduler,
 * and so closes down the server.
 */
void CUsbServer::CShutdownTimer::RunL()
	{
	CActiveScheduler::Stop();
	}

