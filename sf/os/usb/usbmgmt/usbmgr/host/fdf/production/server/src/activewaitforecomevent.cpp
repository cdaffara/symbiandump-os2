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
#include "activewaitforecomevent.h"
#include "utils.h"
#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "activewaitforecomeventTraces.h"
#endif
#ifdef _DEBUG
_LIT( KFdfEcomEventAOPanicCategory, "FdfEcomEventAO" );
#endif

CActiveWaitForEComEvent::CActiveWaitForEComEvent(MEComEventObserver& aObserver)
:	CActive(CActive::EPriorityStandard),
	iObserver(aObserver)
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORECOMEVENT_CACTIVEWAITFORECOMEVENT_CONS_ENTRY );
    
	CActiveScheduler::Add(this);
	OstTraceFunctionExit0( CACTIVEWAITFORECOMEVENT_CACTIVEWAITFORECOMEVENT_CONS_EXIT );
	}

CActiveWaitForEComEvent::~CActiveWaitForEComEvent()
	{
	OstTraceFunctionEntry0( CACTIVEWAITFORECOMEVENT_CACTIVEWAITFORECOMEVENT_DES_ENTRY );
	
    Cancel();
	iEComSession.Close();
	REComSession::FinalClose();
	OstTraceFunctionExit0( CACTIVEWAITFORECOMEVENT_CACTIVEWAITFORECOMEVENT_DES_EXIT );
	}

CActiveWaitForEComEvent* CActiveWaitForEComEvent::NewL(MEComEventObserver& aObserver)
	{
	CActiveWaitForEComEvent* self = new(ELeave) CActiveWaitForEComEvent(aObserver);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}


void CActiveWaitForEComEvent::ConstructL()
	{
	iEComSession = REComSession::OpenL();
	}

void CActiveWaitForEComEvent::Wait()
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORECOMEVENT_WAIT_ENTRY );
    
	iEComSession.NotifyOnChange(iStatus);
	SetActive();
	OstTraceFunctionExit0( CACTIVEWAITFORECOMEVENT_WAIT_EXIT );
	}

void CActiveWaitForEComEvent::RunL()
	{
	OstTraceFunctionEntry0( CACTIVEWAITFORECOMEVENT_RUNL_ENTRY );
	
    iObserver.EComEventReceived();
	Wait();
	OstTraceFunctionExit0( CACTIVEWAITFORECOMEVENT_RUNL_EXIT );
	}

void CActiveWaitForEComEvent::DoCancel()
	{
    OstTraceFunctionEntry0( CACTIVEWAITFORECOMEVENT_DOCANCEL_ENTRY );
    
	iEComSession.CancelNotifyOnChange(iStatus);
	OstTraceFunctionExit0( CACTIVEWAITFORECOMEVENT_DOCANCEL_EXIT );
	}

TInt CActiveWaitForEComEvent::RunError(TInt aError)
	{
	OstTraceFunctionEntry0( CACTIVEWAITFORECOMEVENT_RUNERROR_ENTRY );
	OstTrace1( TRACE_FATAL, CACTIVEWAITFORECOMEVENT_RUNERROR, "ECOM change notification error = %d ", aError );
	
    __ASSERT_DEBUG(EFalse,User::Panic(KFdfEcomEventAOPanicCategory, aError));
	OstTraceFunctionExit0( CACTIVEWAITFORECOMEVENT_RUNERROR_EXIT );
	return KErrNone;
	}
