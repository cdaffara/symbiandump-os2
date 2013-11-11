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
* Talks directly to the USB Logical Device Driver (LDD) and 
* watches any state changes
*
*/

/**
 @file
*/

#include <usb/usbshared.h>
#include <usb/usblogger.h>
#include "CUsbScheduler.h"
#include "cusbotgwatcher.h"
#include "CUsbOtg.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbotgwatcherTraces.h"
#endif


static _LIT_SECURITY_POLICY_PASS(KAllowAllPolicy);
static _LIT_SECURITY_POLICY_S1(KNetworkControlPolicy,KUsbmanSvrSid,ECapabilityNetworkControl);
static _LIT_SECURITY_POLICY_C1(KRequestSessionPolicy,ECapabilityCommDD);

//-----------------------------------------------------------------------------
//------------------------------ Helper watchers ------------------------------ 
//-----------------------------------------------------------------------------
//--------------------- Base class for all helper watchers -------------------- 
/**
 * The CUsbOtgBaseWatcher::CUsbOtgBaseWatcher method
 *
 * Constructor
 *
 * @param	aOwner	The device that owns the state watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 */
CUsbOtgBaseWatcher::CUsbOtgBaseWatcher(RUsbOtgDriver& aLdd)
	: CActive(CActive::EPriorityStandard), iLdd(aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGBASEWATCHER_CUSBOTGBASEWATCHER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CUSBOTGBASEWATCHER_CUSBOTGBASEWATCHER_CONS_EXIT );
	}

/**
 * The CUsbOtgBaseWatcher::~CUsbOtgBaseWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgBaseWatcher::~CUsbOtgBaseWatcher()
	{
	OstTraceFunctionEntry0( CUSBOTGBASEWATCHER_CUSBOTGBASEWATCHER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CUSBOTGBASEWATCHER_CUSBOTGBASEWATCHER_DES_EXIT );
	}

/**
 * Instructs the state watcher to start watching.
 */
void CUsbOtgBaseWatcher::Start()
	{
	OstTraceFunctionEntry0( CUSBOTGBASEWATCHER_START_ENTRY );
	Post();
	OstTraceFunctionExit0( CUSBOTGBASEWATCHER_START_EXIT );
	}

//---------------------------- Id-Pin watcher class --------------------------- 
/**
 * The CUsbOtgIdPinWatcher::NewL method
 *
 * Constructs a new CUsbOtgWatcher object
 *
 * @internalComponent
 * @param	aLdd	A reference to the USB Logical Device Driver
 *
 * @return	A new CUsbOtgWatcher object
 */
CUsbOtgIdPinWatcher* CUsbOtgIdPinWatcher::NewL(RUsbOtgDriver& aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_NEWL_ENTRY );

	CUsbOtgIdPinWatcher* self = new (ELeave) CUsbOtgIdPinWatcher(aLdd);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_NEWL_EXIT );
	return self;
	}


/**
 * The CUsbOtgIdPinWatcher::~CUsbOtgIdPinWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgIdPinWatcher::~CUsbOtgIdPinWatcher()
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_CUSBOTGIDPINWATCHER_DES_ENTRY );
	Cancel();
	RProperty::Delete(KUsbOtgIdPinPresentProperty);
	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_CUSBOTGIDPINWATCHER_DES_EXIT );
	}

void CUsbOtgIdPinWatcher::ConstructL()
/**
 * Performs 2nd phase construction of the OTG object.
 */
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_CONSTRUCTL_ENTRY );

	TInt err = RProperty::Define(KUsbOtgIdPinPresentProperty, RProperty::EInt, KAllowAllPolicy, KNetworkControlPolicy);
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    User::LeaveIfError(err);
	    }
	err = RProperty::Set(KUidUsbManCategory,KUsbOtgIdPinPresentProperty,EFalse);
	if ( err != KErrNone )
	    {
	    User::LeaveIfError(err);
	    }
	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_CONSTRUCTL_EXIT );
	}

/**
 * The CUsbOtgIdPinWatcher::CUsbOtgIdPinWatcher method
 *
 * Constructor
 *
 * @param	aLdd	A reference to the USB Logical Device Driver
 */

CUsbOtgIdPinWatcher::CUsbOtgIdPinWatcher(RUsbOtgDriver& aLdd)
	: CUsbOtgBaseWatcher(aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_CUSBOTGIDPINWATCHER_CONS_ENTRY );
	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_CUSBOTGIDPINWATCHER_CONS_EXIT );
	}

/**
 * Called when the ID-Pin status change is reported
 */
void CUsbOtgIdPinWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_RUNL, "CUsbOtgIdPinWatcher::RunL;iStatus=%d", iStatus.Int() );

    TInt err = iStatus.Int();
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_RUNL_DUP1, "CUsbOtgIdPinWatcher::RunL;iStatus.Int() with error=%d", err );
        User::Leave(err);
        }
	
	Post();

	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_RUNL_EXIT );
	}


/**
 * Automatically called when the ID-Pin watcher is cancelled.
 */
void CUsbOtgIdPinWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_DOCANCEL_ENTRY );
	iLdd.CancelOtgIdPinNotification();
	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_DOCANCEL_EXIT );
	}

/**
 * Sets state watcher in active state
 */
void CUsbOtgIdPinWatcher::Post()
	{
	OstTraceFunctionEntry0( CUSBOTGIDPINWATCHER_POST_ENTRY );

	OstTrace0( TRACE_NORMAL, CUSBOTGIDPINWATCHER_POST, "CUsbOtgIdPinWatcher::Post - About to call QueueOtgIdPinNotification" );
	
	iLdd.QueueOtgIdPinNotification(iOtgIdPin, iStatus);
	switch (iOtgIdPin)
		{
		case RUsbOtgDriver::EIdPinAPlug:
			if (RProperty::Set(KUidUsbManCategory,KUsbOtgIdPinPresentProperty,ETrue) != KErrNone)
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_POST_DUP1, 
				        "CUsbOtgIdPinWatcher::Post; [iOtgIdPin=%d] - failed to set the property value", iOtgIdPin );				
				}
			else
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_POST_DUP2, 
				        "CUsbOtgIdPinWatcher::Post; [iOtgIdPin=%d] - property is set to 1", iOtgIdPin );				
				}
			break;
		case RUsbOtgDriver::EIdPinBPlug:
		case RUsbOtgDriver::EIdPinUnknown:
			if (RProperty::Set(KUidUsbManCategory,KUsbOtgIdPinPresentProperty,EFalse) != KErrNone)
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_POST_DUP3, 
				        "CUsbOtgIdPinWatcher::Post; [iOtgIdPin=%d] - failed to set the property value", iOtgIdPin );				
				}
			else
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_POST_DUP4, 
				        "CUsbOtgIdPinWatcher::Post; [iOtgIdPin=%d] - property is set to 0", iOtgIdPin );				
				}
			break;
		default:
			OstTrace1( TRACE_NORMAL, CUSBOTGIDPINWATCHER_POST_DUP5, 
			        "CUsbOtgIdPinWatcher::Post; [iOtgIdPin=%d] is unrecognized, re-request QueueOtgIdPinNotification", iOtgIdPin );
			break;
		}
	SetActive();
	OstTraceFunctionExit0( CUSBOTGIDPINWATCHER_POST_EXIT );
	}

//----------------------------- VBus watcher class ---------------------------- 
/**
 * The CUsbOtgVbusWatcher::NewL method
 *
 * Constructs a new CUsbOtgVbusWatcher object
 *
 * @internalComponent
 * @param	aLdd	A reference to the USB OTG Logical Device Driver
 *
 * @return	A new CUsbOtgVbusWatcher object
 */
CUsbOtgVbusWatcher* CUsbOtgVbusWatcher::NewL(RUsbOtgDriver& aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_NEWL_ENTRY );

	CUsbOtgVbusWatcher* self = new (ELeave) CUsbOtgVbusWatcher(aLdd);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_NEWL_EXIT );
	return self;
	}


/**
 * The CUsbOtgVbusWatcher::~CUsbOtgVbusWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgVbusWatcher::~CUsbOtgVbusWatcher()
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_CUSBOTGVBUSWATCHER_DES_ENTRY );
	Cancel();

	RProperty::Delete(KUsbOtgVBusPoweredProperty);
	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_CUSBOTGVBUSWATCHER_DES_EXIT );
	}

void CUsbOtgVbusWatcher::ConstructL()
/**
 * Performs 2nd phase construction of the OTG object.
 */
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_CONSTRUCTL_ENTRY );

	TInt err = RProperty::Define(KUsbOtgVBusPoweredProperty, RProperty::EInt, KAllowAllPolicy, KNetworkControlPolicy);
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    User::LeaveIfError(err);
	    }
	err = RProperty::Set(KUidUsbManCategory,KUsbOtgVBusPoweredProperty,EFalse);
	if ( err != KErrNone )
	    {
	    User::LeaveIfError(err);
	    }
	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_CONSTRUCTL_EXIT );
	}

