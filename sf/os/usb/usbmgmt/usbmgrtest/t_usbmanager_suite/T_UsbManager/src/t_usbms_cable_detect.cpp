/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Part of Base Test - F32 Test Suite
* SYMTestCaseID Cable_plug_unplug
* SYMTestCaseDesc Test mass storage system behavior on USB cable 
* plug-in/plug-out (automated integration test)
* To run the test, invoke
* t_usbms_cable_detect X
* where X is the appropriate mass storage drive letter.
* Notes: 
* - This test does not work on the emulator due to LDD requirements.
* - The USB cable must be connected to a host.
* Test sequence:
* - (USB/MS intialisation)
* - Wait for DriveStatus = Connecting
* - Dismount FAT, Mount MSFS
* - Wait for DriveStatus = Connected
* - Simulate cable disconnection
* - Wait for USB Status != Configured
* - Dismount MSFS, Mount FAT
* - Wait for DriveStatus = Disconnected
* - Simulate cable re-connection
* - Wait for DriveStatus = Connecting
* - Dismount FAT, Mount MSFS
* - Wait for DriveStatus = Connected
* - Simulate cable disconnection
* - Wait for USB Status != Configured
* - Dismount MSFS, Mount FAT
* - (USB/MS shutdown)
* This test program includes a 3 minute timeout to ensure that 
* it does not hang in case of failure.
*
*/

/**
 @file
 @publishedPartner
*/

// This test code is based on usbmsapp and shares its class declarations.
#include "../../../usbmsapp/usbmsapp.h"

#include <e32std.h>
#include <e32svr.h>
#include <f32file.h>
#include <usbman.h>
#include <e32test.h>
#include <d32usbc.h>
#include <usbmsshared.h>


_LIT(KTxtApp,"USBMSCABLEDETECT");
LOCAL_D RTest test(_L("USBMSCABLE"));

#ifndef __WINS__

_LIT(KMsFsy, "MSFS.FSY");
_LIT(KMsFs, "MassStorageFileSystem");
_LIT(KOk,"OK");
_LIT(KError,"Error");

LOCAL_D RFs fs;
LOCAL_D RDevUsbcClient ldd;
LOCAL_D TDriveNumber selectedDriveNumber = EDriveA;
LOCAL_D TInt selectedDriveIndex = -1;
LOCAL_D TFileName gOldFileSysName;

LOCAL_C void MountMsFs(RUsb& aUsb, TInt driveNumber)
	{
	test.Printf(_L("MountMsFs driveNumber=%d\n"), driveNumber); 

	TInt error = fs.FileSystemName(gOldFileSysName, driveNumber);
	test(error==KErrNone);

    error = fs.DismountFileSystem(gOldFileSysName, driveNumber);
   	test.Printf(_L("%S Dismount %c: %S (%d)\n"), &gOldFileSysName,
	   		'A' + driveNumber, (error?&KError:&KOk), error);
	
	error = fs.MountFileSystem(KMsFs, driveNumber);
	test.Printf(_L("MSFS Mount %c:   %S (%d)"), 
		'A' + driveNumber, (error?&KError:&KOk), error);
		
	if(error!=KErrNone)
		{
		test.Printf(_L("Failed to mount Mass Storage (%d), shutting down.\n"), error); 
		TRequestStatus status;
		aUsb.TryStop(status);
		User::WaitForRequest(status);
		test(false);
		}
	}

LOCAL_C void UnmountMsFs(RUsb& aUsb, TInt driveNumber)
	{
	test.Printf(_L("UnmountMsFs driveNumber=%d\n"), driveNumber); 

	TInt error = fs.DismountFileSystem(KMsFs, driveNumber);
	test.Printf(_L("MSFS Dismount:%S (%d)\n"), (error?&KError:&KOk), error);
	if(error!=KErrNone)
		{
		test.Printf(_L("Failed to dismount Mass Storage (%d), shutting down.\n"), error); 
		TRequestStatus status;
		aUsb.TryStop(status);
		User::WaitForRequest(status);
		test(false);
		}

	error = fs.MountFileSystem(gOldFileSysName, driveNumber);
   	test.Printf(_L("%S Mount:    %S (%d)\n"), 
   				&gOldFileSysName, (error?&KError:&KOk), error);
	}

