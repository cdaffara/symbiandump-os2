/*
* Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#ifndef __OVER_DUMMYUSBLDD__
#include <d32usbc.h>
#else
#include <dummyusblddapi.h>
#endif
#include "AcmPanic.h"
#include "linkstatenotifier.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "linkstatenotifierTraces.h"
#endif


CLinkStateNotifier* CLinkStateNotifier::NewL(MLinkStateObserver& aParent, RDevUsbcClient& aUsb)
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_NEWL_ENTRY );
	CLinkStateNotifier* self = new (ELeave) CLinkStateNotifier(aParent, aUsb);
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_NEWL_EXIT );
	return self;
	}


CLinkStateNotifier::CLinkStateNotifier(MLinkStateObserver& aParent, RDevUsbcClient& aUsb)
	 : CActive(EPriorityStandard),
	   iParent(aParent), iUsb(aUsb)
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_CLINKSTATENOTIFIER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_CLINKSTATENOTIFIER_CONS_EXIT );
	}



/**
CObexUsbHandler destructor.
*/
CLinkStateNotifier::~CLinkStateNotifier()
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_CLINKSTATENOTIFIER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_CLINKSTATENOTIFIER_DES_EXIT );
	}


/**
Standard active object error function.

@return	KErrNone because currently nothing should cause this to be called.
*/
TInt CLinkStateNotifier::RunError(TInt /*aError*/)
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_RUNERROR_ENTRY );
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_RUNERROR_EXIT );
	return KErrNone;
	}


/**
This function will be called upon a change in the state of the device
(as set up in AcceptL).
*/
void CLinkStateNotifier::RunL()
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_RUNL_ENTRY );
	OstTrace1( TRACE_NORMAL, CLINKSTATENOTIFIER_RUNL, "CLinkStateNotifier::RunL;called state=0x%X", iUsbState );
	
	if (iStatus != KErrNone)
		{
		OstTrace1( TRACE_NORMAL, CLINKSTATENOTIFIER_RUNL_DUP1, "CLinkStateNotifier::RunL;- Error = %d", iStatus.Int() );
		LinkDown();
		iParent.MLSOStateChange(KDefaultMaxPacketTypeBulk);

		OstTraceFunctionExit0( CLINKSTATENOTIFIER_RUNL_EXIT );
		return;
		}

	if (!(iUsbState & KUsbAlternateSetting))
		{
		TUsbcDeviceState deviceState = static_cast<TUsbcDeviceState>(iUsbState);

		switch(deviceState)
			{
		case EUsbcDeviceStateUndefined:
			LinkDown();
			break;
				
		case EUsbcDeviceStateAttached:
		case EUsbcDeviceStatePowered:
		case EUsbcDeviceStateDefault:
		case EUsbcDeviceStateAddress:
		case EUsbcDeviceStateSuspended:
			break;

		case EUsbcDeviceStateConfigured:
			LinkUp();
			break;

		default:
			OstTrace0( TRACE_FATAL, CLINKSTATENOTIFIER_RUNL_DUP2, "CLinkStateNotifier::RunL;EPanicUnknownDeviceState" );
			__ASSERT_DEBUG( EFalse, User::Panic(KAcmPanicCat, EPanicUnknownDeviceState) );
			break;
			}
		}

	iParent.MLSOStateChange(iPacketSize || KDefaultMaxPacketTypeBulk);

	// Await further notification of a state change.
	iUsb.AlternateDeviceStatusNotify(iStatus, iUsbState);
	SetActive();
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_RUNL_EXIT_DUP1 );
	}


/**
Standard active object cancellation function.
*/
void CLinkStateNotifier::DoCancel()
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_DOCANCEL_ENTRY );
	iUsb.AlternateDeviceStatusNotifyCancel();
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_DOCANCEL_EXIT );
	}



/**
Accept an incoming connection.
*/
void CLinkStateNotifier::Start()
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_START_ENTRY );
	iUsb.AlternateDeviceStatusNotify(iStatus, iUsbState);
	SetActive();
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_START_EXIT );
	}


void CLinkStateNotifier::LinkUp()
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_LINKUP_ENTRY );
	if (iUsb.CurrentlyUsingHighSpeed())
		{
		iPacketSize = KMaxPacketTypeBulkHS;
		}
	else
		{
		iPacketSize = KMaxPacketTypeBulkFS;
		}
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_LINKUP_EXIT );
	}


void CLinkStateNotifier::LinkDown()
	{
	OstTraceFunctionEntry0( CLINKSTATENOTIFIER_LINKDOWN_ENTRY );
	iPacketSize = 0;
	OstTraceFunctionExit0( CLINKSTATENOTIFIER_LINKDOWN_EXIT );
	}