/**
 * The CUsbOtgVbusWatcher::CUsbOtgVbusWatcher method
 *
 * Constructor
 *
 * @param	aLdd	A reference to the USB OTG Logical Device Driver
 */
CUsbOtgVbusWatcher::CUsbOtgVbusWatcher(RUsbOtgDriver& aLdd)
	: CUsbOtgBaseWatcher(aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_CUSBOTGVBUSWATCHER_CONS_ENTRY );
	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_CUSBOTGVBUSWATCHER_CONS_EXIT );
	}

/**
 * Called when the Vbus status is changed
 */
void CUsbOtgVbusWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_RUNL, "CUsbOtgVbusWatcher::RunL;iStatus=%d", iStatus.Int() );

    TInt err = iStatus.Int();
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_RUNL_DUP1, "CUsbOtgVbusWatcher::RunL;iStatus.Int() with error=%d", err );
        User::Leave(err);
        }

	Post();

	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_RUNL_EXIT );
	}


/**
 * Automatically called when the VBus status watcher is cancelled.
 */
void CUsbOtgVbusWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_DOCANCEL_ENTRY );
	iLdd.CancelOtgVbusNotification();
	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_DOCANCEL_EXIT );
	}

/**
 * Sets state watcher in active state
 */
void CUsbOtgVbusWatcher::Post()
	{
	OstTraceFunctionEntry0( CUSBOTGVBUSWATCHER_POST_ENTRY );

	OstTrace0( TRACE_NORMAL, CUSBOTGVBUSWATCHER_POST, "CUsbOtgVbusWatcher::Post - About to call QueueOtgVbusNotification" );
	
	iLdd.QueueOtgVbusNotification(iOtgVbus, iStatus);
	switch (iOtgVbus)
		{
		case RUsbOtgDriver::EVbusHigh:
			if (RProperty::Set(KUidUsbManCategory,KUsbOtgVBusPoweredProperty,ETrue) != KErrNone)
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_POST_DUP1, 
				        "CUsbOtgVbusWatcher::Post;[iOtgVbus=%d](EVbusHigh) - failed to set the property value", iOtgVbus );
				}
			else
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_POST_DUP2, 
				        "CUsbOtgVbusWatcher::Post;[iOtgVbus=%d](EVbusHigh) - property is set to ETrue", iOtgVbus );
				}
			break;
		case RUsbOtgDriver::EVbusLow:
		case RUsbOtgDriver::EVbusUnknown:
			if (RProperty::Set(KUidUsbManCategory,KUsbOtgVBusPoweredProperty,EFalse) != KErrNone)
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_POST_DUP3, 
				        "CUsbOtgVbusWatcher::Post;[iOtgVbus=%d](1 - EVbusLow, 2 - EVbusUnknown) - failed to set the property value", iOtgVbus );
				}
			else
				{
				OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_POST_DUP4, 
				        "CUsbOtgVbusWatcher::Post;[iOtgVbus=%d](1 - EVbusLow, 2 - EVbusUnknown) - property is set to EFalse", iOtgVbus );
				}
			break;
		default:
			OstTrace1( TRACE_NORMAL, CUSBOTGVBUSWATCHER_POST_DUP5, 
			        "CUsbOtgVbusWatcher::Post;iOtgVbus=%d] is unrecognized, re-request QueueOtgVbusNotification", iOtgVbus );
			break;
		}
	SetActive();
	OstTraceFunctionExit0( CUSBOTGVBUSWATCHER_POST_EXIT );
	}


//-------------------------- OTG State watcher class -------------------------- 
/**
 * The CUsbOtgStateWatcher::NewL method
 *
 * Constructs a new CUsbOtgWatcher object
 *
 * @internalComponent
 * @param	aLdd	A reference to the USB Logical Device Driver
 *
 * @return	A new CUsbOtgWatcher object
 */
CUsbOtgStateWatcher* CUsbOtgStateWatcher::NewL(RUsbOtgDriver& aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_NEWL_ENTRY );

	CUsbOtgStateWatcher* self = new (ELeave) CUsbOtgStateWatcher(aLdd);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_NEWL_EXIT );
	return self;
	}


/**
 * The CUsbOtgStateWatcher::~CUsbOtgStateWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgStateWatcher::~CUsbOtgStateWatcher()
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_CUSBOTGSTATEWATCHER_DES_ENTRY );
	Cancel();
	RProperty::Delete(KUsbOtgStateProperty);
	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_CUSBOTGSTATEWATCHER_DES_EXIT );
	}

void CUsbOtgStateWatcher::ConstructL()
/**
 * Performs 2nd phase construction of the OTG object.
 */
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_CONSTRUCTL_ENTRY );

	TInt err = RProperty::Define(KUsbOtgStateProperty, RProperty::EInt, KAllowAllPolicy, KNetworkControlPolicy);
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    User::LeaveIfError(err);
	    }
	err = RProperty::Set(KUidUsbManCategory,KUsbOtgStateProperty,RUsbOtgDriver::EStateReset);
	if ( err != KErrNone )
	    {
	    User::LeaveIfError(err);
	    }
	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_CONSTRUCTL_EXIT );
	}

