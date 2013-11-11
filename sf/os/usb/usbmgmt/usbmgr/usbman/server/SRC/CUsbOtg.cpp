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
* Implements the object of Usbman that manages all the USB OTG
* activity (via CUsbOtgWatcher).
*
*/

/**
 @file
*/

#include <e32property.h> //Publish & Subscribe header
#include "CUsbOtg.h"
#include "cusbotgwatcher.h"
#include "CUsbDevice.h"
#include "musbotghostnotifyobserver.h"
#include "CUsbServer.h"
#include "usberrors.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbOtgTraces.h"
#endif


#ifndef __OVER_DUMMYUSBDI__
//Name used in call to User::LoadLogicalDevice/User::FreeLogicalDevice
_LIT(KUsbOtgLDDName,"otgdi");
#endif



CUsbOtg* CUsbOtg::NewL()
/**
 * Constructs a CUsbOtg object.
 *
 * @return	A new CUsbOtg object
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_NEWL_ENTRY );

	CUsbOtg* self = new (ELeave) CUsbOtg();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBOTG_NEWL_EXIT );
	return self;
	}


CUsbOtg::~CUsbOtg()
/**
 * Destructor.
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_CUSBOTG_DES_ENTRY );
	// Cancel any outstanding asynchronous operation.
	Stop();
	
	// Free any memory allocated to the list of observers. Note that
	// we don't want to call ResetAndDestroy, because we don't own
	// the observers themselves.
	iObservers.Reset();
	
	OstTrace1( TRACE_NORMAL, CUSBOTG_CUSBOTG, "CUsbOtg::~CUsbOtg;iIdPinWatcher=%08x", (TUint32)iIdPinWatcher );
	
	if (iIdPinWatcher)
		{
		iIdPinWatcher->Cancel();
		delete iIdPinWatcher;
		iIdPinWatcher = NULL;
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP1, "CUsbOtg::~CUsbOtg;deleted Id-Pin watcher" );
		
		}
	OstTrace1( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP2, "CUsbOtg::~CUsbOtg;about to stop Vbus watcher @ %08x", (TUint32)iVbusWatcher );
	
	if (iVbusWatcher)
		{
		iVbusWatcher->Cancel();
		delete iVbusWatcher;
		iVbusWatcher = NULL;
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP3, "CUsbOtg::~CUsbOtg;deleted Vbus watcher" );
		
		}
	OstTrace1( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP4, "CUsbOtg::~CUsbOtg;about to stop OTG State watcher @ %08x", (TUint32) iVbusWatcher );
	
	if (iOtgStateWatcher)
		{
		iOtgStateWatcher->Cancel();
		delete iOtgStateWatcher;
		iOtgStateWatcher = NULL;
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP5, "CUsbOtg::~CUsbOtg;deleted OTG State watcher" );
		
		}
	OstTrace1( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP6, "CUsbOtg::~CUsbOtg;about to stop OTG Event watcher @ %08x", (TUint32) iVbusWatcher );
	
	if (iOtgEventWatcher)
		{
		iOtgEventWatcher->Cancel();
		delete iOtgEventWatcher;
		iOtgEventWatcher = NULL;
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP7, "CUsbOtg::~CUsbOtg;deleted OTG Event watcher" );

		}
	
	if (iRequestSessionWatcher)
		{
		delete iRequestSessionWatcher;
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP8, "CUsbOtg::~CUsbOtg;deleted Session Request watcher" );
		
		}

	OstTrace1( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP9, 
	        "CUsbOtg::~CUsbOtg;about to stop Connection Idle watcher @ %08x", (TUint32)iOtgConnectionIdleWatcher );
	
	if (iOtgConnectionIdleWatcher)
		{
		iOtgConnectionIdleWatcher->Cancel();
		delete iOtgConnectionIdleWatcher;
		iOtgConnectionIdleWatcher= NULL;
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP10, "CUsbOtg::~CUsbOtg;deleted Connection Idle watcher" );
		
		}

	// Unload OTGDI components if it was ever started
	if ( iOtgDriver.Handle() )
		{
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP11, "CUsbOtg::~CUsbOtg; Stopping stacks" );
		
		iOtgDriver.StopStacks();
		iOtgDriver.Close();
		}
	else
		{
		OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP12, "CUsbOtg::~CUsbOtg; No OTG Driver session was opened, nothing to do" );
		
		}

#ifndef __OVER_DUMMYUSBDI__
	OstTrace0( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP13, "CUsbOtg::~CUsbOtg; Freeing logical device" );
	
	TInt err = User::FreeLogicalDevice(KUsbOtgLDDName);
	//Putting the LOGTEXT2 inside the if statement prevents a compiler
	//warning about err being unused in UREL builds.
	if(err)
		{
		OstTrace1( TRACE_NORMAL, CUSBOTG_CUSBOTG_DUP14, "CUsbOtg::~CUsbOtg;     User::FreeLogicalDevice returned %d", err );
		
		}
#endif
	
	iCriticalSection.Close();
	OstTraceFunctionExit0( CUSBOTG_CUSBOTG_DES_EXIT );
	}


CUsbOtg::CUsbOtg()
/**
 * Constructor.
 */
	{
    OstTraceFunctionEntry0( CUSBOTG_CUSBOTG_CONS_ENTRY );
    
	OstTraceFunctionExit0( CUSBOTG_CUSBOTG_CONS_EXIT );
	}


