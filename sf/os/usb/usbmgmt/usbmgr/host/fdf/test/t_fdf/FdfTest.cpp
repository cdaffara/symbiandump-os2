/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "fdftest.h"
#include "testmanager.h"
#include <usb/usblogger.h>
#include <e32cmn.h>

#include <d32usbdi_hubdriver.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, "fdftest      ");
#endif
//--------------------------------

CActiveNotifyDeviceEvent::CActiveNotifyDeviceEvent(RUsbHostStack& aUsbHostStack,
												   MNotifyDeviceEventObserver& aObserver,
												   TDeviceEventInformation& aDeviceEventInformation)
:	CActive(CActive::EPriorityStandard),
	iUsbHostStack(aUsbHostStack),
	iObserver(aObserver),
	iDeviceEventInformation(aDeviceEventInformation)
	{
	CActiveScheduler::Add(this);
	}

CActiveNotifyDeviceEvent::~CActiveNotifyDeviceEvent()
	{
	Cancel();
	}

CActiveNotifyDeviceEvent* CActiveNotifyDeviceEvent::NewL(RUsbHostStack& aUsbHostStack,
														 MNotifyDeviceEventObserver& aObserver,
														 TDeviceEventInformation& aDeviceEventInformation)
	{
	CActiveNotifyDeviceEvent* self = new(ELeave) CActiveNotifyDeviceEvent(aUsbHostStack,
		aObserver,
		aDeviceEventInformation);
	return self;
	}

void CActiveNotifyDeviceEvent::Post()
	{
	iUsbHostStack.NotifyDeviceEvent(iStatus, iDeviceEventInformation);
	SetActive();
	}

void CActiveNotifyDeviceEvent::RunL()
	{
	iObserver.MndeoDeviceEvent();

	// We don't want to get into an infinite loop if the FDF dies.
	if ( iStatus.Int() != KErrServerTerminated )
		{
		Post();
		}
	}

void CActiveNotifyDeviceEvent::DoCancel()
	{
	iUsbHostStack.NotifyDeviceEventCancel();
	}

//--------------------------------

CActiveNotifyDevmonEvent::CActiveNotifyDevmonEvent(RUsbHostStack& aUsbHostStack,
												   MNotifyDevmonEventObserver& aObserver,
												   TInt& aEvent)
:	CActive(CActive::EPriorityStandard),
	iUsbHostStack(aUsbHostStack),
	iObserver(aObserver),
	iEvent(aEvent)
	{
	CActiveScheduler::Add(this);
	}

CActiveNotifyDevmonEvent::~CActiveNotifyDevmonEvent()
	{
	Cancel();
	}

CActiveNotifyDevmonEvent* CActiveNotifyDevmonEvent::NewL(RUsbHostStack& aUsbHostStack,
														 MNotifyDevmonEventObserver& aObserver,
														 TInt& aEvent)
	{
	CActiveNotifyDevmonEvent* self = new(ELeave) CActiveNotifyDevmonEvent(aUsbHostStack,
		aObserver,
		aEvent);
	return self;
	}

void CActiveNotifyDevmonEvent::Post()
	{
	iUsbHostStack.NotifyDevmonEvent(iStatus, iEvent);
	SetActive();
	}

void CActiveNotifyDevmonEvent::RunL()
	{
	iObserver.MndeoDevmonEvent();

	// We don't want to get into an infinite loop if the FDF dies.
	if ( iStatus.Int() != KErrServerTerminated )
		{
		Post();
		}
	}

void CActiveNotifyDevmonEvent::DoCancel()
	{
	iUsbHostStack.NotifyDevmonEventCancel();
	}

//--------------------------------

CFdfTest::CFdfTest(MTestManager& aManager)
 :	CTestBase(aManager)
	{
	}