/**
 * The CUsbOtgIdPinWatcher::CUsbOtgIdPinWatcher method
 *
 * Constructor
 *
 * @param	aLdd	A reference to the USB Logical Device Driver
 */

CUsbOtgStateWatcher::CUsbOtgStateWatcher(RUsbOtgDriver& aLdd)
	: CUsbOtgBaseWatcher(aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_CUSBOTGSTATEWATCHER_CONS_ENTRY );
	iOtgState = RUsbOtgDriver::EStateReset;
	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_CUSBOTGSTATEWATCHER_CONS_EXIT );
	}

/**
 * Called when the OTG State change is reported
 */
void CUsbOtgStateWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBOTGSTATEWATCHER_RUNL, "CUsbOtgStateWatcher::RunL;iStatus.Int()=%d", iStatus.Int() );

    TInt err = iStatus.Int();
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBOTGSTATEWATCHER_RUNL_DUP1, "CUsbOtgStateWatcher::RunL;iStatus.Int() with error=%d", err );
        User::Leave(err);
        }

	Post();

	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_RUNL_EXIT );
	}


/**
 * Automatically called when the OTG State watcher is cancelled.
 */
void CUsbOtgStateWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_DOCANCEL_ENTRY );
	iLdd.CancelOtgStateNotification();
	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_DOCANCEL_EXIT );
	}

/**
 * Sets state watcher in active state
 */
void CUsbOtgStateWatcher::Post()
	{
	OstTraceFunctionEntry0( CUSBOTGSTATEWATCHER_POST_ENTRY );

	OstTrace0( TRACE_NORMAL, CUSBOTGSTATEWATCHER_POST, "CUsbOtgStateWatcher::Post - About to call QueueOtgStateNotification" );
	iLdd.QueueOtgStateNotification(iOtgState, iStatus);
	OstTraceExt2( TRACE_NORMAL, CUSBOTGSTATEWATCHER_POST_DUP1, 
	        "CUsbOtgStateWatcher::Post;[iStatus=%d], iOtgState = %d", iStatus.Int(), iOtgState );
	if (RProperty::Set(KUidUsbManCategory,KUsbOtgStateProperty,(TInt)iOtgState) != KErrNone)
	{
		OstTraceExt2( TRACE_NORMAL, CUSBOTGSTATEWATCHER_POST_DUP2, 
		        "CUsbOtgStateWatcher::Post;[iStatus=%d], iOtgState = %d - failed to set the property", iStatus.Int(), iOtgState );
	}

	SetActive();
	OstTraceFunctionExit0( CUSBOTGSTATEWATCHER_POST_EXIT );
	}

//-------------------------- OTG Events watcher class ------------------------- 
/**
 * The CUsbOtgEventWatcher::NewL method
 *
 * Constructs a new CUsbOtgEventWatcher object
 *
 * @internalComponent
 * @param	aOwner		The CUsbOtg that owns the state watcher
 * @param	aLdd		A reference to the USB Logical Device Driver
 * @param	aOtgEvent	A reference to the OTG Event
 *
 * @return	A new CUsbOtgEventWatcher object
 */
CUsbOtgEventWatcher* CUsbOtgEventWatcher::NewL(CUsbOtg& aOwner, RUsbOtgDriver& aLdd,
											   RUsbOtgDriver::TOtgEvent& aOtgEvent)
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_NEWL_ENTRY );

	CUsbOtgEventWatcher* self = new (ELeave) CUsbOtgEventWatcher(aOwner, aLdd, aOtgEvent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_NEWL_EXIT );
	return self;
	}


/**
 * The CUsbOtgEventWatcher::~CUsbOtgEventWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgEventWatcher::~CUsbOtgEventWatcher()
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_CUSBOTGEVENTWATCHER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_CUSBOTGEVENTWATCHER_DES_EXIT );
	}

void CUsbOtgEventWatcher::ConstructL()
/**
 * Performs 2nd phase construction of the OTG object.
 */
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_CONSTRUCTL_ENTRY );
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_CONSTRUCTL_EXIT );
	}

