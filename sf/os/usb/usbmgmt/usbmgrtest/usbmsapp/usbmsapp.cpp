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
* USB Mass Storage Sample Application
*
*/

/**
 @file
 @publishedPartner
*/

#include "usbmsapp.h"

#include <e32std.h>
#include <e32svr.h>
#include <e32cons.h>
#include <f32file.h>
#include <usbman.h>

#include <d32usbc.h>

#include <usbmsshared.h>

//////////////////////////////////////////////////////////////////////////////

_LIT(KTxtApp,"USBMSEXAMPLEAPP");
_LIT(KMsFsy, "MSFS.FSY");
_LIT(KMsFs, "MassStorageFileSystem");
_LIT(KOk,"OK");
_LIT(KError,"Error");
_LIT(KBytesTransferredFmt, "%c:%d/%d ");
_LIT(KErrFmt, "Property not found (%d)\r");
_LIT8(KDefPwd,"123");

//Mass Storage Device States xRef cedar/generic/base/f32/smassstorage/inc/usbmsshared.h
_LIT(KUsbMsDriveStateDisconnected, "Disconnected");
_LIT(KUsbMsDriveStateConnecting, "Connecting");
_LIT(KUsbMsDriveStateConnected, "Connected");
_LIT(KUsbMsDriveStateDisconnecting, "Disconnecting");
_LIT(KUsbMsDriveStateActive, "Active");
_LIT(KUsbMsDriveStateLocked, "Locked");
_LIT(KUsbMsDriveStateMediaNotPresent, "Media Not Present");
_LIT(KUsbMsDriveStateRemoved, "Removed");
_LIT(KUsbMsDriveStateError, "Error");
_LIT(KUsbMsDriveStateUnkown, "Unkown");

//Usb Device States
_LIT(KEUsbDeviceStateUndefined,"Undefined");
_LIT(KEUsbDeviceStateDefault,"Default");
_LIT(KEUsbDeviceStateAttached,"Attached");
_LIT(KEUsbDeviceStatePowered,"Powered");
_LIT(KEUsbDeviceStateConfigured,"Configured");
_LIT(KEUsbDeviceStateAddress,"Address");
_LIT(KEUsbDeviceStateSuspended ,"Suspended");

//////////////////////////////////////////////////////////////////////////////

/**
Console shared by main routine and active objects.
*/
LOCAL_D CConsoleBase* console = NULL;

/**
File system resource shared by main routine and active objects.
*/
LOCAL_D RFs fs;

/**
Currently selected drive, index into PropertyHandlers' objects.
*/
LOCAL_D TInt selectedDriveIndex = 0;

/**
Command line text buffer, sufficiently large so that it will
hold any reasonable input.
*/
LOCAL_D TBuf<0x40> mountList;

static TFixedArray<TBool, KMaxDrives>                   msfsMountedList;  ///< 'true' entry corresponds to the drive with mounted MSFS.FSY
static TFixedArray<CFileSystemDescriptor*, KMaxDrives>  unmountedFsList;  ///< every non-NULL entry corresponds to the unmounted original FS for the drive

/**
*/
LOCAL_D TMediaPassword  password(KDefPwd);

//////////////////////////////////////////////////////////////////////////////

/**
Move cursor to row 'row', column 0.
Erase 'count' rows starting with 'row'.
*/
LOCAL_C void Clear(int row, int count=1)
	{
	_LIT(KBlank,"                                        ");
	for(TInt i=0; i<count; i++)
		{
		console->SetPos(0,row+i);
		console->Printf(KBlank);
		}
	console->SetPos(0,row);
	}

/**
Display the user's drive selection.
*/
LOCAL_C void ShowDriveSelection()
	{
	Clear(15);
	if(PropertyHandlers::allDrivesStatus.Length()/2 > selectedDriveIndex)
		{
		console->Printf(_L("Selected Drive: %c"),
					'A' + PropertyHandlers::allDrivesStatus[selectedDriveIndex*2]);
		}
	else
		{
		console->Printf(_L("Selected Drive: (none)"));
		}
	}