void CUsbOtg::ConstructL()
/**
 * Performs 2nd phase construction of the OTG object.
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_CONSTRUCTL_ENTRY );
	
#ifndef __OVER_DUMMYUSBDI__
	OstTrace0( TRACE_NORMAL, CUSBOTG_CONSTRUCTL, "CUsbOtg::ConstructL; About to open LDD" );
	
	iLastError = User::LoadLogicalDevice(KUsbOtgLDDName);
	if ( (iLastError != KErrNone) && (iLastError != KErrAlreadyExists) )
		{
        if(iLastError < 0)
            {
            OstTraceExt2( TRACE_NORMAL, CUSBOTG_CONSTRUCTL_DUP1, "CUsbOtg::ConstructL; Error %d: Unable to load driver: %S", iLastError, KUsbOtgLDDName );
            User::Leave(iLastError);
            }
		}
#endif
	
	OstTrace0( TRACE_NORMAL, CUSBOTG_CONSTRUCTL_DUP3, "CUsbOtg::ConstructL; About to open RUsbOtgDriver" );
	
	iLastError = iOtgDriver.Open();
	if ( (iLastError != KErrNone) && (iLastError != KErrAlreadyExists) )
		{
        if(iLastError < 0)
            {
            OstTrace1( TRACE_NORMAL, CUSBOTG_CONSTRUCTL_DUP4, "CUsbOtg::ConstructL; Error %d: Unable to open RUsbOtgDriver session", iLastError );
            User::Leave(iLastError);
            }	
		}

	OstTrace0( TRACE_NORMAL, CUSBOTG_CONSTRUCTL_DUP2, "CUsbOtg::ConstructL; About to start OTG stacks" );
	
	iLastError = iOtgDriver.StartStacks();
	if (iLastError != KErrNone)
		{
        if(iLastError < 0)
            {
            OstTrace1( TRACE_NORMAL, CUSBOTG_CONSTRUCTL_DUP5, "CUsbOtg::ConstructL; Error %d: Unable to open start OTG stacks", iLastError );
            User::Leave(iLastError);
            }
		}

	// Request Otg notifications
	iIdPinWatcher = CUsbOtgIdPinWatcher::NewL(iOtgDriver);
	iIdPinWatcher->Start();

	iVbusWatcher = CUsbOtgVbusWatcher::NewL(iOtgDriver);
	iVbusWatcher->Start();

	iOtgStateWatcher = CUsbOtgStateWatcher::NewL(iOtgDriver);
	iOtgStateWatcher->Start();
	
	iOtgEventWatcher = CUsbOtgEventWatcher::NewL(*this, iOtgDriver, iOtgEvent);
	iOtgEventWatcher->Start();
	
    iOtgConnectionIdleWatcher = CUsbOtgConnectionIdleWatcher::NewL(iOtgDriver);
    iOtgConnectionIdleWatcher->Start();

	iRequestSessionWatcher = CRequestSessionWatcher::NewL(*this);
	
	iCriticalSection.CreateLocal(EOwnerProcess);
	
	OstTrace0( TRACE_NORMAL, CUSBOTG_CONSTRUCTL_DUP6, "CUsbOtg::ConstructL; UsbOtg::ConstructL() finished" );
	
	OstTraceFunctionExit0( CUSBOTG_CONSTRUCTL_EXIT );
	}
	
void CUsbOtg::NotifyMessage(TInt aMessage)
/**
 * The CUsbOtg::NotifyMessage method
 *
 * Reports the OTG message to the observers
 *
 * @internalComponent
 */
	{
	iCriticalSection.Wait();

	TInt msg = aMessage == 0 ? iOtgMessage : aMessage;
	TUint length = iObservers.Count();
	for (TUint i = 0; i < length; i++)
		{
		iObservers[i]->UsbOtgHostMessage(msg);
		}

	iCriticalSection.Signal();
	}

