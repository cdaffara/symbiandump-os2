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
* This test program provides menu-driven functional tests for the USB Manager which exercise
* every exported API it provides. Note that, to use this program with Hurricane USB, the macro
* HURRICANE should be defined in the t_usbman.mmp file.
* WARNING: Some of these tests rely on the 'dummy class controller' build of 
* USBMAN. This is produced with t_usbman_dummycc.mmp, and it includes three 
* instances of the dummy class controller (mainly used for GT171 automated 
* tests). To get this to operate correctly a suitable ini file must be copied 
* to c:\dummy.ini, for instance usbman\data\test.ini, which provides delayed 
* asynchronous Start and Stop behaviour for each of the dummy CC instances.
*
*/

#include <e32base.h>
#include <e32math.h>
#include <e32cons.h>
#include <badesca.h>
#include <c32comm.h>
#include <usbman.h>
#include <e32property.h> //Publish & Subscribe header, required for setting serial number
#include "usb.h"

LOCAL_D CConsoleBase* console;

RTimer TheTimer;
RCommServ TheCommServ;
RUsb TheUsb;
RUsb TheSecondUsbSession;
TBool ThePrimaryClient;

_LIT(KUsbLddName, "EUSBC");

// These are used in the startup stress test.
TInt64 TheRandomSeed = 0;
const TInt KMaxCancelTime = 2000000;
const TInt KMaxStopTime = 10000000;

#define _printf console->Printf
#define _getch console->Getch
#define LEAVE(_x) VerboseLeaveL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define LEAVEIFERROR(_x) VerboseLeaveIfErrorL(_x, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))
#define CHECK(_x) if (! (_x)) VerboseLeaveL(KErrGeneral, __LINE__, TPtrC8((const TUint8*)__FILE__), _L8(#_x))

////////////////////////////////////////////////////////////////////////////////
// Helper functions below this point
////////////////////////////////////////////////////////////////////////////////

void VerboseLeaveL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
	{
	TInt filenameOffset = aFileName.LocateReverse('\\') + 1;
	if (filenameOffset < 0) filenameOffset = 1;
	TPtrC8 shortFileName = aFileName.Mid(filenameOffset);
	TBuf<64> fName, code;
	fName.Copy(shortFileName.Left(64));
	code.Copy(aCode.Left(64));
	_printf(_L("ERROR (%d) on line %d of file %S\n"), aError, aLineNum, &fName);
	_printf(_L("Code: %S\n\n"), &code);
	_printf(_L("[ press any key ]"));
	_getch();
	User::Leave(aError);
	}

void VerboseLeaveIfErrorL(TInt aError, TInt aLineNum, const TDesC8& aFileName, const TDesC8& aCode)
	{
	if (aError) 
		VerboseLeaveL(aError, aLineNum, aFileName, aCode);
	}

void ReadString(TDes& aDes)
/**
 * Reads user input into the start of the descriptor aDes.
 */
	{
	TChar inputKey;
	TInt count = 0;

	aDes.Zero();
	for (;;)
		{
		inputKey = (TInt) _getch();

		if ((TInt)inputKey == EKeyEnter)
			break;

		if(inputKey == EKeyBackspace)
			{
			if (count > 0)
				{
 				_printf(_L("%C"), (TUint) inputKey);
				aDes.Delete(--count,1);
				}
			}
		else if(inputKey.IsPrint())
			{
			_printf(_L("%C"), (TUint) inputKey);
			aDes.Append(inputKey);
			count++;
			}
		}
	}


void PrintUsbServiceState(TUsbServiceState aServiceState)
	{
	switch (aServiceState)
		{
	case EUsbServiceIdle: _printf(_L("USB service is idle.\n")); break;
	case EUsbServiceStarting: _printf(_L("USB service is starting.\n")); break;
	case EUsbServiceStarted: _printf(_L("USB service is started.\n")); break;
	case EUsbServiceStopping: _printf(_L("USB service is stopping.\n")); break;
#ifndef HURRICANE
	case EUsbServiceFatalError: _printf(_L("USB service has a fatal error.\n")); break;
#endif // HURRICANE
	default:
		_printf(_L("ERROR: unknown service state!\n"));
		break;
		}
	}