//////////////////////////////////////////////////////////////////////////////
//
// CPropertyWatch
// An active object that tracks changes to the KUsbMsDriveState properties
//
//////////////////////////////////////////////////////////////////////////////

CPropertyWatch* CPropertyWatch::NewLC(TUsbMsDriveState_Subkey aSubkey, PropertyHandlers::THandler aHandler)
	{
	CPropertyWatch* me=new(ELeave) CPropertyWatch(aHandler);
	CleanupStack::PushL(me);
	me->ConstructL(aSubkey);
	return me;
	}

CPropertyWatch::CPropertyWatch(PropertyHandlers::THandler aHandler)
	: CActive(CActive::EPriorityStandard), iHandler(aHandler)
	{}

void CPropertyWatch::ConstructL(TUsbMsDriveState_Subkey aSubkey)
	{
	User::LeaveIfError(iProperty.Attach(KUsbMsDriveState_Category, aSubkey));
	CActiveScheduler::Add(this);
	// initial subscription and process current property value
	RunL();
	}

CPropertyWatch::~CPropertyWatch()
	{
	Cancel();
	iProperty.Close();
	}

void CPropertyWatch::DoCancel()
	{
	iProperty.Cancel();
	}

void CPropertyWatch::RunL()
	{
	iHandler(iProperty);

	iProperty.Subscribe(iStatus);
	SetActive();
	}

//////////////////////////////////////////////////////////////////////////////
//
// CUsbWatch
//
//////////////////////////////////////////////////////////////////////////////

CUsbWatch* CUsbWatch::NewLC(RUsb& aUsb)
	{
	CUsbWatch* me=new(ELeave) CUsbWatch(aUsb);
	CleanupStack::PushL(me);
	me->ConstructL();
	return me;
	}

CUsbWatch::CUsbWatch(RUsb& aUsb)
	: 
	CActive(CActive::EPriorityStandard), 
	iUsb(aUsb),
	iUsbDeviceState(EUsbDeviceStateUndefined)
	{}

void CUsbWatch::ConstructL()
	{
	CActiveScheduler::Add(this);
	RunL();
	}

CUsbWatch::~CUsbWatch()
	{
	Cancel();
	}

void CUsbWatch::DoCancel()
	{
	iUsb.DeviceStateNotificationCancel();
	}

static TBool IsDriveConnected(TInt driveStatusIndex)
	{
	TInt driveStatus = PropertyHandlers::allDrivesStatus[2*driveStatusIndex+1];
	return driveStatus == EUsbMsDriveState_Connected 
		|| driveStatus >= EUsbMsDriveState_Active;
	}

void CUsbWatch::RunL()
	{
	static TBool done = EFalse;
	
	test.Printf(_L("CUsbWatch DeviceStateNotification: status=%d state=%d\n"), 
						iStatus.Int(), iUsbDeviceState);
	
	if(iUsbDeviceState != EUsbDeviceStateConfigured)
		{
		// note this may be called before selectedDriveIndex is initialised
		if(selectedDriveIndex!=-1 && IsDriveConnected(selectedDriveIndex))
			{
			test.Printf(_L("CUsbWatch calling UnmountMsFs\n"));
			UnmountMsFs(iUsb, selectedDriveNumber);	

			// stop only the second time we get here
			if(done) CActiveScheduler::Stop();
			done = ETrue;
			}
		}
	else
		{
		if(selectedDriveIndex!=-1 && !IsDriveConnected(selectedDriveIndex))
			{
			test.Printf(_L("CUsbWatch calling MountMsFs\n"));
			MountMsFs(iUsb, selectedDriveNumber);	
			}
		}
	const TUint stateMask = 0xFF;
	iUsb.DeviceStateNotification(stateMask, iUsbDeviceState, iStatus);
	SetActive();
	}