CTestBase* CFdfTest::NewL(MTestManager& aManager)
	{
	CFdfTest* self = new(ELeave) CFdfTest(aManager);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CFdfTest::ConstructL()
	{
	iNotifyDeviceEvent = CActiveNotifyDeviceEvent::NewL(iUsbHostStack,
		*this,
		iDeviceEventInformation);
	iNotifyDevmonEvent = CActiveNotifyDevmonEvent::NewL(iUsbHostStack,
		*this,
		iDevmonEvent);
	}

CFdfTest::~CFdfTest()
	{
	iManager.Write(_L8("CFdfTest::~CFdfTest"));

	delete iNotifyDeviceEvent;
	delete iNotifyDevmonEvent;
	iOtgDriver.Close();
	iUsbHostStack.Close();
	}

void CFdfTest::ProcessKeyL(TKeyCode aKeyCode)
	{
	iManager.Write(_L8("CFdfTest::ProcessKeyL"));

	switch ( aKeyCode )
		{
	case '0':
		{
		if ( iUsbHostStack.Handle() )
			{
			iManager.Write(_L8("\talready connected"));
			}
		else
			{

			iManager.Write(_L8("About to open LDD"));
			_LIT(KUsbOtgLDDName,"otgdi");
			TInt err = User::LoadLogicalDevice(KUsbOtgLDDName);
			if ( (err != KErrNone) && (err != KErrAlreadyExists) )
				{
				iManager.Write(_L8("Error %d: Unable to load driver: %S"), err, &KUsbOtgLDDName);
				LEAVEIFERRORL(err);
				}

			iManager.Write(_L8("About to open RUsbOtgDriver"));
			err = iOtgDriver.Open();
			if (err != KErrNone && err != KErrAlreadyExists)
				{
				iManager.Write(_L8("Error %d: Unable to open RUsbOtgDriver session"), err);
				LEAVEIFERRORL(err);
				}

			iManager.Write(_L8("About to start OTG stacks"));
			TInt iLastError = iOtgDriver.StartStacks();
			if (iLastError != KErrNone)
				{
				iManager.Write(_L8("Error %d: Unable to open start OTG stacks"), iLastError);
				LEAVEIFERRORL(err);
				}

			TInt errorBus = iOtgDriver.BusRequest();
			iManager.Write(_L8("Error %d: iOtgDriver.BusRequest()"), errorBus);
			if (errorBus == KErrNone)
				{
				iBusRequested = 1;
				}


			TInt error = iUsbHostStack.Connect();
			iManager.Write(_L8("\terror = %d"), error);
			if ( !error )
				{
				iManager.Write(_L8("\t Post() to wait for event "));
				iNotifyDeviceEvent->Post();
				iNotifyDevmonEvent->Post();
				iManager.Write(_L8("\t After Post() to wait for event "));
				}
			}
		}
		break;

	case '1':
		{
		TInt err = iUsbHostStack.EnableDriverLoading();
		iManager.Write(_L8("\terr = %d"), err);
		}
		break;

	case '2':
		iUsbHostStack.DisableDriverLoading();
		break;

	case '3':
		iNotifyDeviceEvent->Cancel();
		iNotifyDevmonEvent->Cancel();
		iUsbHostStack.Close();
		break;
	case '4':
		if(iBusRequested)
			{
			iManager.Write(_L8("Call BusDrop()"));
			TInt err = iOtgDriver.BusDrop();
			iManager.Write(_L8("Call BusDrop() err = %d "),err);
			}
		else
			{
			iManager.Write(_L8("Cannot Call BusDrop() as the bus has not been Requested()"));
			}
		break;

	default:
		iManager.Write(_L8("Unknown selection"));
		break;
		}
	}

void CFdfTest::DisplayTestSpecificMenu()
	{
	iManager.Write(_L8("0 - RUsbHostStack::Connect"));
	iManager.Write(_L8("1 - RUsbHostStack::EnableDriverLoading"));
	iManager.Write(_L8("2 - RUsbHostStack::DisableDriverLoading"));
	iManager.Write(_L8("3 - RUsbHostStack::Close"));
	iManager.Write(_L8("4 - RUsbHostStack::StopBus"));

	}

void CFdfTest::MndeoDeviceEvent()
	{
	iManager.Write(_L8("CFdfTest::MndeoDeviceEvent"));
	iManager.Write(KNullDesC8());
	iManager.Write(_L8("CFdfTest::MndeoDeviceEvent"));

	// Apologies for the squashed-up output this gives, but this text is no
	// good if it scrolls off the top of the target board's screen.
	iManager.Write(_L8("Raw data:"));
	iManager.Write(_L8("device ID = %d, event type = %d, error = %d, driver load status = %d, vid = 0x%04x, pid = 0x%04x"),
		iDeviceEventInformation.iDeviceId,
		iDeviceEventInformation.iEventType,
		iDeviceEventInformation.iError,
		iDeviceEventInformation.iDriverLoadStatus,
		iDeviceEventInformation.iVid,
		iDeviceEventInformation.iPid);

	iManager.Write(_L8("Interpretation:"));

	_LIT8(KAttachment, "Attachment");
	_LIT8(KDriverLoad, "Driver load");
	_LIT8(KDetachment, "Detachment");
	TFixedArray<const TDesC8*, 3> eventType;
	eventType[EDeviceAttachment] = &KAttachment();
	eventType[EDriverLoad] = &KDriverLoad();
	eventType[EDeviceDetachment] = &KDetachment();

	_LIT8(KSuccess, "Success");
	_LIT8(KPartialSuccess, "Partial success");
	_LIT8(KFailure, "Failure");
	TFixedArray<const TDesC8*, 3> driverLoadStatus;
	driverLoadStatus[EDriverLoadSuccess] = &KSuccess();
	driverLoadStatus[EDriverLoadPartialSuccess] = &KPartialSuccess();
	driverLoadStatus[EDriverLoadFailure] = &KFailure();

	switch ( iDeviceEventInformation.iEventType )
		{
	case EDeviceAttachment:
		{
		iManager.Write(_L8("%S | device %d | error %d"),
			eventType[iDeviceEventInformation.iEventType],
			iDeviceEventInformation.iDeviceId,
			iDeviceEventInformation.iError);
		}
		break;
	case EDriverLoad:
		iManager.Write(_L8("%S | device %d | %S | error %d"),
			eventType[iDeviceEventInformation.iEventType],
			iDeviceEventInformation.iDeviceId,
			driverLoadStatus[iDeviceEventInformation.iDriverLoadStatus],
			iDeviceEventInformation.iError);
		break;
	case EDeviceDetachment:
		iManager.Write(_L8("%S | device %d"),
			eventType[iDeviceEventInformation.iEventType],
			iDeviceEventInformation.iDeviceId);
		break;
	default:
		iManager.Write(_L8("INVALID iEventType"));
		return;
		}

	if ( iDeviceEventInformation.iEventType == EDeviceAttachment && iDeviceEventInformation.iError == KErrNone )
		{
		iManager.Write(_L8("VID: 0x%04x, PID: 0x%04x"),
			iDeviceEventInformation.iVid,
			iDeviceEventInformation.iPid);

		RArray<TUint> langIds;
		TInt err = iUsbHostStack.GetSupportedLanguages(iDeviceEventInformation.iDeviceId, langIds);
		if ( err == KErrNone )
			{
			const TUint count = langIds.Count();
			iManager.Write(_L8("there is/are %d supported language(s)"), count);
			for ( TUint ii = 0 ; ii < count ; ++ii )
				{
				iManager.Write(_L8("using langid %d (index %d)"), langIds[ii], ii);

				TName string;

				TInt err = iUsbHostStack.GetManufacturerStringDescriptor(iDeviceEventInformation.iDeviceId, langIds[ii], string);
				if ( err == KErrNone )
					{
					TBuf8<256> buf;
					buf.Copy(string);
					iManager.Write(_L8("manufacturer string descriptor = \"%S\""), &buf);
					}
				else
					{
					iManager.Write(_L8("GetManufacturerStringDescriptor returned %d"), err);
					}

				err = iUsbHostStack.GetProductStringDescriptor(iDeviceEventInformation.iDeviceId, langIds[ii], string);
				if ( err == KErrNone )
					{
					TBuf8<256> buf;
					buf.Copy(string);
					iManager.Write(_L8("product string descriptor = \"%S\""), &buf);
					}
				else
					{
					iManager.Write(_L8("GetProductStringDescriptor returned %d"), err);
					}
				}
			}
		else
			{
			iManager.Write(_L8("GetSupportedLanguages returned %d"), err);
			}
		langIds.Close();

		}
	}

void CFdfTest::MndeoDevmonEvent()
	{
	iManager.Write(_L8("CFdfTest::MndeoDevmonEvent"));
	iManager.Write(KNullDesC8());
	iManager.Write(_L8("CFdfTest::MndeoDevmonEvent"));

	iManager.Write(_L8("\tdevmon event: %d"), iDevmonEvent);
	}
