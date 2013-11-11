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
* Implements a Session of a Symbian OS server for the RUsb API
*
*/

/**
 @file
*/

#include <usbstates.h>
#include <usberrors.h>
#include <usb/usbshared.h>
#include <usb/usblogger.h>

#include "CUsbSession.h"
#include "CUsbDevice.h"
#include "CUsbServer.h"

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
#include "CUsbOtg.h"
#include "cusbhost.h"
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

#include "CPersonality.h"
#include "rusb.h"
#include "UsbSettings.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "CUsbSessionTraces.h"
#endif



#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
CUsbSession* CUsbSession::iCtlSession = NULL;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

/**
 * Construct a Symbian OS session object.
 *
 * @internalComponent
 * @param	aServer		Service the session will be a member of
 *
 * @return	A new CUsbSession object
 */
CUsbSession* CUsbSession::NewL(CUsbServer* aServer)
	{
	OstTraceFunctionEntry0( CUSBSESSION_NEWL_ENTRY );

	//this class has moved away from standard NewL() semantics
	//and now uses the virtual CSession2::CreateL() function
	//[instead of ConstructL()] which is called by CServer2
	//and finalises the construction of the session
	return (new (ELeave) CUsbSession(aServer));
	}


/**
 * Constructor.
 *
 * @internalComponent
 * @param	aServer	Service the session will be a member of
 */
CUsbSession::CUsbSession(CUsbServer* aServer)
	: iUsbServer(aServer)
	{
	OstTraceFunctionEntry0( CUSBSESSION_CUSBSESSION_CONS_ENTRY );

	iUsbServer->IncrementSessionCount();
	OstTraceFunctionExit0( CUSBSESSION_CUSBSESSION_CONS_EXIT );
	}


/**
 * Destructor.
 */
CUsbSession::~CUsbSession()
	{
	OstTraceFunctionEntry0( CUSBSESSION_CUSBSESSION_DES_ENTRY );

	OstTrace1( TRACE_NORMAL, CUSBSESSION_CUSBSESSION, "CUsbSession::~CUsbSession;About to Device().DeRegisterObserver(%08x)", this );
	iUsbServer->Device().DeRegisterObserver(*this);

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	OstTrace1( TRACE_NORMAL, CUSBSESSION_CUSBSESSION_DUP1, "CUsbSession::~CUsbSession;About to Otg().DeRegisterObserver(%08x)", this );
	iUsbServer->Otg().DeRegisterObserver(*this);

	OstTrace1( TRACE_NORMAL, CUSBSESSION_CUSBSESSION_DUP2, "CUsbSession::~CUsbSession;About to Host().DeRegisterObserver(%08x)", this );
	iUsbServer->Host().DeregisterObserver(*this);

	if ( iCtlSession && (iCtlSession == this) )
		{
		iCtlSession = NULL;
		}
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

	OstTrace0( TRACE_NORMAL, CUSBSESSION_CUSBSESSION_DUP3, "CUsbSession::~CUsbSession;About to iUsbServer->DecrementSessionCount()" );
	iUsbServer->DecrementSessionCount();
	OstTraceFunctionExit0( CUSBSESSION_CUSBSESSION_DES_EXIT );
	}


/**
 * Called when a message is received from the client.
 *
 * @param	aMessage	Message received from the client
 */
void CUsbSession::ServiceL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_SERVICEL_ENTRY );

	DispatchMessageL(aMessage);
	OstTraceFunctionExit0( CUSBSESSION_SERVICEL_EXIT );
	}

/**
 * Handles 2nd Phase Construction.  Implementation of the virtual method defined in CSession2 and called from
 * CServer2::DoConnectL() which executes when the client makes a connection request through CServer2::Connect().  If
 * a Leave occurs at any point the CUsbSession object is cleaned up in CServer2::DoConnect().
 */
void CUsbSession::CreateL()
	{
	OstTraceFunctionEntry0( CUSBSESSION_CREATEL_ENTRY );

	// This code originally existed in the typical non-virtual ConstructL() method.
	// However it was moved to this method for minor optimisation reasons [three less
	// function calls and several lines less code in the NewL() method].

	iPersonalityCfged = iUsbServer->Device().isPersonalityCfged();

    OstTrace0( TRACE_NORMAL, CUSBSESSION_CREATEL, "CUsbSession::CreateL;Registering Device Observer" );
	iUsbServer->Device().RegisterObserverL(*this);

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	OstTrace0( TRACE_NORMAL, CUSBSESSION_CREATEL_DUP1, "CUsbSession::CreateL;Registering OTG Observer" );
	iUsbServer->Otg().RegisterObserverL(*this);

	OstTrace0( TRACE_NORMAL, CUSBSESSION_CREATEL_DUP2, "CUsbSession::CreateL;Registering HOST Observer" );
	iUsbServer->Host().RegisterObserverL(*this);
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	OstTraceFunctionExit0( CUSBSESSION_CREATEL_EXIT );
	}

/**
 * Called by CUsbDevice when the service state changes.
 * CUsbSession is an observer of the device.
 *
 * @param	aLastError	The last error happened
 * @param	aOldState	The device's service state just before it changed
 * @param	aNewState	The device's new and current service state
 */
void CUsbSession::UsbServiceStateChange(TInt aLastError, TUsbServiceState aOldState,
										TUsbServiceState aNewState)
	{
	OstTraceFunctionEntry0( CUSBSESSION_USBSERVICESTATECHANGE_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBSERVICESTATECHANGE, "CUsbSession::UsbServiceStateChange;aOldState=0x%X, aNewState=0x%X", aOldState, aNewState );
	(void) aOldState; // a-void build warning in UREL

	// Note that it's possible to have both a start and a stop outstanding!

	if (iStartOutstanding)
		{
		HandleServiceStateChangeWhileStarting(aLastError, aNewState);
		}

	if (iStopOutstanding)
		{
		HandleServiceStateChangeWhileStopping(aLastError, aNewState);
		}

	// Check whether we have an observer of the service state.

	if (iServiceObserverOutstanding)
		{
		TPckg<TUint32> pckg(aNewState);
		iServiceObserverOutstanding = EFalse;
		const TInt err = iServiceObserverMessage.Write(0, pckg);
		iServiceObserverMessage.Complete(err);
		}
	OstTraceFunctionExit0( CUSBSESSION_USBSERVICESTATECHANGE_EXIT );
	}

/**
 * Handles a state change while a start request is currently outstanding.
 *
 * @param aLastError The last error happened
 * @param aNewState The state we've moved to
 */
void CUsbSession::HandleServiceStateChangeWhileStarting(TInt aLastError,
												 TUsbServiceState aNewState)
	{
	OstTraceFunctionEntry0( CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTARTING_ENTRY );

	switch (aNewState)
		{
	case EUsbServiceStarted:
		OstTrace0( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTARTING, "CUsbSession::HandleServiceStateChangeWhileStarting    Completing Start successfully" );

		// If the user has tried to cancel the start, they're too late!
		if (iCancelOutstanding)
			{
			OstTrace0( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTARTING_DUP1, "CUsbSession::HandleServiceStateChangeWhileStarting    Completing cancel request with KErrNone" );
			iCancelOutstanding = EFalse;
			iCancelMessage.Complete(KErrNone);
			}

		iStartMessage.Complete(KErrNone);
		iStartOutstanding = EFalse;
		break;

	case EUsbServiceIdle:
		OstTrace1( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTARTING_DUP2, "CUsbSession::HandleServiceStateChangeWhileStarting;    Completing Start with error=%d", aLastError );

		// If there hasn't actually been an error, but we're in an unexpected
		// state now, that means that this client cancelled the request, or
		// another client stopped the service.
		if (aLastError == KErrNone)
			{
			// If there's a cancel outstanding, then that message succeeded, but
			// the start message should be completed with KErrCancel.
			if (iCancelOutstanding)
				{
				OstTrace0( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTARTING_DUP3, "CUsbSession::HandleServiceStateChangeWhileStarting    Completing original message with KErrCancel" );
				iCancelOutstanding = EFalse;
				iCancelMessage.Complete(KErrNone);
				iStartMessage.Complete(KErrCancel);
				}
			else
				{
				iStartMessage.Complete(KErrUsbServiceStopped);
				}
			}
		else
			{
			// There's been some kind of error, so complete the original message
			// with the right error code.
			if (iCancelOutstanding)
				{
				iCancelOutstanding = EFalse;
				iCancelMessage.Complete(KErrNone);
				}
			iStartMessage.Complete(aLastError);
			}

		iStartOutstanding = EFalse;
		break;

	default:
		break;
		}
	OstTraceFunctionExit0( CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTARTING_EXIT );
	}

