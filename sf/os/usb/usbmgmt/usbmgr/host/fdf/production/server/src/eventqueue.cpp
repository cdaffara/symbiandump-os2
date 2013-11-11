/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <usb/usblogger.h>
#include "eventqueue.h"
#include "fdf.h"
#include "fdfsession.h"
#include "utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "eventqueueTraces.h"
#endif

#ifdef _DEBUG
#define LOG Log()
_LIT(KPanicCategory, "eventqueue");
#else
#define LOG
#endif



class CFdfSession;

CEventQueue* CEventQueue::NewL(CFdf& aFdf)
	{
    OstTraceFunctionEntry0( CEVENTQUEUE_NEWL_ENTRY );
    
	CEventQueue* self = new(ELeave) CEventQueue(aFdf);
	OstTraceFunctionExit0( CEVENTQUEUE_NEWL_EXIT );
	return self;
	}

CEventQueue::CEventQueue(CFdf& aFdf)
:	iFdf(aFdf),
	iDeviceEvents(_FOFF(TDeviceEvent, iLink))
	{
	OstTraceFunctionEntry0( CEVENTQUEUE_CEVENTQUEUE_CONS_ENTRY );
	}

CEventQueue::~CEventQueue()
	{
    OstTraceFunctionEntry0( CEVENTQUEUE_CEVENTQUEUE_DES_ENTRY );

	// There will be things left on the queue at this time if USBMAN shuts us
	// down without having picked up everything that was on the queue.
	// This is valid behaviour and these events need destroying now.
	TSglQueIter<TDeviceEvent> iter(iDeviceEvents);
	iter.SetToFirst();
	TDeviceEvent* event;
	while ( ( event = iter++ ) != NULL )
		{
		delete event;
		}
	OstTraceFunctionExit0( CEVENTQUEUE_CEVENTQUEUE_DES_EXIT );
	}

// Increments the count of failed attachments.
void CEventQueue::AttachmentFailure(TInt aError)
	{
	OstTraceFunctionEntry0( CEVENTQUEUE_ATTACHMENTFAILURE_ENTRY );
	OstTrace1( TRACE_NORMAL, CEVENTQUEUE_ATTACHMENTFAILURE, "\taError = %d", aError );
	
    LOG;

	TUint index = 0;
	switch ( aError )
		{
	case KErrUsbSetAddrFailed:
		index = KSetAddrFailed;
		break;
	case KErrUsbNoPower:
		index = KNoPower;
		break;
	case KErrBadPower:
		index = KBadPower;
		break;
	case KErrUsbIOError:
		index = KIOError;
		break;
	case KErrUsbTimeout:
		index = KTimeout;
		break;
	case KErrUsbStalled:
		index = KStalled;
		break;
	case KErrNoMemory:
		index = KNoMemory;
		break;
	case KErrUsbConfigurationHasNoInterfaces:
		index = KConfigurationHasNoInterfaces;
		break;
	case KErrUsbInterfaceCountMismatch:
		index = KInterfaceCountMismatch;
		break;
	case KErrUsbDuplicateInterfaceNumbers:
		index = KDuplicateInterfaceNumbers;
		break;
	case KErrBadHandle:
		index = KBadHandle;
		break;

	default:
		// we must deal with every error we are ever given
	    OstTrace1( TRACE_NORMAL, CEVENTQUEUE_ATTACHMENTFAILURE_DUP1, "\tFDF did not expect this error %d as a fail attachment", aError );
	    
		index = KAttachmentFailureGeneralError;
		break;
		}
	++(iAttachmentFailureCount[index]);

	PokeSession();
	LOG;
	OstTraceFunctionExit0( CEVENTQUEUE_ATTACHMENTFAILURE_EXIT );
	}

// Called to add an event to the tail of the queue.
// Takes ownership of aEvent.
void CEventQueue::AddDeviceEvent(TDeviceEvent& aEvent)
	{
    OstTraceFunctionEntry0( CEVENTQUEUE_ADDDEVICEEVENT_ENTRY );

	OstTrace1( TRACE_NORMAL, CEVENTQUEUE_ADDDEVICEEVENT, "\t&aEvent = 0x%08x", &aEvent );
	LOG;

	iDeviceEvents.AddLast(aEvent);

	PokeSession();
	LOG;
	OstTraceFunctionExit0( CEVENTQUEUE_ADDDEVICEEVENT_EXIT );
	}

