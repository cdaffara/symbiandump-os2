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

#include "activewaitforbusevent.h"
#include <usb/usblogger.h>
#include "utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "activewaitforbuseventTraces.h"
#endif




CActiveWaitForBusEvent::CActiveWaitForBusEvent(RUsbHubDriver& aHubDriver,
											   RUsbHubDriver::TBusEvent& aBusEvent,
											   MBusEventObserver& aObserver)
:	CActive(CActive::EPriorityStandard),
	iHubDriver(aHubDriver),
	iBusEvent(aBusEvent),
	iObserver(aObserver)
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORBUSEVENT_CACTIVEWAITFORBUSEVENT_CONS_ENTRY );
    
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEWAITFORBUSEVENT_CACTIVEWAITFORBUSEVENT_CONS_EXIT );
	}

CActiveWaitForBusEvent::~CActiveWaitForBusEvent()
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORBUSEVENT_CACTIVEWAITFORBUSEVENT_DES_ENTRY );
    
	Cancel();
	OstTraceFunctionExit0( CACTIVEWAITFORBUSEVENT_CACTIVEWAITFORBUSEVENT_DES_EXIT );
	}

CActiveWaitForBusEvent* CActiveWaitForBusEvent::NewL(RUsbHubDriver& aHubDriver,
													 RUsbHubDriver::TBusEvent& aBusEvent,
													 MBusEventObserver& aObserver)
	{
	CActiveWaitForBusEvent* self = new(ELeave) CActiveWaitForBusEvent(aHubDriver, aBusEvent, aObserver);
	return self;
	}

void CActiveWaitForBusEvent::Wait()
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORBUSEVENT_WAIT_ENTRY );
    
	iHubDriver.WaitForBusEvent(iBusEvent, iStatus);
	SetActive();
	OstTraceFunctionExit0( CACTIVEWAITFORBUSEVENT_WAIT_EXIT );
	}

void CActiveWaitForBusEvent::RunL()
	{
	OstTraceFunctionEntry0( CACTIVEWAITFORBUSEVENT_RUNL_ENTRY );
	OstTraceExt2( TRACE_NORMAL, CACTIVEWAITFORBUSEVENT_RUNL, "\tiStatus = %d , iBusEvent.iError=%d ", iStatus.Int(),iBusEvent.iError );
	
	iObserver.MbeoBusEvent();
	OstTraceFunctionExit0( CACTIVEWAITFORBUSEVENT_RUNL_EXIT );
	}

void CActiveWaitForBusEvent::DoCancel()
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORBUSEVENT_DOCANCEL_ENTRY );

	iHubDriver.CancelWaitForBusEvent();
	OstTraceFunctionExit0( CACTIVEWAITFORBUSEVENT_DOCANCEL_EXIT );
	}