/**
 * Handles a state change while a stop request is currently outstanding.
 *
 * @param aLastError The last error happened
 * @param aNewState The state we've moved to
 */
void CUsbSession::HandleServiceStateChangeWhileStopping(TInt aLastError,
												 TUsbServiceState aNewState)
	{
	OstTraceFunctionEntry0( CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTOPPING_ENTRY );

	switch (aNewState)
		{
	case EUsbServiceStarted:
		OstTrace1( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTOPPING, "CUsbSession::HandleServiceStateChangeWhileStopping;    Completing Stop with error=%d", aLastError );

		// If there hasn't actually been an error, but we're in an unexpected
		// state now, that means that this client cancelled the request, or
		// another client has started the service.
		if (aLastError == KErrNone)
			{
			// If there's a cancel outstanding, then that message succeeded, but
			// the stop message should be completed with KErrCancel.
			if (iCancelOutstanding)
				{
				OstTrace0( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTOPPING_DUP1, "CUsbSession::HandleServiceStateChangeWhileStopping;    Completing original message with KErrCancel" );
				iCancelOutstanding = EFalse;
				iCancelMessage.Complete(KErrNone);
				iStopMessage.Complete(KErrCancel);
				}
			else
				{
				iStopMessage.Complete(KErrUsbServiceStarted);
				}
			}
		else
			{
			// There's been some kind of error, so complete the original message
			// with the right error code.
			if (iCancelOutstanding)
				{
				iCancelOutstanding = EFalse;
				iCancelMessage.Complete(KErrNone);
				}
			iStopMessage.Complete(aLastError);
			}

		iStopOutstanding = EFalse;
		break;

	case EUsbServiceIdle:
		OstTrace0( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTOPPING_DUP2, "CUsbSession::HandleServiceStateChangeWhileStopping    Completing Stop with KErrNone" );

		// If the user has tried to cancel the stop, they're too late!
		if (iCancelOutstanding)
			{
			OstTrace0( TRACE_NORMAL, CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTOPPING_DUP3, "CUsbSession::HandleServiceStateChangeWhileStopping    Completing cancel request with KErrNone" );
			iCancelOutstanding = EFalse;
			iCancelMessage.Complete(KErrNone);
			}

		iStopMessage.Complete(KErrNone);
		iStopOutstanding = EFalse;
		break;

	default:
		break;
		}
	OstTraceFunctionExit0( CUSBSESSION_HANDLESERVICESTATECHANGEWHILESTOPPING_EXIT );
	}

/**
 * Called by CUsbDevice when it state change. CUsbSession is an observer of
 * the device. If the client has an Observer outstanding then complete it,
 * otherwise put it in a circular queue.
 *
 * @internalComponent
 * @param	aLastError	The last error happened
 * @param	aOldState	The device's state just before it changed
 * @param	aNewState	The device's new and current state
 */
void CUsbSession::UsbDeviceStateChange(TInt /*aLastError*/, TUsbDeviceState /*aOldState*/,
									   TUsbDeviceState aNewState)
	{
	OstTraceFunctionEntry0( CUSBSESSION_USBDEVICESTATECHANGE_ENTRY );

	// can we bypass the queue?
 	if ((iDeviceObserverOutstanding) && (iDevStateQueueHead == iDevStateQueueTail))
		{
		if ((iDeviceObserverMessage.Int0() & aNewState) ||
			(aNewState == EUsbDeviceStateUndefined))
			{
			TPckg<TUint32> pckg(aNewState);

			iNotifiedDevState = aNewState;

			iDeviceObserverOutstanding = EFalse;
			const TInt err = iDeviceObserverMessage.Write(1, pckg);
			iDeviceObserverMessage.Complete(err);
			}
		}
	else if (iObserverQueueEvents)
		{
		TBool addToQueue = ETrue;

 		// Search queue for similar event, truncate event queue if found
 		if (aNewState == EUsbDeviceStateUndefined)
 			{
 			// erase Event queue, just want this event - not interested in how we got here
 			iDevStateQueueTail = iDevStateQueueHead;

 			// if this is also the event mostly recently notified then don't bother to queue it
 			if(aNewState == iNotifiedDevState)
 				addToQueue = EFalse;
 			}
 		else
 			{
 			TInt queuePtr = iDevStateQueueTail;

 			// search forward from tail to head
 			while (queuePtr != iDevStateQueueHead)
 				{
 				if (aNewState == iDeviceStateQueue[queuePtr])
 					{
 					// Event is already queued; discard the duplicate and in-between events
 					OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBDEVICESTATECHANGE, "CUsbSession::UsbDeviceStateChange;--- collapsing queue head (%d, %d)", iDevStateQueueHead, ((queuePtr + 1) % KDeviceStatesQueueSize) );


 					// queue head moved to position following the match
 					iDevStateQueueHead = (queuePtr + 1) % KDeviceStatesQueueSize;
 					addToQueue = EFalse;
 					break;
 					}

 				// work our way through queue
 				queuePtr = (queuePtr + 1) % KDeviceStatesQueueSize;
 				}
 			}

 		// still want to add to queue?
 		if (addToQueue)
 			{
 			// add event to head of queue
 			iDeviceStateQueue[iDevStateQueueHead] = aNewState;
 			iDevStateQueueHead = (iDevStateQueueHead + 1) % KDeviceStatesQueueSize;
 			OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBDEVICESTATECHANGE_DUP1, "CUsbSession::UsbDeviceStateChange;+++ addqueue (%d, %d)", iDevStateQueueHead, iDevStateQueueTail );
 			}

 		// UsbDeviceDequeueEvent() will read from queue when RegisterObserver()
		// is next called.
		}
	OstTraceFunctionExit0( CUSBSESSION_USBDEVICESTATECHANGE_EXIT );
	}

/**
 * Dequeues an event and completes the observer's request with it.
 */
void CUsbSession::UsbDeviceDequeueEvent()
 	{
	OstTraceFunctionEntry0( CUSBSESSION_USBDEVICEDEQUEUEEVENT_ENTRY );

 	// Work our way through the queue, until we reach the end
 	// OR we find an event the current observer wants.
 	while ((iDeviceObserverOutstanding) && (iDevStateQueueHead != iDevStateQueueTail))
 		{
 		// inform the observer of state changes they are interested in AND
 		// if the cable is pulled out (EUsbDeviceStateUndefined)
 		TUsbDeviceState newState = iDeviceStateQueue[iDevStateQueueTail];

 		// advance tail towards the head
 		iDevStateQueueTail = (iDevStateQueueTail + 1) % KDeviceStatesQueueSize;

 		// is this state one the Observer wants?
 		if ((iDeviceObserverMessage.Int0() & newState) ||
			(newState == EUsbDeviceStateUndefined))
 			{
 			TPckg<TUint32> pckg(newState);

 			iNotifiedDevState = newState;

 			OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBDEVICEDEQUEUEEVENT, "CUsbSession::UsbDeviceDequeueEvent;dequeued event #%d (0x%x)", iDevStateQueueTail, newState );

  			iDeviceObserverOutstanding = EFalse;
			const TInt err = iDeviceObserverMessage.Write(1, pckg);
 			iDeviceObserverMessage.Complete(err);
 			break;
   			}
   		}
   	OstTraceFunctionExit0( CUSBSESSION_USBDEVICEDEQUEUEEVENT_EXIT );
   	}

/**
 * Handles the request (in the form of a the message) received from the client
 *
 * @internalComponent
 * @param	aMessage	The received message
 */
