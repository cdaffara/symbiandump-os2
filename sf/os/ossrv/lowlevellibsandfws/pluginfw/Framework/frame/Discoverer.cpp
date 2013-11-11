// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
// Contains the implementation of the CDiscoverer class.
// 
//

/**
 @file
 @internalComponent
*/

#include <e32uid.h>
#include <startup.hrh> // for EStartupStateNonCritical and EStartupStateCritical
#include <bacntf.h>

#include <sacls.h>

#include "EComDebug.h"
#include "TestUtilities.h"	// For __FILE__LINE__
#include "Discoverer.h"
#include "DiscovererObserver.h"
#include "EComUidCodes.h"
#include "baspi.h"
#include "bautils.h"
#include "DriveInfo.h"
#include <ecom/ecomerrorcodes.h>
#include <saclscommon.h>


#define UNUSED_VAR(a) a = a


/** Interface Implementation Collection resource file search path */
_LIT(KEComResourceFileSearch,	"\\resource\\plugins\\*");

_LIT(KEComResourceFilePathAny,	"\\resource\\plugins\\");
_LIT(KEComResourceFolderPath,	"?:\\resource\\plugins\\"); 

// Relative to the Drive with a fixed path
_LIT(KEComSPIFilePath, "\\private\\10009D8F\\");

/** 
	Begin directory scanning after a delay of 1 Second
	Allowing multiple directory changes to be applied before
	beginning a scan.
 */
static const TInt32 KEComDefaultBeginScanPeriod	=	1000000;