// Poke the session object (if it exists) to complete any outstanding event
// notifications it has.
// It only makes sense to call this function if there's some event to give up.
void CEventQueue::PokeSession()
	{
	OstTraceFunctionEntry0( CEVENTQUEUE_POKESESSION_ENTRY );
	// If the session exists, and has a notification outstanding, give them
	// the head event.
	CFdfSession* sess = iFdf.Session();
	if ( sess )
		{
		if ( sess->NotifyDevmonEventOutstanding() )
			{
			TInt event;
			if ( GetDevmonEvent(event) )
				{
				sess->DevmonEvent(event);
				}
			}
		if ( sess->NotifyDeviceEventOutstanding() )
			{
			TDeviceEvent event;
			if ( GetDeviceEvent(event) )
				{
				sess->DeviceEvent(event);
				}
			}
		}
	OstTraceFunctionExit0( CEVENTQUEUE_POKESESSION_EXIT );
	}

// This is called to get a device event. Attachment failures are given up
// before actual queued events.
// If ETrue is returned, the queued event is destroyed and a copy is delivered
// in aEvent.
TBool CEventQueue::GetDeviceEvent(TDeviceEvent& aEvent)
	{
    OstTraceFunctionEntry0( CEVENTQUEUE_GETDEVICEEVENT_ENTRY );
	LOG;

	TBool ret = EFalse;

	// We need to see if any attachment failures have been collected by us. If
	// there have, report exactly one of them to the client.
	for ( TUint ii = 0 ; ii < KNumberOfAttachmentFailureTypes ; ++ii )
		{
		TUint& errorCount = iAttachmentFailureCount[ii];
		if ( errorCount )
			{
			--errorCount;
			switch ( ii )
				{
			case KSetAddrFailed:
				aEvent.iInfo.iError = KErrUsbSetAddrFailed;
				break;
			case KNoPower:
				aEvent.iInfo.iError = KErrUsbNoPower;
				break;
			case KBadPower:
				aEvent.iInfo.iError = KErrBadPower;
				break;
			case KIOError:
				aEvent.iInfo.iError = KErrUsbIOError;
				break;
			case KTimeout:
				aEvent.iInfo.iError = KErrUsbTimeout;
				break;
			case KStalled:
				aEvent.iInfo.iError = KErrUsbStalled;
				break;
			case KNoMemory:
				aEvent.iInfo.iError = KErrNoMemory;
				break;
			case KConfigurationHasNoInterfaces:
				aEvent.iInfo.iError = KErrUsbConfigurationHasNoInterfaces;
				break;
			case KInterfaceCountMismatch:
				aEvent.iInfo.iError = KErrUsbInterfaceCountMismatch;
				break;
			case KDuplicateInterfaceNumbers:
				aEvent.iInfo.iError = KErrUsbDuplicateInterfaceNumbers;
				break;
			case KBadHandle:
				aEvent.iInfo.iError = KErrBadHandle;
				break;
			case KAttachmentFailureGeneralError:
				aEvent.iInfo.iError = KErrUsbAttachmentFailureGeneralError;
				break;

			case KNumberOfAttachmentFailureTypes:
			default:
				// this switch should deal with every error type we store
			    OstTrace0( TRACE_FATAL, CEVENTQUEUE_GETDEVICEEVENT, "Empty handler" );
			    __ASSERT_DEBUG(EFalse, User::Panic(KPanicCategory,__LINE__));
				}

			ret = ETrue;
			aEvent.iInfo.iEventType = EDeviceAttachment;
			OstTrace1( TRACE_NORMAL, CEVENTQUEUE_GETDEVICEEVENT_DUP1, "\treturning attachment failure event (code %d)", aEvent.iInfo.iError );
			// Only give the client one error at a time.
			break;
			}
		}

	if ( !ret && !iDeviceEvents.IsEmpty() )
		{
		TDeviceEvent* const event = iDeviceEvents.First();
		OstTrace1( TRACE_NORMAL, CEVENTQUEUE_GETDEVICEEVENT_DUP2, "\tevent = 0x%08x", event );
		iDeviceEvents.Remove(*event);
		(void)Mem::Copy(&aEvent, event, sizeof(TDeviceEvent));
		delete event;
		ret = ETrue;
		}

	LOG;
	OstTrace1( TRACE_NORMAL, CEVENTQUEUE_GETDEVICEEVENT_DUP3, "\treturning %d", ret );
	OstTraceFunctionExit0( CEVENTQUEUE_GETDEVICEEVENT_EXIT );
	return ret;
	}