//-----------------------------------------------------------------------------
/**
    Tries to restore the original FS on the drive using the FS descriptor provided
    @return standard error code.
*/
LOCAL_C TInt DoRestoreFS(RFs& aFs, TInt aDrive, CFileSystemDescriptor* apFsDesc)
    {
    TInt nRes;
    RDebug::Print(_L("# DoRestoreFS drv:%d\n"), aDrive);
    //-- 1. check that there is no FS installed   
    TBuf<128>   fsName;
    nRes = aFs.FileSystemName(fsName, aDrive);
    if(nRes == KErrNone)
        {//-- probably no file system installed at all
        RDebug::Print(_L("# This drive already has FS intalled:%S \n"), &fsName);
        return KErrAlreadyExists;
        }
        
    TPtrC ptrN  (apFsDesc->FsName());
    TPtrC ptrExt(apFsDesc->PrimaryExtName());
    RDebug::Print(_L("# Mounting FS:%S, Prim ext:%S, synch:%d\n"), &ptrN, &ptrExt, apFsDesc->DriveIsSynch());

    if(ptrExt.Length() >0)
        {//-- there is a primary extension to be mounted
        nRes = aFs.AddExtension(ptrExt);
        if(nRes != KErrNone && nRes != KErrAlreadyExists)
            {
            return nRes;
            }

        nRes = aFs.MountFileSystem(ptrN, ptrExt, aDrive, apFsDesc->DriveIsSynch());
        }
    else
        {
        nRes = aFs.MountFileSystem(ptrN, aDrive, apFsDesc->DriveIsSynch());
        }

    if(nRes != KErrNone)
        {
        RDebug::Print(_L("# Mount failed! code:%d\n"),nRes);    
        }

    return nRes;
    }


//-----------------------------------------------------------------------------
/**
    Dismounts the originally mounted FS and optional primary extension from the drive and stores 
    this information in the FS descriptor

    @return on success returns a pointer to the instantinated FS descriptor
*/
LOCAL_C CFileSystemDescriptor* DoDismountOrginalFS(RFs& aFs, TInt aDrive)
    {
    TInt        nRes;
    TBuf<128>   fsName;
    TBuf<128>   primaryExtName;
    TBool       bDrvSync = EFalse;

    RDebug::Print(_L("# DoDismountOrginalFS drv:%d\n"), aDrive);

    //-- 1. get file system name
    nRes = aFs.FileSystemName(fsName, aDrive);
    if(nRes != KErrNone)
        {//-- probably no file system installed at all
        return NULL;
        }

    //-- 2. find out if the drive sync/async
    TPckgBuf<TBool> drvSyncBuf;
    nRes = aFs.QueryVolumeInfoExt(aDrive, EIsDriveSync, drvSyncBuf);
    if(nRes == KErrNone)
        {
        bDrvSync = drvSyncBuf();
        }

    //-- 3. find out primary extension name if it is present; we will need to add it againt when mounting the FS
    //-- other extensions (non-primary) are not supported yet
    nRes = aFs.ExtensionName(primaryExtName, aDrive, 0);
    if(nRes != KErrNone)
        {   
        primaryExtName.SetLength(0);
        }

    //-- 3.1 check if the drive has non-primary extensions, fail in this case, because this FS can't be mounted back normally
    nRes = aFs.ExtensionName(primaryExtName, aDrive, 1);
    if(nRes == KErrNone)
        {   
        RDebug::Print(_L("Non-primary extensions are not supported!\n"));
        return NULL;
        }

    RDebug::Print(_L("# DoDismountOrginalFS FS:%S, Prim ext:%S, synch:%d\n"), &fsName, &primaryExtName, bDrvSync);

    //-- create FS descriptor and dismount the FS
    CFileSystemDescriptor* pFsDesc = NULL; 
    
    TRAP(nRes, pFsDesc = CFileSystemDescriptor::NewL(fsName, primaryExtName, bDrvSync));
    if(nRes != KErrNone)
        return NULL; //-- OOM ?

    nRes = aFs.DismountFileSystem(fsName, aDrive);
    if(nRes != KErrNone)
        {
        delete pFsDesc;
        pFsDesc = NULL;
        RDebug::Print(_L("# DoDismountOrginalFS Dismounting Err:%d\n"), nRes);
        }
    
    return pFsDesc;
}