void PrintUsbDeviceState(TUsbDeviceState aDeviceState)
	{
	switch (aDeviceState)
		{
	case EUsbDeviceStateUndefined:
		_printf(_L("USB device state is undefined.\n"));
		break;
	case EUsbDeviceStateDefault:
		_printf(_L("USB device state is default.\n"));
		break;
	case EUsbDeviceStateAttached:
		_printf(_L("USB device state is attached.\n"));
		break;
	case EUsbDeviceStatePowered:
		_printf(_L("USB device state is powered.\n"));
		break;
	case EUsbDeviceStateConfigured:
		_printf(_L("USB device state is configured.\n"));
		break;
	case EUsbDeviceStateAddress:
		_printf(_L("USB device state is address.\n"));
		break;
	case EUsbDeviceStateSuspended:
		_printf(_L("USB device state is suspended.\n"));
		break;
	default:
		_printf(_L("ERROR: unknown device state!\n"));
		break;
		}
	}

////////////////////////////////////////////////////////////////////////////////
// Test cases below this point
////////////////////////////////////////////////////////////////////////////////

void OpenUsbL()
	{
	LEAVEIFERROR(TheUsb.Connect());
	}

void CloseUsbL()
	{
	TheUsb.Close();
	}

// USB test cases 3.1.1, 3.1.2 and 3.1.4, startup from primary and non-primary sessions.
void StartUsbL()
	{
	TRequestStatus status;
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("Started USB.\n"));
	}

// USB test cases 3.2 and (part of) 3.4, startup cancellation.
void CancelStartingUsbL(TBool aCancelByStopping)
	{
	TRequestStatus cancelStatus;
	TRequestStatus timerStatus;
	TInt cancelTime = 0;

	while (cancelTime < (100 * 1000000))
		{
		_printf(_L("Cancelling after %d microseconds\n"), cancelTime);

		TheTimer.After(timerStatus, cancelTime);
		TheUsb.Start(cancelStatus);

		User::WaitForRequest(cancelStatus, timerStatus);

		// If the request has finished before the timer completed,
		// cancel the timer and check the request status for errors.
		if (timerStatus == KRequestPending)
			{
			TheTimer.Cancel();
			User::WaitForRequest(timerStatus);
			LEAVEIFERROR(cancelStatus.Int());
			break;
			}

		// The timer should never complete with an error.
		LEAVEIFERROR(timerStatus.Int());
		
		// Cancel the start. The option of cancelling by stopping is only available
		// from version 7.0s, because in Hurricane a stop while starting is
		// actually implemented as a cancel.

#ifndef HURRICANE
		if (aCancelByStopping)
			{
			TRequestStatus stopStatus;
			TheUsb.Stop(stopStatus);
			User::WaitForRequest(stopStatus);
			LEAVEIFERROR(stopStatus.Int());
			}
		else
#endif // HURRICANE
			{
			TheUsb.StartCancel();
			}

		User::WaitForRequest(cancelStatus);

		// If the start actually completed, just return.
		if (cancelStatus == KErrNone)
			break;

#ifndef HURRICANE
		// If we're cancelling by stopping, then getting this error code is
		// expected.
		if (cancelStatus == KErrUsbServiceStopped)
			{
			cancelTime += 50000;
			continue;
			}
#endif // HURRICANE

		if (cancelStatus != KErrCancel)
			LEAVE(cancelStatus.Int());

		cancelTime += 50000;
		}

	TUsbServiceState serviceState;
	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	PrintUsbServiceState(serviceState);
	}

// Part of USB test case 3.4, stopping class controllers.
void StopUsbSynchronousL()
	{
	TheUsb.Stop();
	_printf(_L("Stopped USB.\n"));
	}

#ifndef HURRICANE

// Part of USB test case 3.4, stopping class controllers.
void StopUsbAsynchronousL()
	{
	TRequestStatus status;

	TheUsb.Stop(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("Stopped USB.\n"));
	}