//////////////////////////////////////////////////////////////////////////////
//
// PropertyHandlers (declared in usbmsapp.h)
//
//////////////////////////////////////////////////////////////////////////////

// Unused members
TUsbMsBytesTransferred PropertyHandlers::iKBytesRead;
TUsbMsBytesTransferred PropertyHandlers::iKBytesWritten;
void PropertyHandlers::Read(RProperty& ) {}
void PropertyHandlers::Written(RProperty& ) {}
void PropertyHandlers::Transferred(RProperty& , TUsbMsBytesTransferred& ) {}


TUsbMsDrivesStatus PropertyHandlers::allDrivesStatus;
	
void PropertyHandlers::DriveStatus(RProperty& aProperty)
	{
	static TBool hasBeenConnected = EFalse;
	static TInt lastSelectedDriveStatus = -1;
		
	TInt err = aProperty.Get(allDrivesStatus);
	if(err == KErrNone)
		{
		for(TInt i=0; i<allDrivesStatus.Length()/2; i++)
			{
			TInt driveNumber = allDrivesStatus[2*i];
			TInt driveStatus = allDrivesStatus[2*i+1];
			TUint16 driveLetter = static_cast<TUint16>(driveNumber-EDriveA+'A');
			test.Printf(_L("PropertyHandlers::DriveStatus: drive %c: = %d\n"), 
						driveLetter, driveStatus);
			
			if(driveNumber == selectedDriveNumber)
				{
				selectedDriveIndex = i;
				
				if(driveStatus != lastSelectedDriveStatus)
				switch(driveStatus)
					{
					case EUsbMsDriveState_Connecting:
						test.Printf(_L("EUsbMsDriveState_Connecting: don't care\n"));
						break;
					case EUsbMsDriveState_Disconnecting:
						test.Printf(_L("EUsbMsDriveState_Disconnecting: don't care\n"));
						break;
					case EUsbMsDriveState_Disconnected:
						if(hasBeenConnected)
							{
							test.Printf(_L("EUsbMsDriveState_Disconnected: connecting cable (DeviceConnectToHost)\n"));
							test(KErrNone == ldd.DeviceConnectToHost());
							}
						break;
					case EUsbMsDriveState_Connected:
					case EUsbMsDriveState_Active:
					case EUsbMsDriveState_Locked:
						hasBeenConnected = ETrue;
						test.Printf(_L("DriveStatus: disconnecting cable (DeviceDisconnectFromHost)\n"));
						test(KErrNone == ldd.DeviceDisconnectFromHost());
						break;
					default:
						test.Printf(_L("DriveStatus: Error or media removed\n"));
						test(false);
					}
					
				lastSelectedDriveStatus = driveStatus;

				break; // found the drive
				}
			}
		}
	else
		{
		test.Printf(_L("DriveStatus error=%d\n"), err);
		test(false);
		}
	}

//////////////////////////////////////////////////////////////////////////////
//
// Timeout, to ensure test doesn't hang
//
//////////////////////////////////////////////////////////////////////////////
class CTimeout : public CTimer
	{
public:
	static CTimeout* NewLC()
		{
		CTimeout* self = new (ELeave) CTimeout;
		CleanupStack::PushL(self);
		self->ConstructL();
		return self;
		}
protected:
	void ConstructL()
		{
		CTimer::ConstructL();
		CActiveScheduler::Add(this);
		After(180*1000*1000);
		}
	CTimeout() : CTimer(0) 
		{
		}
	void RunL() 
		{
		test.Printf(_L("Test did not complete within timeout period.\n"));
		test(false);
		}
	void DoCancel() 
		{
		}
	};