/**
Unmount FAT and mount MSFS.
@param driveNumber
*/
LOCAL_C void MountMsFs(TInt driveNumber)
	{
	TInt x = console->WhereX();
	TInt y = console->WhereY();
	Clear(11,2);
	RDebug::Print(_L("UsbMsExampleApp::MountMsFs driveNumber=%d\n"), driveNumber);

	//-- 1. try dismounting the original FS
    CFileSystemDescriptor* fsDesc = DoDismountOrginalFS(fs, driveNumber);
    unmountedFsList[driveNumber] = fsDesc; 
  
    if(fsDesc)
        {
        TPtrC ptrN(fsDesc->FsName());
        RDebug::Print(_L("drv:%d FS:%S Dismounted OK"),driveNumber, &ptrN);
	   	console->Printf(_L("%S Dismount %c: %S\n"), &ptrN, 'A' + driveNumber, &KOk);
        }
    else
        {
        RDebug::Print(_L("drv:%d Dismount FS Failed!"),driveNumber);
        }

    //-- 2. try to mount the "MSFS"
    TInt error;
    error = fs.MountFileSystem(KMsFs, driveNumber);
	RDebug::Print(_L("MSFS Mount:   %S (%d)"), (error?&KError:&KOk), error);

	if (!error)
		msfsMountedList[driveNumber] = ETrue;

	// restore console position
	console->Printf(_L("MSFS Mount %c:   %S (%d)"), 'A' + driveNumber, (error?&KError:&KOk), error);
	console->SetPos(x,y);
	}

/**
Unmount MSFS and mount FAT.
@param driveNumber
*/
LOCAL_C TInt RestoreMount(TInt driveNumber)
	{
	TInt err = KErrNone;
	TInt x = console->WhereX();
	TInt y = console->WhereY();
	Clear(11,2);

    //-- 1. try dismounting the "MSFS"
	if (msfsMountedList[driveNumber])
		{
		err = fs.DismountFileSystem(KMsFs, driveNumber);
		RDebug::Print(_L("MSFS Dismount:%S (%d)"), (err?&KError:&KOk), err);
		if (err)
			return err;

		msfsMountedList[driveNumber] = EFalse;
		console->Printf(_L("MSFS Dismount:%S (%d)\n"), (err?&KError:&KOk), err);
        }

    //-- 2. try to mount the original FS back
    CFileSystemDescriptor* fsDesc = unmountedFsList[driveNumber];
    if(fsDesc)
        {
        err = DoRestoreFS(fs, driveNumber, fsDesc);

        TPtrC ptrN(fsDesc->FsName());
        RDebug::Print(_L("%S Mount:    %S (%d)"), &ptrN, (err?&KError:&KOk), err);      
        delete fsDesc;
        unmountedFsList[driveNumber] = NULL;
       	console->Printf(_L("FAT Mount:    %S (%d)"), (err?&KError:&KOk), err);
        }
   
    // restore console position
	console->SetPos(x,y);
	return err;
	}

/**
Return a string description of the Drive Status
@param DriveStatus
@return a descriptor containing the description of the Drive Status
 */
LOCAL_C const TDesC& DriveStatusDescription(TInt & aDriveStatus)
  {
 	switch(aDriveStatus)
		{
		case EUsbMsDriveState_Disconnected:
			return KUsbMsDriveStateDisconnected;

		case EUsbMsDriveState_Connecting:
			return KUsbMsDriveStateConnecting;

		case EUsbMsDriveState_Connected:
			return KUsbMsDriveStateConnected;

		case EUsbMsDriveState_Disconnecting:
			return KUsbMsDriveStateDisconnecting;

		case EUsbMsDriveState_Active:
			return KUsbMsDriveStateActive;

		case EUsbMsDriveState_Locked:
			return KUsbMsDriveStateLocked;

		case EUsbMsDriveState_MediaNotPresent:
			return KUsbMsDriveStateMediaNotPresent;

		case EUsbMsDriveState_Removed:
			return KUsbMsDriveStateRemoved;

		case EUsbMsDriveState_Error:
			return KUsbMsDriveStateError;

		default :
			return KUsbMsDriveStateUnkown;

		}
  }

/**
Return a string description of the Usb Device State
@param aUsbState Usb Device State
@return the descriptor containing the device state description
 */
