/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/** @file
 @internalComponent
 @test
 */

#include "servicewatcher.h"
#include "ncmtestconsole.h"
#include "ncmtestdefines.hrh"

CServiceWatcher* CServiceWatcher::NewL(CUsbNcmConsole& aTestConsole)
	{
	LOG_STATIC_FUNC_ENTRY

	CServiceWatcher* self = new(ELeave) CServiceWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CServiceWatcher::~CServiceWatcher()
	{
	Cancel();
	}

CServiceWatcher::CServiceWatcher(CUsbNcmConsole& aTestConsole)
	: CActive(EPriorityStandard), iTestConsole(aTestConsole)
	{
	CActiveScheduler::Add(this);
	}

void CServiceWatcher::ConstructL()
	{
	iTestConsole.Usb().ServiceStateNotification(iServiceState, iStatus);
	SetActive();

	TUsbServiceState serviceState;
	User::LeaveIfError(iTestConsole.Usb().GetServiceState(serviceState));
	DisplayServiceState(serviceState);
	}

void CServiceWatcher::DoCancel()
	{
	iTestConsole.Usb().ServiceStateNotificationCancel();
	}

void CServiceWatcher::RunL()
	{	
	LOG_FUNC
	__FLOG_STATIC2(KSubSys, KLogComponent , 
			_L8("CServiceWatcherCommand::RunL - iStatus = %d, iServiceState = %d"), 
			iStatus.Int(), iServiceState);

	User::LeaveIfError(iStatus.Int());

	iTestConsole.Usb().ServiceStateNotification(iServiceState, iStatus);
	SetActive();

	iTestConsole.Usb().GetServiceState(iServiceState);
	//Display the service state on main console
	DisplayServiceState(iServiceState);

	}

TInt CServiceWatcher::RunError(TInt aError)
	{
	User::Panic(_L("CServiceWatcher"), aError);
	return aError;
	}

void CServiceWatcher::DisplayServiceState(TUsbServiceState aServiceState)
/**
Display the service state on main console
  @param  aServiceState 	Service state
*/
	{
	TBuf<DISPLAY_USB_SERVICE_STATE_LEN> servStatus;
	switch(aServiceState)
		{
    case EUsbServiceIdle:
    	servStatus =
    		//  12345678901
			_L("Idle       ");
    	break;

	case EUsbServiceStarting:
		servStatus =
    		//  12345678901
			_L("Starting   ");
		break;

	case EUsbServiceStarted:
		servStatus =
    		//  12345678901
			_L("Started    ");
		break;

	case EUsbServiceStopping:
		servStatus =
    		//  12345678901
			_L("Stopping   ");
		break;

	case EUsbServiceFatalError:
		servStatus =
    		//  12345678901
			_L("Error      ");
		break;

	default:
		servStatus =
			_L("Unknown    ");

		break;
		}
	
	iTestConsole.SetDisplayItem(EUsbServiceStateItem, servStatus);
	
	}