void CUsbSession::DispatchMessageL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_DISPATCHMESSAGEL_ENTRY );

	TBool complete = ETrue;
	TInt ret = KErrNone;

	OstTrace1( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL, "CUsbSession::DispatchMessageL;func#=%d", aMessage.Function() );

	switch (aMessage.Function())
		{
	case EUsbStart:
		ret = StartDeviceL(aMessage, complete);
		break;
	case EUsbStop:
		ret = StopDeviceL(aMessage, complete);
		break;
	case EUsbGetCurrentState:
		ret = GetCurrentServiceState(aMessage);
		break;
	case EUsbGetCurrentDeviceState:
		ret = GetCurrentDeviceState(aMessage);
		break;
	case EUsbRegisterServiceObserver:
		ret = RegisterServiceObserver(aMessage, complete);
		break;
	case EUsbRegisterObserver:
		ret = RegisterDeviceObserver(aMessage, complete);
		break;
	case EUsbStartCancel:
		ret = StartCancel(aMessage, complete);
		break;
	case EUsbStopCancel:
		ret = StopCancel(aMessage, complete);
		break;
	case EUsbCancelServiceObserver:
		ret = DeRegisterServiceObserver();
		break;
	case EUsbCancelObserver:
		ret = DeRegisterDeviceObserver();
		break;
	case EUsbTryStart:
		ret = TryStartDeviceL(aMessage, complete);
		break;
	case EUsbTryStop:
		ret = TryStopDeviceL(aMessage, complete);
		break;
	case EUsbCancelInterest:
		ret = CancelInterest(aMessage);
		break;
	case EUsbGetCurrentPersonalityId:
		ret = GetCurrentPersonalityId(aMessage);
		break;
	case EUsbGetSupportedClasses:
		ret = GetSupportedClasses(aMessage);
		break;
	case EUsbGetPersonalityIds:
		ret = GetPersonalityIds(aMessage);
		break;
	case EUsbGetDescription:
		ret = GetDescription(aMessage);
		break;
	case EUsbGetPersonalityProperty:
		ret = GetPersonalityProperty(aMessage);
		break;
	case EUsbClassSupported:
		ret = ClassSupported(aMessage);
		break;

#ifdef _DEBUG
	// Heap failure debug APIs.

	case EUsbDbgMarkHeap:
		OstTrace0( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL_DUP1, "CUsbSession::DispatchMessageL;Marking heap" );
		__UHEAP_MARK;
		break;
	case EUsbDbgCheckHeap:
		OstTrace1( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL_DUP2, "CUsbSession::DispatchMessageL;Checking heap (expecting %d cells)", aMessage.Int0() );
		__UHEAP_CHECK(aMessage.Int0());
		break;
	case EUsbDbgMarkEnd:
		OstTrace1( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL_DUP3, "CUsbSession::DispatchMessageL;End of marking heap (expecting %d cells)", aMessage.Int0() );
		__UHEAP_MARKENDC(aMessage.Int0());
		break;
	case EUsbDbgFailNext:
		{
		OstTrace1( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL_DUP4, "CUsbSession::DispatchMessageL;Simulating failure after %d allocation(s)", aMessage.Int0() );
		if (aMessage.Int0() == 0)
			__UHEAP_RESET;
		else
			__UHEAP_FAILNEXT(aMessage.Int0());
		}
		break;
	case EUsbDbgAlloc:
		{
		ret = KErrNone;
#ifdef _DEBUG
		OstTrace0( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL_DUP5, "CUsbSession::DispatchMessageL;allocate on the heap" );
		TInt* x = NULL;
		TRAP(ret, x = new(ELeave) TInt);
		delete x;
#endif // _DEBUG
		}
		break;

#endif

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	case EUsbSetCtlSessionMode:
		ret = SetCtlSessionMode(aMessage);
		break;
	case EUsbRegisterMessageObserver:
		ret = RegisterMsgObserver(aMessage, complete);
		break;
	case EUsbCancelMessageObserver:
		ret = DeRegisterMsgObserver();
		break;
	case EUsbBusRequest:
		ret = BusRequest();
		break;
	case EUsbBusRespondSrp:
		ret = BusRespondSrp();
		break;
	case EUsbBusClearError:
		ret = BusClearError();
		break;
	case EUsbBusDrop:
		ret = BusDrop();
		break;
	case EUsbRegisterHostObserver:
		ret = RegisterHostObserver(aMessage, complete);
		break;
	case EUsbCancelHostObserver:
		ret = DeRegisterHostObserver();
		break;
	case EUsbEnableFunctionDriverLoading:
		ret = EnableFunctionDriverLoading();
		break;
	case EUsbDisableFunctionDriverLoading:
		ret = DisableFunctionDriverLoading();
		break;
	case EUsbGetSupportedLanguages:
		ret = GetSupportedLanguages(aMessage);
		break;
	case EUsbGetManufacturerStringDescriptor:
		ret = GetManufacturerStringDescriptor(aMessage);
		break;
	case EUsbGetProductStringDescriptor:
		ret = GetProductStringDescriptor(aMessage);
		break;
	case EUsbGetOtgDescriptor:
		ret = GetOtgDescriptor(aMessage);
		break;
	case EUsbRequestSession:
		ret = RequestSession();
		break;
#else // !SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	case EUsbSetCtlSessionMode:
	case EUsbRegisterMessageObserver:
	case EUsbCancelMessageObserver:
	case EUsbBusRequest:
	case EUsbBusRespondSrp:
	case EUsbBusClearError:
	case EUsbBusDrop:
	case EUsbRegisterHostObserver:
	case EUsbCancelHostObserver:
	case EUsbEnableFunctionDriverLoading:
	case EUsbDisableFunctionDriverLoading:
	case EUsbGetSupportedLanguages:
	case EUsbGetManufacturerStringDescriptor:
	case EUsbGetProductStringDescriptor:
	case EUsbGetOtgDescriptor:
	case EUsbRequestSession:
		ret = KErrNotSupported;
		break;
#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

	default:
		OstTrace1( TRACE_NORMAL, CUSBSESSION_DISPATCHMESSAGEL_DUP6, "CUsbSession::DispatchMessageL;Illegal IPC argument(%d) - Panicking Client...", aMessage.Function() );
		aMessage.Panic(KUsbCliPncCat, EUsbPanicIllegalIPC);
		complete = EFalse;
		break;
		}

	if (complete)
		aMessage.Complete(ret);
	OstTraceFunctionExit0( CUSBSESSION_DISPATCHMESSAGEL_EXIT );
	}


/**
 * Client request to start the device.
 *
 * @param	aMessage	Message received from the client
 * @param	aComplete	Whether the request is complete or not
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::StartDeviceL(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_STARTDEVICEL_ENTRY );

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Only 'control' session is allowed to start USB support
	if ( !iSessionCtlMode )
		{
		OstTraceFunctionExit0( CUSBSESSION_STARTDEVICEL_EXIT );
		return KErrAccessDenied;
		}
#endif

	if (iStartOutstanding)
		return KErrInUse;

#ifndef __OVER_DUMMYUSBDI__

	// If the service is idle or stopping, then we just need to start it.
	// If it's starting (ie. by another client), then we need to perform nothing
	// but wait for the start to complete.
	// If it's already started, we just return immediately.
	TUsbServiceState state = iUsbServer->Device().ServiceState();

	if ((state == EUsbServiceIdle) || (state == EUsbServiceStopping))
		{
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
		iUsbServer->Host().StartL();
#endif
		iUsbServer->Device().StartL();

		aComplete = EFalse;
		iStartMessage = aMessage;
		iStartOutstanding = ETrue;
		}
	else if (state == EUsbServiceStarting)
		{
		aComplete = EFalse;
		iStartMessage = aMessage;
		iStartOutstanding = ETrue;
		}

	OstTraceFunctionExit0( CUSBSESSION_STARTDEVICEL_EXIT_DUP1 );
	return KErrNone;

#else
	// pretend that the server is in Started state
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	iUsbServer->Host().StartL();
#endif
	iStartOutstanding = EFalse;
	aMessage.IsNull();
	aComplete = ETrue;
	OstTraceFunctionExit0( CUSBSESSION_STARTDEVICEL_EXIT_DUP2 );
	return KErrNone;

#endif
	}

/**
 * Client request to stop the device.
 *
 * @param	aMessage	Message received from the client
 * @param	aComplete	Whether the request is complete or not
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::StopDeviceL(const RMessage2& aMessage, TBool& aComplete)
    {
	OstTraceFunctionEntry0( CUSBSESSION_STOPDEVICEL_ENTRY );

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Only 'control' session is allowed to stop USB support
	if ( !iSessionCtlMode )
		{
		OstTraceFunctionExit0( CUSBSESSION_STOPDEVICEL_EXIT );
		return KErrAccessDenied;
		}
#endif

	if (iStopOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_STOPDEVICEL_EXIT_DUP1 );
		return KErrInUse;
		}

#ifndef __OVER_DUMMYUSBDI__

	// Only do anything if the service isn't currently idle. If it is, we just
	// need to complete the user's request immediately.
	if (iUsbServer->Device().ServiceState() != EUsbServiceIdle)
		{
		iUsbServer->Device().Stop();
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
		iUsbServer->Host().Stop();
#endif

		aComplete = EFalse;
		iStopMessage = aMessage;
		iStopOutstanding = ETrue;
		}

	OstTraceFunctionExit0( CUSBSESSION_STOPDEVICEL_EXIT_DUP2 );
	return KErrNone;

#else
	// pretend that the server is in Started state
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	iUsbServer->Host().Stop();
#endif
	aComplete = ETrue;
	aMessage.IsNull();
	iStopOutstanding = EFalse;
	OstTraceFunctionExit0( CUSBSESSION_STOPDEVICEL_EXIT_DUP3 );
	return KErrNone;

#endif
    }

/**
 * Cancel the pending start operation. Note that this can just be implemented
 * as a synchronous stop, if the start operation is pending. However, we have to
 * retain the cancel message, so we can complete it when the stop completes.
 *
 * @param aMessage The message from the client
 * @param aComplete Whether the message is complete or not
 * @return Always KErrNone
 */