/**
 * The CUsbOtgEventWatcher::CUsbOtgEventWatcher method
 *
 * Constructor
 *
 * @param	aOwner		A reference to the CUsbOtg object that owns the state watcher
 * @param	aLdd		A reference to the USB Logical Device Driver
 * @param	aOtgEvent	A reference to the OTG Event
 */
CUsbOtgEventWatcher::CUsbOtgEventWatcher(CUsbOtg& aOwner, RUsbOtgDriver& aLdd, 
										 RUsbOtgDriver::TOtgEvent& aOtgEvent)
	: CUsbOtgBaseWatcher(aLdd), iOwner(aOwner), iOtgEvent(aOtgEvent)
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_CUSBOTGEVENTWATCHER_CONS_ENTRY );
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_CUSBOTGEVENTWATCHER_CONS_EXIT );
	}

/**
 * Called when the OTG Event is reported
 */
void CUsbOtgEventWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBOTGEVENTWATCHER_RUNL, "CUsbOtgEventWatcher::RunL;iStatus=%d", iStatus.Int() );
    TInt err = iStatus.Int();
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBOTGEVENTWATCHER_RUNL_DUP3, "CUsbOtgEventWatcher::RunL;iStatus.Int() with error=%d", err );
        User::Leave(err);
        }
    
	OstTrace1( TRACE_NORMAL, CUSBOTGEVENTWATCHER_RUNL_DUP1, 
	        "CUsbOtgEventWatcher::RunL; - Otg Event reported: %d", (TInt)iOtgEvent );
	if (  ( iOtgEvent == RUsbOtgDriver::EEventHnpDisabled )
	    ||( iOtgEvent == RUsbOtgDriver::EEventHnpEnabled )
	    ||( iOtgEvent == RUsbOtgDriver::EEventSrpInitiated )
	    ||( iOtgEvent == RUsbOtgDriver::EEventSrpReceived )
	    ||( iOtgEvent == RUsbOtgDriver::EEventVbusRaised )
	    ||( iOtgEvent == RUsbOtgDriver::EEventVbusDropped )
	   )
		{
		iOwner.NotifyOtgEvent();
		OstTrace1( TRACE_NORMAL, CUSBOTGEVENTWATCHER_RUNL_DUP2, 
		        "CUsbOtgEventWatcher::RunL - The owner is notified about Otg Event = %d", (TInt)iOtgEvent );
		}
	Post();
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_RUNL_EXIT );
	}

#ifndef _DEBUG
void CUsbOtgEventWatcher::LogEventText(RUsbOtgDriver::TOtgEvent /*aState*/)
	{
	}
#else
void CUsbOtgEventWatcher::LogEventText(RUsbOtgDriver::TOtgEvent aEvent)
	{
	switch (aEvent)
		{
		case RUsbOtgDriver::EEventAPlugInserted:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT, 
			        "CUsbOtgEventWatcher::LogEventText ***** A-Plug Inserted *****" );
			break;
		case RUsbOtgDriver::EEventAPlugRemoved:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP1, 
			        "CUsbOtgEventWatcher::LogEventText ***** A-Plug Removed *****" );
			break;
		case RUsbOtgDriver::EEventVbusRaised:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP2, 
			        "CUsbOtgEventWatcher::LogEventText ***** VBus Raised *****" );
			break;
		case RUsbOtgDriver::EEventVbusDropped:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP3, 
			        "CUsbOtgEventWatcher::LogEventText ***** VBus Dropped *****" );
			break;
		case RUsbOtgDriver::EEventSrpInitiated:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP4, 
			        "CUsbOtgEventWatcher::LogEventText ***** SRP Initiated *****" );
			break;
		case RUsbOtgDriver::EEventSrpReceived:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP5, 
			        "CUsbOtgEventWatcher::LogEventText ***** SRP Received *****" );
			break;
		case RUsbOtgDriver::EEventHnpEnabled:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP6, 
			        "CUsbOtgEventWatcher::LogEventText ***** HNP Enabled *****" );
			break;
		case RUsbOtgDriver::EEventHnpDisabled:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP7, 
			        "CUsbOtgEventWatcher::LogEventText ***** HNP Disabled *****" );
			break;
		case RUsbOtgDriver::EEventRoleChangedToHost:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP8, 
			        "CUsbOtgEventWatcher::LogEventText ***** Role Changed to Host *****" );
			break;
		case RUsbOtgDriver::EEventRoleChangedToDevice:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP9, 
			        "CUsbOtgEventWatcher::LogEventText ***** Role Changed to Device *****" );
			break;
		case RUsbOtgDriver::EEventRoleChangedToIdle:
			OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_LOGEVENTTEXT_DUP10, 
			        "CUsbOtgEventWatcher::LogEventText ***** Role Changed to Idle *****" );
			break;
		default:
			break;
		}
	}
