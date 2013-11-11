/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "usbchargingarmtest.h"
#include <usb/usblogger.h>
#include <e32cmn.h>
#include "usbbatterycharging.h"
#include "testmanager.h"

CUsbChargingArmTest::CUsbChargingArmTest(MTestManager& aManager)
 :	CTestBase(aManager)
	{
	}

CTestBase* CUsbChargingArmTest::NewL(MTestManager& aManager)
	{
	CUsbChargingArmTest* self = new(ELeave) CUsbChargingArmTest(aManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CUsbChargingArmTest::ConstructL()
	{

	// -- Insert initialization code here
	iRepository = CRepository::NewL(KUsbBatteryChargingCentralRepositoryUid);
	iId = KUsbBatteryChargingKeyEnabledUserSetting;

	}

CUsbChargingArmTest::~CUsbChargingArmTest()
	{
	iManager.Write(_L8("CUsbChargingArmTest::~CUsbChargingArmTest"));

	// -- Insert cleanup code here
	delete iRepository;
	}

void CUsbChargingArmTest::ProcessKeyL(TKeyCode aKeyCode)
	{
	iManager.Write(_L8("CUsbChargingArmTest::ProcessKeyL"));

	switch ( aKeyCode )
		{
	case '0':
		{
		iManager.Write(_L8("About to connect to USBMan"));
		TInt res = iUsbMan.Connect();
		if(res != KErrNone)
			{
			iManager.Write(_L8("Connection to USBMan failed"));
			}
		else
			{
			iManager.Write(_L8("Now connected to USBMan"));
			}
		}
		break;

	case '1':
		{
		iManager.Write(_L8("About to start USBMan"));
		TRequestStatus requestStatus;
		iUsbMan.Start(requestStatus);
		User::WaitForRequest(requestStatus);
		if(requestStatus.Int() == KErrNone)
			{
			iManager.Write(_L8("USBMan Started OK"));
			}
		else
			{
			iManager.Write(_L8("Failed to start USBMan"));
			}
		}
		break;
	case '2':
		{
		iManager.Write(_L8("About to stop USBMan"));
		TRequestStatus requestStatus;
		iUsbMan.Stop(requestStatus);
		User::WaitForRequest(requestStatus);
		if(requestStatus.Int() == KErrNone)
			{
			iManager.Write(_L8("USBMan Stopped OK"));
			}
		else
			{
			iManager.Write(_L8("Failed to stop USBMan"));
			}
		}
		break;
	case '3':
		{
		TInt state = KErrUnknown;
		iRepository->Get(iId, state);
		iManager.Write(_L8("CenRep Enable setting is: <%d>"), state);
		}
		break;
	case '4':
		{
		TInt state = KErrUnknown;
		iRepository->Get(iId, state);
		iManager.Write(_L8("CenRep Enable setting is: <%d>"), state);
		if(state == EUsbBatteryChargingUserSettingDisabled)
			{
			iRepository->Set(iId, EUsbBatteryChargingUserSettingEnabled);
			}
		else
			{
			iRepository->Set(iId, EUsbBatteryChargingUserSettingDisabled);
			}
		
		iRepository->Get(iId, state);
		iManager.Write(_L8("CenRep Enable setting is now: <%d>"), state);
		}
		break;


	default:
		iManager.Write(_L8("Unknown selection"));
		break;
		}
	}

void CUsbChargingArmTest::DisplayTestSpecificMenu()
	{
	iManager.Write(_L8("0 - RUsb::Connect"));
	iManager.Write(_L8("1 - RUsb::Start"));
	iManager.Write(_L8("2 - RUsb::Stop"));
	iManager.Write(_L8("3 - CenRep Enable setting read"));
	iManager.Write(_L8("4 - CenRep Enable setting toggle"));

	// -- Add test titles here
	}

