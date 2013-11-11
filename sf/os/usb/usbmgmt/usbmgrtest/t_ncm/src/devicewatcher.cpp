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

#include "devicewatcher.h"
#include "ncmtestconsole.h"

CDeviceWatcher* CDeviceWatcher::NewL(CUsbNcmConsole& aTestConsole)
	{
	LOG_STATIC_FUNC_ENTRY
	
	CDeviceWatcher* self = new(ELeave) CDeviceWatcher(aTestConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CDeviceWatcher::~CDeviceWatcher()
	{
	Cancel();
	iLdd.Close();
	}

CDeviceWatcher::CDeviceWatcher(CUsbNcmConsole& aTestConsole)
	: CActive(EPriorityStandard), iTestConsole(aTestConsole), iWatchMode(EWatchMode)
	{
	CActiveScheduler::Add(this);
	}

void CDeviceWatcher::ConstructL()
	{
	User::LeaveIfError(iLdd.Open(0));
	iLdd.AlternateDeviceStatusNotify(iStatus, iDeviceState);

	SetActive();

	//Get current device state
	TUsbcDeviceState deviceState;
	User::LeaveIfError(iLdd.DeviceStatus(deviceState));

	DisplayDeviceState(deviceState);
	
	}

void CDeviceWatcher::DoCancel()
	{
	iLdd.AlternateDeviceStatusNotifyCancel();
	}

void CDeviceWatcher::RunL()
	{
	//Get USB device state change
	iLdd.AlternateDeviceStatusNotify(iStatus, iDeviceState);
	SetActive();

	LOG_FUNC
	__FLOG_STATIC1(KSubSys, KLogComponent , _L8("CDeviceWatcherCommand::RunL - iStatus = %d"), iStatus.Int());

	//Display the current state on main console
	DisplayDeviceState(iDeviceState);
 
	TUsbServiceState serviceState;
	User::LeaveIfError(iTestConsole.Usb().GetServiceState(serviceState));

	__FLOG_STATIC3(KSubSys, KLogComponent , 
			_L8("iDeviceState = %d, serviceState = %d, iStartNcm=%d"), 
			iDeviceState, serviceState, iStartNcm);

	iOldDeviceState = iDeviceState;
	}

TInt CDeviceWatcher::RunError(TInt aError)
	{
	User::Panic(_L("CDeviceWatcher"), aError);
	return aError;
	}

TDeviceWatchMode CDeviceWatcher::GetWatchMode() const
	{
	return iWatchMode;
	}

void CDeviceWatcher::SetWatchMode(TDeviceWatchMode aWatchMode)
/**
Set the watch mode : If the mode is EWatchMode, only monitor the USB device status.
If the mode is EWatchAndStartMode, when cable plugin, NCM will be started automaticly
  @param  aWatchMode 	The mode user want to set 
*/
	{	
	iWatchMode = aWatchMode;
	switch(iWatchMode)
		{
		case EWatchMode:
			iStartNcm = EFalse;
			break;
		case EWatchAndStartMode:
			iStartNcm = ETrue;
			break;
		}
	//When USB device state changed, CDeviceWatcher will check iStartNcm. The detail please see
	//RunL()
	}

void CDeviceWatcher::DisplayDeviceState(TUint aDeviceState)
/**
Display device status on main console
  @param  aDeviceState 	the device state
*/
	{
 	TBuf<DISPLAY_USB_DEVICE_STATE_LEN> devStatus;
	switch(aDeviceState)
		{
	case EUsbcDeviceStateUndefined:
		devStatus =
    		//  12345678901
			_L("Undefined  ");
		break;

	case EUsbcDeviceStateDefault:
		devStatus =
    		//  12345678901
			_L("Default    ");
		break;

	case EUsbcDeviceStateAttached:
		devStatus =
    		//  12345678901
			_L("Attached   ");
		break;

	case EUsbcDeviceStatePowered:
		devStatus =
    		//  12345678901
			_L("Powered    ");
		break;

	case EUsbcDeviceStateConfigured:
		devStatus =
    		//  12345678901
			_L("Configured ");
		break;

	case EUsbcDeviceStateAddress:
		devStatus =
    		//  12345678901
			_L("Address    ");
		break;

	case EUsbcDeviceStateSuspended:
		devStatus =
    		//  12345678901
			_L("Suspended  ");
		break;
	default:
		devStatus =
		    //  12345678901
			_L("Unknown    ");
		break;
		}
	iTestConsole.SetDisplayItem(EUsbDeviceStateItem, devStatus);
	}