#endif

/**
 * Automatically called when the OTG Event watcher is cancelled.
 */
void CUsbOtgEventWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_DOCANCEL_ENTRY );
	iLdd.CancelOtgEventRequest();
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_DOCANCEL_EXIT );
	}

/**
 * Sets state watcher in active state
 */
void CUsbOtgEventWatcher::Post()
	{
	OstTraceFunctionEntry0( CUSBOTGEVENTWATCHER_POST_ENTRY );

	OstTrace0( TRACE_NORMAL, CUSBOTGEVENTWATCHER_POST, "CUsbOtgEventWatcher::Post - About to call QueueOtgEventRequest" );
	iLdd.QueueOtgEventRequest(iOtgEvent, iStatus);
	SetActive();
	OstTraceFunctionExit0( CUSBOTGEVENTWATCHER_POST_EXIT );
	}


//-----------------------------------------------------------------------------
//----------------- OTG watcher class to monitor OTG Messages ----------------- 
//-----------------------------------------------------------------------------
/**
 * The CUsbOtgWatcher::NewL method
 *
 * Constructs a new CUsbOtgWatcher object
 *
 * @internalComponent
 * @param	aOwner	A reference to the object that owns the state watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 *
 * @return	A new CUsbOtgWatcher object
 */
CUsbOtgWatcher* CUsbOtgWatcher::NewL(MUsbOtgObserver& aOwner, RUsbOtgDriver& aLdd, TUint& aOtgMessage)
	{
	OstTraceFunctionEntry0( CUSBOTGWATCHER_NEWL_ENTRY );

	CUsbOtgWatcher* r = new (ELeave) CUsbOtgWatcher(aOwner, aLdd, aOtgMessage);
	OstTraceFunctionExit0( CUSBOTGWATCHER_NEWL_EXIT );
	return r;
	}


/**
 * The CUsbOtgWatcher::~CUsbOtgWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgWatcher::~CUsbOtgWatcher()
	{
	OstTraceFunctionEntry1( CUSBOTGWATCHER_CUSBOTGWATCHER_DES_ENTRY, this );
	Cancel();
	}


/**
 * The CUsbOtgWatcher::CUsbOtgWatcher method
 *
 * Constructor
 *
 * @param	aOwner	The device that owns the state watcher
 * @param	aLdd	A reference to the USB Logical Device Driver
 */
CUsbOtgWatcher::CUsbOtgWatcher(MUsbOtgObserver& aOwner, RUsbOtgDriver& aLdd, TUint& aOtgMessage)
	: CActive(CActive::EPriorityStandard), iOwner(aOwner), iLdd(aLdd), iOtgMessage(aOtgMessage)
	{
	OstTraceFunctionEntry0( CUSBOTGWATCHER_CUSBOTGWATCHER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CUSBOTGWATCHER_CUSBOTGWATCHER_CONS_EXIT );
	}

/**
 * Called when the OTG component changes its state.
 */
void CUsbOtgWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBOTGWATCHER_RUNL_ENTRY );
	if (iStatus.Int() != KErrNone)
		{
		OstTrace1( TRACE_NORMAL, CUSBOTGWATCHER_RUNL, "CUsbOtgWatcher::RunL;Error=%d", iStatus.Int() );
		OstTraceFunctionExit0( CUSBOTGWATCHER_RUNL_EXIT );
		return;
		}

	OstTrace1( TRACE_NORMAL, CUSBOTGWATCHER_RUNL_DUP1, "CUsbOtgWatcher::RunL;Otg Message reported: %d", iOtgMessage );
	iOwner.NotifyMessage();

	Post();
	OstTraceFunctionExit0( CUSBOTGWATCHER_RUNL_EXIT_DUP1 );
	}


/**
 * Automatically called when the state watcher is cancelled.
 */
void CUsbOtgWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBOTGWATCHER_DOCANCEL_ENTRY );
	iLdd.CancelOtgMessageRequest();
	OstTraceFunctionExit0( CUSBOTGWATCHER_DOCANCEL_EXIT );
	}


/**
 * Instructs the state watcher to start watching.
 */
void CUsbOtgWatcher::Start()
	{
	OstTraceFunctionEntry0( CUSBOTGWATCHER_START_ENTRY );
	Post();
	OstTraceFunctionExit0( CUSBOTGWATCHER_START_EXIT );
	}

/**
 * Sets state watcher in active state
 */