TInt CUsbSession::StartCancel(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_STARTCANCEL_ENTRY );

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Only 'control' session is allowed to cancel outstaning start request
	if ( !iSessionCtlMode )
		{
		OstTraceFunctionExit0( CUSBSESSION_STARTCANCEL_EXIT );
		return KErrAccessDenied;
		}
#endif

	if (!iStartOutstanding)
		return KErrNone;

	aComplete = EFalse;
	iCancelMessage = aMessage;
	iCancelOutstanding = ETrue;

	if (iUsbServer->Device().ServiceState() != EUsbServiceIdle)
		{
		iUsbServer->Device().Stop();
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
		iUsbServer->Host().Stop();
#endif
		}

	OstTraceFunctionExit0( CUSBSESSION_STARTCANCEL_EXIT_DUP1 );
	return KErrNone;
	}

/**
 * Cancel the pending stop operation. Note that this can just be implemented as
 * a synchronous start, if the stop operation is pending. However, we have to
 * retain the cancel message, so we can complete it when the start completes.
 *
 * @param aMessage The message from the client
 * @param aComplete Whether the message is complete or not
 * @return KErrNone on success, otherwise standard error codes
 */
TInt CUsbSession::StopCancel(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_STOPCANCEL_ENTRY );
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Only 'control' session is allowed to cancel outstaning stop request
	if ( !iSessionCtlMode )
		{
		OstTraceFunctionExit0( CUSBSESSION_STOPCANCEL_EXIT );
		return KErrAccessDenied;
		}
#endif

	if (!iStopOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_STOPCANCEL_EXIT_DUP1 );
		return KErrNone;
		}

	aComplete = EFalse;
	iCancelMessage = aMessage;
	iCancelOutstanding = ETrue;

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	TRAPD(errHost,iUsbServer->Host().StartL());
	if (errHost != KErrNone)
		return errHost;
#endif
	TRAPD(err, iUsbServer->Device().StartL());
	OstTraceFunctionExit0( CUSBSESSION_STOPCANCEL_EXIT_DUP2 );
	return err;
	}

/**
 * Client request to observe the device (for state changes).
 * Asks the device to register the session as an observer.
 * Assures initialisation/dequeueing of Event queue.
 *
 * @param	aMessage	Message received from the client
 * @param	aComplete	set to true to complete the request
 *
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::RegisterDeviceObserver(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_REGISTERDEVICEOBSERVER_ENTRY );

	if (iDeviceObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_REGISTERDEVICEOBSERVER_EXIT );
		return KErrInUse;
		}

	iDeviceObserverMessage = aMessage;
	iDeviceObserverOutstanding = ETrue;
	aComplete = EFalse;

 	if (iObserverQueueEvents == EFalse)
	 	{
 		// This is the first observer after c'tor or DeregisterObserver(),
 		// so zap the device event queue.
 		OstTrace0( TRACE_NORMAL, CUSBSESSION_REGISTERDEVICEOBSERVER, "CUsbSession::RegisterDeviceObserver    Reset Device Event Queue" );
 		iDevStateQueueHead = 0;
 		iDevStateQueueTail = 0;
 		iObserverQueueEvents = ETrue;
	 	}
 	else if (iDevStateQueueHead != iDevStateQueueTail)
	 	{
 		// event(s) queued, we can de-queue one now
 		UsbDeviceDequeueEvent();
	 	}

	OstTraceFunctionExit0( CUSBSESSION_REGISTERDEVICEOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}

/**
 * Client request to observe the service (for state changes)
 * Asks the device to register the session as an observer
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @param	aComplete	set to true to complete the request
 *
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::RegisterServiceObserver(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_REGISTERSERVICEOBSERVER_ENTRY );

	if (iServiceObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_REGISTERSERVICEOBSERVER_EXIT );
		return KErrInUse;
		}

	iServiceObserverMessage = aMessage;
	iServiceObserverOutstanding = ETrue;
	aComplete = EFalse;
	OstTraceFunctionExit0( CUSBSESSION_REGISTERSERVICEOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}


/**
 * Client request to fetch the current service state of the device
 * Asks the device for its current service state
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 *
 * @return	Any errors that occurred or KErrNone
 */
TInt CUsbSession::GetCurrentServiceState(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETCURRENTSERVICESTATE_ENTRY );

	TUsbServiceState state = iUsbServer->Device().ServiceState();
	OstTrace1( TRACE_NORMAL, CUSBSESSION_GETCURRENTSERVICESTATE, "CUsbSession::GetCurrentServiceState;state=%d", state );
	TPckg<TUint32> pckg(state);
	return aMessage.Write(0, pckg);
	}

/**
 * Client request to fetch the current device state of the device
 * Asks the device for its current device state
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 *
 * @return	Any errors that occurred or KErrNone
 */
TInt CUsbSession::GetCurrentDeviceState(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETCURRENTDEVICESTATE_ENTRY );

	TUsbDeviceState state = iUsbServer->Device().DeviceState();
	OstTrace1( TRACE_NORMAL, CUSBSESSION_GETCURRENTDEVICESTATE, "CUsbSession::GetCurrentDeviceState;state=%d", state );
	TPckg<TUint32> pckg(state);
	return aMessage.Write(0, pckg);
	}


/**
 * Deregister the client as an observer of device state changes. Note that we don't
 * deregister ourselves as an observer, because we need to be notified when the
 * device state changes, so we can complete Start and Stop requests.
 *
 * @return Always KErrNone
 */
TInt CUsbSession::DeRegisterDeviceObserver()
	{
	OstTraceFunctionEntry0( CUSBSESSION_DEREGISTERDEVICEOBSERVER_ENTRY );

	if (!iDeviceObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_DEREGISTERDEVICEOBSERVER_EXIT );
		return KErrNone;
		}

	iDeviceObserverOutstanding = EFalse;
	iDeviceObserverMessage.Complete(KErrCancel);

	// client doesn't need events queuing any more
 	iObserverQueueEvents = EFalse;

	OstTraceFunctionExit0( CUSBSESSION_DEREGISTERDEVICEOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}

/**
 * Deregister the client as an observer of service state changes. Note that we don't
 * deregister ourselves as an observer, because we need to be notified when the
 * service state changes, so we can complete Start and Stop requests.
 *
 * @return Always KErrNone
 */
TInt CUsbSession::DeRegisterServiceObserver()
	{
	OstTraceFunctionEntry0( CUSBSESSION_DEREGISTERSERVICEOBSERVER_ENTRY );

	if (!iServiceObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_DEREGISTERSERVICEOBSERVER_EXIT );
		return KErrNone;
		}

	iServiceObserverOutstanding = EFalse;
	iServiceObserverMessage.Complete(KErrCancel);
	OstTraceFunctionExit0( CUSBSESSION_DEREGISTERSERVICEOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}