void CancelStoppingUsbL(TBool aCancelByStarting)
	{
	TRequestStatus cancelStatus;
	TRequestStatus timerStatus;
	TInt cancelTime = 0;

	while (cancelTime < (100 * 1000000))
		{
		_printf(_L("Cancelling after %d microseconds\n"), cancelTime);

		TheTimer.After(timerStatus, cancelTime);
		TheUsb.Stop(cancelStatus);

		User::WaitForRequest(cancelStatus, timerStatus);

		// If the request has finished before the timer completed,
		// cancel the timer and check the request status for errors.
		if (timerStatus == KRequestPending)
			{
			TheTimer.Cancel();
			User::WaitForRequest(timerStatus);
			LEAVEIFERROR(cancelStatus.Int());
			break;
			}

		// The timer should never complete with an error.
		LEAVEIFERROR(timerStatus.Int());

#ifndef HURRICANE 
		if (aCancelByStarting) 
			{ 
			TRequestStatus startStatus; 
			TheUsb.Start(startStatus); 
			User::WaitForRequest(startStatus); 
			LEAVEIFERROR(startStatus.Int()); 
			} 
		else 
#endif // HURRICANE 
			{ 
			TheUsb.StopCancel(); 
			}

		User::WaitForRequest(cancelStatus);

		// If the stop actually completed, just return.
		if (cancelStatus == KErrNone)
			break;

#ifndef HURRICANE 
		// If we're cancelling by starting, then getting this 
		// error code is expected. 
	    if (cancelStatus == KErrUsbServiceStarted) 
			{ 
			cancelTime += 50000; 
			continue; 
			} 
#endif // HURRICANE 
	 
		if (cancelStatus != KErrCancel)
			LEAVE(cancelStatus.Int());

		cancelTime += 50000;
		}

	TUsbServiceState serviceState;
	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	PrintUsbServiceState(serviceState);
	}

#endif // HURRICANE

void RestartUsbL()
	{
	TheUsb.Stop();

	TRequestStatus status;
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	_printf(_L("\nRestarted USB.\n"));
	}

// USB test case 3.6, stress testing of startup and shutdown.
void StartupStressTestL()
	{
	TRequestStatus cancelStatus;
	TRequestStatus timerStatus;
	TInt cancelTime = Math::Rand(TheRandomSeed) % KMaxCancelTime;
	TInt stopTime;

	for (TInt i = 0; i < 100; i++)
		{
		_printf(_L("Cancelling after %d microseconds\n"), cancelTime);

		TheTimer.After(timerStatus, cancelTime);
		TheUsb.Start(cancelStatus);

		User::WaitForRequest(cancelStatus, timerStatus);

		// If the request has finished before the timer completed,
		// cancel the timer and check the request status for errors.
		if (timerStatus == KRequestPending)
			{
			TheTimer.Cancel();
			LEAVEIFERROR(cancelStatus.Int());
			}
		else
			{
			// The timer should never complete with an error.
			LEAVEIFERROR(timerStatus.Int());

			// Cancel the start.
			TheUsb.StartCancel();
			User::WaitForRequest(cancelStatus);
			}

		// If the start actually completed, then try stopping.
		if (cancelStatus == KErrNone)
			{
			stopTime = Math::Rand(TheRandomSeed) % KMaxStopTime;
			_printf(_L("Stopping after %d microseconds\n"), stopTime);
			User::After(stopTime);
			TheUsb.Stop();
			}
		else if (cancelStatus != KErrCancel)
			{
			LEAVE(cancelStatus.Int());
			}

		cancelTime = Math::Rand(TheRandomSeed) % KMaxCancelTime;
		}

	_printf(_L("USB is now stopped\n"));
	}

// USB test case 3.3, monitoring the state of the USB.
void MonitorUsbDeviceStateL()
	{
	TRequestStatus consoleStatus;
	TRequestStatus notifyStatus;
	TUsbServiceState serviceState;
	TUsbDeviceState deviceState;

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));

	_printf(_L("\n"));

	PrintUsbServiceState(serviceState);

#ifndef HURRICANE
	LEAVEIFERROR(TheUsb.GetDeviceState(deviceState));
	PrintUsbDeviceState(deviceState);
#endif // HURRICANE

	_printf(_L("\nMonitoring the state of USB - press any key to stop\n\n"));

	console->Read(consoleStatus);

	while (consoleStatus == KRequestPending)
		{
		TheUsb.StateNotification(0xFFFFFFFF, deviceState, notifyStatus);
		User::WaitForRequest(notifyStatus, consoleStatus);

		if (consoleStatus == KRequestPending)
			{
			if (notifyStatus != KErrNone)
				{
				// need to cancel the read to avoid a CBase-77 panic!
				console->ReadCancel();
				LEAVE(notifyStatus.Int());
				}
			}
		else
			{
			// The user's pressed a key.
			TheUsb.StateNotificationCancel();
			User::WaitForRequest(notifyStatus);
			break;
			}

		PrintUsbDeviceState(deviceState);
		}

	_printf(_L("\nTest complete\n"));
	}