void CUsbOtgWatcher::Post()
	{
	OstTraceFunctionEntry0( CUSBOTGWATCHER_POST_ENTRY );

	OstTrace0( TRACE_NORMAL, CUSBOTGWATCHER_POST, "CUsbOtgWatcher::Post - About to call QueueOtgMessageRequest" );
	iLdd.QueueOtgMessageRequest((RUsbOtgDriver::TOtgMessage&)iOtgMessage, iStatus);
	SetActive();
	OstTraceFunctionExit0( CUSBOTGWATCHER_POST_EXIT );
	}




//-----------------------------------------------------------------------------
//------ A watcher class to monitor the P&S property for VBus marshalling ----- 
//-----------------------------------------------------------------------------

CRequestSessionWatcher* CRequestSessionWatcher::NewL(MUsbOtgObserver& aOwner)
	{
	CRequestSessionWatcher* self = new(ELeave) CRequestSessionWatcher(aOwner);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CRequestSessionWatcher::~CRequestSessionWatcher()
	{
	Cancel();
	iProp.Close();
	}

void CRequestSessionWatcher::ConstructL()
/**
 * Performs 2nd phase construction of the OTG object.
 */
	{
	OstTraceFunctionEntry0( CREQUESTSESSIONWATCHER_CONSTRUCTL_ENTRY );

	TInt err = RProperty::Define(KUsbRequestSessionProperty, RProperty::EInt, KAllowAllPolicy, KRequestSessionPolicy);
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    User::LeaveIfError(err);
	    }
	err = RProperty::Set(KUidUsbManCategory,KUsbRequestSessionProperty,0);
	if ( err != KErrNone )
	    {
	    User::LeaveIfError(err);
	    }
	User::LeaveIfError(iProp.Attach(KUidUsbManCategory, KUsbRequestSessionProperty));
	iProp.Subscribe(iStatus);
	SetActive();
	OstTraceFunctionExit0( CREQUESTSESSIONWATCHER_CONSTRUCTL_EXIT );
	}

CRequestSessionWatcher::CRequestSessionWatcher(MUsbOtgObserver& aOwner)
	: CActive(CActive::EPriorityStandard), iOwner(aOwner)
	{
	OstTraceFunctionEntry0( CREQUESTSESSIONWATCHER_CREQUESTSESSIONWATCHER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CREQUESTSESSIONWATCHER_CREQUESTSESSIONWATCHER_CONS_EXIT );
	}

/**
 * Called when the OTG Event is reported
 */
void CRequestSessionWatcher::RunL()
	{
	OstTraceFunctionEntry0( CREQUESTSESSIONWATCHER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CREQUESTSESSIONWATCHER_RUNL, "CRequestSessionWatcher::RunL;iStatus=%d", iStatus.Int() );
	RDebug::Printf(">>CRequestSessionWatcher::RunL [iStatus=%d]", iStatus.Int());
	
	iProp.Subscribe(iStatus);
	SetActive();

	TInt val;
	User::LeaveIfError(iProp.Get(val));
	RDebug::Printf(">>value=%d", val);

	iOwner.NotifyMessage(KUsbMessageRequestSession);
	
	OstTraceFunctionExit0( CREQUESTSESSIONWATCHER_RUNL_EXIT );
	}


/**
 * Automatically called when the OTG Event watcher is cancelled.
 */
void CRequestSessionWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CREQUESTSESSIONWATCHER_DOCANCEL_ENTRY );
	iProp.Cancel();
	OstTraceFunctionExit0( CREQUESTSESSIONWATCHER_DOCANCEL_EXIT );
	}

//---------------------------- Connection Idle watcher class --------------------------- 
/**
 * The CUsbOtgConnectionIdleWatcher::NewL method
 *
 * Constructs a new CUsbOtgWatcher object
 *
 * @internalComponent
 * @param	aLdd	A reference to the USB Logical Device Driver
 *
 * @return	A new CUsbOtgWatcher object
 */
CUsbOtgConnectionIdleWatcher* CUsbOtgConnectionIdleWatcher::NewL(RUsbOtgDriver& aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_NEWL_ENTRY );

	CUsbOtgConnectionIdleWatcher* self = new (ELeave) CUsbOtgConnectionIdleWatcher(aLdd);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_NEWL_EXIT );
	return self;
	}


/**
 * The CUsbOtgConnectionIdleWatcher::~CUsbOtgConnectionIdleWatcher method
 *
 * Destructor
 *
 * @internalComponent
 */
