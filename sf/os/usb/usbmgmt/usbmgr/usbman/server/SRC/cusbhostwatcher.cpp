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


#include "cusbhostwatcher.h"
#include "cusbhost.h"

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "cusbhostwatcherTraces.h"
#endif



/*
 * 	Base class for USB Host watchers
 */
CActiveUsbHostWatcher::CActiveUsbHostWatcher(RUsbHostStack& aUsbHostStack, 
											 MUsbHostObserver& aOwner,
											 TUint aWatcherId):
	CActive(CActive::EPriorityStandard),
	iUsbHostStack(aUsbHostStack),
	iOwner(aOwner),
	iWatcherId(aWatcherId)
	{
    OstTraceFunctionEntry0( CACTIVEUSBHOSTWATCHER_CACTIVEUSBHOSTWATCHER_CONS_ENTRY );
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEUSBHOSTWATCHER_CACTIVEUSBHOSTWATCHER_CONS_EXIT );
	}

CActiveUsbHostWatcher::~CActiveUsbHostWatcher()
	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTWATCHER_CACTIVEUSBHOSTWATCHER_DES_ENTRY );	
	Cancel();
	OstTraceFunctionExit0( CACTIVEUSBHOSTWATCHER_CACTIVEUSBHOSTWATCHER_DES_EXIT );
	}

void CActiveUsbHostWatcher::RunL()
	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTWATCHER_RUNL_ENTRY );

	ASSERT(iStatus.Int() == KErrNone);
	iOwner.NotifyHostEvent(iWatcherId);
	Post();
	OstTraceFunctionExit0( CACTIVEUSBHOSTWATCHER_RUNL_EXIT );
	}



/*
 * 	Monitors host events (attach/load/detach)
 */
CActiveUsbHostEventWatcher* CActiveUsbHostEventWatcher::NewL(
												RUsbHostStack& aUsbHostStack, 
												MUsbHostObserver& aOwner,
												TDeviceEventInformation& aHostEventInfo)
	{
	CActiveUsbHostEventWatcher* self = new (ELeave) CActiveUsbHostEventWatcher(aUsbHostStack,aOwner,aHostEventInfo);
	return self;
	}


CActiveUsbHostEventWatcher::CActiveUsbHostEventWatcher(
									RUsbHostStack& aUsbHostStack,
									MUsbHostObserver& aOwner,
									TDeviceEventInformation& aHostEventInfo)
									:CActiveUsbHostWatcher(aUsbHostStack,
														   aOwner,
														   KHostEventMonitor)
														 , iHostEventInfo(aHostEventInfo)

	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTEVENTWATCHER_CACTIVEUSBHOSTEVENTWATCHER_CONS_ENTRY );
	OstTraceFunctionExit0( CACTIVEUSBHOSTEVENTWATCHER_CACTIVEUSBHOSTEVENTWATCHER_CONS_EXIT );
	}

CActiveUsbHostEventWatcher::~CActiveUsbHostEventWatcher()
	{
    OstTraceFunctionEntry0( CACTIVEUSBHOSTEVENTWATCHER_CACTIVEUSBHOSTEVENTWATCHER_DES_ENTRY );
	Cancel();
	OstTraceFunctionExit0( CACTIVEUSBHOSTEVENTWATCHER_CACTIVEUSBHOSTEVENTWATCHER_DES_EXIT );
	}

void CActiveUsbHostEventWatcher::Post()
	{
    OstTraceFunctionEntry0( CACTIVEUSBHOSTEVENTWATCHER_POST_ENTRY );

	iUsbHostStack.NotifyDeviceEvent(iStatus, iHostEventInfo);
	SetActive();
	OstTraceFunctionExit0( CACTIVEUSBHOSTEVENTWATCHER_POST_EXIT );
	}

void CActiveUsbHostEventWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTEVENTWATCHER_DOCANCEL_ENTRY );

	iUsbHostStack.NotifyDeviceEventCancel();
	OstTraceFunctionExit0( CACTIVEUSBHOSTEVENTWATCHER_DOCANCEL_EXIT );
	}


/*
 * 	Monitors device monitor events
 */

CActiveUsbHostMessageWatcher* CActiveUsbHostMessageWatcher::NewL(
		RUsbHostStack& aUsbHostStack, 
		MUsbHostObserver& aOwner,
		TInt& aHostMessage)
	{
	CActiveUsbHostMessageWatcher* self = new (ELeave)CActiveUsbHostMessageWatcher(aUsbHostStack,aOwner,aHostMessage);
	return self;
	}

CActiveUsbHostMessageWatcher::~CActiveUsbHostMessageWatcher()
	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTMESSAGEWATCHER_CACTIVEUSBHOSTMESSAGEWATCHER_DES_ENTRY ); 
	Cancel();
	OstTraceFunctionExit0( CACTIVEUSBHOSTMESSAGEWATCHER_CACTIVEUSBHOSTMESSAGEWATCHER_DES_EXIT );
	}

CActiveUsbHostMessageWatcher::CActiveUsbHostMessageWatcher(
										RUsbHostStack& aUsbHostStack,
										MUsbHostObserver& aOwner,
										TInt& aHostMessage)
										:CActiveUsbHostWatcher(aUsbHostStack,
															   aOwner,
															   KHostMessageMonitor)
										, iHostMessage(aHostMessage)
	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTMESSAGEWATCHER_CACTIVEUSBHOSTMESSAGEWATCHER_CONS_ENTRY );
	OstTraceFunctionExit0( CACTIVEUSBHOSTMESSAGEWATCHER_CACTIVEUSBHOSTMESSAGEWATCHER_CONS_EXIT );
	}

void CActiveUsbHostMessageWatcher::Post()
	{
    OstTraceFunctionEntry0( CACTIVEUSBHOSTMESSAGEWATCHER_POST_ENTRY );
	
	iUsbHostStack.NotifyDevmonEvent(iStatus, iHostMessage);
	SetActive();
	OstTraceFunctionExit0( CACTIVEUSBHOSTMESSAGEWATCHER_POST_EXIT );
	}


void CActiveUsbHostMessageWatcher::DoCancel()
	{
	OstTraceFunctionEntry0( CACTIVEUSBHOSTMESSAGEWATCHER_DOCANCEL_ENTRY );
	
	iUsbHostStack.NotifyDevmonEventCancel();
	OstTraceFunctionExit0( CACTIVEUSBHOSTMESSAGEWATCHER_DOCANCEL_EXIT );
	}