// __________________________________________________________________________
//
CDiscoverer::CSwiChangeNotifier* CDiscoverer::CSwiChangeNotifier::NewL(CDiscoverer& aDiscoverer)
	{
	CSwiChangeNotifier* self = new(ELeave) CSwiChangeNotifier(aDiscoverer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDiscoverer::CSwiChangeNotifier::CSwiChangeNotifier(CDiscoverer& aDiscoverer)
: CActive(CActive::EPriorityHigh), iDiscoverer(aDiscoverer)
	{
	// Safe because it cannot fail
	CActiveScheduler::Add(this);
	}

void CDiscoverer::CSwiChangeNotifier::ConstructL()
	{
	// Attach to SWI property
	User::LeaveIfError(
        iProperty.Attach(KUidSystemCategory, KSAUidSoftwareInstallKeyValue));
	}

CDiscoverer::CSwiChangeNotifier::~CSwiChangeNotifier()
	{
	Cancel();
	iProperty.Close();
	}

void CDiscoverer::CSwiChangeNotifier::DoCancel()
	{
	iProperty.Cancel();	// Cancel SWI change notifications
	}

void CDiscoverer::CSwiChangeNotifier::Subscribe()
	{
	if(!IsActive())
		{
		iProperty.Subscribe(iStatus);
		SetActive();
		}
	}

void CDiscoverer::CSwiChangeNotifier::RunL()
	{
	Subscribe();
	
	TInt swiProperty;
	User::LeaveIfError(
        iProperty.Get(KUidSystemCategory, KSAUidSoftwareInstallKeyValue, swiProperty));

	// Do a discovery each time an install, uninstall or restore is completed.
	iDiscoverer.SwiChangeNotificationL(swiProperty);	
	}

TInt CDiscoverer::CSwiChangeNotifier::RunError(TInt /*aError*/)
	{
	//If unable to read the SWI P&S variable set the 
	//discoverers SWI state to ESASwiNone as this will return
	//EComs back to its default behaviour
	TRAP_IGNORE(iDiscoverer.SwiChangeNotificationL(ESASwisNone));	
	return KErrNone; //avoid CActiveScheduler panic
	}

// __________________________________________________________________________
//
/*
	The notification object which watches the Interface Implementation 
	Collection directories for any changes on specific drive.
	When its RunL method is called, it notifies its owning CDiscoverer class
	object to re-scan of the Interface Implementation Collection directories.
*/
CDiscoverer::CDirChangeNotifier::CDirChangeNotifier(CDiscoverer& aDiscoverer, RFs& aFs, const TDriveUnit& aDriveUnit)
: CActive(CActive::EPriorityHigh), iDiscoverer(aDiscoverer), iFs(aFs),iDriveUnit(aDriveUnit)
	{
	
	iNotificationFilePath.Append(iDriveUnit.Name());
	iNotificationFilePath.Append(KEComResourceFilePathAny);
	// Safe because it cannot fail
	CActiveScheduler::Add(this);
	}

CDiscoverer::CDirChangeNotifier::~CDirChangeNotifier()
	{
	Cancel();
	}

void CDiscoverer::CDirChangeNotifier::DoCancel()
	{
	iFs.NotifyChangeCancel(iStatus);	// Cancel change notifications
	}

void CDiscoverer::CDirChangeNotifier::Activate()
	{
	if(!IsActive())
		{
		iStatus = KRequestPending;
		SetActive();
		iFs.NotifyChange(ENotifyEntry, iStatus, iNotificationFilePath); 
		}
	}

void CDiscoverer::CDirChangeNotifier::RunL()
	{
	RECORD_START_NOTIFIER_RUNL_TIMER_RESULT(iDriveUnit)
	// Signal the notification
	// If iStatus.Int() is not KErrNone
	// then reactivation will not occur
	if(iDiscoverer.NotificationL(iStatus.Int(), iDriveUnit))
		Activate();
	RECORD_END_NOTIFIER_RUNL_TIMER_RESULT(iDriveUnit)
	}

TInt CDiscoverer::CDirChangeNotifier::RunError(TInt aError)
	{
	// Entered most likely because of an error condition during file system
    // rescanning and plugin registration that could not be handled locally. 
    // As indexes in the registry are updated after each registration and the
	// tree of registrations is updated at the end for some scan use-cases there
	// is a chance that the registration tree and the indexes can get out of 
    // sync when a leave occurs. 
    // The code is not present to handle a recovery so the best policy 
    // is to panic the server and have it restart on next use.
    // We can't trap leaves in RunL() and continue as we can not be sure the 
    // registry is in sync. The registry and discovery code would need to be 
    // totally reviewed and reworked if panic's were not acceptable. So far they 
    // have allowed error conditions found in the field to be reported as 
    // incidents allowing us to idenitify and resovle the underlying causes.
	__ECOM_LOG1("ECOM: PANIC in CDiscoverer::CDirChangeNotifier::RunError(), error= %d", aError);
	User::Panic(KEComServerPanicCategory, EEComPanic_CDiscoverer_CDirChangeNotifier_RunError);
	return KErrNone;   // dummy return to stop warnings on missing return
	}

// __________________________________________________________________________
//
/*
	The timer Active object for providing plugin directory scanning on rediscovery events. 
	The processing of notification will be performed only on drive(s) that has notification event(s)
	triggered on it.
	It uses data member iPendingDriveList to hold all pending drive nums, and executes only once. 
	It is activated by the CDirChangeNotifier's notification call. 
	The default priority is idle time execution only.
*/
CDiscoverer::CIdleScanningTimer* CDiscoverer::CIdleScanningTimer::NewL(CDiscoverer& aDiscoverer)
	{
	CIdleScanningTimer* self = new(ELeave) CIdleScanningTimer(aDiscoverer);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
CDiscoverer::CIdleScanningTimer::CIdleScanningTimer(CDiscoverer& aDiscoverer)
: CTimer(CActive::EPriorityIdle), iDiscoverer(aDiscoverer), iPendingDriveList(2)
	{
	// Safe because it cannot fail
	CActiveScheduler::Add(this);
	}

void CDiscoverer::CIdleScanningTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

CDiscoverer::CIdleScanningTimer::~CIdleScanningTimer()
	{
	Cancel();
	iPendingDriveList.Close();
	}

void CDiscoverer::CIdleScanningTimer::DoCancel()
	{
	// Call the base class to ensure the timer is cancelled
	CTimer::DoCancel();

	iDiscoverer.ScanDirectoryCancel();
	}

void CDiscoverer::CIdleScanningTimer::RunL()
//	When the object activates on a specfic drive, this is method is called
//  and delegates to the CDiscoverer to scan the Interface Implementation 
//	Collection directories
//
	{
	// Only carry out a rediscovery if SWI is not in progress
	if(!iDiscoverer.SwiOperationInProgress()) 
		{
		RECORD_START_TIMER_RUNL_TIMER_RESULT
		// Do scan on all pending drives stored in iPendingDriveList array
		TInt length = iPendingDriveList.Count();
		for(TInt count = 0; count < length; ++count)
			{
			iDiscoverer.RediscoveryScanDirectoryL(TDriveUnit(iPendingDriveList[count]));
			}
	
		// Signal the observer that the scans have been completed successfully.
		iDiscoverer.iDiscovererObserver.DiscoveriesComplete(ETrue, EPluginProcessingTypeAll);
		// Reset pending drive list when finishes scan.
		iPendingDriveList.Reset();
		// Reset the state of discoverer as all notifications processed.
		iDiscoverer.CompleteNotificationProcessing();
		RECORD_END_TIMER_RUNL_TIMER_RESULT
		}
	}

TInt CDiscoverer::CIdleScanningTimer::RunError(TInt aError)
	{
	// Entered most likely because of an error condition during file system
    // rescanning and plugin registration that could not be handled locally. 
    // As indexes in the registry are updated after each registration and the
	// tree of registrations is updated at the end for some scan use-cases there
	// is a chance that the registration tree and the indexes can get out of 
    // sync when a leave occurs. 
    // The code is not present to handle a recovery so the best policy 
    // is to panic the server and have it restart on next use.
    // We can't trap leaves in RunL() and continue as we can not be sure the 
    // registry is in sync. The registry and discovery code would need to be 
    // totally reviewed and reworked if panic's were not acceptable. So far they 
    // have allowed error conditions found in the field to be reported as 
    // incidents allowing us to idenitify and resovle the underlying causes.
	__ECOM_LOG1("ECOM: PANIC in CDiscoverer::CIdleScanningTimer::RunError(), error = %d", aError);
	User::Panic(KEComServerPanicCategory, EEComPanic_CDiscoverer_CIdleScanningTimer_RunError);
	return KErrNone;	// dummy return to stop warnings on mising return
	}
	
void CDiscoverer::CIdleScanningTimer::RestartScanPeriod()
	{
	if (!iSuspended)
		{
		Cancel();
		After(KEComDefaultBeginScanPeriod);
		}
	}
	
void CDiscoverer::CIdleScanningTimer::Suspend()
	{
	Cancel();
	iSuspended = ETrue;
	}
	
void CDiscoverer::CIdleScanningTimer::Resume()
	{
	iSuspended = EFalse;
	if(IsAnyNotificationProcessingPending())
		{
		RestartScanPeriod();
		}
	}
// __________________________________________________________________________
//
/*
		CDirScanner implements incremental scanning of the Interface Implementation 
		Collection directory 
		on behalf of the CDiscoverer.
		It's methods are called in response to the timer task execution,
		thereby requiring the incremental scheduling.
*/
CDiscoverer::CDirScanner* CDiscoverer::CDirScanner::NewL(CDiscoverer& aDiscoverer, RFs& aFs)
	{
	CDirScanner* self = new(ELeave)CDirScanner(aDiscoverer,aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CDiscoverer::CDirScanner::ConstructL()
	{
	}

CDiscoverer::CDirScanner::CDirScanner(CDiscoverer& aDiscoverer, RFs& aFs)
: CBase(), iDiscoverer(aDiscoverer), iFs(aFs)
	{
	}

CDiscoverer::CDirScanner::~CDirScanner()
// D'tor
	{
	}
	

void CDiscoverer::CDirScanner::ScanDriveL(const TDriveUnit& aDrive,  TBool aIsRO)
	{
	RECORD_START_REDISCOVERYSCANDIRECTORY_RESULT(aDrive)
	TDriveName driveName(aDrive.Name());
	TBool scanDirectoryForPlugins = ETrue;
	TBool found = EFalse;

	
	// If RO then attempt to discover plugins from SPI file
	if(aIsRO)
		{
		TFileName spiFilePath;
		spiFilePath.Append(driveName);
		spiFilePath.Append(KEComSPIFilePath);
		
		TEntry entry;
		//check if the path exists
		if (iFs.Entry(spiFilePath,entry)==KErrNone)
			{
			TParse spiPath;
			spiPath.Set(spiFilePath, NULL, NULL);
			// Discover plugins from SPI
			found = DoScanSpiFileL(spiPath);
			}
		scanDirectoryForPlugins = !found;
		}
 
 	// scan directory for plugins if not already discovered from SPI file. SPI applies to RO.
 	if(scanDirectoryForPlugins)
 		{
 	
		// Find plugins via resoure files
		TUidType rscUidType(KNullUid,KUidInterfaceImplementationCollectionInfo,KNullUid);
		TBool foundRsc = DoScanDriveL(aDrive, rscUidType, aIsRO);
		found = found || foundRsc; 
 		}

	if (!found)
		{
		iDiscoverer.DriveUnmountedL(aDrive);
		}
	RECORD_END_REDISCOVERYSCANDIRECTORY_RESULT(aDrive)
	}

TBool CDiscoverer::CDirScanner::DoScanSpiFileL(const TParse& aSpiPath)
{
	iDiscoverer.DriveMountedL(aSpiPath.Drive());

	RResourceArchive resourceArchive;
	//ECom server should continue if OpenL leaves because no spi exists 
	TRAPD(err,resourceArchive.OpenL(iFs, aSpiPath.DriveAndPath(),_L("ecom")));
	if(err==KErrNotFound || err==KErrPathNotFound)
		return EFalse;
	User::LeaveIfError(err);
	CleanupClosePushL(resourceArchive);
	// check SPI file type. On failure do not scan archives
	if(resourceArchive.Type() != KEcomSpiFileTypeUid)
		{
		CleanupStack::PopAndDestroy(&resourceArchive);
		return EFalse;
		}
	
	CPluginBase* entryBase=NULL;
	TBool resourceExistsIndicator = EFalse;
	while(!resourceArchive.End())
		{
		TRAPD(error,iDiscoverer.ValidateEntryL(resourceArchive,entryBase));
		CleanupStack::PushL(entryBase);
		if (error==KErrNoMemory)
			User::LeaveNoMemory();
		if (error==KErrNone)
			{
			// When SPI is on no DAT file exists,and also RO Internal drive is not rediscovered. 
			//Therefore this RO Internal drive is always at its initial discovery. No Dll
			// is ever discovered before. Always pass EFalse to ProcessEntryL method.
			iDiscoverer.ProcessEntryL(aSpiPath.Drive(),entryBase, EFalse);
			// set to indicate at least 1 resource exists
			resourceExistsIndicator = ETrue;	
			}
		else
			{
			__ECOM_TRACE1("ECOM: CDiscoverer::DoScanSpiFileL(). Fail Validate: %S\n.",&aSpiPath.FullName());
			}	
		CleanupStack::PopAndDestroy(entryBase);
		entryBase=NULL;
		}
	CleanupStack::PopAndDestroy(&resourceArchive);
	return resourceExistsIndicator;
}

TBool CDiscoverer::CDirScanner::DoScanDriveL(const TDriveUnit& aDrive, const TUidType& aUidType, TBool aIsRO)
	{	
	RDir dir;
	
	TDriveName driveName(aDrive.Name());
	TParse searchDir;
	User::LeaveIfError(searchDir.Set(KEComResourceFileSearch,NULL,&driveName));

	// Match the directory list UID's to a Polymorphic DLL UID and Interface
	// Implementation Collection UID.
	// Resource files are sorted by UID. However, since these files have same UID,
	// they are actually sorted by their names (alphanumerically).

  	TInt error = dir.Open(iFs, searchDir.FullName(), aUidType);
 
	if(error == KErrNone)
		{
		// Have found the plugin directory
		CleanupClosePushL(dir);
		
		TFileName* lastRscNameBuf = new TFileName;
		
		if (!lastRscNameBuf) 
		{
			CleanupStack::PopAndDestroy(&dir); 
			return EFalse;
		}
		CleanupStack::PushL(lastRscNameBuf);
		
		TEntryArray *dirEntriesArray = new TEntryArray;
	 
		if (!dirEntriesArray) 
		{
			CleanupStack::PopAndDestroy(lastRscNameBuf); 
			CleanupStack::PopAndDestroy(&dir); 
			return EFalse;
		}
		CleanupStack::PushL(dirEntriesArray);
				
		
		TPtrC lastRscName(KNullDesC);
		
		// Iterate through the directory reading multiple entries at a 
		// time
		TInt count = 0;
		TInt readError = KErrNone;
		CPluginBase* entryBase=NULL;

 		iDiscoverer.DriveMountedL(aDrive);
		TBool anyDllRegistered = iDiscoverer.IsAnyDllRegisteredWithDriveL(aDrive);
 
	 

		while (readError != KErrEof)  
			{
		
 			// Read the next set of entries
 			readError =	dir.Read(*dirEntriesArray);
 				
			if ((readError != KErrNone) &&  (readError != KErrEof))
				{
				User::Leave(readError);	
				}
			else 
				{
 				// for KErrEof, dirEntriesArray still has items to process 
				count = dirEntriesArray->Count();
 				// Ok use the entries to populate the file list
				for(TInt i = 0; i < count; ++i)
					{
 
 					// Compare current file name against previous one ignoring extension. If it is same
					// then there is no need to process it.
					TPtrC currName = (*dirEntriesArray)[i].iName.Left((*dirEntriesArray)[i].iName.Length()-KExtensionLength);
					if (lastRscName.Compare(currName) == 0)
						{
						continue;
						}
					else if (i < (count - 1))
						{
						lastRscName.Set(currName);
						}
					else
						{
						lastRscNameBuf->Copy(currName);
						lastRscName.Set(*lastRscNameBuf);
						}
						
 
					// Obtain a copy of the current directory entry
					TRAP(error,iDiscoverer.ValidateEntryL((*dirEntriesArray)[i], driveName, entryBase, aIsRO));			
					CleanupStack::PushL(entryBase);		
 
					if (error==KErrNoMemory) 
						User::LeaveNoMemory();
				
					if (error==KErrNone)
						{
						iDiscoverer.ProcessEntryL(driveName,entryBase,anyDllRegistered);
 						}
					else
						{
						__ECOM_TRACE1("ECOM: CDiscoverer::DoScanDriveL(). Fail Validate entry: %S\n.",&(*dirEntriesArray)[i].iName);
						}		
				    CleanupStack::PopAndDestroy(entryBase);
					entryBase=NULL;
					}
				}
			}
 		CleanupStack::PopAndDestroy(dirEntriesArray); 
		CleanupStack::PopAndDestroy(lastRscNameBuf); 
		CleanupStack::PopAndDestroy(&dir); 
		return ETrue; 
		}

	return EFalse;
	}

void CDiscoverer::CDirScanner::DiscoverPluginsL(TBool aDiscoverReadOnlyDrives)
	{
	// iterator which returns only the drives need to be scanned.
	TEComCachedDriveInfoIterator iter(*iDiscoverer.iCachedDriveInfo);

	// Iterate from highest drive letter (Z:) towards lowest drive letter (A:).
	for (iter.Last(); iter.InRange(); iter.Prev())
		{
		if (iter.DriveIsReadOnlyInternal() == aDiscoverReadOnlyDrives)
			{
			ScanDriveL(iter.DriveUnit(), aDiscoverReadOnlyDrives);
			}
		}
	}
	

// __________________________________________________________________________
//
/*
	Responsible for identifying new Interface Implementation Collections,
	installed in the Interface Implementation Collection directories.
*/

CDiscoverer* CDiscoverer::NewL(MDiscovererObserver& aDiscovererObserver, RFs& aFs)
	{
	CDiscoverer* self = new(ELeave) CDiscoverer(aDiscovererObserver, aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Default d'tor

CDiscoverer::~CDiscoverer()
	{
	// Cancel any scanning behaviour or notifications
	if(iDirScanner != NULL)
		{
		// Left in the middle of a scan
		// So clear up
		delete iDirScanner;
		iDirScanner = NULL;
		// Signal the observer that the scan has 
		// not been completed successfully.
		iDiscovererObserver.DiscoveriesComplete(EFalse, EPluginProcessingTypeAll);
		}
	Suspend();
	iDrivesDiscovered.Reset();
	delete iSwiChangeNotifier;
	delete iScanningTimer;
	delete iLanguageChangeNotifier;
	delete iCachedDriveInfo;
	iRscDirNotifierList.ResetAndDestroy();

	}

// Default c'tor
CDiscoverer::CDiscoverer(MDiscovererObserver& aDiscovererObserver, RFs& aFs)
: CBase(),   iSwiChangeDiscoveryPending(EFalse), iLanguageChangeDiscoveryPending(EFalse),
  iState(EDisc_Undefined), iDiscovererObserver(aDiscovererObserver), iFs(aFs)
	{
	// Do nothing here
	}

void CDiscoverer::ConstructL()
	{
	iCachedDriveInfo = CEComCachedDriveInfo::NewL(iFs);
	
	// Construct the Interface Implementation Collection
	// directory change notifier list
	// and the scan step control object.

	CDirChangeNotifier *dirChangeNotifierPtr;

	// iterator which returns only the drives need to be scanned.
	TEComCachedDriveInfoIterator iter(*iCachedDriveInfo);

	for (iter.First(); iter.InRange(); iter.Next())
		{
		//Don't need to monitor read-only drives. They don't change.
		if ( !iter.DriveIsReadOnlyInternal() )
			{				
			dirChangeNotifierPtr = new(ELeave)CDirChangeNotifier(*this,iFs,iter.DriveUnit());
		
			CleanupStack::PushL(dirChangeNotifierPtr);				
			iRscDirNotifierList.AppendL(dirChangeNotifierPtr);
			CleanupStack::Pop();
			}
		}
	iSwiChangeNotifier = CSwiChangeNotifier::NewL(*this);

	iScanningTimer = CIdleScanningTimer::NewL(*this);

	//Create the language change notifier and install the callback function
	const TCallBack myCallBackFunction(&CDiscoverer::LocaleChangedL, this);
	iLanguageChangeNotifier = CEnvironmentChangeNotifier::NewL(CActive::EPriorityStandard, myCallBackFunction);
	
	iDirScanner = CDirScanner::NewL(*this,iFs);	

	InitialiseEvent();	
	}


TInt CDiscoverer::Resume()
	{
	// Reactivate the scanning timer if not NULL
	if (iScanningTimer != NULL)
		{
		iScanningTimer->Resume();
		}
	
	TCallBackState cbData = ECallBackState_EventEnd;
	iBurChangeCallBack.CallBack(ECallBackId_BurEvent, &cbData);

	/*
	iLanguageChangeNotifier is not activated because it is not cancelled during CDiscoverer::Suspend().
	It is not suspended because a language change should not occur whilst a backup/restore operation
	is taking place.
	*/

	return KErrNone;
	}


TInt CDiscoverer::Suspend()
	{
	// Suspend the scanning timer if not NULL
	if (iScanningTimer != NULL)
		{
		iScanningTimer->Suspend();
		}

	TCallBackState cbData = ECallBackState_EventStart;
	iBurChangeCallBack.CallBack(ECallBackId_BurEvent, &cbData);

	/*
	iLanguageChangeNotifier is not cancelled because a language change should not occur
	whilst a backup/restore operation is taking place.
	*/

	return KErrNone;
	}


TBool CDiscoverer::NotificationL(TInt aStatus, const TDriveUnit& aDriveUnit)
	{

	TBool okToContinue = ETrue;
	if(aStatus != KErrNone)
		{
		// Big trouble with the notification
		// Tell our observer
		// Notifications will cease if EFalse is returned!!!!
		okToContinue = iDiscovererObserver.NotifiedWithErrorCode(aStatus);	
		}
	else
		{
		//call ProcessDNEventL() to indicate Plugins have been added or removed on a specfic drive,
		// then do the state transition and to start a re-discovery .
		ProcessDNEventL(EPluginsModified,aDriveUnit );
		ProcessDNEventL(EPluginsRediscover, aDriveUnit);
		}
	return okToContinue;
	}

void CDiscoverer::SwiChangeNotificationL(TInt aSwiOperation)
	{
	// Store the current SWI operation, ignore operation status
	iSwiOperation = aSwiOperation & KSASwisOperationMask;
	
	TCallBackState cbData = SwiOperationInProgress() ? ECallBackState_EventStart : ECallBackState_EventEnd;
	iSwiChangeCallBack.CallBack(ECallBackId_SwiEvent, &cbData);

	// Test no SWI operation in progress
	if(!SwiOperationInProgress())
		{
		TBool rediscoveryPending = EFalse;
		if(!iSwiChangeDiscoveryPending)
			{		 
			// for each removable drive call ProcessDNEventL() to do the state transition and to start
			// a re-discovery for that drive.
			TInt count = iDrivesDiscovered.Count();		
			for(TInt i=0; i < count; i++)
				{
				TDriveUnit drvUnit(iDrivesDiscovered[i]);
				if(iCachedDriveInfo->DriveIsRemovableL(drvUnit))
					{
					rediscoveryPending = ETrue;
					ProcessDNEventL(EPluginsModified, drvUnit );
					ProcessDNEventL(EPluginsRediscover, drvUnit);
					iSwiChangeDiscoveryPending = ETrue;
					}
				}
			}
		
		//If there are no removable drives to be scanned check if there are any
		//pending notifications that couldn't be processed during SWI
		if(!rediscoveryPending && iScanningTimer->IsAnyNotificationProcessingPending())
			{
			// Activate timer if there is any notification processing pending
			iScanningTimer->RestartScanPeriod();
			}
		}
	}

TBool CDiscoverer::SwiOperationInProgress()
	{
	return (iSwiOperation != ESASwisNone);
	}

void CDiscoverer::LanguageChangeNotificationL()
	{
	if (!iLanguageChangeDiscoveryPending)
		{
		// for each drive call ProcessDNEventL() to do the state transition and to start
		// a re-discovery for that drive.
		TInt count = iDrivesDiscovered.Count();
		for(TInt i=0; i < count; i++)
			{
			ProcessDNEventL(EPluginsModified, iDrivesDiscovered[i] );
			ProcessDNEventL(EPluginsRediscover, iDrivesDiscovered[i]);
			}
		iLanguageChangeDiscoveryPending = ETrue;
		}
	}
void CDiscoverer::RediscoveryScanDirectoryL(const TDriveUnit& aDriveUnit) 
	{
	TBool doScan = EFalse;
	if(iDrivesDiscovered.Find(aDriveUnit) != KErrNotFound)
		{
		// If the drive has plugins on it previously, do ScanDriveL on any notifications.
		doScan = ETrue;
		}
	else // Otherwise the drive doesn't contain any plugin before, do further check.
		{
		TBuf<KEComPlugRSCPathMaxLen> pluginsDirPath(KEComResourceFolderPath);
		pluginsDirPath[0] = ('A' + TInt(aDriveUnit));
		TEntry entry;
		if(iFs.Entry(pluginsDirPath,entry) == KErrNone)
			{
			// Now it has plugins folder on it, do ScanDriveL.
			doScan = ETrue;
			}
		// If it still doesn't have plugins folder on it, skip unnecessary scanning.
		// NOTE: other returned error code could be KErrPathNotFound, KErrNotReady etc.
		//  As long as no plugin has been found, always skip scanning on this drive.
		}
	
	// Performs scanning according to above checks.
	if(doScan)
		{
		// Signal the observer that a scan has commenced.
		iDiscovererObserver.DiscoveriesBegin();	
		
		iDirScanner->ScanDriveL(aDriveUnit, iCachedDriveInfo->DriveIsReadOnlyInternalL(aDriveUnit));
		
		// Signal the observer that the scan has 
		// been completed successfully.
		iDiscovererObserver.SetDiscoveryFlagL(aDriveUnit);
		}
	}

void CDiscoverer::ScanDirectoryCancel()
	{
	if(iDirScanner != NULL)
		{
		// Signal the observer that the scan has 
		// been completed un-successfully.
		iDiscovererObserver.DiscoveriesComplete(EFalse, EPluginProcessingTypeAll);
		}
	}


void CDiscoverer::CompleteNotificationProcessing()
	{
	iState = EDisc_AllPluginsDisc;
	iSwiChangeDiscoveryPending = EFalse;
	iLanguageChangeDiscoveryPending = EFalse;
	}


void CDiscoverer::ValidateEntryL(const TEntry& aEntry, const TDriveName& aDriveName, CPluginBase*& aEntryToFill, TBool aIsRO)
   	{
	aEntryToFill=CSecurePlugin::NewL(iFs,aEntry,aDriveName, aIsRO);
  	}

void CDiscoverer::ValidateEntryL(RResourceArchive& aRscArchive,CPluginBase*& aEntryToFill)
   	{
    aEntryToFill = CSpiPlugin::NewL(aRscArchive);
	}
  	

void CDiscoverer::ProcessEntryL(const TDriveName& aDrive,CPluginBase*& aEntry, TBool aAnyDllDiscovered)
	{
	iDiscovererObserver.RegisterDiscoveryL(aDrive,aEntry,aAnyDllDiscovered);
	}
	
void CDiscoverer::DriveMountedL(TDriveUnit aDrive)
	{
	TInt index = iDrivesDiscovered.Find(aDrive);
	if(index == KErrNotFound)
		{
		User::LeaveIfError(iDrivesDiscovered.Append(aDrive));
		iDiscovererObserver.DriveReinstatedL(aDrive);	// Wasn't there before
		}
	}
	
TBool CDiscoverer::IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive)const
	{
	return 	iDiscovererObserver.IsAnyDllRegisteredWithDriveL(aDrive);
	}

void CDiscoverer::DriveUnmountedL(TDriveUnit aDrive)
	{
	TInt index = iDrivesDiscovered.Find(aDrive);
	if(index != KErrNotFound)
		{
		iDrivesDiscovered.Remove(index);
		iDiscovererObserver.DriveRemovedL(aDrive);		// Was there before
		}
	}
	
CDiscoverer::TDiscovererState CDiscoverer::State() const
	{
	return iState;
	}

	
void CDiscoverer::ProcessSSAEventL(TStartupStateIdentifier aKnownState)
	{

	if(iState == EDisc_NoPluginsDisc && aKnownState == EStartupStateCriticalStatic)
		{
		__ECOM_TRACE("ECOM: CDiscoverer::ProcessSSAEventL():EStartupStateCriticalStatic is reached,discover the RO Internal drives only.");

		// Signal the observer that the scanning is started
		iDiscovererObserver.DiscoveriesBegin();
		
		//scan the RO drives
	    iDirScanner->DiscoverPluginsL(ETrue);

		//change the state
		iState = EDisc_CriticalPluginsDisc;

		// Signal the observer that the scan has 
		// been completed successfully.
		iDiscovererObserver.DiscoveriesComplete(ETrue, EPluginProcessingTypeCriticalOnly);
		}
	else if(iState == EDisc_CriticalPluginsDisc && aKnownState == EStartupStateNonCritical)
		{
		__ECOM_TRACE("ECOM: CDiscoverer::ProcessSSAEventL():EStartupStateNonCritical is reached,discover the Non RO Internal drives.");

		// Signal the observer that the scanning is started
		iDiscovererObserver.DiscoveriesBegin();
		
		//scan the non-ro drives
		iDirScanner->DiscoverPluginsL(EFalse);
	
		//change the state
		iState = EDisc_AllPluginsDisc;
		
		// Signal the observer that the scan has 
		// been completed successfully.
		iDiscovererObserver.DiscoveriesComplete(ETrue, EPluginProcessingTypeNonCriticalOnly);

	
		StartNotifiers();		
		}
	else if(iState == EDisc_NoPluginsDisc && aKnownState == EStartupStateNonCritical)
		{
		__ECOM_TRACE("ECOM: CDiscoverer::ProcessSSAEventL():EStartupStateNonCritical is reached all at once,discover all the drives.");

		// Signal the observer that the scanning is started
		iDiscovererObserver.DiscoveriesBegin();
		
		//scan a specified the drives
		iDirScanner->DiscoverPluginsL(ETrue);
		iDirScanner->DiscoverPluginsL(EFalse);

		//change the state
		iState = EDisc_AllPluginsDisc;
		
		// Signal the observer that the scan has 
		// been completed successfully.
		iDiscovererObserver.DiscoveriesComplete(ETrue, EPluginProcessingTypeAll);

		StartNotifiers();
		}
	}
	
void CDiscoverer::StartNotifiers()
	{
		
	for(TInt i = 0; i<iRscDirNotifierList.Count(); i++)
		{
		if (iRscDirNotifierList[i] != NULL)
		iRscDirNotifierList[i]->Activate();
		}
	iSwiChangeNotifier->Subscribe();
	iLanguageChangeNotifier->Start();
	}
	
void CDiscoverer::ProcessDNEventL(TNotificationFlag aFlag, const TDriveUnit& aDriveUnit)
	{
	if(iState == EDisc_AllPluginsDisc && aFlag == EPluginsModified)
		{
		iState = EDisc_PluginsDirty;
		return;	
		}
	if(iState == EDisc_PluginsDirty && aFlag == EPluginsRediscover)
		{
		// Add drive number to the pending drive list and activate timer.
		iScanningTimer->AddDriveL(aDriveUnit);
		iScanningTimer->RestartScanPeriod();
		}
	}
	
void CDiscoverer::SetSwiChangeCallBack(const TCallBackWithArg& aCallBack)
	{
	iSwiChangeCallBack = aCallBack;
	}

void CDiscoverer::SetBurChangeCallBack(const TCallBackWithArg& aCallBack)
	{
	iBurChangeCallBack = aCallBack;
	}

void CDiscoverer::InitialiseEvent()
	{
	iState = EDisc_NoPluginsDisc;
	}
TInt CDiscoverer::LocaleChangedL(TAny* aPtr)
	{
	CDiscoverer* thisLocaleManager = (CDiscoverer *) aPtr ;
	
	if(!thisLocaleManager->iLanguageChangeNotifier)	
		{
	    __ECOM_TRACE("ECOM: LocaleChangedL: Bad Change Notification");
		return KErrGeneral;
		}

 	TInt stat = thisLocaleManager->iLanguageChangeNotifier->Change();
	if((stat & EChangesLocale) && (!thisLocaleManager->iLanguageChangeDiscoveryPending))
	   	{
	   	//
	   	// System Locale data has been updated 
	   	// if the downgrade path has changed we 
		// re-scan resource files for all drives and get the right language.
		TBool isLanguageChanged;
		thisLocaleManager->iDiscovererObserver.LanguageChangedL(isLanguageChanged);
		if(isLanguageChanged)
			{
			thisLocaleManager->LanguageChangeNotificationL();
			}
	   	}
	 return KErrNone;
	}