LOCAL_C const TDesC& UsbStateDescription(TUsbDeviceState& aUsbState)
  {
	switch(aUsbState)
		{
		case EUsbDeviceStateDefault:
			return KEUsbDeviceStateDefault;

		case EUsbDeviceStateAttached:
			return KEUsbDeviceStateAttached;

		case EUsbDeviceStatePowered:
			return KEUsbDeviceStatePowered;

		case EUsbDeviceStateConfigured:
			return KEUsbDeviceStateConfigured;

		case EUsbDeviceStateAddress:
			return KEUsbDeviceStateAddress;

 		case  EUsbDeviceStateSuspended:
			return KEUsbDeviceStateSuspended;

		case EUsbDeviceStateUndefined:
		default :
			return KEUsbDeviceStateUndefined;
		}

  }

//////////////////////////////////////////////////////////////////////////////

CPropertyWatch* CPropertyWatch::NewLC(TUsbMsDriveState_Subkey aSubkey, PropertyHandlers::THandler aHandler)
	{
	CPropertyWatch* me=new(ELeave) CPropertyWatch(aHandler);
	CleanupStack::PushL(me);
	me->ConstructL(aSubkey);
	return me;
	}

CPropertyWatch::CPropertyWatch(PropertyHandlers::THandler aHandler)
	: CActive(0), iHandler(aHandler)
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
	// resubscribe before processing new value to prevent missing updates
	iProperty.Subscribe(iStatus);
	SetActive();

	iHandler(iProperty);
	}

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
	CActive(0),
	iUsb(aUsb),
	iUsbDeviceState(EUsbDeviceStateUndefined),
	iIsConfigured(EFalse)
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

static TChar DriveNumberToLetter(TInt driveNumber)
	{
	TChar driveLetter = '?';
	fs.DriveToChar(driveNumber, driveLetter);
	return driveLetter;
	}

static TBool IsDriveInMountList(TUint driveLetter)
	{
	TUint16 driveLetter16 = static_cast<TUint16>(driveLetter);
	return(!mountList.Length() || KErrNotFound != mountList.Find(&driveLetter16, 1));
	}

void CUsbWatch::RunL()
	{
	RDebug::Print(_L("CUsbWatch DeviceStateNotification: iUsbDeviceState=%d %s"), iUsbDeviceState, &UsbStateDescription(iUsbDeviceState));

	// If the cable is disconnected, unmount all the connected drives.
	if(iIsConfigured && (iUsbDeviceState == EUsbDeviceStateUndefined))
		{
		for(TInt i=0; i<PropertyHandlers::allDrivesStatus.Length()/2; i++)
			{
			if(IsDriveConnected(i))
				{
				RDebug::Print(_L("CUsbWatch calling UnmountMsFs"));
				RestoreMount(PropertyHandlers::allDrivesStatus[2*i]);
				}
			}

		iIsConfigured = EFalse;
		}

  	if(iUsbDeviceState == EUsbDeviceStateConfigured)
  		{
  		iIsConfigured = ETrue;
  		}

	const TUint stateMask = 0xFF;
	iUsb.DeviceStateNotification(stateMask, iUsbDeviceState, iStatus);
	SetActive();
	}

//////////////////////////////////////////////////////////////////////////////

TUsbMsDrivesStatus PropertyHandlers::allDrivesStatus;
TUsbMsBytesTransferred PropertyHandlers::iKBytesRead;
TUsbMsBytesTransferred PropertyHandlers::iKBytesWritten;

/**
Handle a publish event for the Bytes Read property.
*/
void PropertyHandlers::Read(RProperty& aProperty)
	{
	Transferred(aProperty, iKBytesRead);
	}

/**
Handle a publish event for the Bytes Written property.
*/
void PropertyHandlers::Written(RProperty& aProperty)
	{
	Transferred(aProperty, iKBytesWritten);
	}

/**
Helper function for Read and Written
*/
void PropertyHandlers::Transferred(RProperty& aProperty, TUsbMsBytesTransferred& aReadOrWritten)
	{
	console->SetPos(0,1);
	console->Printf(_L("KB R/W:  "));
	TInt err = aProperty.Get(aReadOrWritten);
	if(err == KErrNone)
		{
		for(TInt i=0; i<allDrivesStatus.Length()/2; i++)
			{
			console->Printf(KBytesTransferredFmt,
				static_cast<char>(DriveNumberToLetter(allDrivesStatus[2*i])), iKBytesRead[i], iKBytesWritten[i]);
			}
		}
	else
		{
		console->Printf(KErrFmt, err);
		}
	}