// Concurrency test case (3.9.1 in test spec). One client starts USB and then another
// one stops it.
void StartAThenStopBL()
	{
	LEAVEIFERROR(TheSecondUsbSession.Connect());

	TRequestStatus status;
	TUsbServiceState serviceState;

	// Start on client 1.
	TheUsb.Start(status);
	User::WaitForRequest(status);
	LEAVEIFERROR(status.Int());

	// Get status on both sessions.

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	_printf(_L("\nSession 1: "));
	PrintUsbServiceState(serviceState);

	LEAVEIFERROR(TheSecondUsbSession.GetCurrentState(serviceState));
	_printf(_L("Session 2: "));
	PrintUsbServiceState(serviceState);

	// Stop on client 2.
	TheSecondUsbSession.Stop();

	// Get status on both sessions.

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	_printf(_L("Session 1: "));
	PrintUsbServiceState(serviceState);

	LEAVEIFERROR(TheSecondUsbSession.GetCurrentState(serviceState));
	_printf(_L("Session 2: "));
	PrintUsbServiceState(serviceState);

	TheSecondUsbSession.Close();
	}

// Concurrency test case (3.9.2 in test spec). One client stops USB while it's being
// started by another one.
void StopBWhileAStartingL()
	{
	LEAVEIFERROR(TheSecondUsbSession.Connect());

	TRequestStatus status;
	TUsbServiceState serviceState;

	// Start on client 1.
	TheUsb.Start(status);

	// Stop on client 2.
	TheSecondUsbSession.Stop();

	// Wait for start request to complete.
	User::WaitForRequest(status);
	if (status.Int() != KErrUsbServiceStopped)
		LEAVEIFERROR(status.Int());

	// Get status on both sessions.

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	_printf(_L("\nSession 1: "));
	PrintUsbServiceState(serviceState);

	LEAVEIFERROR(TheSecondUsbSession.GetCurrentState(serviceState));
	_printf(_L("Session 2: "));
	PrintUsbServiceState(serviceState);

	TheSecondUsbSession.Close();
	}

// Concurrency test case (3.9.5 in test spec). One client starts USB while it's being
// started by another one.
void StartBWhileAStartingL()
	{
	LEAVEIFERROR(TheSecondUsbSession.Connect());

	TRequestStatus status1;
	TRequestStatus status2;
	TUsbServiceState serviceState;

	// Start on client 1.
	TheUsb.Start(status1);

	// Start on client 2.
	TheSecondUsbSession.Start(status2);

	// Wait for first request to complete.
	User::WaitForRequest(status1);
	LEAVEIFERROR(status1.Int());

	// Wait for second request to complete.
	User::WaitForRequest(status2);
	LEAVEIFERROR(status2.Int());

	// Get status on both sessions.

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	_printf(_L("\nSession 1: "));
	PrintUsbServiceState(serviceState);

	LEAVEIFERROR(TheSecondUsbSession.GetCurrentState(serviceState));
	_printf(_L("Session 2: "));
	PrintUsbServiceState(serviceState);

	TheSecondUsbSession.Close();
	}

#ifndef HURRICANE

// Concurrency test case (3.9.3 in test spec). One client starts USB while it's being
// stopped by another one.
void StartBWhileAStoppingL()
	{
	LEAVEIFERROR(TheSecondUsbSession.Connect());

	TRequestStatus status1;
	TRequestStatus status2;
	TUsbServiceState serviceState;

	// Stop on client 1.
	TheUsb.Stop(status1);

	// Start on client 2.
	TheSecondUsbSession.Start(status2);

	// Wait for first request to complete.
	User::WaitForRequest(status1);
	if (status1.Int() != KErrUsbServiceStarted)
		LEAVEIFERROR(status1.Int());

	// Wait for second request to complete.
	User::WaitForRequest(status2);
	LEAVEIFERROR(status2.Int());

	// Get status on both sessions.

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	_printf(_L("\nSession 1: "));
	PrintUsbServiceState(serviceState);

	LEAVEIFERROR(TheSecondUsbSession.GetCurrentState(serviceState));
	_printf(_L("Session 2: "));
	PrintUsbServiceState(serviceState);

	TheSecondUsbSession.Close();
	}