TInt CUsbOtg::TranslateOtgEvent()
/**
 * The CUsbOtg::TranslateOtgEvent method
 *
 * Attempts to translate the OTG event into OTG message
 *
 * @internalComponent
 */
	{
	TInt otgEvent = KErrBadName;
	switch (iOtgEvent)
	{
	case RUsbOtgDriver::EEventHnpDisabled:
		otgEvent = KUsbMessageHnpDisabled;
		break;
	case RUsbOtgDriver::EEventHnpEnabled:
		otgEvent = KUsbMessageHnpEnabled;
		break;
	case RUsbOtgDriver::EEventSrpReceived:
		otgEvent = KUsbMessageSrpReceived;
		break;
	case RUsbOtgDriver::EEventSrpInitiated:
		otgEvent = KUsbMessageSrpInitiated;
		break;
	case RUsbOtgDriver::EEventVbusRaised:
		otgEvent = KUsbMessageVbusRaised;
		break;
	case RUsbOtgDriver::EEventVbusDropped:
		otgEvent = KUsbMessageVbusDropped;
		break;
	}

	return otgEvent;
	}

void CUsbOtg::NotifyOtgEvent()
/**
 * The CUsbOtg::NotifyOtgEvent method
 *
 * Reports the OTG message translated from OTG Event to the observers
 *
 * @internalComponent
 */
	{
	TUint length = iObservers.Count();
	TInt otgEvent = TranslateOtgEvent();
	if ( otgEvent == KErrBadName )
		{
		OstTrace1( TRACE_NORMAL, CUSBOTG_NOTIFYOTGEVENT,
		        "CUsbOtg::NotifyOtgEvent;OTG event %d was reported, but not propagated", (TInt)iOtgEvent );
		
		return;
		}

	for (TUint i = 0; i < length; i++)
		{
		iObservers[i]->UsbOtgHostMessage(otgEvent);
		}
	}

void CUsbOtg::RegisterObserverL(MUsbOtgHostNotifyObserver& aObserver)
/**
 * Register an observer of the OTG events.
 * Presently, the device supports watching state.
 *
 * @param	aObserver	New Observer of the OTG events
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_REGISTEROBSERVERL_ENTRY );

    TInt err = iObservers.Append(&aObserver);
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBOTG_REGISTEROBSERVERL, "CUsbOtg::RegisterObserverL; iObservers.Append(&aObserver) error, Leave error=%d", err );
        User::Leave(err);
        }

	OstTraceFunctionExit0( CUSBOTG_REGISTEROBSERVERL_EXIT );
	}


void CUsbOtg::DeRegisterObserver(MUsbOtgHostNotifyObserver& aObserver)
/**
 * De-registers an existing OTG events observer.
 *
 * @param	aObserver	The existing OTG events observer to be de-registered
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_DEREGISTEROBSERVER_ENTRY );

	TInt index = iObservers.Find(&aObserver);

	if (index >= 0 && index < iObservers.Count())
		{
		iObservers.Remove(index);
		}
	OstTraceFunctionExit0( CUSBOTG_DEREGISTEROBSERVER_EXIT );
	}


void CUsbOtg::StartL()
/**
 * Start the USB OTG events watcher
 * Reports errors and OTG events via observer interface.
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_STARTL_ENTRY );

	iOtgWatcher = CUsbOtgWatcher::NewL(*this, iOtgDriver, iOtgMessage);
	iOtgWatcher->Start();
	OstTraceFunctionExit0( CUSBOTG_STARTL_EXIT );
	}

void CUsbOtg::Stop()
/**
 * Stop the USB OTG events watcher
 */
	{
	OstTraceFunctionEntry0( CUSBOTG_STOP_ENTRY );

	OstTrace1( TRACE_NORMAL, CUSBOTG_STOP, "CUsbOtg::Stop;about to stop OTG watcher @ %08x", (TUint32)iOtgWatcher );
	
	
	if (iOtgWatcher)
		{
		iOtgWatcher->Cancel();
		delete iOtgWatcher;
		iOtgWatcher = NULL;
		OstTrace0( TRACE_NORMAL, CUSBOTG_STOP_DUP1, "CUsbOtg::Stop" );
		
		}
	
	iLastError = KErrNone;
	OstTraceFunctionExit0( CUSBOTG_STOP_EXIT );
	}

TInt CUsbOtg::BusRequest()
	{
	OstTraceFunctionEntry0( CUSBOTG_BUSREQUEST_ENTRY );
	return iOtgDriver.BusRequest();
	}
	
TInt CUsbOtg::BusRespondSrp()
	{
	OstTraceFunctionEntry0( CUSBOTG_BUSRESPONDSRP_ENTRY );
	return iOtgDriver.BusRespondSrp();
	}

TInt CUsbOtg::BusClearError()
	{
	OstTraceFunctionEntry0( CUSBOTG_BUSCLEARERROR_ENTRY );
	return iOtgDriver.BusClearError();
	}

TInt CUsbOtg::BusDrop()
	{
	OstTraceFunctionEntry0( CUSBOTG_BUSDROP_ENTRY );
	return iOtgDriver.BusDrop();
	}