/**
 * Try starting the USB device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @param	aComplete	set to true to complete the request
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::TryStartDeviceL(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_TRYSTARTDEVICEL_ENTRY );

#ifndef __OVER_DUMMYUSBDI__

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Only 'control' session is allowed to start USB support
	if ( !iSessionCtlMode )
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT );
		return KErrAccessDenied;
		}
#endif

	if (!iPersonalityCfged)
	{
	OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT_DUP1 );
	return KErrNotSupported;
	}

	if (iStartOutstanding || iStopOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT_DUP2 );
		return KErrServerBusy;
		}

	// Obtains the curent service state
	TUsbServiceState state = iUsbServer->Device().ServiceState();


	// USB Peripheral Stack Starting sequence:
	// if the server is in the idle state
	//		start it;
	// if the server is in the started state
	//		return KErrNone immediately;
	// if the server is in the stopping state
	//		return KErrServerBusy immediately;
 	// if the server is in the starting state
	// (was already called by this very session in OTG/Host configuration and/or
	//  by another session for Client Only configuration)
	// 		if requested personality is not equal to the current one
	//			return KErrAbort;
	//		else
 	//			mark this request as outstanding and let the caller to wait
	//			for start operation to complete;
	if (state == EUsbServiceIdle)
		{
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
		iUsbServer->Host().StartL();
#endif
		iUsbServer->Device().TryStartL(aMessage.Int0());
		aComplete = EFalse;
		iStartMessage = aMessage;
		iStartOutstanding = ETrue;
		}
	else if (state == EUsbServiceStarting || state == EUsbServiceStarted)
		{
		if (aMessage.Int0() != iUsbServer->Device().CurrentPersonalityId())
			{
			OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT_DUP3 );
			return KErrAbort;
			}

		if (state == EUsbServiceStarting)
			{
			aComplete = EFalse;
			iStartMessage = aMessage;
			iStartOutstanding = ETrue;
			}
		}
	else if (state == EUsbServiceStopping)
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT_DUP4 );
		return KErrServerBusy;
		}

	OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT_DUP5 );
	return KErrNone;

#else
	// pretend that the server is in Started state
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	iUsbServer->Host().StartL();
#endif
	iStartOutstanding = EFalse;
	aMessage.IsNull();
	aComplete = ETrue;
	OstTraceFunctionExit0( CUSBSESSION_TRYSTARTDEVICEL_EXIT_DUP6 );
	return KErrNone;
#endif
	}

/**
 * Try stopping the USB device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @param	aComplete	Whether the request is complete or not
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::TryStopDeviceL(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_TRYSTOPDEVICEL_ENTRY );

#ifndef __OVER_DUMMYUSBDI__

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	// Only 'control' session is allowed to stop USB support
	if ( !iSessionCtlMode )
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTOPDEVICEL_EXIT );
		return KErrAccessDenied;
		}
#endif

	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTOPDEVICEL_EXIT_DUP1 );
		return KErrNotSupported;
		}

	if (iStartOutstanding || iStopOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTOPDEVICEL_EXIT_DUP2 );
		return KErrServerBusy;
		}

	// Obtains the curent service state
	TUsbServiceState state = iUsbServer->Device().ServiceState();

	// USB Peripheral Stack Stopping sequence:
	// if the server is in the started state
	//		stop it;
	// if the server is in the starting state
	//		return KErrServerBusy immediately;
	// if the server is in the idle state
	//		return KErrNone immediately;
	// if the server is in the stopping state (must by another client)
 	//			mark this request as outstanding and let the caller to wait
	//			for stop operation to complete;
	if (state == EUsbServiceStarted)
		{
		iUsbServer->Device().Stop();
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
		iUsbServer->Host().Stop();
#endif

		aComplete = EFalse;
		iStopMessage = aMessage;
		iStopOutstanding = ETrue;
		}
	else if (state == EUsbServiceStarting)
		{
		OstTraceFunctionExit0( CUSBSESSION_TRYSTOPDEVICEL_EXIT_DUP3 );
		return KErrServerBusy;
		}
	else if (state == EUsbServiceStopping)
		{
		aComplete = EFalse;
		iStopMessage = aMessage;
		iStopOutstanding = ETrue;
		}

	OstTraceFunctionExit0( CUSBSESSION_TRYSTOPDEVICEL_EXIT_DUP4 );
	return KErrNone;

#else
	// pretend that the server is in Started state
#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
	iUsbServer->Host().Stop();
#endif
	aMessage.IsNull();
	aComplete = ETrue;
	iStopOutstanding = EFalse;
	OstTraceFunctionExit0( CUSBSESSION_TRYSTOPDEVICEL_EXIT_DUP5 );
	return KErrNone;
#endif
	}

/**
 * Cancels the interest to the outstanding request. The acutal request itself
 * is not cancelled.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	KErrCancel
 */
TInt CUsbSession::CancelInterest(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_CANCELINTEREST_ENTRY );

	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_CANCELINTEREST_EXIT );
		return KErrNotSupported;
		}

	TUsbMessages toBeCancelledMsg = static_cast<TUsbMessages>(aMessage.Int0());
	if (toBeCancelledMsg == EUsbStart || toBeCancelledMsg == EUsbTryStart)
		{
		if (iStartOutstanding)
			{
			iStartMessage.Complete(KErrNone);
			iStartOutstanding = EFalse;
			}
		}
	else if (toBeCancelledMsg == EUsbStop || toBeCancelledMsg == EUsbTryStop)
		{
		if (iStopOutstanding)
			{
			iStopMessage.Complete(KErrNone);
			iStopOutstanding = EFalse;
			}
		}

	OstTraceFunctionExit0( CUSBSESSION_CANCELINTEREST_EXIT_DUP1 );
	return KErrCancel;
	}

/**
 * Gets the current personality id.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetCurrentPersonalityId(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETCURRENTPERSONALITYID_ENTRY );

	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_GETCURRENTPERSONALITYID_EXIT );
		return KErrNotSupported;
		}

	TInt currentPersonalityId = iUsbServer->Device().CurrentPersonalityId();
	OstTrace1( TRACE_NORMAL, CUSBSESSION_GETCURRENTPERSONALITYID, "CUsbSession::GetCurrentPersonalityId;currentPersonalityId=%d", currentPersonalityId );
	TPckgC<TInt> pckg(currentPersonalityId);
	return aMessage.Write(0, pckg);
	}

/**
 * Gets supported classes.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	KerrTooBig if supported class > KUsbMaxSupportedClasses;
 *          KErrNotSupported if personality is not configured
 * 			return code from RMessage2.Write()
 */
TInt CUsbSession::GetSupportedClasses(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETSUPPORTEDCLASSES_ENTRY );

	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_GETSUPPORTEDCLASSES_EXIT );
		return KErrNotSupported;
		}

	// +1 for the size of actual class uid count
	TInt32 classUids[KUsbMaxSupportedClasses + 1];
	classUids[0] = 0;	// initializes class uids count to zero
	// Gets all class uids for the given personality
	const RPointerArray<CPersonality>&  personalities = iUsbServer->Device().Personalities();
	TInt personalityCount = personalities.Count();
	for (TInt i = 0; i < personalityCount; i++)
		{
		if(personalities[i] == NULL)
		    {
            OstTrace1( TRACE_NORMAL, CUSBSESSION_GETSUPPORTEDCLASSES, "CUsbSession::GetSupportedClasses;ENullPersonalityPointer=%d", ENullPersonalityPointer );
            User::Panic(KUsbSvrPncCat, ENullPersonalityPointer);
		    }
		if (aMessage.Int0() == personalities[i]->PersonalityId())
			{
			classUids[0] = personalities[i]->SupportedClasses().Count();
			for (TInt j = 1; j <= classUids[0]; j++)
				{
				if (j < KUsbMaxSupportedClasses + 1)
					{
					classUids[j] = personalities[i]->SupportedClasses()[j - 1].iClassUid.iUid;
					OstTrace1( TRACE_NORMAL, CUSBSESSION_GETSUPPORTEDCLASSES_DUP1, "CUsbSession::GetSupportedClasses;classUids[%d] = ", j );
					OstTrace1( TRACE_NORMAL, CUSBSESSION_GETSUPPORTEDCLASSES_DUP2, "%d", classUids[j] );					
					}
				else
					{
					OstTraceFunctionExit0( CUSBSESSION_GETSUPPORTEDCLASSES_EXIT_DUP1 );
					return KErrTooBig;
					}
				}
			break;
			}
		}

	if (classUids[0] == 0)
		{
		// No supported classes are found
		OstTraceFunctionExit0( CUSBSESSION_GETSUPPORTEDCLASSES_EXIT_DUP2 );
		return KErrNotSupported;
		}

	TInt ret;
	HBufC8* buf = NULL;
	TRAP(ret, buf = HBufC8::NewL((classUids[0] + 1)*sizeof(TInt32)));
	if (ret == KErrNone)
		{
		TPtr8 ptr8 = buf->Des();
		ptr8.Copy(reinterpret_cast<TUint8*>(classUids), (classUids[0] + 1)*sizeof(TInt32));
		ret = aMessage.Write(1, ptr8);
		}

	delete buf;
	OstTraceFunctionExit0( CUSBSESSION_GETSUPPORTEDCLASSES_EXIT_DUP3 );
	return ret;
	}

/**
 * Gets all personality ids.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetPersonalityIds(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETPERSONALITYIDS_ENTRY );

	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_GETPERSONALITYIDS_EXIT );
		return KErrNotSupported;
		}

	// +1 for the size of actual personality id count
	TInt personalityIds[KUsbMaxSupportedPersonalities + 1];

	const RPointerArray<CPersonality>&  personalities = iUsbServer->Device().Personalities();
	TInt personalityCount = personalities.Count();
	for (TInt i = 0; i < personalityCount; ++i)
		{
        if(personalities[i] == NULL)
            {
            OstTrace1( TRACE_NORMAL, CUSBSESSION_GETPERSONALITYIDS, "CUsbSession::GetPersonalityIds;Panic reason=%d", ENullPersonalityPointer );
            User::Panic(KUsbSvrPncCat, ENullPersonalityPointer);
            }
		personalityIds[i + 1] = personalities[i]->PersonalityId();
		}
	personalityIds[0] = personalityCount;

	TInt ret;
	HBufC8* buf = NULL;
	TRAP(ret, buf = HBufC8::NewL((personalityCount + 1)*sizeof(TInt)));
	if (ret == KErrNone)
		{
		TPtr8 ptr8 = buf->Des();
		ptr8.Copy(reinterpret_cast<TUint8*>(personalityIds), (personalityCount + 1)*sizeof(TInt));
		ret = aMessage.Write(0, ptr8);
		}

	delete buf;
	OstTraceFunctionExit0( CUSBSESSION_GETPERSONALITYIDS_EXIT_DUP1 );
	return ret;
	}

/**
 * Gets personality description
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetDescription(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETDESCRIPTION_ENTRY );

	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_GETDESCRIPTION_EXIT );
		return KErrNotSupported;
		}

	TInt personalityId = aMessage.Int0();
	const CPersonality* personality = iUsbServer->Device().GetPersonality(personalityId);
	if (personality)
		{
		return aMessage.Write(1, *(personality->Description()));
		}

	// We should never reach here
	OstTraceFunctionExit0( CUSBSESSION_GETDESCRIPTION_EXIT_DUP1 );
	return KErrNotSupported;
	}


/**
 * Gets personality property
 *
 * @internalComponent
 * @param   aMessage    Message received from the client
 * @return  Any error that occurred or KErrNone
 */