// Concurrency test case (3.9.4 in test spec). One client stops USB while it's being
// stopped by another one.
void StopBWhileAStoppingL()
	{
	LEAVEIFERROR(TheSecondUsbSession.Connect());

	TRequestStatus status1;
	TRequestStatus status2;
	TUsbServiceState serviceState;

	// Stop on client 1.
	TheUsb.Stop(status1);

	// Stop on client 2.
	TheSecondUsbSession.Stop(status2);

	// Wait for first request to complete.
	User::WaitForRequest(status1);
	LEAVEIFERROR(status1.Int());

	// Wait for second request to complete.
	User::WaitForRequest(status2);
	LEAVEIFERROR(status2.Int());

	// Get status on both sessions.

	LEAVEIFERROR(TheUsb.GetCurrentState(serviceState));
	_printf(_L("\nSession 1: "));
	PrintUsbServiceState(serviceState);

	LEAVEIFERROR(TheSecondUsbSession.GetCurrentState(serviceState));
	_printf(_L("Session 2: "));
	PrintUsbServiceState(serviceState);

	TheSecondUsbSession.Close();
	}

void DbgCheckHeapL()
	{
	_printf(_L("Expected number of allocated heap cells: "));

	TBuf<20> numBuf;
	ReadString(numBuf);

	TLex lex(numBuf);
	TInt expected;
	
	if (lex.Val(expected) != KErrNone)
		expected = 0;

	LEAVEIFERROR(TheUsb.__DbgCheckHeap(expected));
	}

void DbgMarkEndL()
	{
	_printf(_L("Expected number of allocated heap cells: "));

	TBuf<20> numBuf;
	ReadString(numBuf);

	TLex lex(numBuf);
	TInt expected;
	
	if (lex.Val(expected) != KErrNone)
		expected = 0;

	LEAVEIFERROR(TheUsb.__DbgMarkEnd(expected));
	}

void DbgFailNextL()
	{
	_printf(_L("Number of allocation which will fail: "));

	TBuf<20> numBuf;
	ReadString(numBuf);

	TLex lex(numBuf);
	TInt allocNumber;
	
	if (lex.Val(allocNumber) != KErrNone)
		allocNumber = 1;

	LEAVEIFERROR(TheUsb.__DbgFailNext(allocNumber));
	}

#endif // HURRICANE

void SetUsbSerialNumberL()
	{
	//Get a number from user
	TBuf16<KUsbStringDescStringMaxSize> serNum;
	
	_printf(_L("\nEnter new USB serial number:\n"));
	ReadString(serNum);
	if(serNum.Length()==0)
		{
		TInt r = RProperty::Delete(KUidSystemCategory,0x101FE1DB);
		if(r!=KErrNotFound && r!=KErrNone)
			{
			User::Leave(r);
			}
		_printf(_L("Serial number un-published. Restart USB for change to take effect.\ne.g. Option 1 -> 1 (calls RUsb::Start)\n"));		
		}
	else
		{
		_printf(_L("\nSetting serial number to %S\n"),&serNum);	
		//Publish the number. It will be read by CUsbDevice::SetUsbDeviceSettingsL
		//The key used is the UID KUidUsbmanServer = 0x101FE1DB
		//The size of the serial number value published is KUsbStringDescStringMaxSize. The
		//USB spec doesn't give a specific max size for the serial number, it just says it is
		//a string descriptor, hence it can have size KUsbStringDescStringMaxSize (252 characters).
		TInt r = RProperty::Define(KUidSystemCategory,0x101FE1DB,RProperty::EText,KUsbStringDescStringMaxSize);
		if(r!=KErrAlreadyExists && r!=KErrNone)
			{
			User::Leave(r);
			}
		r = RProperty::Set(KUidSystemCategory,0x101FE1DB,serNum);
		User::LeaveIfError(r);

		_printf(_L("Serial number published. Restart USB for change to take effect.\ne.g. Option 1 -> 1 (calls RUsb::Start)\n"));		
		}
	}

////////////////////////////////////////////////////////////////////////////////
// Menus etc. below this point
////////////////////////////////////////////////////////////////////////////////