/**
Handle a publish event for the Drive Status property.
*/
void PropertyHandlers::DriveStatus(RProperty& aProperty)
	{
	RDebug::Print(_L(">> PropertyHandlers::DriveStatus"));
	TInt err = aProperty.Get(allDrivesStatus);
	console->SetPos(0,0);
	if(err == KErrNone)
		{
		TInt numDrives = allDrivesStatus.Length()/2;
		console->Printf(_L("Status:  "));
		for(TInt i=0; i<numDrives; i++)
			{
			TInt driveNumber = allDrivesStatus[2*i];
			TInt driveStatus = allDrivesStatus[2*i+1];
			TChar driveLetter = DriveNumberToLetter(driveNumber);


			RDebug::Print(_L("PropertyHandlers::DriveStatus: drive %c: = %d %S"),
						static_cast<char>(driveLetter), driveStatus, &DriveStatusDescription(driveStatus));
			console->Printf(_L("%c:%d %S          "), static_cast<char>(driveLetter), driveStatus, &DriveStatusDescription(driveStatus));

			if(IsDriveInMountList(driveLetter))
				{
				if(driveStatus == EUsbMsDriveState_Connecting)
					{
					MountMsFs(driveNumber);
					}
				else if(driveStatus == EUsbMsDriveState_Disconnected)
					{
					RestoreMount(driveNumber);
					}
				else
					{
					RDebug::Print(_L("PropertyHandlers::DriveStatus: nothing to do"));
					}
				}
			else
				{
				RDebug::Print(_L("PropertyHandlers::DriveStatus: %c: is not in mountList\n"),
				(TUint) driveLetter);
				}
			}
		}
	else
		{
		console->Printf(KErrFmt, err);
		}

	RDebug::Print(_L("<< PropertyHandlers::DriveStatus"));
	}

//////////////////////////////////////////////////////////////////////////////

CMessageKeyProcessor::CMessageKeyProcessor(CConsoleBase* aConsole)
	: CActive(CActive::EPriorityUserInput), iConsole(aConsole)
	{
	}