TInt CUsbSession::GetPersonalityProperty(const RMessage2& aMessage)
	{
		OstTraceFunctionEntry0( CUSBSESSION_GETPERSONALITYPROPERTY_ENTRY );

		if (!iPersonalityCfged)
			{
			OstTraceFunctionExit0( CUSBSESSION_GETPERSONALITYPROPERTY_EXIT );
			return KErrNotSupported;
			}

		TInt personalityId = aMessage.Int0();
		const CPersonality* personality = iUsbServer->Device().GetPersonality(personalityId);
		if (personality)
			{
			TPckg<TUint32> pckg(personality->Property());
			return aMessage.Write(1, pckg);
			}

		OstTraceFunctionExit0( CUSBSESSION_GETPERSONALITYPROPERTY_EXIT_DUP2 );
		return KErrNotSupported;
	}

/**
 * Checks if a given class is supported by a personality.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::ClassSupported(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_CLASSSUPPORTED_ENTRY );
	if (!iPersonalityCfged)
		{
		OstTraceFunctionExit0( CUSBSESSION_CLASSSUPPORTED_EXIT );
		return KErrNotSupported;
		}

	TBool isSupported = EFalse;
	TInt personalityId = aMessage.Int0();
	TUid classUid = TUid::Uid(aMessage.Int1());
	const CPersonality* personality = iUsbServer->Device().GetPersonality(personalityId);
	if (personality)
		{
		isSupported = personality->ClassSupported(classUid);
		TPckg<TBool> pkg2(isSupported);
		return aMessage.Write(2, pkg2);
		}

	// We should never reach here
	OstTraceFunctionExit0( CUSBSESSION_CLASSSUPPORTED_EXIT_DUP1 );
	return KErrNotSupported;
	}

#ifdef SYMBIAN_ENABLE_USB_OTG_HOST_PRIV
/**
 * Sets or resets the control mode flag for this session.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::SetCtlSessionMode(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_SETCTLSESSIONMODE_ENTRY );

	TInt ret = KErrNone;

	TBool value = (TBool)aMessage.Int0();
	OstTrace1( TRACE_NORMAL, CUSBSESSION_SETCTLSESSIONMODE, "CUsbSession::SetCtlSessionMode;Setting = %d", static_cast<TInt>(value) );

	// Verify if this is the same session which set the value before
	if ( iCtlSession && (iCtlSession != this) )
		{
		ret = KErrAccessDenied;
		}
	else
		{
		iSessionCtlMode = value;

		// Set control session pointer if the flag is set
		if ( iSessionCtlMode )
			{
			iCtlSession = this;
			}
		else
			{
			iCtlSession = NULL;
			}
		}

	OstTraceFunctionExit0( CUSBSESSION_SETCTLSESSIONMODE_EXIT );
	return ret;
	}

/**
 * Asserts a request to drive VBus.
 *
 * @internalComponent
 * @return	If control mode flag is not set returns KErrAccessDenied
 * 			An error code for all other cases
 */
TInt CUsbSession::BusRequest()
	{
	OstTraceFunctionEntry0( CUSBSESSION_BUSREQUEST_ENTRY );

	TInt ret = KErrNone;
	if ( iSessionCtlMode )
		{
		ret = iUsbServer->Otg().BusRequest();
		}
	else
		{
		ret = KErrAccessDenied;
		}

	OstTraceFunctionExit0( CUSBSESSION_BUSREQUEST_EXIT );
	return ret;
	}

/**
 * Asserts a request to raise VBUS but assumes this is after B-Device
 * has used SRP to make a request to become session Host, so may be able
 * to take some short cuts when enumerating the B-Device.
 *
 * @internalComponent
 * @return	If control mode flag is not set returns KErrAccessDenied
 * 			Any error that occurred or KErrNone for all other cases
 */
TInt CUsbSession::BusRespondSrp()
	{
	OstTraceFunctionEntry0( CUSBSESSION_BUSRESPONDSRP_ENTRY );

	TInt ret = KErrNone;
	if ( iSessionCtlMode )
		{
		ret = iUsbServer->Otg().BusRespondSrp();
		}
	else
		{
		ret = KErrAccessDenied;
		}

	OstTraceFunctionExit0( CUSBSESSION_BUSRESPONDSRP_EXIT );
	return ret;
	}

/**
 * Clears a possible VBUS error condition (VBUS inexplicably low after
 * having been driven)
 *
 * @internalComponent
 * @return	If control mode flag is not set returns KErrAccessDenied
 * 			Any error that occurred or KErrNone for all other cases
 */
TInt CUsbSession::BusClearError()
	{
	OstTraceFunctionEntry0( CUSBSESSION_BUSCLEARERROR_ENTRY );

	TInt ret = KErrNone;
	if ( iSessionCtlMode )
		{
		ret = iUsbServer->Otg().BusClearError();
		}
	else
		{
		ret = KErrAccessDenied;
		}

	OstTraceFunctionExit0( CUSBSESSION_BUSCLEARERROR_EXIT );
	return ret;
	}

/**
 * Drops VBus.
 *
 * @internalComponent
 * @return	If control mode flag is not set returns KErrAccessDenied
 * 			Any error that occurred or KErrNone for all other cases
 */
TInt CUsbSession::BusDrop()
	{
	OstTraceFunctionEntry0( CUSBSESSION_BUSDROP_ENTRY );

	TInt ret = KErrNone;
	if ( iSessionCtlMode )
		{
		ret = iUsbServer->Otg().BusDrop();
		}
	else
		{
		ret = KErrAccessDenied;
		}

	OstTraceFunctionExit0( CUSBSESSION_BUSDROP_EXIT );
	return ret;
	}

/**
 * Enables loading of Function Drivers.
 *
 * @internalComponent
 * @return	If control mode flag is not set returns KErrAccessDenied
 * 			Any error that occurred or KErrNone for all other cases
 */
TInt CUsbSession::EnableFunctionDriverLoading()
	{
	OstTraceFunctionEntry0( CUSBSESSION_ENABLEFUNCTIONDRIVERLOADING_ENTRY );

	TInt ret = KErrNone;
	if ( iSessionCtlMode )
		{
		ret = iUsbServer->Host().EnableDriverLoading();
		}
	else
		{
		ret = KErrAccessDenied;
		}

	OstTraceFunctionExit0( CUSBSESSION_ENABLEFUNCTIONDRIVERLOADING_EXIT );
	return ret;
	}

/**
 * Disables loading of Function Drivers.
 *
 * @internalComponent
 * @return	If control mode flag is not set returns KErrAccessDenied
 * 			KErrNone for all other cases
 */
TInt CUsbSession::DisableFunctionDriverLoading()
	{
	OstTraceFunctionEntry0( CUSBSESSION_DISABLEFUNCTIONDRIVERLOADING_ENTRY );

	TInt ret = KErrNone;
	if ( iSessionCtlMode )
		{
		iUsbServer->Host().DisableDriverLoading();
		}
	else
		{
		ret = KErrAccessDenied;
		}

	OstTraceFunctionExit0( CUSBSESSION_DISABLEFUNCTIONDRIVERLOADING_EXIT );
	return ret;
	}