TBool SelectStartingTestL(TChar aChar)
	{
	TBool result = ETrue;
	
	switch (aChar)
		{
	case '1': StartUsbL(); break;
	case '2': CancelStartingUsbL(EFalse); break;
	case '3': CancelStartingUsbL(ETrue); break;
	case '4': StartupStressTestL(); break;
	case 'q':
	case 'Q':
		result = EFalse;
//		break; // commented out as it's unreachable
	default:
		_printf(_L("\nInvalid key\n"));
		break;
		}
	return result;
	}

void StartingTestsL()
	{
	_printf(_L("\nStarting tests\n"));
	_printf(_L("--------------\n"));
	_printf(_L("Available tests:\n\n"));
	_printf(_L("1. Start USB\n"));
	_printf(_L("2. Cancel starting USB\n"));
	_printf(_L("3. Stop USB while it's starting\n"));
	_printf(_L("4. Startup stress test\n"));
	_printf(_L("\nSelection (q for main menu): "));

	TChar ch = (TChar) _getch();
	_printf(_L("\n"));

	TRAP_IGNORE(SelectStartingTestL(ch));
	}

TBool SelectStoppingTestL(TChar aChar)
	{
	TBool result = ETrue;
	
	switch (aChar)
		{
	case '1': StopUsbSynchronousL(); break;
#ifndef HURRICANE
	case '2': StopUsbAsynchronousL(); break;
    case '3': CancelStoppingUsbL(EFalse); break; 
    case '4': CancelStoppingUsbL(ETrue); break; 
#endif // HURRICANE
	case 'q':
	case 'Q':
		result = EFalse;
//		break; // commented out as it's unreachable
	default:
		_printf(_L("\nInvalid key\n"));
		break;
		}
	return result;
	}

void StoppingTestsL()
	{
	_printf(_L("\nStopping tests\n"));
	_printf(_L("--------------\n"));
	_printf(_L("Available tests:\n\n"));
	_printf(_L("1. Stop USB synchronously\n"));
#ifndef HURRICANE
	_printf(_L("2. Stop USB asynchronously\n"));
	_printf(_L("3. Cancel stopping USB\n"));
	_printf(_L("4. Start USB whilst stopping\n"));
#endif // HURRICANE
	_printf(_L("\nSelection (q for main menu): "));

	TChar ch = (TChar) _getch();
	_printf(_L("\n"));

	TRAP_IGNORE(SelectStoppingTestL(ch));
	}

TBool SelectConcurrencyTestL(TChar aChar)
	{
	TBool result = ETrue;
	
	switch (aChar)
		{
	case '1': StartAThenStopBL(); break;
	case '2': StopBWhileAStartingL(); break;
	case '3': StartBWhileAStartingL(); break;
#ifndef HURRICANE
	case '4': StartBWhileAStoppingL(); break;
	case '5': StopBWhileAStoppingL(); break;
#endif // HURRICANE
	case 'q':
	case 'Q':
		result = EFalse;
//		break; // commented out as it's unreachable
	default:
		_printf(_L("\nInvalid key\n"));
		break;
		}
	return result;
	}

void ConcurrencyTestsL()
	{
	_printf(_L("\nConcurrency tests\n"));
	_printf(_L("-----------------\n"));
	_printf(_L("Available tests:\n\n"));
	_printf(_L("1. Client A starts, then client B stops\n"));
	_printf(_L("2. Client B stops while client A is starting\n"));
	_printf(_L("3. Client B starts while client A is starting\n"));
#ifndef HURRICANE
	_printf(_L("4. Client B starts while client A is stopping\n"));
	_printf(_L("5. Client B stops while client A is stopping\n"));
#endif // HURRICANE
	_printf(_L("\nSelection (q for main menu): "));

	TChar ch = (TChar) _getch();
	_printf(_L("\n"));

	TRAP_IGNORE(SelectConcurrencyTestL(ch));
	}