CUsbOtgConnectionIdleWatcher::~CUsbOtgConnectionIdleWatcher()
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_CUSBOTGCONNECTIONIDLEWATCHER_DES_ENTRY );
	Cancel();
	RProperty::Delete(KUsbOtgConnectionIdleProperty);
	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_CUSBOTGCONNECTIONIDLEWATCHER_DES_EXIT );
	}

/**
 * Performs 2nd phase construction of the OTG object.
 */
void CUsbOtgConnectionIdleWatcher::ConstructL()
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_CONSTRUCTL_ENTRY );

	TInt err = RProperty::Define(KUsbOtgConnectionIdleProperty, RProperty::EInt, KAllowAllPolicy, KNetworkControlPolicy);
	if ( err != KErrNone && err != KErrAlreadyExists )
	    {
	    User::LeaveIfError(err);
	    }
	err = RProperty::Set(KUidUsbManCategory,KUsbOtgConnectionIdleProperty,ETrue);
	if ( err != KErrNone )
	    {
	    User::LeaveIfError(err);
	    }
	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_CONSTRUCTL_EXIT );
	}

/**
 * The CUsbOtgConnectionIdleWatcher::CUsbOtgConnectionIdleWatcher method
 *
 * Constructor
 *
 * @param	aLdd	A reference to the USB Logical Device Driver
 */

CUsbOtgConnectionIdleWatcher::CUsbOtgConnectionIdleWatcher(RUsbOtgDriver& aLdd)
	: CUsbOtgBaseWatcher(aLdd)
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_CUSBOTGCONNECTIONIDLEWATCHER_CONS_ENTRY );
	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_CUSBOTGCONNECTIONIDLEWATCHER_CONS_EXIT );
	}

/**
 * Called when the Connection Idle status change is reported
 */
void CUsbOtgConnectionIdleWatcher::RunL()
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CUSBOTGCONNECTIONIDLEWATCHER_RUNL, 
	        "CUsbOtgConnectionIdleWatcher::RunL;iStatus=%d", iStatus.Int() );
	
    TInt err = iStatus.Int();
    if(err < 0)
        {
        OstTrace1( TRACE_NORMAL, CUSBOTGCONNECTIONIDLEWATCHER_RUNL_DUP1, "CUsbOtgConnectionIdleWatcher::RunL;iStatus.Int() with error=%d", err );
        User::Leave(err);
        }
    
	Post();

	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_RUNL_EXIT );
	}


/**
 * Automatically called when the Connection Idle watcher is cancelled.
 */
void CUsbOtgConnectionIdleWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_DOCANCEL_ENTRY );
	iLdd.CancelOtgConnectionNotification();
	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_DOCANCEL_EXIT );
	}

/**
 * Sets state watcher in active state
 */
void CUsbOtgConnectionIdleWatcher::Post()
	{
	OstTraceFunctionEntry0( CUSBOTGCONNECTIONIDLEWATCHER_POST_ENTRY );

	OstTrace0( TRACE_NORMAL, CUSBOTGCONNECTIONIDLEWATCHER_POST, 
	        "CUsbOtgConnectionIdleWatcher::Post - About to call QueueOtgIdPinNotification" );
	iLdd.QueueOtgConnectionNotification(iConnectionIdle, iStatus);
	switch (iConnectionIdle)
		{
		case RUsbOtgDriver::EConnectionIdle:
		case RUsbOtgDriver::EConnectionUnknown:
			RProperty::Set(KUidUsbManCategory,KUsbOtgConnectionIdleProperty,ETrue);
			OstTrace1( TRACE_NORMAL, CUSBOTGCONNECTIONIDLEWATCHER_POST_DUP1, 
			        "CUsbOtgConnectionIdleWatcher::Post;[iConnectionIdle=%d] - property is set to 1", iConnectionIdle );
			break;
		case RUsbOtgDriver::EConnectionBusy:
			RProperty::Set(KUidUsbManCategory,KUsbOtgConnectionIdleProperty,EFalse);
			OstTrace1( TRACE_NORMAL, CUSBOTGCONNECTIONIDLEWATCHER_POST_DUP2, 
			        "CUsbOtgConnectionIdleWatcher::Post;[iConnectionIdle=%d] - property is set to 0", iConnectionIdle );
			break;
		default:
			OstTrace1( TRACE_NORMAL, CUSBOTGCONNECTIONIDLEWATCHER_POST_DUP3, 
			        "CUsbOtgConnectionIdleWatcher::Post;[iConnectionIdle=%d] is unrecognized, re-request QueueOtgIdPinNotification", iConnectionIdle );
			break;
		}
	SetActive();
	OstTraceFunctionExit0( CUSBOTGCONNECTIONIDLEWATCHER_POST_EXIT );
	}