/**
 * Requests an array of language identifiers supported by connected device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetSupportedLanguages(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETSUPPORTEDLANGUAGES_ENTRY );
	TRAPD(err, GetSupportedLanguagesL(aMessage));
	OstTraceFunctionExit0( CUSBSESSION_GETSUPPORTEDLANGUAGES_EXIT );
	return err;
	}

/**
 * Requests an array of language identifiers supported by connected device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetSupportedLanguagesL(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETSUPPORTEDLANGUAGESL_ENTRY );

	const TUint deviceId = aMessage.Int0();
	OstTrace1( TRACE_NORMAL, CUSBSESSION_GETSUPPORTEDLANGUAGESL, "CUsbSession::GetSupportedLanguagesL;deviceId=%d", deviceId );

	RArray<TUint> langIds;
	CleanupClosePushL(langIds);
	TInt ret = iUsbServer->Host().GetSupportedLanguages(deviceId,langIds);

	if (ret == KErrNone)
		{
		const TUint count = langIds.Count();
		OstTrace1( TRACE_NORMAL, CUSBSESSION_GETSUPPORTEDLANGUAGESL_DUP1, "CUsbSession::GetSupportedLanguagesL;count=%d", count );

		// Set error code if there is no languages or there are too many
		if ( count == 0 )
			{
			ret = KErrNotSupported;
			}
		else if ( count > KUsbMaxSupportedLanguageIds )
			{
			ret = KErrTooBig;
			}

		if ( ret == KErrNone )
			{
			// Create a buffer to keep an array size and all received language Ids
			RBuf8 buf;
			buf.CreateL((count + 1) * sizeof(TUint));
			CleanupClosePushL(buf);

			// Save the length of the array
			buf.Append((TUint8*)&count, sizeof(TUint));

			// Save all received language Ids
			for ( TUint ii = 0 ; ii < count; ++ii )
				{
				buf.Append((TUint8*)&(langIds[ii]), sizeof(TUint));
				OstTraceExt2( TRACE_NORMAL, CUSBSESSION_GETSUPPORTEDLANGUAGESL_DUP2, "CUsbSession::GetSupportedLanguagesL;Append langID[%d] = %d", ii, langIds[ii] );
				}

			// Write back to the client.
			ret = aMessage.Write(1, buf);
			CleanupStack::PopAndDestroy(&buf);
			}
		}

	CleanupStack::PopAndDestroy();

	OstTraceFunctionExit0( CUSBSESSION_GETSUPPORTEDLANGUAGESL_EXIT );
	return ret;
	}

/**
 * Requests a manufacturer string descriptor of connected device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetManufacturerStringDescriptor(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETMANUFACTURERSTRINGDESCRIPTOR_ENTRY );

	const TUint deviceId = aMessage.Int0();
	const TUint langId = aMessage.Int1();
	OstTraceExt2( TRACE_NORMAL, CUSBSESSION_GETMANUFACTURERSTRINGDESCRIPTOR, "CUsbSession::GetManufacturerStringDescriptor;langId=%d;deviceId=%d", langId, deviceId );

	TName string;
	TInt ret = iUsbServer->Host().GetManufacturerStringDescriptor(deviceId,langId,string);
	if (ret == KErrNone)
		{
		OstTraceExt1( TRACE_NORMAL, CUSBSESSION_GETMANUFACTURERSTRINGDESCRIPTOR_DUP1, "CUsbSession::GetManufacturerStringDescriptor;string = \"%S\"", string );
		ret = aMessage.Write(2, string);
		}

	OstTraceFunctionExit0( CUSBSESSION_GETMANUFACTURERSTRINGDESCRIPTOR_EXIT );
	return ret;
	}

/**
 * Requests a product string descriptor of connected device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetProductStringDescriptor(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETPRODUCTSTRINGDESCRIPTOR_ENTRY );

	const TUint deviceId = aMessage.Int0();
	const TUint langId = aMessage.Int1();
	OstTraceExt2( TRACE_NORMAL, CUSBSESSION_GETPRODUCTSTRINGDESCRIPTOR, "CUsbSession::GetProductStringDescriptor;deviceId=%d;langId=%d", deviceId, langId );

	TName string;
	TInt ret = iUsbServer->Host().GetProductStringDescriptor(deviceId,langId,string);
	if (ret == KErrNone)
		{
		OstTraceExt1( TRACE_NORMAL, CUSBSESSION_GETPRODUCTSTRINGDESCRIPTOR_DUP1, "CUsbSession::GetProductStringDescriptor;string = \"%S\"", string );
		ret = aMessage.Write(2, string);
		}

	OstTraceFunctionExit0( CUSBSESSION_GETPRODUCTSTRINGDESCRIPTOR_EXIT );
	return ret;
	}

/**
 * Requests a OTG descriptor of connected device.
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::GetOtgDescriptor(const RMessage2& aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_GETOTGDESCRIPTOR_ENTRY );

	const TUint deviceId = aMessage.Int0();
	OstTrace1( TRACE_NORMAL, CUSBSESSION_GETOTGDESCRIPTOR, "CUsbSession::GetOtgDescriptor;deviceId=%d", deviceId );

    TOtgDescriptor otgDescriptor;
	TInt ret = iUsbServer->Host().GetOtgDescriptor(deviceId, otgDescriptor);
	if (ret == KErrNone)
		{
		TPckg<TOtgDescriptor> buf(otgDescriptor);
		ret = aMessage.Write(1, buf);
		}

	OstTraceFunctionExit0( CUSBSESSION_GETOTGDESCRIPTOR_EXIT );
	return ret;
	}

/**
 * Client request to observe the host (for state changes).
 * Asks the host to register the session as an observer.
 * Assures initialisation/dequeueing of Event queue.
 * No events are queued until the first observer is registered
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @param	aComplete	set to true to complete the request
 *
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::RegisterHostObserver(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_REGISTERHOSTOBSERVER_ENTRY );

	if (iHostEventObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_REGISTERHOSTOBSERVER_EXIT );
		return KErrInUse;
		}

	iHostEventObserverMessage = aMessage;
	iHostEventObserverOutstanding = ETrue;
	aComplete = EFalse;

 	if (iHostEventObserverQueueEvents == EFalse)
	 	{
 		// This is the first observer after c'tor or DeregisterObserver(),
 		// so zap the device event queue.
 		OstTrace0( TRACE_NORMAL, CUSBSESSION_REGISTERHOSTOBSERVER, "CUsbSession::RegisterHostObserver;    Reset OTG Host State Queue" );
 		iHostEventQueueHead = 0;
 		iHostEventQueueTail = 0;
 		iHostEventObserverQueueEvents = ETrue;
	 	}
 	else if (iHostEventQueueHead != iHostEventQueueTail)
	 	{
 		// event(s) queued, we can de-queue one now
 		UsbHostEventDequeue();
	 	}

	OstTraceFunctionExit0( CUSBSESSION_REGISTERHOSTOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}

/**
 * Deregister the client as an observer of host state changes.
 *
 * @internalComponent
 * @return Always KErrNone
 */
TInt CUsbSession::DeRegisterHostObserver()
	{
	OstTraceFunctionEntry0( CUSBSESSION_DEREGISTERHOSTOBSERVER_ENTRY );

	if (!iHostEventObserverQueueEvents)
		{
		//Never register
		OstTrace0( TRACE_NORMAL, CUSBSESSION_DEREGISTERHOSTOBSERVER, "CUsbSession::DeRegisterHostObserver;iHostEventObserverQueueEvents is FALSE!" );
		OstTraceFunctionExit0( CUSBSESSION_DEREGISTERHOSTOBSERVER_EXIT );
		return KErrNone;
		}

	if (iHostEventObserverOutstanding)
		{
		iHostEventObserverOutstanding = EFalse;
		iHostEventObserverMessage.Complete(KErrCancel);
		OstTrace0( TRACE_NORMAL, CUSBSESSION_DEREGISTERHOSTOBSERVER_DUP1, "CUsbSession::DeRegisterHostObserver;iHostEventObserverMessage.Complete(KErrCancel);" );
		}

	// client doesn't need events queuing any more
 	iHostEventObserverQueueEvents = EFalse;
	//Reset OTG Host State Queue
	iHostEventQueueHead = 0;
	iHostEventQueueTail = 0;

	OstTraceFunctionExit0( CUSBSESSION_DEREGISTERHOSTOBSERVER_EXIT_DUP1 );
	return KErrNone;	
	}

/**
 * Client request to observe both OTG and HOST (for events and errors).
 * Asks HOST and OTGDI components to register the session as an observer.
 * Assures initialisation/dequeueing of Event queue.
 * No events are queued until the first observer is registered
 *
 * @internalComponent
 * @param	aMessage	Message received from the client
 * @param	aComplete	set to true to complete the request
 *
 * @return	Any error that occurred or KErrNone
 */