void SanityTestL()
/**
 * Call each API. Just a quick way of sanity-checking the API.
 */
	{
	_printf(_L("\nQuick sanity check of all APIs\n"));
	
	RUsb usb;
	LEAVEIFERROR(usb.Connect());
	usb.Version();
	
	TUsbDeviceState deviceState;
	TRequestStatus deviceStateNotStat;
	usb.StateNotification(0xFFFF, deviceState, deviceStateNotStat);
	usb.StateNotificationCancel();
	User::WaitForRequest(deviceStateNotStat);
	if ( deviceStateNotStat != KErrCancel )
		{
		LEAVEIFERROR(__LINE__);
		}

	usb.Stop(); // get it into a known state

	TUsbServiceState serviceState;
	LEAVEIFERROR(usb.GetServiceState(serviceState));
	if ( serviceState != EUsbServiceIdle )
		{
		LEAVEIFERROR(__LINE__);
		}
	TRequestStatus serviceStateNotStat;
	usb.ServiceStateNotification(serviceState, serviceStateNotStat);
	// Start & Stop & Cancels
	TRequestStatus startStat;
	usb.Start(startStat);
	User::WaitForRequest(startStat);
	LEAVEIFERROR(startStat.Int());
	User::WaitForRequest(serviceStateNotStat);
	LEAVEIFERROR(serviceStateNotStat.Int());
	if ( serviceState != EUsbServiceStarting )
		{
		LEAVEIFERROR(__LINE__);
		}
	LEAVEIFERROR(usb.GetServiceState(serviceState));
	if ( serviceState != EUsbServiceStarted )
		{
		LEAVEIFERROR(__LINE__);
		}
	usb.Stop();
	usb.StartCancel();
	usb.StopCancel();
	TRequestStatus stopStat;
	usb.Stop(stopStat);
	User::WaitForRequest(stopStat);
	LEAVEIFERROR(stopStat.Int());

	// Service state
	LEAVEIFERROR(usb.GetServiceState(serviceState));
	if ( serviceState != EUsbServiceIdle )
		{
		LEAVEIFERROR(__LINE__);
		}
	LEAVEIFERROR(usb.GetCurrentState(serviceState));
	if ( serviceState != EUsbServiceIdle )
		{
		LEAVEIFERROR(__LINE__);
		}
	usb.ServiceStateNotification(serviceState, serviceStateNotStat);
	usb.ServiceStateNotificationCancel();
	User::WaitForRequest(serviceStateNotStat);
	if ( serviceStateNotStat != KErrCancel )
		{
		LEAVEIFERROR(__LINE__);
		}
	User::After(1000000);
	// Device state
	LEAVEIFERROR(usb.GetDeviceState(deviceState));		// need default for Cedar below
	if ( deviceState != EUsbDeviceStateUndefined && deviceState != EUsbDeviceStateDefault )
		{
		LEAVEIFERROR(__LINE__);
		}
	usb.DeviceStateNotification(0xFFFF, deviceState, deviceStateNotStat);
	usb.DeviceStateNotificationCancel();
	User::WaitForRequest(deviceStateNotStat);
	if ( deviceStateNotStat != KErrCancel )
		{
		LEAVEIFERROR(__LINE__);
		}
	usb.DeviceStateNotification(EUsbDeviceStateDefault, deviceState, deviceStateNotStat);
	usb.Start(startStat);
	User::WaitForRequest(startStat);
	LEAVEIFERROR(startStat.Int());
	User::WaitForRequest(deviceStateNotStat);
	LEAVEIFERROR(deviceStateNotStat.Int());		  // need undefined for Cedar below
	if ( deviceState != EUsbDeviceStateDefault && deviceState != EUsbDeviceStateUndefined )
		{
		LEAVEIFERROR(__LINE__);
		}
	User::After(1000000);
	LEAVEIFERROR(usb.GetDeviceState(deviceState));	// need default & undefined & configured for Cedar below
	if ( deviceState != EUsbDeviceStateSuspended && deviceState != EUsbDeviceStateDefault && deviceState != EUsbDeviceStateUndefined && deviceState != EUsbDeviceStateConfigured)
		{
		LEAVEIFERROR(__LINE__);
		}
	// Heap marking
	LEAVEIFERROR(usb.__DbgMarkHeap());
	LEAVEIFERROR(usb.__DbgCheckHeap(0));
	LEAVEIFERROR(usb.__DbgMarkEnd(0));
	// NB __DbgFailNext is not checked
	usb.Stop();
	usb.Close();
	}

#ifndef HURRICANE

TBool SelectMemoryTestL(TChar aChar)
	{
	TBool result = ETrue;
			
	switch (aChar)
		{
	case '1': LEAVEIFERROR(TheUsb.__DbgMarkHeap()); break;
	case '2': DbgCheckHeapL(); break;
	case '3': DbgMarkEndL(); break;
	case '4': DbgFailNextL(); break;
	case 'q':
	case 'Q':
		result = EFalse;
//		break; // commented out as it's unreachable
	default:
		_printf(_L("\nInvalid key\n"));
		break;
		}
	return result;
	}

