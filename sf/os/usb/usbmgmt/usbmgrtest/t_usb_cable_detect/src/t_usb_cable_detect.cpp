/*
* Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* This is a simple program which demonstrates how to start USB services when
* the cable is plugged in, and stop them when it's unplugged. A couple of
* caveats:
* 1. This mechanism will only work on certain hardware platforms (eg. Lubbock).
* 2. The USB Manager will be loaded for the lifetime of this program.
*
*/

#include <e32base.h>
#include <e32math.h>
#include <e32cons.h>
#include <badesca.h>
#include <c32comm.h>
#include <usbman.h>

// The name of the logical device driver to load.
_LIT(KUsbLddName, "EUSBC");

// An argument which, when passed to this program, means "don't run another
// instance of yourself in the background".
_LIT(KForkFlag, "f");

class CUsbCableDetector : public CActive
/**
 * This class provides a USB cable detection service. It monitors the state of
 * the USB cable and starts and stops USB services appropriately.
 */
	{
public:
	enum TReqState
		{
		EIdle = 0,
		ENotifyDeviceStateChange,
		EStart,
		EStop
		};

	static CUsbCableDetector* NewL();
	static CUsbCableDetector* NewLC();
	virtual ~CUsbCableDetector();

	void CheckAndMonitorCableL();
	void MonitorCable();
	void RunL();

protected:
	CUsbCableDetector();
	void ConstructL();

	void DoCancel();
	TInt RunError(TInt aError);

private:
	RUsb iUsbMan;
	TUsbDeviceState iDeviceState;
	TReqState iReqState;
	};


CUsbCableDetector::CUsbCableDetector()
	: CActive(EPriorityStandard)
/**
 * Constructor. Performs standard active object setup.
 */
	{
	CActiveScheduler::Add(this);
	}

CUsbCableDetector* CUsbCableDetector::NewLC()
/**
 * Factory function which leaves the new object on the cleanup stack.
 *
 * @return a new CUsbCableDetector which is still on the cleanup stack
 */
	{
	CUsbCableDetector* self = new (ELeave) CUsbCableDetector;
	CleanupStack::PushL(self);
	self->ConstructL();

	return self;
	}

CUsbCableDetector* CUsbCableDetector::NewL()
/**
 * Factory function.
 *
 * @return a new CUsbCableDetector
 */
	{
	CUsbCableDetector* self = CUsbCableDetector::NewLC();
	CleanupStack::Pop(self);

	return self;
	}

void CUsbCableDetector::ConstructL()
/**
 * Connects to the USB Manager and gets the current device state.
 */
	{
	User::LeaveIfError(iUsbMan.Connect());
	User::LeaveIfError(iUsbMan.GetDeviceState(iDeviceState));
	}

CUsbCableDetector::~CUsbCableDetector()
/**
 * Destructor. Cleans up all resources.
 */
	{
	Cancel();
	iUsbMan.Close();
	}

void CUsbCableDetector::CheckAndMonitorCableL()
/**
 * Checks that the current USB service state is consistent with the state of
 * the cable. If the cable is unplugged and USB services are started, we want
 * to stop them. If the cable is plugged in and USB services are stopped, we
 * want to stop them. In any other situation, we just want to go on monitoring.
 */
	{
	TUsbServiceState serviceState;
	User::LeaveIfError(iUsbMan.GetServiceState(serviceState));



	if ((serviceState != EUsbServiceIdle) &&
		(iDeviceState == EUsbDeviceStateUndefined))
		{
		iReqState = EStop;
		iUsbMan.Stop(iStatus);
		SetActive();
		}
	else if ((serviceState != EUsbServiceStarted) &&
		(iDeviceState != EUsbDeviceStateUndefined))
		{
		iReqState = EStart;
		iUsbMan.Start(iStatus);
		SetActive();
		}
	else
		{
		MonitorCable();
		}
	}

void CUsbCableDetector::MonitorCable()
/**
 * Requests a notification from the USB Manager when the device state changes.
 */
	{
	iReqState = ENotifyDeviceStateChange;
	iUsbMan.DeviceStateNotification(KMaxTInt, iDeviceState, iStatus);
	SetActive();
	}

void CUsbCableDetector::RunL()
/**
 * An asynchronous request has completed. The way we handle this depends on our
 * current state.
 */
	{
	User::LeaveIfError(iStatus.Int());

	switch (iReqState)
		{
	case ENotifyDeviceStateChange:
		CheckAndMonitorCableL();
		break;

	case EStart:
	case EStop:
		MonitorCable();
		break;

	default:
		break;
		}
	}

void CUsbCableDetector::DoCancel()
/**
 * Cancels the current asynchronous request.
 */
	{
	switch (iReqState)
		{
	case ENotifyDeviceStateChange:
		iUsbMan.DeviceStateNotificationCancel();
		break;

	case EStart:
		iUsbMan.StartCancel();
		break;

	case EStop:
		iUsbMan.StopCancel();
		break;

	default:
		break;
		}
	}

TInt CUsbCableDetector::RunError(TInt /*aError*/)
/**
 * Error handler. Stops the active scheduler, which will cause this program to
 * terminate.
 *
 * @param aError Unused
 * @return Always KErrNone to avoid a panic
 */
	{
	CActiveScheduler::Stop();
	return KErrNone;
	}


void UsbCableDetectL()
/**
 * Starts USB cable detection. This function will never return unless an error
 * occurs.
 */
	{
	TInt ret;

	// Load the USB logical device driver.
	ret = User::LoadLogicalDevice(KUsbLddName);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		User::Leave(ret);

	// Start the C32 process (in which the USB Manager currently lives).
	ret = StartC32();
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		User::Leave(ret);

	// Create an active scheduler for the cable detector object.
	CActiveScheduler* scheduler = new (ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);

	CActiveScheduler::Install(scheduler);

	// Prepare to start monitoring cable state.
	CUsbCableDetector* cableDetector = CUsbCableDetector::NewLC();
	cableDetector->CheckAndMonitorCableL();

	// Action!
	CActiveScheduler::Start();

	// Clean everything up. We'll only get here if an error occurred.
	CleanupStack::PopAndDestroy(2); // scheduler, cableDetector
	}

void mainL()
/**
 * Main function. If run without any arguments, creates another instance of
 * this process with an argument which signifies that it should start cable
 * detection. This allows the program to be fired off from the command-line.
 */
	{
	RProcess thisProcess;

	// Create a buffer big enough for the command-line arguments.
	HBufC* commandLine = HBufC::NewLC(User::CommandLineLength());
	TPtr commandLinePtr = commandLine->Des();
	
	// Get the command-line arguments.
	User::CommandLine(commandLinePtr);

	if (commandLinePtr == KForkFlag)
		{
		UsbCableDetectL();
		}
	else
		{
		thisProcess.Create(thisProcess.FileName(), KForkFlag);
		thisProcess.Resume();
		}

	CleanupStack::PopAndDestroy(commandLine);
	}

GLDEF_C TInt E32Main()
/**
 * Symbian OS entry point. Creates a cleanup stack and runs the rest of the
 * program.
 *
 * @return Always 0
 */
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	TRAP_IGNORE(mainL());
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