CMessageKeyProcessor* CMessageKeyProcessor::NewLC(CConsoleBase* aConsole
												 )
	{
	CMessageKeyProcessor* self=new (ELeave) CMessageKeyProcessor(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CMessageKeyProcessor* CMessageKeyProcessor::NewL(CConsoleBase* aConsole
												)
	{
	CMessageKeyProcessor* self = NewLC(aConsole);
	CleanupStack::Pop();
	return self;
	}

void CMessageKeyProcessor::ConstructL()
	{
	// Add to active scheduler
	CActiveScheduler::Add(this);
	RequestCharacter();
	}


CMessageKeyProcessor::~CMessageKeyProcessor()
	{
	// Make sure we're cancelled
	Cancel();
	}

void  CMessageKeyProcessor::DoCancel()
	{
	iConsole->ReadCancel();
	}

void  CMessageKeyProcessor::RunL()
	{
	  // Handle completed request
	ProcessKeyPress(TChar(iConsole->KeyCode()));
	}

void CMessageKeyProcessor::RequestCharacter()
	{
	  // A request is issued to the CConsoleBase to accept a
	  // character from the keyboard.
	iConsole->Read(iStatus);
	SetActive();
	}

void CMessageKeyProcessor::ProcessKeyPress(TChar aChar)
	{
	TInt error = KErrNone;

	aChar.UpperCase();
	switch(aChar)
		{
		case 'Q':
		case EKeyEscape:
			for(TInt j=0; j<KMaxDrives; j++)
				{
				error = RestoreMount(j);
				if (error)
					{
					// Mount is busy/locked and can not be restored.
					break;
					}
				}
			CActiveScheduler::Stop();
			return;

		case 'D':
			if(++selectedDriveIndex >= PropertyHandlers::allDrivesStatus.Length()/2)
				{
				selectedDriveIndex = 0;
				}
			ShowDriveSelection();
			break;

		case 'M':
			if(PropertyHandlers::allDrivesStatus.Length())
				{
				MountMsFs(PropertyHandlers::allDrivesStatus[selectedDriveIndex*2]);
				}
			break;

		case 'U':
			if(PropertyHandlers::allDrivesStatus.Length())
				{
				RestoreMount(PropertyHandlers::allDrivesStatus[selectedDriveIndex*2]);
				}
			break;

		case 'L':
			{
			_LIT(KEmpty, "");
			TMediaPassword nul;
			nul.Copy(KEmpty);
			error = fs.LockDrive(PropertyHandlers::allDrivesStatus[selectedDriveIndex*2],
				nul, password, ETrue);
			console->SetPos(0,9);
			console->Printf(_L("LockDrive %S (%d)"), (error?&KError:&KOk), error);
			break;
			}

		case 'N':
			error = fs.UnlockDrive(PropertyHandlers::allDrivesStatus[selectedDriveIndex*2],
				password, ETrue);
			Clear(9);
			console->Printf(_L("UnlockDrive %S (%d)"), (error?&KError:&KOk), error);
			break;

		case 'C':
			error = fs.ClearPassword(PropertyHandlers::allDrivesStatus[selectedDriveIndex*2],
				password);
			Clear(9);
			console->Printf(_L("ClearPassword %S (%d)"), (error?&KError:&KOk), error);
			break;
		}
	RequestCharacter();
	}


//-----------------------------------------------------------------------------

CFileSystemDescriptor::~CFileSystemDescriptor()
    {
    iFsName.Close();
    iPrimaryExtName.Close();
    }

//-----------------------------------------------------------------------------
CFileSystemDescriptor* CFileSystemDescriptor::NewL(const TDesC& aFsName, const TDesC& aPrimaryExtName, TBool aDrvSynch)
    {
    CFileSystemDescriptor* pSelf = new (ELeave) CFileSystemDescriptor;

    CleanupStack::PushL(pSelf);
    
    pSelf->iFsName.CreateMaxL(aFsName.Length());
    pSelf->iFsName.Copy(aFsName);
    
    pSelf->iPrimaryExtName.CreateMaxL(aPrimaryExtName.Length());
    pSelf->iPrimaryExtName.Copy(aPrimaryExtName);

    pSelf->iDriveSynch = aDrvSynch;

    CleanupStack::Pop();

    return pSelf;
    }








//////////////////////////////////////////////////////////////////////////////

/**
Initialize mass storage. Enter Active Scheduler. Shutdown mass storage.
*/
LOCAL_C void RunAppL()
	{
    TInt error = KErrUnknown;
	TRequestStatus status;

	PropertyHandlers::allDrivesStatus.Fill(0);

	RDebug::Print(_L("USBMSEXAMPLEAPP: Creating console\n"));
	console = Console::NewL(KTxtApp,TSize(KConsFullScreen,KConsFullScreen));
	CleanupStack::PushL(console);

	console->SetPos(0,2);
	console->Printf(_L("========================================"));

	RDebug::Print(_L("USBMSEXAMPLEAPP: Getting CommandLine\n"));
	// Command line: list of drive letters to auto-mount (all if not specified)
	User::CommandLine(mountList);
	mountList.UpperCase();

	RDebug::Print(_L("USBMSEXAMPLEAPP: Creating CActiveScheduler\n"));
	CActiveScheduler* sched = new(ELeave) CActiveScheduler;
	CleanupStack::PushL(sched);
	CActiveScheduler::Install(sched);

	RDebug::Print(_L("USBMSEXAMPLEAPP: Connecting to file system\n"));
	User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);

	_LIT(KMountAllDefault,"(all)");
	console->SetPos(0,3);
	console->Printf(_L("Drives to auto-mount: %S"),
		(mountList.Length() ? &mountList : &KMountAllDefault));

#if defined(__WINS__)
	_LIT(KDriverFileName,"TESTUSBC.LDD");
	error = User::LoadLogicalDevice(KDriverFileName);
	console->SetPos(0,4);
	console->Printf(_L("Load %S: %S (%d)"),
				&KDriverFileName, (error?&KError:&KOk), error);
#endif

	console->SetPos(0,5);
	RDebug::Print(_L("USBMSEXAMPLEAPP: Add MS file system\n"));
	// Add MS file system
	error = fs.AddFileSystem(KMsFsy);
	console->Printf(_L("Add MS File System: %S (%d)"),
				(error?&KError:&KOk), error);

	RUsb usb;
	error = usb.Connect();
	User::LeaveIfError(error);
	CleanupClosePushL(usb);

	console->SetPos(0,6);
	RDebug::Print(_L("USBMSEXAMPLEAPP: Find the class controller\n"));

    // Find the personality that supports the massstorage uid
	TInt personalityId=-1;
	RArray<TInt> personalityIds;
	usb.GetPersonalityIds(personalityIds);
	for (TInt i=0; i < personalityIds.Count(); i++)
	    {
    	TBool supported=EFalse;
        User::LeaveIfError(usb.ClassSupported(personalityIds[i], KUsbMsClassControllerUID, supported));

        HBufC* localizedPersonalityDescriptor;
        User::LeaveIfError(usb.GetDescription(personalityIds[i],localizedPersonalityDescriptor));
        console->Printf(_L("USB Personality id=%d - '%S' %s\n"),
        	personalityIds[i], localizedPersonalityDescriptor,
        	supported?_S("*"):_S(""));

        if(supported)
        	{
        	personalityId = personalityIds[i];
        	}
        delete localizedPersonalityDescriptor;
		}
	personalityIds.Close();

	if(personalityId != -1)
		{
		RDebug::Print(_L("USBMSEXAMPLEAPP: Start USB\n"));
		usb.TryStart(personalityId, status);
		User::WaitForRequest(status);
		if(status.Int() != KErrNone)
			{
			console->Printf(_L("USB Service failed to start (%d)"), status.Int());
			}
		else
			{
			TUsbServiceState currentState;
			User::LeaveIfError(usb.GetServiceState(currentState));
			if(EUsbServiceStarted != currentState)
				{
				console->Printf(_L("USB Service failed to enter EUsbServiceStarted"));
				User::Leave(KErrUnknown);
				}
			console->Printf(_L("USB Service Started, personality id=%d"), personalityId);
			}
		}
	else
		{
		console->Printf(_L("USBMS class controller not found"));
		}

    console->SetPos(0,14);
    TBuf16<8> lpwd;
    lpwd.Copy(password);
    console->Printf(_L("Password: %S"), &lpwd);

    RDebug::Print(_L("USBMSEXAMPLEAPP: Create active objects\n"));

	CMessageKeyProcessor::NewLC(console);
	CPropertyWatch::NewLC(EUsbMsDriveState_KBytesRead, PropertyHandlers::Read);
	CPropertyWatch::NewLC(EUsbMsDriveState_KBytesWritten, PropertyHandlers::Written);
	CPropertyWatch::NewLC(EUsbMsDriveState_DriveStatus, PropertyHandlers::DriveStatus);
	CUsbWatch::NewLC(usb);

	ShowDriveSelection();

	console->SetPos(0,16);
	_LIT(KMsgTitleB,"Menu: q=quit d=chg drv m=mount u=unmount\n      l=lock n=unlock c=clr pwd");
	console->Printf(KMsgTitleB);

	RDebug::Print(_L("USBMSEXAMPLEAPP: Start CActiveScheduler\n"));
	CActiveScheduler::Start();

	RDebug::Print(_L("Shutting down. Unmounting all mounted drives\n"));

	for(TInt j=0; j<PropertyHandlers::allDrivesStatus.Length()/2; j++)
		{
		if(IsDriveConnected(j))
			{
			RestoreMount(PropertyHandlers::allDrivesStatus[2*j]);
			}
		}

	usb.TryStop(status);
	User::WaitForRequest(status);
	RDebug::Print(_L("USB TryStop returned %d\n"), status.Int());

	error = fs.RemoveFileSystem(KMsFs);
	RDebug::Print(_L("RemoveFileSystem returned %d\n"), error);

#if defined(__WINS__)
	error = User::FreeLogicalDevice(KDriverFileName);
	RDebug::Print(_L("FreeLogicalDevice returned %d\n"), error);
#endif

	CleanupStack::PopAndDestroy();	// CUsbWatch
	CleanupStack::PopAndDestroy();	// CPropertyWatch
	CleanupStack::PopAndDestroy();	// CPropertyWatch
	CleanupStack::PopAndDestroy();	// CPropertyWatch
	CleanupStack::PopAndDestroy();	// CMessageKeyProcessor
	CleanupStack::PopAndDestroy(&usb);
	CleanupStack::PopAndDestroy(&fs);
	CleanupStack::PopAndDestroy(sched);
	CleanupStack::PopAndDestroy(console);
	}


/**
Application entry point.
*/
GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanup=CTrapCleanup::New();

	TRAPD(error,RunAppL());
	__ASSERT_ALWAYS(!error, User::Panic(KTxtApp, error));

	delete cleanup;
	__UHEAP_MARKEND;
	return 0;
	}