void MemoryTestsL()
	{
	_printf(_L("\nMemory tests\n"));
	_printf(_L("------------\n"));
	_printf(_L("Available tests:\n\n"));
	_printf(_L("1. Mark heap\n"));
	_printf(_L("2. Check heap\n"));
	_printf(_L("3. End marking heap\n"));
	_printf(_L("4. Simulate allocation failure\n"));
	_printf(_L("\nSelection (q for main menu): "));

	TChar ch = (TChar) _getch();
	_printf(_L("\n"));

	TRAP_IGNORE(SelectMemoryTestL(ch));
	}

#endif // HURRICANE

TBool SelectTestL(TChar aChar)
	{
	TBool result = ETrue;
			
	switch (aChar)
		{
	case '1': StartingTestsL(); break;
	case '2': StoppingTestsL(); break;
	case '3': MonitorUsbDeviceStateL(); break;
	case '4': ConcurrencyTestsL(); break;
	case '5': SetUsbSerialNumberL(); break;
	case 's': case 'S': SanityTestL(); break;
#ifndef HURRICANE
	case 'm': MemoryTestsL(); break;
#endif // HURRICANE
	case 'c': case 'C': CloseUsbL(); break;
	case 'o': case 'O': OpenUsbL(); break;
	case 'q':
	case 'Q':
		result = EFalse;
//		break; // commented out as it's unreachable
	default:
		_printf(_L("\nInvalid key\n"));
		break;
		}
	return result;
	}

void mainL()
	{
	// Create the global timer. This is used throughout the code for timing services.
	TheTimer.CreateLocal();

	TInt ret;

#ifndef __WINS__
	ret = User::LoadLogicalDevice(KUsbLddName);
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		LEAVE(ret);

	_printf(_L("Loaded USB LDD\n"));
#endif

	ret = StartC32();
	if ((ret != KErrNone) && (ret != KErrAlreadyExists))
		LEAVE(ret);

	_printf(_L("Started C32\n"));

	OpenUsbL();

	TUsbServiceState serviceState;

	TBool noExit = ETrue;
	while (noExit)
		{
		_printf(_L("\nMain menu\n"));
		_printf(_L("---------\n"));
		_printf(_L("Available tests:\n\n"));
		_printf(_L("1. Starting tests\n"));
		_printf(_L("2. Stopping tests\n"));
		_printf(_L("3. Monitor USB device state\n"));
		_printf(_L("4. Concurrency tests\n"));
		_printf(_L("5. Publish Serial Number\n"));
		_printf(_L("S. Sanity test- quickly calls all APIs to make sure they don't panic\n"));
#ifndef HURRICANE
		_printf(_L("M. Memory tests\n"));
#endif // HURRICANE
		_printf(_L("C. Close connection to USB Manager\n"));
		_printf(_L("O. Connect to USB Manager\n"));

		if (TheUsb.Handle())
			{
			_printf(_L("\nConnected to USB "));
			if (ThePrimaryClient)
				_printf(_L("as the primary client.\n"));
			else
				_printf(_L("as a non-primary client.\n"));

			ret = TheUsb.GetCurrentState(serviceState);
			if (ret == KErrNone)
				PrintUsbServiceState(serviceState);
			else
				_printf(_L("Couldn't get USB service state (error %d).\n"), ret);
			}
		else
			{
			_printf(_L("\nNot connected to USB.\n"));
			}

		_printf(_L("\nSelection (q to quit): "));

		TChar ch = (TChar) _getch();
		_printf(_L("\n"));

		TRAP_IGNORE(noExit = SelectTestL(ch));
		}

	TheCommServ.Close();
	TheUsb.Close();
	}

void consoleMainL()
	{
	console=Console::NewL(_L("T_USBMAN"),TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);
	mainL();
	_printf(_L("[ press any key ]"));
	_getch();
	CleanupStack::PopAndDestroy();
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack=CTrapCleanup::New();
	TRAP_IGNORE(consoleMainL());
	delete cleanupStack;
	__UHEAP_MARKEND;
	return 0;
	}