TInt CUsbSession::RegisterMsgObserver(const RMessage2& aMessage, TBool& aComplete)
	{
	OstTraceFunctionEntry0( CUSBSESSION_REGISTERMSGOBSERVER_ENTRY );

	if (iMsgObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_REGISTERMSGOBSERVER_EXIT );
		return KErrInUse;
		}

	iMsgObserverMessage = aMessage;
	iMsgObserverOutstanding = ETrue;
	aComplete = EFalse;

 	if (iMsgObserverQueueEvents == EFalse)
	 	{
 		// This is the first observer after c'tor or DeregisterObserver(),
 		// so zap the device event queue.
 		OstTrace0( TRACE_NORMAL, CUSBSESSION_REGISTERMSGOBSERVER, "CUsbSession::RegisterMsgObserver;    Reset OTG Message Queue" );
 		iMsgQueueHead = 0;
 		iMsgQueueTail = 0;
 		iMsgObserverQueueEvents = ETrue;
	 	}
 	else if (iMsgQueueHead != iMsgQueueTail)
	 	{
 		// event(s) queued, we can de-queue one now
 		UsbMsgDequeue();
	 	}

	OstTraceFunctionExit0( CUSBSESSION_REGISTERMSGOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}

/**
 * Deregister the client as an observer of OTG/HOST events and errors.
 *
 * @internalComponent
 * @return Always KErrNone
 */
TInt CUsbSession::DeRegisterMsgObserver()
	{
	OstTraceFunctionEntry0( CUSBSESSION_DEREGISTERMSGOBSERVER_ENTRY );

	if (!iMsgObserverOutstanding)
		{
		OstTraceFunctionExit0( CUSBSESSION_DEREGISTERMSGOBSERVER_EXIT );
		return KErrNone;
		}

	iMsgObserverOutstanding = EFalse;
	iMsgObserverMessage.Complete(KErrCancel);

	// client doesn't need events queuing any more
 	iMsgObserverQueueEvents = EFalse;

	OstTraceFunctionExit0( CUSBSESSION_DEREGISTERMSGOBSERVER_EXIT_DUP1 );
	return KErrNone;
	}



/**
 * Called by CUsbOtg or CUsbHost when the USB OTG/HOST message has arrived
 *
 * @internalComponent
 * @param aMessage The new OTG Message
 */
void CUsbSession::UsbOtgHostMessage(TInt aMessage)
	{
	OstTraceFunctionEntry0( CUSBSESSION_USBOTGHOSTMESSAGE_ENTRY );

	// can we bypass the queue?
 	if ((iMsgObserverOutstanding) && (iMsgQueueHead == iMsgQueueTail))
		{
		TPckg<TInt> pckg(aMessage);

		iNotifiedMsg = aMessage;

		iMsgObserverOutstanding = EFalse;
		const TInt err = iMsgObserverMessage.Write(0, pckg);
		iMsgObserverMessage.Complete(err);
		}
	else if (iMsgObserverQueueEvents)
		{
		// add event to head of queue
		iMsgQueue[iMsgQueueHead] = aMessage;
		iMsgQueueHead = (iMsgQueueHead + 1) % KOtgHostMessageQueueSize;
		OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBOTGHOSTMESSAGE, "CUsbSession::UsbOtgHostMessage; addqueue (%d, %d)", iMsgQueueHead, iMsgQueueTail );

 		// UsbMsgDequeueEvent() will read from queue when RegisterMsgObserver()
		// is next called.
		}
	OstTraceFunctionExit0( CUSBSESSION_USBOTGHOSTMESSAGE_EXIT );
	}

/**
 * Called by CUsbHost when it state change. CUsbSession is an observer of
 * the device. If the client has an Observer outstanding then complete it,
 * otherwise put it in a circular queue.
 *
 * @internalComponent
 * @param	aDevInfo	The information about the device being attached or detached
 * 						along with the status of Function Driver loading
 */
void CUsbSession::UsbHostEvent(TDeviceEventInformation& aDevInfo)
	{
	OstTraceFunctionEntry0( CUSBSESSION_USBHOSTEVENT_ENTRY );

	// can we bypass the queue?
 	if ((iHostEventObserverOutstanding) && (iHostEventQueueHead == iHostEventQueueTail))
		{
		iNotifiedHostState = aDevInfo;
		iHostEventObserverOutstanding = EFalse;

		OstTrace0( TRACE_NORMAL, CUSBSESSION_USBHOSTEVENT, "CUsbSession::UsbHostEvent detected outstanding request" );

		TPckg<TDeviceEventInformation> info(aDevInfo);
		const TInt err = iHostEventObserverMessage.Write(0, info);
		iHostEventObserverMessage.Complete(err);
		OstTrace1( TRACE_NORMAL, CUSBSESSION_USBHOSTEVENT_DUP1, "CUsbSession::UsbHostEvent; detects outstanding request: request is compeleted with %d", err );
		}
	else if (iHostEventObserverQueueEvents)
		{
		// add dev info to head of queue
		iHostStateQueue[iHostEventQueueHead] = aDevInfo;
		iHostEventQueueHead = (iHostEventQueueHead + 1) % KDeviceStatesQueueSize;
		OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBHOSTEVENT_DUP2, "CUsbSession::UsbHostEvent; addqueue (%d, %d)", iHostEventQueueHead, iHostEventQueueTail );

 		// UsbHostStateDequeueEvent() will read from queue when RegisterHostObserver()
		// is next called.
		}
	OstTraceFunctionExit0( CUSBSESSION_USBHOSTEVENT_EXIT );
	}

/**
 * Dequeues an event and completes the observer's request with it.
 */
void CUsbSession::UsbMsgDequeue()
 	{
	OstTraceFunctionEntry0( CUSBSESSION_USBMSGDEQUEUE_ENTRY );

	// Work our way through the queue, until we reach the end
 	// OR we find an event the current observer wants.
 	if ((iMsgObserverOutstanding) && (iMsgQueueHead != iMsgQueueTail))
 		{
 		TInt newMsg = iMsgQueue[iMsgQueueTail];

 		// advance tail towards the head
 		iMsgQueueTail = (iMsgQueueTail + 1) % KOtgHostMessageQueueSize;

 		TPckg<TUint32> pckg(newMsg);
 		iNotifiedMsg = newMsg;

 		OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBMSGDEQUEUE, "CUsbSession::UsbMsgDequeue;dequeued event #%d (0x%x)", iMsgQueueTail, newMsg );

		iMsgObserverOutstanding = EFalse;
		const TInt err = iMsgObserverMessage.Write(0, pckg);
		iMsgObserverMessage.Complete(err);
   		}
  	OstTraceFunctionExit0( CUSBSESSION_USBMSGDEQUEUE_EXIT );
  	}

/**
 * Dequeues an event and completes the observer's request with it.
 */
void CUsbSession::UsbHostEventDequeue()
 	{
	OstTraceFunctionEntry0( CUSBSESSION_USBHOSTEVENTDEQUEUE_ENTRY );

	// Work our way through the queue, until we reach the end
 	// OR we find an event the current observer wants.
 	if ((iHostEventObserverOutstanding) && (iHostEventQueueHead != iHostEventQueueTail))
 		{
 		// inform the observer of state changes they are interested in AND
 		// if the cable is pulled out (EUsbDeviceStateUndefined)
 		TDeviceEventInformation newDevInfo = iHostStateQueue[iHostEventQueueTail];
		iNotifiedHostState = newDevInfo;

 		// advance tail towards the head
 		iHostEventQueueTail = (iHostEventQueueTail + 1) % KDeviceStatesQueueSize;

		OstTraceExt2( TRACE_NORMAL, CUSBSESSION_USBHOSTEVENTDEQUEUE, "CUsbSession::UsbHostEventDequeue; dequeued event #%d (0x%x)", iHostEventQueueTail, newDevInfo.iEventType );

		TPckg<TDeviceEventInformation> info(newDevInfo);
		iHostEventObserverOutstanding = EFalse;
		const TInt err = iHostEventObserverMessage.Write(0, info);
		iHostEventObserverMessage.Complete(err);

		OstTrace1( TRACE_NORMAL, CUSBSESSION_USBHOSTEVENTDEQUEUE_DUP1, "CUsbSession::UsbHostStateDequeueEvent() detects outstanding request: request is compeleted with %d", err );
   		}
   	OstTraceFunctionExit0( CUSBSESSION_USBHOSTEVENTDEQUEUE_EXIT );
   	}

TInt CUsbSession::RequestSession()
	{
	return DoRequestSession();
	}

TInt CUsbSession::DoRequestSession()
	{
	if ( iCtlSession )
		{
		if ( iCtlSession == this )
			{
	 		if (iMsgObserverQueueEvents)
	 			{
	 			UsbOtgHostMessage(KUsbMessageRequestSession);
	 			return KErrNone;
	 			}
		 	else
		 		{
		 		return KErrNotFound;
		 		}
			}
		else
			{
			return iCtlSession->DoRequestSession();
			}
		}
 	return KErrNotFound;
	}

#endif // SYMBIAN_ENABLE_USB_OTG_HOST_PRIV