//////////////////////////////////////////////////////////////////////////////
//
// Application entry point
//
//////////////////////////////////////////////////////////////////////////////
LOCAL_C void RunAppL()
	{
    TInt error = KErrUnknown;
	TRequestStatus status;

	// Command line: the massstorage drive letter
	TBuf<0x40> cmdLine;
	User::CommandLine(cmdLine);
	
	cmdLine.UpperCase();
	if(cmdLine.Length() != 1 
		|| cmdLine[0] < 'A'
		|| cmdLine[0] > 'Z')
		{
		test.Printf(_L("You must specify a drive [A-Z|a-z] on the command line\n"));
		test(false);
		}
		
	selectedDriveNumber = (TDriveNumber)(cmdLine[0] - 'A');
	test.Printf(_L("Selected drive: %S\n"), &cmdLine);

	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	// Ensure test cannot hang
	CTimeout::NewLC();

	test(KErrNone==fs.Connect());
	CleanupClosePushL(fs);

	// Add MS file system
	error = fs.AddFileSystem(KMsFsy);
	if(error != KErrNone && error != KErrAlreadyExists)
		{
		test.Printf(_L("AddFileSystem failed, err=%d\n"), error);
		User::Leave(error);
		}
   	test.Printf(_L("MSFS file system: Added OK\n"));

	RUsb usb;
	error = usb.Connect();
	User::LeaveIfError(error);
	CleanupClosePushL(usb);
	
    // Find the personality that supports the massstorage UID
	TInt personalityId=-1;
	RArray<TInt> personalityIds;
	usb.GetPersonalityIds(personalityIds);
	for (TInt i=0; i < personalityIds.Count(); i++)
	    {
    	TBool supported=EFalse;
        usb.ClassSupported(personalityIds[i], KUsbMsClassControllerUID, supported);

        HBufC* localizedPersonalityDescriptor;
        usb.GetDescription(personalityIds[i],localizedPersonalityDescriptor);
        test.Printf(_L("USB Class Controller id=%d - '%S'\n"), 
        	personalityIds[i], localizedPersonalityDescriptor);

        if(supported)
        	{
        	personalityId = personalityIds[i];	
        	}
        delete localizedPersonalityDescriptor;
		}
	personalityIds.Close();

	if(personalityId != -1)
		{
		usb.TryStart(personalityId, status);
		User::WaitForRequest(status);
		test(KErrNone == status.Int());
		
		TUsbServiceState currentState;
		test(KErrNone == usb.GetServiceState(currentState));
		if(EUsbServiceStarted != currentState)
			{
			test.Printf(_L("USB Service did not start\n"));
			test(false);
			}
		test.Printf(_L("USB Service Started, personality id=%d\n"), personalityId);
		}
	else
		{
		test.Printf(_L("USB: USBMS personality 10204bbc not found\n"));
		test(false);
		}

	error = ldd.Open(0);
    test(KErrNone == error);	
	CleanupClosePushL(ldd);

	CPropertyWatch::NewLC(EUsbMsDriveState_DriveStatus, PropertyHandlers::DriveStatus);
	CUsbWatch::NewLC(usb);
		
	CActiveScheduler::Start();

	usb.TryStop(status);
	User::WaitForRequest(status);
	test.Printf(_L("USB TryStop returned %d\n"), status.Int());

	error = fs.RemoveFileSystem(KMsFs);
	test.Printf(_L("RemoveFileSystem returned %d\n"), error);

	CleanupStack::PopAndDestroy();	// CUsbWatch
	CleanupStack::PopAndDestroy();	// CPropertyWatch
	CleanupStack::PopAndDestroy(&ldd);
	CleanupStack::PopAndDestroy(&usb);
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy();	// CTimeout
	CleanupStack::PopAndDestroy(sched);
	}
#endif //__WINS__


GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	test.Start(KTxtApp);

#ifdef __WINS__
	test.Printf(_L("This test cannot be run on WINS.\n"));
#else
	
	TRAPD(error, RunAppL());
	if (error) 
		{
		test.Printf(_L("Leave occurred; code=%d\n"), error);
		test(false);
		}
	
#endif //__WINS__

	test.End();	// output success/fail
	test.Close();
	
	delete cleanup;
	__UHEAP_MARKEND;
	return 0;
	}