TBool CEventQueue::GetDevmonEvent(TInt& aEvent)
	{
    OstTraceFunctionEntry0( CEVENTQUEUE_GETDEVMONEVENT_ENTRY );
    LOG;

	TBool ret = EFalse;

	for ( TUint ii = 0 ; ii < KNumberOfDevmonEventTypes ; ++ii )
		{
		TUint& eventCount = iDevmonEventCount[ii];
		if ( eventCount )
			{
			--eventCount;
			switch ( ii )
				{

			case KUsbDeviceRejected:
				aEvent = KErrUsbDeviceRejected;
				break;
			case KUsbDeviceFailed:
				aEvent = KErrUsbDeviceFailed;
				break;
			case KUsbBadDevice:
				aEvent = KErrUsbBadDevice;
				break;
			case KUsbBadHubPosition:
				aEvent = KErrUsbBadHubPosition;
				break;
			case KUsbBadHub:
				aEvent = KErrUsbBadHub;
				break;
			case KUsbEventOverflow:
				aEvent = KErrUsbEventOverflow;
				break;
			
			case KNumberOfDevmonEventTypes:
			default:
			    OstTrace1( TRACE_FATAL, CEVENTQUEUE_GETDEVMONEVENT, "\tUnexpected devmon error, not handled properly %d", ii );
			    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
				aEvent = KErrUsbDeviceRejected;
				// this switch should deal with every error type we store
				}

			ret = ETrue;
			// Only give the client one error at a time.
			break;
			}
		}

	LOG;
	OstTrace1( TRACE_NORMAL, CEVENTQUEUE_GETDEVMONEVENT_DUP1, "\treturning %d", ret );
	OstTraceFunctionExit0( CEVENTQUEUE_GETDEVMONEVENT_EXIT );
	return ret;
	}

void CEventQueue::AddDevmonEvent(TInt aEvent)
	{
    OstTraceFunctionEntry0( CEVENTQUEUE_ADDDEVMONEVENT_ENTRY );
    OstTrace1( TRACE_NORMAL, CEVENTQUEUE_ADDDEVMONEVENT, "\taEvent = %d", aEvent );
 
    // Increment the relevant count.
	TInt index = 0;
	switch ( aEvent )
		{

		case KErrUsbDeviceRejected:
			index = KUsbDeviceRejected;
			break;
		case KErrUsbDeviceFailed:
			index = KUsbDeviceFailed;
			break;
		case KErrUsbBadDevice:
			index = KUsbBadDevice;
			break;
		case KErrUsbBadHubPosition:
			index = KUsbBadHubPosition;
			break;
		case KErrUsbBadHub:
			index = KUsbBadHub;
			break;
		case KErrUsbEventOverflow:
			index = KUsbEventOverflow;
			break;			

		default:
		    
		    OstTrace1( TRACE_FATAL, CEVENTQUEUE_ADDDEVMONEVENT_DUP1, "Unexpected devmon error, not handled properly %d", aEvent );
		    // this switch should deal with every type of event we ever receive from devmon
		    __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
            }

	TUint& eventCount = iDevmonEventCount[index];
	if(!(eventCount < KMaxTUint))
	    {
        OstTrace1( TRACE_FATAL, CEVENTQUEUE_ADDDEVMONEVENT_DUP2, "eventCount is too big;eventCount=%d", eventCount );
        __ASSERT_DEBUG(EFalse,User::Panic(KPanicCategory,__LINE__));
	    }
	++eventCount;
	PokeSession();
	OstTraceFunctionExit0( CEVENTQUEUE_ADDDEVMONEVENT_EXIT );
	}

#ifdef _DEBUG
void CEventQueue::Log()
	{
	OstTraceFunctionEntry0( CEVENTQUEUE_LOG_ENTRY );

	for ( TUint ii = 0 ; ii < KNumberOfAttachmentFailureTypes ; ++ii )
		{
		const TInt& errorCount = iAttachmentFailureCount[ii];
		if ( errorCount )
			{
            OstTraceExt2( TRACE_DUMP, CEVENTQUEUE_LOG, "Number of attachment failures of type %d is %d", ii, errorCount );
            }
		}

	for ( TUint ii = 0 ; ii < KNumberOfDevmonEventTypes ; ++ii )
		{
		const TInt& eventCount = iDevmonEventCount[ii];
		if ( eventCount )
			{
            OstTraceExt2( TRACE_DUMP, CEVENTQUEUE_LOG_DUP1, "Number of devmon events of type %d is %d", ii, eventCount );
			}
		}

	TUint pos = 0;
	TSglQueIter<TDeviceEvent> iter(iDeviceEvents);
	iter.SetToFirst();
	TDeviceEvent* event;
	while ( ( event = iter++ ) != NULL )
		{
		OstTrace1( TRACE_DUMP, DUP1_CEVENTQUEUE_LOG_DUP2, "Logging event at position %d", pos );
		
        event->Log();
		++pos;
		}
	OstTraceFunctionExit0( CEVENTQUEUE_LOG_EXIT );
	}
#endif

