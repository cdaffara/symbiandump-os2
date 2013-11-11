// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// This file contains test classes and their implementations
// to test production class CDiscoverer. Where necessary stubs
// are implemented to help in writing test harness using RTest.
// 
//

#include "Discoverer.h"
#include "EComErrorCodes.h"
#include "ImplementationInformation.h"
#include "DiscovererObserver.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include "EComUidCodes.h"
#include "RegistryData.h"
#include "DriveInfo.h"

#include <ecom/ecompanics.h>
#include <e32test.h>
#include <bautils.h>
#include <s32file.h>
#include <barsread2.h>
#include <e32uid.h>
#include <startup.hrh>
#include "baspi.h"
#include <sacls.h>


//New RSC based plugins
_LIT(KNewResourceFileName,		 		"C:\\resource\\plugins\\EComExample5.rsc");
_LIT(KNewExampleDllFileName,	 		"C:\\sys\\bin\\EComExample5.dll");
_LIT(KNewResourceFileNameOnZ,   		"z:\\RAMOnly\\EComExample5.rsc");
_LIT(KNewExampleDllFileNameOnZ, 		"z:\\RAMOnly\\EComExample5.dll");
_LIT(KNewExampleDllFileNameOnly,		"EComExample5.dll");
_LIT(KNewResourceSearchPath,			"\\resource\\plugins\\*");
//PlugIn3 RSC based plugins
_LIT(KPlugIn3ResourceFileName,		 		"C:\\resource\\plugins\\EComExample12.rsc");
_LIT(KPlugIn3ExampleDllFileName,	 		"C:\\sys\\bin\\EComExample12.dll");
_LIT(KPlugIn3ResourceFileNameOnZ,   		"z:\\RAMOnly\\EComExample12.rsc");
_LIT(KPlugIn3ExampleDllFileNameOnZ, 		"z:\\RAMOnly\\EComExample12.dll");
_LIT(KPlugIn3ExampleDllFileNameOnly,		"EComExample12.dll");

//Old DLL based plugins

// spi test file
_LIT(KEComSpiTestFilePathAndName, "Z:\\Test\\Data\\EcomTest.spi");
_LIT(KEComSpiTestFilePlugIn3PathAndName, "Z:\\Test\\Data\\EcomTestType3.spi");
// file name detremined from example spi file
_LIT(KEComSpiTestFileDllPathAndName, "Z:\\sys\\bin\\EComExample5.dll");
_LIT(KEComSpiTestFilePlugIn3DllPathAndName, "Z:\\sys\\bin\\EComExample12.dll");

// spi file
_LIT(KEComSPIFilePathAndName, "Z:\\private\\10009D8F\\Ecom.spi");

const TInt KOneSecond = 1000000;

LOCAL_D RFs	TheFs;

LOCAL_D RTest test(_L("t_discoverer.exe"));

LOCAL_D CTrapCleanup* 	  TheTrapCleanup 	 = NULL;

LOCAL_D CActiveScheduler* TheActiveScheduler = NULL;

// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

// Used for OOM test
#define TEST_OOM_ERR if(err == KErrNoMemory) User::Leave(err)

/**
Stub classes are provided to act as replacement members for the class
under test. This ensures the test class is constructed correctly and its
behaviour is also correct within the context of the test. It allows the
class to be tested in isolation. Other tests are available that test the
interoperability of the class
*/
class CObserverStub : public CBase, public MDiscovererObserver
{
public:
	enum OSState
		{
		OSS_Undefined,
		OSS_NoPlugins,
		OSS_CriticalPlugins,
		OSS_DiscoverNonCriticalPlugins,
		OSS_AllPlugins
		};
	static CObserverStub* NewL();
	virtual ~CObserverStub();

	// Inherited from MDiscovererObserver
	virtual void DiscoveriesBegin();
	virtual void RegisterDiscoveryL(const TDriveName& aDrive,CPluginBase*& aDirEntry,TBool aDatFileExists);
	virtual void DiscoveriesComplete(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType);
	virtual void DriveRemovedL(TDriveUnit aDrive);
	virtual void DriveReinstatedL(TDriveUnit aDrive);
	virtual void DriveIsSpiBasedL(const TDriveUnit& /*aDrive*/){}
	virtual TBool NotifiedWithErrorCode(TInt aError);
	virtual TBool IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive) const;
	virtual void SetDiscoveryFlagL(const TDriveUnit &aDrive);
	virtual void LanguageChangedL(TBool& aLanguageChanged);


	// Result verifing methods
	TBool IsDriveMounted(const TDriveUnit aDrive);
	TBool IsEntryProcessed(CPluginBase*& aDirEntry);
	//TBool IsSpiEntryProcessed(const TEntry& aEntry);
	TBool IsDiscoveriesCompleteSuccessfully();
	void SetDiscoverer(CDiscoverer* aDiscoverer);
	OSState GetState();

private:
	CObserverStub();

private:
	/** Base class ecom entry */
	CPluginBase* iEntryBase;

	/** Information of the dll which contains interface implementations */
	//TEComEntryDetails		iEntry;

	/** Information of the spi file which contains interface implementations */
	//TEComSpiResourceEntryDetails		iSpiEntry;

	/** iDriveUnit is drive mounted */
	TDriveUnit	iDriveUnit;

	/** Flag to indicate whether iDriveUnit is mounted or not */
	TBool		iDriveMounted;

	/** Flag to indicate whether iEntry is processed or not */
	TBool		iEntryProcessed;

	/** Flag to indicate whether iSpiEntry is processed or not */
	TBool		iSpiEntryProcessed;

	/** Flag to indicate whether Discovery process is Complete or not */
	TBool		iDiscoveriesComplete;

	/** The current start-up state of this object */
	OSState     iState;

	/** A reference to the CDiscoverer object that this class is observing*/
	CDiscoverer* iDiscoverer;
};  // End of CObserverStub declaration


/**
The TDiscoverer_StateAccessor class allows to access the private and protected
members of production code class CDiscoverer, as its a friend class.
*/
class TDiscoverer_StateAccessor
	{
public:

	// Auxiliary functions that provide access to
	// CDiscoverer private/protected members
	void ScanDirectoryL(CDiscoverer& aDiscoverer, TDriveUnit aDriveUnit);
	void ScanDirectoryCancel(CDiscoverer& aDiscoverer);
	void CompleteNotificationProcessing(CDiscoverer& aDiscoverer);
	void ValidateEntryL(CDiscoverer& aDiscoverer,
						const TEntry& aEntry,
						const TDriveName& aDriveName,
						CPluginBase*& aEntryToFill,
						TBool aIsRO);
	void ValidateEntryL(CDiscoverer& aDiscoverer,
						RResourceArchive& aRscArchive,
						CPluginBase*& aEntryToFill);
	void ProcessEntryL(CDiscoverer& aDiscoverer,const TDriveName& aDrive, CPluginBase*& aEntry, TBool aDatFileExists);
	void DriveMountedL(CDiscoverer& aDiscoverer, const TDriveUnit aDrive);
	void DriveUnmountedL(CDiscoverer& aDiscoverer, const TDriveUnit aDrive);
	void SwiChangeNotificationL(CDiscoverer& aDiscoverer, TInt aSwiOperation);
	void IdleScanningTimerRunErrorL(CDiscoverer& aDiscoverer, TInt aError);
	void DirChangeNotifierRunErrorL(CDiscoverer& aDiscoverer, TInt aError);
	void SwiChangeNotifierRunError(CDiscoverer& aDiscoverer, TInt aError);

	// Verification functions to check the state of CDiscoverer object against tests
	TBool IsDriveMounted(CDiscoverer& aDiscoverer, const TDriveUnit aDrive);
	TBool IsDiscovererActive(CDiscoverer& aDiscoverer);
	TBool IsDirectoryScanCancelled(CDiscoverer& aDiscoverer);
	TBool IsScanDirectoryComplete(CDiscoverer& aDiscoverer);
	TBool IsScanDirectoryPending(CDiscoverer& aDiscoverer, TDriveUnit aDrive);
	TUint PendingDriveListCount(CDiscoverer& aDiscoverer);
	CDiscoverer::TDiscovererState State(CDiscoverer& aDiscoverer);
	CObserverStub::OSState GetDiscovererObserverState(CDiscoverer& aDiscoverer);
	void ScanDriveL(CDiscoverer& aDiscoverer, TDriveUnit aDrive,  TBool aIsRO);
	void LanguageChangedL(CDiscoverer& aDiscoverer);
	};

/**
Scans plugin directories

@param		aDiscoverer The CDiscoverer class object under test
@return		void
*/
void TDiscoverer_StateAccessor::ScanDirectoryL(CDiscoverer& aDiscoverer, TDriveUnit aDriveUnit)
	{
	aDiscoverer.RediscoveryScanDirectoryL(aDriveUnit);
	}

/**
Stops scanning of the plugin directories

@param		aDiscoverer The CDiscoverer class object under test
*/
void TDiscoverer_StateAccessor::ScanDirectoryCancel(CDiscoverer& aDiscoverer)
	{
	aDiscoverer.ScanDirectoryCancel();
	}

/**
Signals that the directory change has been fully processed.

@param		aDiscoverer The CDiscoverer class object under test
*/
void TDiscoverer_StateAccessor::CompleteNotificationProcessing(CDiscoverer& aDiscoverer)
	{
	aDiscoverer.CompleteNotificationProcessing();
	}

/**
Verifies that the discovered entry is valid.
Used to test the Interface Implementation Collection entry

@param		aDiscoverer The CDiscoverer class object under test
@param		aEntry The plugin name
@param		aDriveName The drive containing the entry
@param		aEntryToFill On return points to complete
			plugin file name(with path) i.e. aPath + aEntry
*/
void TDiscoverer_StateAccessor::ValidateEntryL(CDiscoverer& aDiscoverer,
											   const TEntry& aEntry,
											   const TDriveName& aDriveName,
											   CPluginBase*& aEntryToFill,
											   TBool aIsRO)
	{
	aDiscoverer.ValidateEntryL(aEntry, aDriveName, aEntryToFill, aIsRO);
	}

/**
Verifies that the discovered entry is valid.
Used to test the Interface Implementation Collection entry

@param		aDiscoverer The CDiscoverer class object under test
@param		aRscArchive reference to the resource archive
@param		aEntryToFill On return points to complete
			plugin file name(with path) i.e. aPath + aEntry
*/
void TDiscoverer_StateAccessor::ValidateEntryL(CDiscoverer& aDiscoverer,
											RResourceArchive& aRscArchive,
											CPluginBase*& aEntryToFill)
	{
	aDiscoverer.ValidateEntryL(aRscArchive,aEntryToFill);
	}
/**
Registers an Interface Implementation Collection

@param		aDiscoverer The CDiscoverer class object under test
@param		aEntry This is the plugin name that needs to be registered
*/
void TDiscoverer_StateAccessor::ProcessEntryL(CDiscoverer& aDiscoverer,const TDriveName& aDrive,
										  CPluginBase*& aEntry, TBool aDatFileExists)
	{
	aDiscoverer.ProcessEntryL(aDrive,aEntry,aDatFileExists);
	}

/**
Notifies the discoverer of an SWI operation change

@param		aDiscoverer The CDiscoverer class object under test
@param		aSwiOperation This new SWI state
*/
void TDiscoverer_StateAccessor::SwiChangeNotificationL(CDiscoverer& aDiscoverer, TInt aSwiOperation)
	{
	aDiscoverer.SwiChangeNotificationL(aSwiOperation);
	}

/**
Signals that a drive is available

@param		aDiscoverer The CDiscoverer class object under test
@param		aDrive Drive that needs to be mounted
*/
void TDiscoverer_StateAccessor::DriveMountedL(CDiscoverer& aDiscoverer,
											  const TDriveUnit aDrive)
	{
	aDiscoverer.DriveMountedL(aDrive);
	}

/**
Signals that a drive is unavailable

@param		aDiscoverer The CDiscoverer class object under test
@param		aDrive Drive that needs to be unmounted/removed
*/
void TDiscoverer_StateAccessor::DriveUnmountedL(CDiscoverer& aDiscoverer,
												const TDriveUnit aDrive)
	{
	aDiscoverer.DriveUnmountedL(aDrive);
	}

/**
Checks whether CDiscoverer object has successfully completed with
the scanning of the plugin directories on all drives

@param		aDiscoverer The CDiscoverer class object under test
@return		true if notification has been processed on all drives.
			false if there is notification processing pending on any drive
*/
TBool TDiscoverer_StateAccessor::IsScanDirectoryComplete(CDiscoverer& aDiscoverer)
	{
	// check state of discoverer to see if all pending drives have been scanned.
	return(aDiscoverer.State() == CDiscoverer::EDisc_AllPluginsDisc);
	}

/**
Checks whether a notification has been signaled but not processed on specified drive.

Error Condition	: Leaves with KErrNotFound if the drive is not registered.
@param		aDiscoverer The CDiscoverer class object under test
@param		aDrive The drive to check
@return		true if notification has been processed on the specified drive.
			false if notification processing is pending
*/
TBool TDiscoverer_StateAccessor::IsScanDirectoryPending(CDiscoverer& aDiscoverer, TDriveUnit aDrive)
	{
	if(aDiscoverer.iScanningTimer->iPendingDriveList.Find(aDrive)!=KErrNotFound)
		{
		return ETrue;
		}
	else
		{
		return EFalse;
		}
	}

/**
Checks the current count of drives in the list waiting for processing.

@param		aDiscoverer The CDiscoverer class object under test
@return		TUint
*/
TUint TDiscoverer_StateAccessor::PendingDriveListCount(CDiscoverer& aDiscoverer)
	{
	return(aDiscoverer.iScanningTimer->iPendingDriveList.Count());
	}

/**
Checks that CDiscoverer object is not scanning the plugin directories.

@param		aDiscoverer The CDiscoverer class object under test
@return		true if plugin dirctory scanning is stopped.
*/
TBool TDiscoverer_StateAccessor::IsDirectoryScanCancelled(CDiscoverer& aDiscoverer)
	{
	if(!aDiscoverer.iDirScanner)
		{
		return ETrue;
		}
	return EFalse;
	}

/**
Checks that CDiscoverer object is currently activated for the plugin dir scanning

@param		aDiscoverer The CDiscoverer class object under test
@return		true if Discoverer object is tracking changes in the plugin directories in each drive.
*/
TBool TDiscoverer_StateAccessor::IsDiscovererActive(CDiscoverer& aDiscoverer)
	{
	//Return ETrue if both iScanningTimer & directory notifier(s) are active
	TBool isActive = ETrue;

	  for(TInt index = 0; index <aDiscoverer.iRscDirNotifierList.Count(); index++)
	  	{
	  	if(!(aDiscoverer.iRscDirNotifierList[index]&& aDiscoverer.iRscDirNotifierList[index]->IsActive()))
			{
			// iRscDirNotifierList should be active, as each active object on list is constantly
			// looking for changes in the plugin directories in each drive.
			isActive = EFalse;
			break;
			}
		}
	return isActive;
	}

/**
Verifies whether given drive is registered

@param		aDiscoverer The CDiscoverer class object under test
@param		aDrive Drive that needs to be verified for registration
@return		return true if given drive is installed
*/
TBool TDiscoverer_StateAccessor::IsDriveMounted(CDiscoverer& aDiscoverer,
												const TDriveUnit aDrive)
	{
	TBool isMounted = ETrue;
	if(aDiscoverer.iDrivesDiscovered.Find(aDrive) == KErrNotFound)
		{
		isMounted = EFalse;
		}
	return isMounted;
	}

/**
Scans plugin directories

@param		aDiscoverer The CDiscoverer class object under test
@param 		aDrive	The drive which is scanned
@param		aIsRO	Whether the drive is ready-only
@return		true if plugin directorie in each drive get scaned successfully
*/
void TDiscoverer_StateAccessor::ScanDriveL(CDiscoverer& aDiscoverer, TDriveUnit aDrive,  TBool aIsRO)
	{
	return aDiscoverer.iDirScanner->ScanDriveL(aDrive, aIsRO);
	}
void TDiscoverer_StateAccessor::LanguageChangedL(CDiscoverer& aDiscoverer)
	{
	aDiscoverer.LanguageChangeNotificationL();
	}
/**
Retrieve the object's current state.

@param		aDiscoverer The CDiscoverer class object under test
@return 	TDiscovererState the current state of the CDiscoverer
class object under test
*/
CDiscoverer::TDiscovererState TDiscoverer_StateAccessor::State(CDiscoverer& aDiscoverer)
	{
	return aDiscoverer.State();
	}

CObserverStub::OSState TDiscoverer_StateAccessor::GetDiscovererObserverState(CDiscoverer& aDiscoverer)
	{
	CObserverStub* discovererObserver =
		static_cast<CObserverStub*>(&aDiscoverer.iDiscovererObserver);
	return discovererObserver->GetState();
	}

/**
Call the CIdleScanningTimer RunError funtion

@param		aDiscoverer The CDiscoverer class object under test
@param 		aError The error code to pass to the RunError function
*/
void TDiscoverer_StateAccessor::IdleScanningTimerRunErrorL(CDiscoverer& aDiscoverer, TInt aError)
	{
	aDiscoverer.iScanningTimer->RunError(aError);
	}

/**
Call the CDirChangeNotifier RunError funtion

@param		aDiscoverer The CDiscoverer class object under test
@param 		aError The error code to pass to the RunError function
*/
void TDiscoverer_StateAccessor::DirChangeNotifierRunErrorL(CDiscoverer& aDiscoverer, TInt aError)
	{
	aDiscoverer.iRscDirNotifierList[0]->RunError(aError);
	}

/**
Call the CSwiChangeNotifier RunError funtion

@param		aDiscoverer The CDiscoverer class object under test
@param 		aError The error code to pass to the RunError function
*/
void TDiscoverer_StateAccessor::SwiChangeNotifierRunError(CDiscoverer& aDiscoverer, TInt aError)
	{
	aDiscoverer.iSwiChangeNotifier->RunError(aError);
	}

/**
Creates an instance of CObserverStub class.

@return		The new'ed object.
*/
CObserverStub* CObserverStub::NewL()
	{
	return new(ELeave) CObserverStub();
	}

/**
Destructor

@post		This object is properly destroyed.
*/
CObserverStub::~CObserverStub()
	{
	// do nothing;
	}

/**
Default constructor.

@post		The object is properly constructed.
*/
CObserverStub::CObserverStub()
				:CBase(),
				iDriveUnit(EDriveC),
				iDriveMounted(EFalse),
				iEntryProcessed(EFalse),
				iSpiEntryProcessed(EFalse),
				iDiscoveriesComplete(EFalse),
				iState(OSS_NoPlugins)
	{
	// do nothing;
	}

/**
MDiscovererObserver callback method, to signal that a discovery
session is starting.
*/
void CObserverStub::DiscoveriesBegin()
	{
	iDiscoveriesComplete = EFalse;
	}

void CObserverStub::SetDiscoveryFlagL(const TDriveUnit & /*aDrive*/)
	{
	iDiscoveriesComplete = ETrue;
	}
/**
MDiscovererObserver callback method to register an Ecom Spi file.

@param		aEntry The Ecom file  that needs to be registered
*/
void CObserverStub::RegisterDiscoveryL(const TDriveName& /* aDriveName */,CPluginBase*& aEntry,TBool /* aDatFileExists*/)
	{
	iEntryBase = aEntry;
	iSpiEntryProcessed = ETrue;
	}

/**
MDiscovererObserver callback method, to signal that a discovery
session is complete.

@param		aSuccessful Indicates discoveries process completed successfully or not
@param		aProcessingType indicates the type of processing for plugins
			for ensuring that plugins are not processed multiple times
			during start-up phase
*/
void CObserverStub::DiscoveriesComplete(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType)
	{
	iDiscoveriesComplete = aSuccessful;

	switch(iState)
		{
		case OSS_NoPlugins:
			if(aProcessingType == EPluginProcessingTypeCriticalOnly &&
			   iDiscoverer->State() == CDiscoverer::EDisc_CriticalPluginsDisc)
				{
				iState = OSS_CriticalPlugins;
				}
			else if(aProcessingType == EPluginProcessingTypeAll &&
				    iDiscoverer->State() == CDiscoverer::EDisc_AllPluginsDisc)
				{
				iState = OSS_AllPlugins;
				}
			break;
		case OSS_CriticalPlugins:
			if(aProcessingType == EPluginProcessingTypeNonCriticalOnly &&
			   iDiscoverer->State() == CDiscoverer::EDisc_AllPluginsDisc)
				{
				iState = OSS_AllPlugins;
				}
			break;
		}
	}

/**
Verifies whether given plugin entry is registered

@param		aDirEntry The Ecom plugin that to be checked for registration.
@return		true if given plugin is registered
*/
TBool CObserverStub::IsEntryProcessed(CPluginBase*& aDirEntry)
	{
	if(iEntryProcessed && (iEntryBase->iDllThirdUid == aDirEntry->iDllThirdUid))
		{
		return ETrue;
		}

	return EFalse;
	}

/**
MDiscovererObserver callback method, to signal that a Drive
is removed/dismounted.

@param		aDrive The drive that is removed.
*/
void CObserverStub::DriveRemovedL(TDriveUnit aDrive)
	{
	iDriveMounted = EFalse;
	iDriveUnit = aDrive;
	}

/**
MDiscovererObserver callback method, to signal that a Drive
is reinstalled.

@param		aDrive The drive that is available now.
*/
void CObserverStub::DriveReinstatedL(TDriveUnit aDrive)
	{
	iDriveMounted = ETrue;
	iDriveUnit = aDrive;
	}

/**
MDiscovererObserver callback method, to signal that
during a discovery session an error has occured.

@param		aError The notification error code.
@return		true if aError is one of the acceptable error codes
*/
TBool CObserverStub::NotifiedWithErrorCode(TInt aError)
	{
	// Test the safe error codes
	return (aError == KErrNotReady ||		// Drive removed
			aError == KErrPathNotFound);	// Directory deleted
	}

/**
MDiscovererObserver callback method,to retrieve the drive unit's DAT
file infor.

@param 		aDrive the identifier of the drive to retrieve DAT file infor from.
@return		ETrue if DAT file exists on the drive unit, otherwise EFlase.

*/
TBool CObserverStub::IsAnyDllRegisteredWithDriveL(const TDriveUnit /*aDrive*/) const
	{
	// To pass the build always returns EFalse to mean no Dll is discovered in the drive,
	// it is not used in this test.
	return EFalse;
	}

/**
Verification method that checks whether given drive is available

@param		aDrive The drive that to be checked.
@return		true if given drive is registered
*/
TBool CObserverStub::IsDriveMounted(const TDriveUnit aDrive)
	{
	TBool mounted = EFalse;
	if(iDriveMounted && (aDrive == iDriveUnit))
		{
		mounted = ETrue;
		}
	return mounted;
	}

/**
Verification method that checks whether discovery process is completed successfully

@return		true if discovery process is completed successfully
*/
TBool CObserverStub::IsDiscoveriesCompleteSuccessfully()
	{
	return iDiscoveriesComplete;
	}

/**
Returns the current start-up state of this object

@return	    The current start-up state of this object
*/
CObserverStub::OSState CObserverStub::GetState()
	{
	return iState;
	}

void CObserverStub::SetDiscoverer(CDiscoverer* aDiscoverer)
	{
	iDiscoverer = aDiscoverer;
	}

void CObserverStub::LanguageChangedL(TBool& aLanguageChanged)
	{
	aLanguageChanged = EFalse;
	}
class CDiscovererTestShutdown : public CTimer
	{
public:
	inline CDiscovererTestShutdown();
	inline void ConstructL();
	inline void StartAfter(TTimeIntervalMicroSeconds32 aTimeInterval);

private:
	void RunL();
	};

inline CDiscovererTestShutdown::CDiscovererTestShutdown()
:CTimer(EPriorityHigh)// Priority set higher than CIdleScanningTimer
	{
	CActiveScheduler::Add(this);
	}

inline void CDiscovererTestShutdown::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CDiscovererTestShutdown::StartAfter(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	After(aTimeInterval);
	}
void CDiscovererTestShutdown::RunL()
	{
	CActiveScheduler::Stop();
	}

/**
Test class for object CDiscoverer.
This class provides the parameters and behaviour that
allows this class to behave normally under a test
scenario.
*/
class CDiscovererTest : public CBase
	{
public:
	static CDiscovererTest* NewL();
	virtual ~CDiscovererTest();

	void ResumeSuspendTestL();
	void DriveMountUnmountTestL();
	void ProcessEntryTestL();
	void ProcessSpiEntryTestL();
	void ValidateSpiEntryTestL();
	void ValidateSpiPluginsTestL();
	void ValidateEntryTestL();
	void ProcessEntryPlugIn3TestL();
	void ProcessSpiEntryPlugIn3TestL();
	void ValidateSpiEntryPlugIn3TestL();
	void ValidateEntryPlugIn3TestL();
	void ScanDirectoryIncrementTestL();
	void ScanDirectoryTestL();
	void ScanDirectoryCancelTestL();
	void StagedDiscoveryStateTransitionTestL();
	void AllAtOnceDiscoveryStateTransitionTestL();
	void MultipleNotificationProcessingTestL();
	void SWINotificationProcessingTestL();
	void IdleScanningTimerRunErrorL();
	void DirChangeNotifierRunErrorL();
	void SwiChangeNotifierRunError();
	void LanguageChangedNotificationTestL();

private:
	CDiscovererTest();
	void ConstructL();

public:
	/** The instance of the class under test */
	CDiscoverer* iDiscoverer;

	/** The instance of the state accessor interface */
	TDiscoverer_StateAccessor* iStateAccessor;

	/** The instance of the observer stub of the CDiscoverer */
	CObserverStub* iDiscovererObserver;

	/** Unique Id of the ECOM dll */
	TUid		iDllUid;

	/** The drive on which interface implementations can be found */
	TDriveUnit	iDriveUnit;

	/** Information on a dll which contains interface implementations */
	TEntry		iDllEntry;

	/** A shutdown timer to manipulate the Active Scheduler of testing server */
	CDiscovererTestShutdown	iShutdown;

	CEComCachedDriveInfo* iCachedDriveInfo;
	};

/**
Standardised safe construction which
leaves nothing on the cleanup stack.

@post			CDiscovererTest is fully constructed and initialised
@return			The new'ed object.
*/
CDiscovererTest* CDiscovererTest::NewL()
	{
	CDiscovererTest* self = new (ELeave) CDiscovererTest();
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Default constructor.

@post	The object is properly constructed.
*/
CDiscovererTest::CDiscovererTest()
				:CBase(),
				 iDriveUnit(EDriveC)
	{
	TUid uid1 = {0};
	TUid uid2 = {0};

	iDllUid.iUid	= 0x10009DB6; // Dll Uid for EComRomOnlyExampleOnC.dll
	iDllEntry.iType = TUidType(uid1, uid2, iDllUid);
	}

/**
Destructor.

@post		This object is properly destroyed.
*/
CDiscovererTest::~CDiscovererTest()
	{
	delete iDiscoverer;
	delete iStateAccessor;
	delete iDiscovererObserver;
	delete iCachedDriveInfo;
	}

/**
Standardized 2nd(Initialization) phase of two phase construction.
Creates supporting class objects for the execution of test.

@post			CDiscovererTest is fully constructed.
*/
void CDiscovererTest::ConstructL()
	{
	iStateAccessor		= new(ELeave) TDiscoverer_StateAccessor;
	iDiscovererObserver	= CObserverStub::NewL();
	iDiscoverer			= CDiscoverer::NewL(*iDiscovererObserver, TheFs);
	iDiscovererObserver->SetDiscoverer(iDiscoverer);
	iShutdown.ConstructL();
	iCachedDriveInfo = CEComCachedDriveInfo::NewL(TheFs);
	}

/**
The test executes by first suspending the discoverer then by
resuming it.

@SYMTestCaseID          SYSLIB-ECOM-CT-0716
@SYMTestCaseDesc	    Tests for resuming and suspending Discoverer
@SYMTestPriority 	    High
@SYMTestActions  	    Suspend if Discoverer is active and then resume. Check for the error conditions(OOM)
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CDiscovererTest::ResumeSuspendTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0716 "));
	// Perform an initial discovery and start the notifiers.
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);

	TBool isDiscActive = iStateAccessor->IsDiscovererActive(*iDiscoverer);
	// Notifiers should be active.
	test(isDiscActive);

	TInt err = iDiscoverer->Suspend();
	TEST_OOM_ERR;
	test(err == KErrNone);

	// Notifiers should still be active.
	isDiscActive = iStateAccessor->IsDiscovererActive(*iDiscoverer);
	test(isDiscActive);

	// Test that there are no drives pending a scan before we copy the plugin.
	test(!iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveC));

	// Raise a plugin change notification on C: drive. This simulates a plugin being copied.
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveC);
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveC);

	// Test that the drive we copied the plugin to still has a scan pending.
	test(iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveC));

	isDiscActive = iStateAccessor->IsDiscovererActive(*iDiscoverer);
	// Notifiers should still be active.
	test(isDiscActive);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check that notifications have not been processed.
	test(!iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	err = iDiscoverer->Resume();
	TEST_OOM_ERR;
	test(err == KErrNone);

	isDiscActive = iStateAccessor->IsDiscovererActive(*iDiscoverer);
	// Notifiers should still be active.
	test(isDiscActive);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check if notifications have been processed on all drives.
	test(iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));
	}

/**
The test executes by first making the test drive unmounted and
then testing for Mount and Unmount.

@SYMTestCaseID          SYSLIB-ECOM-CT-0717
@SYMTestCaseDesc	    Tests for drive mount and unmount
@SYMTestPriority 	    High
@SYMTestActions  	    First remove the drive if its already there, then test for Mount then for unmount
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CDiscovererTest::DriveMountUnmountTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0717 "));
	TInt err;
	TDriveUnit drive(EDriveC);
	//First remove the drive if its already there, then test for Mount then for unmount
	if(iStateAccessor->IsDriveMounted(*iDiscoverer, drive))
		{
		TRAP(err, iStateAccessor->DriveUnmountedL(*iDiscoverer, drive));
		TEST_OOM_ERR;
		test(err == KErrNone);
		}

	// Drive should not be present
	test(!iStateAccessor->IsDriveMounted(*iDiscoverer, drive));

	// Test Mount
	TRAP(err, iStateAccessor->DriveMountedL(*iDiscoverer, drive));
	TEST_OOM_ERR;
	test(err == KErrNone);

	// CDiscoverer->DriveMountedL/DriveUnmountedL updates both itself and
	// DiscovererObserver class for Mount/Unmount of drives. So testing for both
	test(iStateAccessor->IsDriveMounted(*iDiscoverer, drive));
	test(iDiscovererObserver->IsDriveMounted(drive));

	// Test Unmount
	TRAP(err, iStateAccessor->DriveUnmountedL(*iDiscoverer, drive));
	TEST_OOM_ERR;
	test(err == KErrNone );

	// CDiscoverer->DriveMountedL/DriveUnmountedL updates both itself and
	// DiscovererObserver class for Mount/Unmount of drives. So testing for both
	test(!iStateAccessor->IsDriveMounted(*iDiscoverer, drive));
	test(!iDiscovererObserver->IsDriveMounted(drive));
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3559
@SYMTestCaseDesc	    Tests for process entry of PLUGIN3 type.
@SYMTestPriority 	    High
@SYMTestActions  	    Call CDiscoverer::ProcessEntryL().
@SYMTestExpectedResults The entry is registered successfully and no leave occurred.
@SYMEC	                EC43
*/
void CDiscovererTest::ProcessEntryPlugIn3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3559 "));
	CPluginBase* entryBase = NULL;
	TInt err = KErrNone;

	//This part refers to testing of the entry containing the RSC
	_LIT(KExampleRscFileNameOnly,"EComExample12.rsc");
	TDriveName driveName1(iDriveUnit.Name());
	TParse path1;
	path1.Set(KNewResourceSearchPath(),NULL,&driveName1);
	//Creating a entry that represents the Rsc disovered during scanning of \\resource\\plugins
	TEntry rscEntry;
	rscEntry.iName = KExampleRscFileNameOnly;
	rscEntry.iType = TUidType(KNullUid,KUidInterfaceImplementationCollectionInfo,KNullUid);
	entryBase = CSecurePlugin::NewL(TheFs,rscEntry,driveName1, EFalse);
	CleanupStack::PushL(entryBase);

	// ProcessEntryL() updates entryToFill with information on a rsc specified by the other params.
	TRAP(err, iStateAccessor->ProcessEntryL(*iDiscoverer,driveName1,entryBase,ETrue));
	TEST_OOM_ERR;
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(entryBase);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3560
@SYMTestCaseDesc 		Check that the ProcessEntryL for spi data works correctly when using PLUGIN3 entries in the spi file.
@SYMTestPriority 		High
@SYMTestActions  		Call CDiscoverer::ProcessEntryL().
@SYMTestExpectedResults ecomtesttype3.spi is processed successfully and no leave occurred.
@SYMEC					EC43
*/
void CDiscovererTest::ProcessSpiEntryPlugIn3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3560 "));
	TEntry spiEntry;
	test(TheFs.Entry(KEComSpiTestFilePlugIn3PathAndName, spiEntry) == KErrNone);
	TParse spiPath;
	spiPath.Set(KEComSpiTestFilePlugIn3PathAndName, NULL, NULL);

	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFilePlugIn3PathAndName);
	CleanupClosePushL(resourceArchive);

	// check SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	// there is only 1 resource file
	while(!resourceArchive.End())
		{
		CPluginBase* entry = CSpiPlugin::NewL(resourceArchive);
		CleanupStack::PushL(entry);

		TRAPD(err, iStateAccessor->ProcessEntryL(*iDiscoverer,spiPath.Drive(), entry,ETrue));
		TEST_OOM_ERR;
		test(err == KErrNone);

		CleanupStack::PopAndDestroy(entry);
		entry = NULL;
		}
	CleanupStack::PopAndDestroy(&resourceArchive);
	}
/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3561
@SYMTestCaseDesc	    Tests for the reference of the entry containing the DLL of PLUGIN3 type.
@SYMTestPriority 	    High
@SYMTestActions  	    Call CDiscoverer::ValidateEntryL(). Check the entry returned.
@SYMTestExpectedResults The entry is validated successfully and no leave occurred.
@SYMEC	                EC43
*/
void CDiscovererTest::ValidateEntryPlugIn3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3561 "));
	TInt err = KErrNone;

	//This part refers to testing of the entry containing the RSC
	_LIT(KExampleRscFileNameOnly,"EComExample12.rsc");
	TDriveName driveName1(iDriveUnit.Name());
	CPluginBase *entryToFill1 = NULL;
	//Creating an entry that represents the Rsc disovered during scanning of \\resource\\plugins
	TEntry rscEntry;
	rscEntry.iName = KExampleRscFileNameOnly;
	rscEntry.iType = TUidType(KNullUid,KUidInterfaceImplementationCollectionInfo,KNullUid);

	TUid dllUid1 = {KDynamicLibraryUidValue};
	TUid example12Uid = {0x10009E3E};
	TUidType dllUidType = TUidType(dllUid1,KUidInterfaceImplementationCollection,example12Uid);

	//Now call ValidateEntryL() with this entry containg the rsc
	TRAP(err,iStateAccessor->ValidateEntryL(*iDiscoverer,rscEntry,driveName1,entryToFill1, EFalse));
	TEST_OOM_ERR;
	test(err == KErrNone);
	test(entryToFill1->iDllThirdUid == dllUidType[2]);
	TParse dllParse;
	dllParse.Set(KPlugIn3ExampleDllFileName,NULL,NULL);
	test(entryToFill1->iDllName->CompareF(dllParse.NameAndExt()) == 0);

	delete entryToFill1;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-3562
@SYMTestCaseDesc 		Check that the ValidateEntryL for spi data works correctly when using PLUGIN3 entries in the spi file.
@SYMTestPriority 		High
@SYMTestActions  		Call CDiscoverer::ValidateEntryL(). Check the entry returned.
@SYMTestExpectedResults ecomtesttype3.spi file is validated successfully and no leave occurred.
@SYMEC					EC43
*/
void CDiscovererTest::ValidateSpiEntryPlugIn3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3562 "));
	TEntry spiEntry;
	test(TheFs.Entry(KEComSpiTestFilePlugIn3PathAndName, spiEntry) == KErrNone);
	TParse spiPath;
	spiPath.Set(KEComSpiTestFilePlugIn3PathAndName, NULL, NULL);

	TEntry TestEntry;
	TUid uid1 = {0x10000079};
	TUid uid2 = {0x10009D93};
	TUid uid3 = {0x10009E3E}; // Uid for EComExample12.dll
	TestEntry.iType = TUidType(uid1, uid2, uid3);
	TestEntry.iName = KPlugIn3ExampleDllFileNameOnly;

	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFilePlugIn3PathAndName);
	CleanupClosePushL(resourceArchive);

	// check SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	// there is only 1 resource file
	while(!resourceArchive.End())
		{
		CPluginBase* entryToFill = NULL;
		TRAPD(err, iStateAccessor->ValidateEntryL(*iDiscoverer,resourceArchive,entryToFill));
		CleanupStack::PushL(entryToFill);

		TEST_OOM_ERR;
		test(err == KErrNone);
		test(entryToFill->iDllThirdUid == TestEntry.iType[2]);

		TFileName name1(KEComSpiTestFilePlugIn3DllPathAndName);
		TFileName name2(*(entryToFill->iDllName));
		name1.LowerCase();
		name2.LowerCase();
		TParse dllparse;
		dllparse.Set(name1,NULL,NULL);
		test(dllparse.NameAndExt()== name2);

		CleanupStack::PopAndDestroy(entryToFill); // resourceFile, resourceName, entryToFill
		entryToFill = NULL;
		}
	CleanupStack::PopAndDestroy(&resourceArchive);
	}

/**
The test executes by Registering an Interface Implementation Collection
and later verifing it

@SYMTestCaseID          SYSLIB-ECOM-CT-0718
@SYMTestCaseDesc	    Tests for process entry.
@SYMTestPriority 	    High
@SYMTestActions  	    Register an interface implementation collection and later verifies it.
                        Check for OOM error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CDiscovererTest::ProcessEntryTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0718 "));
	CPluginBase* entryBase=NULL;
	TInt err=KErrNone;

	//This part refers to testing of the entry containing the RSC
	_LIT(KExampleRscFileNameOnly,"EComExample5.rsc");
	TDriveName driveName1(iDriveUnit.Name());
	TParse path1;
	path1.Set(KNewResourceSearchPath(),NULL,&driveName1);
	//Creating a entry that represents the Rsc disovered during scanning of \\resource\\plugins
	TEntry rscEntry;
	rscEntry.iName=KExampleRscFileNameOnly;
	rscEntry.iType=TUidType(KNullUid,KUidInterfaceImplementationCollectionInfo,KNullUid);
	TUid dllUid1 = {KDynamicLibraryUidValue};
	TUid example5Uid={0x101F847B};
	TUidType dllUidType=TUidType(dllUid1,KUidInterfaceImplementationCollection,example5Uid);
	entryBase=CSecurePlugin::NewL(TheFs,rscEntry,driveName1, EFalse);
	CleanupStack::PushL(entryBase);

	// ProcessEntryL() updates entryToFill with information on a rsc specified by the other params.
	TRAP(err, iStateAccessor->ProcessEntryL(*iDiscoverer,driveName1,entryBase,ETrue));
	TEST_OOM_ERR;
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(entryBase);
	entryBase=NULL;

	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0091
@SYMTestCaseDesc 	Check that the ProcessEntryL for spi data works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecomtest.spi can be processed successfully
					and no leave occurred.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CDiscovererTest::ProcessSpiEntryTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0091 "));
	TEntry spiEntry;
	test(TheFs.Entry(KEComSpiTestFilePathAndName, spiEntry) == KErrNone);
	TParse spiPath;
	spiPath.Set(KEComSpiTestFilePathAndName, NULL, NULL);

	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFilePathAndName);
	CleanupClosePushL(resourceArchive);

	// check SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	// there is only 1 resource file
	while(!resourceArchive.End())
		{
		CPluginBase* entry = CSpiPlugin::NewL(resourceArchive);
		CleanupStack::PushL(entry);

		TRAPD(err, iStateAccessor->ProcessEntryL(*iDiscoverer,spiPath.Drive(), entry,ETrue));
		TEST_OOM_ERR;
		test(err == KErrNone);

		CleanupStack::PopAndDestroy(entry);
		entry = NULL;
		}
	CleanupStack::PopAndDestroy(&resourceArchive);
	}
/**
The test executes by checking whether discovered plugin entry is valid

@SYMTestCaseID          SYSLIB-ECOM-CT-0719
@SYMTestCaseDesc	    Tests for plugin that resides on C drive
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for the reference of the entry containing in the DLL
                        Check for OOM error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CDiscovererTest::ValidateEntryTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0719 "));
	TInt err=KErrNone;

	//This part refers to testing of the entry containing the RSC
	_LIT(KExampleRscFileNameOnly,"EComExample5.rsc");
	TDriveName driveName1(iDriveUnit.Name());
	TParse path1;
	CPluginBase *entryToFill1 = NULL;
	path1.Set(KNewResourceSearchPath(),NULL,&driveName1);
	//Creating a entry that represents the Rsc disovered during scanning of \\resource\\plugins
	TEntry rscEntry;
	rscEntry.iName=KExampleRscFileNameOnly;
	rscEntry.iType=TUidType(KNullUid,KUidInterfaceImplementationCollectionInfo,KNullUid);

	TUid dllUid1 = {KDynamicLibraryUidValue};
	TUid example5Uid={0x101F847B};
	TUidType dllUidType=TUidType(dllUid1,KUidInterfaceImplementationCollection,example5Uid);

	//Now call ValidateEntryL() with this entry containg the rsc
	TRAP(err,iStateAccessor->ValidateEntryL(*iDiscoverer,rscEntry,driveName1,entryToFill1, EFalse));
	TEST_OOM_ERR;
	test(err==KErrNone);
	test(entryToFill1->iDllThirdUid==dllUidType[2]);
	TParse dllParse;
	dllParse.Set(KNewExampleDllFileName,NULL,NULL);
	test(entryToFill1->iDllName->CompareF(dllParse.NameAndExt())==0);

	delete entryToFill1;
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0092
@SYMTestCaseDesc 	Check that the ValidateEntryL for spi data works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecomtest.spi can be validated successfully
					and no leave occurred.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CDiscovererTest::ValidateSpiEntryTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0092 "));
	TEntry spiEntry;
	test(TheFs.Entry(KEComSpiTestFilePathAndName, spiEntry) == KErrNone);
	TParse spiPath;
	spiPath.Set(KEComSpiTestFilePathAndName, NULL, NULL);

	TEntry TestEntry;
	TUid uid1 = {0x10000079};
	TUid uid2 = {0x10009D8D};
	TUid	 uid3 = {0x101F847B}; // Uid for EComExample5.dll
	TestEntry.iType = TUidType(uid1, uid2, uid3);
	TestEntry.iName = KNewExampleDllFileNameOnly;

	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFilePathAndName);
	CleanupClosePushL(resourceArchive);

	// check SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	// there is only 1 resource file
	while(!resourceArchive.End())
		{
		CPluginBase* entryToFill = NULL;
		TRAPD(err, iStateAccessor->ValidateEntryL(*iDiscoverer,resourceArchive,entryToFill));
		CleanupStack::PushL(entryToFill);

		TEST_OOM_ERR;
		test(err == KErrNone);
		test(entryToFill->iDllThirdUid == TestEntry.iType[2]);

		TFileName name1(KEComSpiTestFileDllPathAndName);
		TFileName name2(*(entryToFill->iDllName));
		name1.LowerCase();
		name2.LowerCase();
		TParse dllparse;
		dllparse.Set(name1,NULL,NULL);
		test(dllparse.NameAndExt()== name2);

		CleanupStack::PopAndDestroy(entryToFill); // resourceFile, resourceName, entryToFill
		entryToFill = NULL;
		}
	CleanupStack::PopAndDestroy(&resourceArchive);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0093
@SYMTestCaseDesc 	Check that there is DLL for each resource in SPI file
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecom.spi contents i.e, rsc, actually exist on drive
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CDiscovererTest::ValidateSpiPluginsTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0093 "));
	__UHEAP_MARK;

	TEntry spiEntry;
	TBool err = TheFs.Entry(KEComSPIFilePathAndName, spiEntry);
	// It is possible for ecom.spi file not to exist. If it does not then no testing can be done.
	if(err != KErrNone)
		return;

	TParse spiPath;
	spiPath.Set(KEComSPIFilePathAndName, NULL, NULL);

	//To read the SPI file
	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSPIFilePathAndName);
	CleanupClosePushL(resourceArchive);
	CRegistryData::CDriveData* driveData = NULL;
	//Check spi file type
	TUid spiType = resourceArchive.Type();
	test(spiType == KEcomSpiFileTypeUid);

	while(!resourceArchive.End())
		{
		CPluginBase* entryToFill = NULL;
		TRAPD(err, iStateAccessor->ValidateEntryL(*iDiscoverer,resourceArchive,entryToFill));
		CleanupStack::PushL(entryToFill);

		TEST_OOM_ERR;
		test(err == KErrNone);

		CRegistryData::CDllData * dllData=CRegistryData::CDllData::NewLC(*(entryToFill->iDllName),entryToFill->iDllModifiedTime,entryToFill->iDllSecondUid, entryToFill->iDllThirdUid,driveData);

		TBool successful=dllData->ProcessSecurityCheckL();
		if(!successful)
		{
			_LIT(KMessage,"ERROR: Plugin SID Mismatch ERROR for %S.");
			RDebug::Print(KMessage, entryToFill->iDllName);
		}
		test(successful);

		CleanupStack::PopAndDestroy(dllData);
		CleanupStack::PopAndDestroy(entryToFill);
		entryToFill=NULL;
		}

		CleanupStack::PopAndDestroy(&resourceArchive);
	__UHEAP_MARKEND;
	}


/**
Scans plugin directories and verifies successfull completion

@SYMTestCaseID          SYSLIB-ECOM-CT-0720
@SYMTestCaseDesc	    Tests for scanning plugin directories and verifies successfull completion
@SYMTestPriority 	    High
@SYMTestActions  	    Scans registered plugin directories from A to Z drive
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CDiscovererTest::ScanDirectoryIncrementTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0720 "));
	TInt err;
	// Scans registered plugin directories from A to Z drive
	TEComCachedDriveInfoIterator iter(*iCachedDriveInfo);
	for(iter.Last(); iter.InRange(); iter.Prev())
		{
		TRAP(err, iStateAccessor->ScanDirectoryL(*iDiscoverer,iter.DriveNumber()));
		TEST_OOM_ERR;
		test(err == KErrNone);
		}
	// After successful completion of ScanDirectoryL, DiscovererObserver is updated
	// with DiscoveriesComplete successful
	// DiscovererObserver is a stub class used in place of CRegistrar. Flags are used for the
	// successfull execution of API
	test(iDiscovererObserver->IsDiscoveriesCompleteSuccessfully());
	}

/**
Stops scaning of plugin directories

@SYMTestCaseID          SYSLIB-ECOM-CT-0722
@SYMTestCaseDesc	    Tests for stopping of scanning plugin directories
@SYMTestPriority 	    High
@SYMTestActions  	    Stops scaning of plugin directories. Check for OOM error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CDiscovererTest::ScanDirectoryCancelTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0722 "));
	TRAPD(err, iStateAccessor->ScanDirectoryCancel(*iDiscoverer));
	TEST_OOM_ERR;
	test(err == KErrNone);
	test(iStateAccessor->IsDirectoryScanCancelled(*iDiscoverer));
	}

/**
Scaning of plugin directories in Z: drive and C:drive, check these two drives is in drive list

@SYMTestCaseID          SYSLIB-ECOM-UT-1859
@SYMTestCaseDesc	    Tests for scanning plugin directories in specific drives.
@SYMTestPriority 	    High
@SYMTestActions  	    scaning of plugin directories. Check for OOM error.
@SYMTestExpectedResults The test must not fail.
@SYMDEF                 DEF088454
*/

void CDiscovererTest::ScanDirectoryTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1859 "));
	EComPerformance::ResetEComPerfTimeRecords();

	// Do scan on specific drive
	TRAPD(err, iStateAccessor->ScanDriveL(*iDiscoverer, EDriveZ, ETrue));
	TEST_OOM_ERR;
	test(err == KErrNone);

	TRAP(err, iStateAccessor->ScanDriveL(*iDiscoverer, EDriveC, EFalse));
	TEST_OOM_ERR;
	test(err == KErrNone);

	// Test record information correct
	test(EComPerformance::iEComPerfTimeRecordCount == 4);
	test(EComPerformance::iEComPerfTimeRecords[0].iType == ECDiscovererRediscoveryScanDirectoryL);
	test(EComPerformance::iEComPerfTimeRecords[0].iInfo == EDriveZ);
	test(EComPerformance::iEComPerfTimeRecords[1].iType == ECDiscovererRediscoveryScanDirectoryL);
	test(EComPerformance::iEComPerfTimeRecords[1].iInfo == EDriveZ);

	test(EComPerformance::iEComPerfTimeRecords[2].iType == ECDiscovererRediscoveryScanDirectoryL);
	test(EComPerformance::iEComPerfTimeRecords[2].iInfo == EDriveC);
	test(EComPerformance::iEComPerfTimeRecords[3].iType == ECDiscovererRediscoveryScanDirectoryL);
	test(EComPerformance::iEComPerfTimeRecords[3].iInfo == EDriveC);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0185
@SYMTestCaseDesc 	Check that SSA related states transition as expected when discovery is staged
@SYMTestPriority 	High
@SYMTestActions  	Use CDiscoverer::ProcessSSAEventL to start discovery in stages i.e. rom only
then non rom only. Check that the
CDiscoverer state is transitioning correctly at every step.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CDiscovererTest::StagedDiscoveryStateTransitionTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0185 "));
	//After construction check that:
	//- current state is EDisc_NoPluginsDisc
	//- registry is empty. i.e. no plugins discovered
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_NoPluginsDisc);

	//Call ProcessSSAEvent with state set to various states that will
	//not cause a transition. Check that:
	//- current state has not changed
	iDiscoverer->ProcessSSAEventL(EStartupStateUndefined);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_NoPluginsDisc);

	//Call ProcessSSAEventL with EStartupStateCriticalStatic state.
	//Check that
	//- current state is EDisc_CriticalPluginsDisc
	//- discoverer observer has processed RO plugins
	iDiscoverer->ProcessSSAEventL(EStartupStateCriticalStatic);
	test(iStateAccessor->GetDiscovererObserverState(*iDiscoverer) == CObserverStub::OSS_CriticalPlugins);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_CriticalPluginsDisc);

	//Call ProcessSSAEvent with state set to various states that will
	//not cause a transition. Check that:
	//- current state has not changed
	iDiscoverer->ProcessSSAEventL(EStartupStateUndefined);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_CriticalPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateCriticalStatic);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_CriticalPluginsDisc);

	//Call ProcessSSAEventL with EStartupStateNonCritical state. Check that
	//- current state is EDisc_AllPluginsDisc
	//- discoverer observer has processed non-RO plugins
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);
	test(iStateAccessor->GetDiscovererObserverState(*iDiscoverer) == CObserverStub::OSS_AllPlugins);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);

	//Call ProcessSSAEvent with state set to various states that will
	//not cause a transition. Check that:
	//- current state has not changed
	iDiscoverer->ProcessSSAEventL(EStartupStateUndefined);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateCriticalStatic);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateCriticalDynamic);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);

	//Call ProcessDNEventL to indicate that current set of plugins is dirty.
	//Check that:
	//- current state is EDisc_PluginsDirty
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveC);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_PluginsDirty);

	//Call ProcessDNEventL to indicate that a rediscovery should take place.
	//Check that:
	//- current state is EDisc_AllPluginsDisc
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveC);
	iStateAccessor->CompleteNotificationProcessing(*iDiscoverer);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0186
@SYMTestCaseDesc 	Check that SSA related states transition as expected when discovery is all at once
@SYMTestPriority 	High
@SYMTestActions  	Use CDiscoverer::ProcessSSAEventL to start discovery all at once. Check that the
CDiscoverer state is transitioning correctly at every step.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CDiscovererTest::AllAtOnceDiscoveryStateTransitionTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0186 "));
	//After construction check that:
	//- current state is EDisc_NoPluginsDisc
	//- registry is empty. i.e. no plugins discovered
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_NoPluginsDisc);

	//Call ProcessSSAEvent with state set to various states that will
	//not cause a transition. Check that:
	//- current state has not changed
	iDiscoverer->ProcessSSAEventL(EStartupStateUndefined);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_NoPluginsDisc);

	//Call ProcessSSAEventL with EStartupStateNonCritical state. Check that
	//- current state is EDisc_AllPluginsDisc
	//- discoverer observer has processed non-RO plugins
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);
	test(iStateAccessor->GetDiscovererObserverState(*iDiscoverer) == CObserverStub::OSS_AllPlugins);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);

	//Call ProcessSSAEvent with state set to various states that will
	//not cause a transition. Check that:
	//- current state has not changed
	iDiscoverer->ProcessSSAEventL(EStartupStateUndefined);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateCriticalStatic);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateCriticalDynamic);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);

	//Call ProcessDNEventL to indicate that current set of plugins is dirty.
	//Check that:
	//- current state is EDisc_PluginsDirty
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveC);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_PluginsDirty);

	//Call ProcessDNEventL to indicate that a rediscovery should take place.
	//Check that:
	//- current state is EDisc_AllPluginsDisc
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveC);
	iStateAccessor->CompleteNotificationProcessing(*iDiscoverer);
	test(iStateAccessor->State(*iDiscoverer) == CDiscoverer::EDisc_AllPluginsDisc);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-1796
@SYMTestCaseDesc	    Tests multiple notification processing for
						"INC087110: ECOM rescanning code could miss a drive?" and
						"DEF088454: [PCB] ECOM CDiscoverer::CIdleScanningTimer::RunL() Performs Unnecessary Tasks"
@SYMTestPriority 	    High
@SYMTestActions  	    Create multiple notification request on both single drive and multiple drives.
						Check if the notifications are ONLY processed on corresponding drive(s) by timer.
						Check the notifications are processed properly by timer.
						Check the state of discoverer was set correctly on completion.
@SYMTestExpectedResults The test must not fail.
@SYMDEF                 INC087110, DEF088454
*/
void CDiscovererTest::MultipleNotificationProcessingTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1796 "));
	TBool pending = EFalse;

	// Multiple notification on single drive:
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);
	// Raise multiple notification on C: drive
	for(TInt num = 0; num < 10; ++num)
		{
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveC);
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveC);
		}

	// Check only one pending drive in the list.
	test(iStateAccessor->PendingDriveListCount(*iDiscoverer) == 1);
	// Check the pending drive is C: drive
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveC);
	test(pending);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check if notifications have been processed on all drives.
	test(iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	// Multiple notification on multiple drives:
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);
	// Raise multiple notification on C: drive
	for(TInt num = 0; num < 10; ++num)
		{
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveC);
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveC);
		}
#if defined(__WINSCW__) // X: drive on emulator
	// Raise multiple notification on X: drive
	for(TInt num = 0; num < 10; ++num)
		{
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveX);
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveX);
		}

#else // E: drive on HW
	// Raise multiple notification on E: drive
	for(TInt num = 0; num < 10; ++num)
		{
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveE);
		iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveE);
		}
#endif	// __WINSCW__

	// Check only one pending drive in the list.
	test(iStateAccessor->PendingDriveListCount(*iDiscoverer) == 2);

	// Check the pending drive are C: and X: on emulator, or C: and E: on hardware.
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveC);
	test(pending);
#if defined(__WINSCW__) // X: drive on emulator
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveX);
	test(pending);
#else // E: drive on HW
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveE);
	test(pending);
#endif	// __WINSCW__

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check if notifications have been processed on all drives.
	test(iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3519
@SYMTestCaseDesc	    Tests notification processing during SWI for
						PDEF110201: Undesireable interaction between ECOM and SWI
@SYMTestActions  	    Create notification request on  drives.
						Check the notifications are not processed by timer if SWI in progress.
						Check that timer processes pending notifications after SWI completes
@SYMTestExpectedResults The test must not fail.
@SYMDEF                 PDEF110201
*/
void CDiscovererTest::SWINotificationProcessingTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3519 "));

	//Begin SWI
	iStateAccessor->SwiChangeNotificationL(*iDiscoverer,ESASwisInstall);

	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);

	// Raise a notification on C: drive
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveC);
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveC);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check that notifications have NOT been processed as SWI is in progress
	test(!iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	//complete SWI
	iStateAccessor->SwiChangeNotificationL(*iDiscoverer,ESASwisUninstall);


#if defined(__WINSCW__) // X: drive on emulator
	// Raise  notification on X: drive
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveX);
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveX);
#else // E: drive on HW
	// Raise notification on E: drive
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified, EDriveE);
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover, EDriveE);
#endif	// __WINSCW__

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check that notifications have NOT been processed as SWI is not finished
	test(!iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	//finalise SWI
	iStateAccessor->SwiChangeNotificationL(*iDiscoverer,ESASwisNone);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check that notifications have been processed on all drives
	test(iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	//Begin SWI
	iStateAccessor->SwiChangeNotificationL(*iDiscoverer,ESASwisInstall);

	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);

	// Raise a notification
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsModified,EDriveC);
	iDiscoverer->ProcessDNEventL(CDiscoverer::EPluginsRediscover,EDriveC);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check that notifications have NOT been processed as SWI is in progress
	test(!iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	//Simulate an error in reading the P&S Variable - This should reset the
	//SWI state to ESASwisNone in the same way as completing SWI
	iStateAccessor->SwiChangeNotifierRunError(*iDiscoverer,KErrNotFound);

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check that notifications have been processed on all drives
	test(iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));
	}

/**
Call the TDiscoverer_StateAccessor::IdleScanningTimerRunErrorL funtion
passing in a reference to the CDiscoverer object and the error code
to be passed to the CIdleScanningTimer::RunError function
*/
void CDiscovererTest::IdleScanningTimerRunErrorL()
	{
	iStateAccessor->IdleScanningTimerRunErrorL(*iDiscoverer, EEComPanic_CDiscoverer_CIdleScanningTimer_RunError);
	}

/**
Call the TDiscoverer_StateAccessor::IdleScanningTimerRunErrorL funtion
passing in a reference to the CDiscoverer object and the error code
to be passed to the CDirChangeNotifier::RunError function
*/
void CDiscovererTest::DirChangeNotifierRunErrorL()
	{
	iStateAccessor->DirChangeNotifierRunErrorL(*iDiscoverer, EEComPanic_CDiscoverer_CDirChangeNotifier_RunError);
	}

/**
Call the TDiscoverer_StateAccessor::IdleScanningTimerRunErrorL funtion
passing in a reference to the CDiscoverer object and the error code
to be passed to the CSwiChangeNotifier::RunError function
*/
void CDiscovererTest::SwiChangeNotifierRunError()
	{
	iStateAccessor->SwiChangeNotifierRunError(*iDiscoverer, KErrNoMemory);
	}
/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3172
@SYMTestCaseDesc	    Tests language switch notification processing for
						"CR0902: Enable Dynamic Language Switching in APPARC, ECOM and EIKSTD"
@SYMTestPriority 	    High
@SYMTestActions  	    Call ProcessSSAEventL with EStartupStateNonCritical state.
						Call DriveMountedL to build up drives in system.
						Call LanguageChangedL to raise multiple notification on all drives in system.
						Check if the notifications are processed on all drive(s) by timer.
						Check the notifications are processed properly by timer.
						Check the state of discoverer was set correctly on completion.
@SYMTestExpectedResults The test must not fail.
@SYMDEF                 CR0902
*/
void CDiscovererTest::LanguageChangedNotificationTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3172 "));
	TBool pending = EFalse;

	//Call ProcessSSAEventL with EStartupStateNonCritical state
	iDiscoverer->ProcessSSAEventL(EStartupStateNonCritical);

	if(iStateAccessor->IsDriveMounted(*iDiscoverer, EDriveK))
		{
		TRAPD(err, iStateAccessor->DriveUnmountedL(*iDiscoverer, EDriveK));
		TEST_OOM_ERR;
		test(err == KErrNone);
		}

	// Mount drives on system
	TRAPD(err, iStateAccessor->DriveMountedL(*iDiscoverer, EDriveZ));
	TEST_OOM_ERR;
	test(err == KErrNone);

	TRAP(err, iStateAccessor->DriveMountedL(*iDiscoverer, EDriveC));
	TEST_OOM_ERR;
	test(err == KErrNone);

#if defined(__WINSCW__) // X: drive on emulator
	TRAP(err, iStateAccessor->DriveMountedL(*iDiscoverer, EDriveX));
	TEST_OOM_ERR;
	test(err == KErrNone);
#else // E: drive on HW
	TRAP(err, iStateAccessor->DriveMountedL(*iDiscoverer, EDriveE));
	TEST_OOM_ERR;
	test(err == KErrNone);
#endif

	// Multiple notification on all drives:
	iStateAccessor->LanguageChangedL(*iDiscoverer);

	// Check three pending drive in the list.
	test(iStateAccessor->PendingDriveListCount(*iDiscoverer) == 3);
	// Check the pending drive is Z: drive
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveZ);
	test(pending);

	// Check the pending drive is C: drive
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveC);
	test(pending);

#if defined(__WINSCW__) // X: drive on emulator
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveX);
	test(pending);
#else // E: drive on HW
	pending = iStateAccessor->IsScanDirectoryPending(*iDiscoverer, EDriveE);
	test(pending);
#endif	// __WINSCW__

	// Start Active Scheduler and shut it down in 3 secs, it will launch RunL of timer to process
	// all pending notification.
	iShutdown.StartAfter(KOneSecond * 3);
	CActiveScheduler::Start();

	// Check if notifications have been processed on all drives.
	test(iStateAccessor->IsScanDirectoryComplete(*iDiscoverer));

	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0723
@SYMTestCaseDesc	    Tests the creation and deletion of CDiscoverer
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete CDiscoverer object,checks for open handles
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDeleteTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0723 CreateDeleteTestL "));
	//
	// Tests the Creating and deletion of CDiscoverer
	// ------------------------------------------------------------------
	//
	// Set up for heap leak checking
	__UHEAP_MARK;

	//Check Thread handles leak
	TInt startProcessHandleCount = 0;
	TInt startThreadHandleCount = 0;
	TInt endProcessHandleCount = 0;
	TInt endThreadHandleCount = 0;

	RThread rThread;
	rThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//START CREATE DELETE TEST//

	CDiscovererTest* discTest = CDiscovererTest::NewL();

	test(discTest != NULL);

	delete discTest;

	//END CREATE DELETE TEST//

	// Check for open handles
	rThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	test(startThreadHandleCount == endThreadHandleCount);

	//Test ends
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0724
@SYMTestCaseDesc	    OOM test for create and delete of CDiscoverer.
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete CDiscoverer object,checks for any memory leak.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void OOMCreateDeleteTest()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0724 OOM CreateDeleteTest "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CDiscovererTest* discTest = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);

		TRAP(err, discTest = CDiscovererTest::NewL());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete discTest;
		discTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}
	while(err == KErrNoMemory);

	test.Printf(_L("- Succeeded at heap failure rate of %i\n"), failAt);
	test(err == KErrNone);
	}

// Type definition for pointer to member function.
// Used in calling the CDiscovererTest member function for testing.
typedef void (CDiscovererTest::*ClassFuncPtrL) (void);

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0725
@SYMTestCaseDesc	    Function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up test function of CDiscovererTest.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions

@param		testFunc pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0725 "));
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CDiscovererTest* discTest = CDiscovererTest::NewL();
	CleanupStack::PushL(discTest);

	(discTest->*testFuncL)();

	CleanupStack::PopAndDestroy(discTest);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
Utility function to continually invoke a test function and cause memory allocation failures

@param		testFuncL pointer to OOM test function
@param	 	tryCount specifies what value to start memory allocation failures at
@param		increment how much to increase the point at which memory allocation will fail on each test attempt
@param		stopCount the value of memory allocation failure to stop testing at
*/
TInt RunTestUnderOOMCondition(ClassFuncPtrL testFuncL, TInt tryCount, TInt increment, TInt stopCount)
{
	TInt err = KErrNone;

	do
		{

		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CDiscovererTest* discTest = CDiscovererTest::NewL();
		CleanupStack::PushL(discTest);


		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);
		TRAP(err, (discTest->*testFuncL)());
		__UHEAP_SETFAIL(RHeap::ENone, 0);


		CleanupStack::PopAndDestroy(discTest);
		discTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;

	 	tryCount = tryCount + increment;
		} while((err == KErrNoMemory) && (tryCount != stopCount));

		tryCount = tryCount - increment;


		if (err == KErrNoMemory)
		{
			// test has not yet been able to pass due to memory allocation failures.
			return -1;
		}

		test(err == KErrNone);

	 	// If enough memory has finally been allocated for the test to pass then return
		// the memory allocation counter value.
		test.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
		return tryCount;
}

/**
Wrapper function to call all OOM test functions

@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	TInt startCount = 1;
	TInt increment = 1;
	TInt successRate = -1;
	TInt stopCount = 256;

 	successRate = RunTestUnderOOMCondition(testFuncL, startCount, increment, stopCount);

 	// (INC115057)
 	// When method CDiscoverer::CDirScanner::DoScanDriveL is encountered in a test hundreds
 	// of plug-in files will be scanned. It takes in the order of 3-4000 memory allocation failure
 	// loops (in techview context) before the scan is finished and the test can complete successfully.
 	// This will take over an hour. Instead determine the approximate failure point by
 	// testing with large increments between failures. Once a failure rate interval is found test
 	// for OOM conditions running up to it.
 	// This in effect means that we are checking OOM at the start and end of tests but skipping
 	// the scanning of each and every plugin in the middle of the test.
 	// (Note that CDiscoverer::CDirScanner::DoScanDriveL may return without leaving when it
 	// can't allocate TFileName. In this case it seems to this function that the test is
 	// successful. Therefore even if the successRate above indicates a pass the test is still
 	// re-run below with larger memory allocation failure valued to make sure that the test does
 	// in fact run to completion.

 		startCount = 256;
 		increment = 256;
 		stopCount = -1;
 		successRate = RunTestUnderOOMCondition(testFuncL, startCount, increment, stopCount);
 		test(successRate > 0);

 		if (successRate > 256)
 		{
 			startCount = successRate - 256;
 			increment = 1;
 			stopCount = -1;
 			successRate = RunTestUnderOOMCondition(testFuncL, startCount, increment, stopCount);
 		}

 	test(successRate > 0);
	}

/**
Creates and installs active scheduler for this thread and calls
CDiscovererTest::IdleScanningTimerRunErrorL

@param		aDiscTest The CDiscovererTest object used to carry out the test
*/
LOCAL_C	void DoIdleScanningTimerRunErrorTestL(CDiscovererTest* aDiscTest)
	{

	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);

	CActiveScheduler::Install(scheduler);

	aDiscTest->IdleScanningTimerRunErrorL();

	// Cleanup CDiscovererTest, TheFs and scheduler
	CleanupStack::PopAndDestroy(scheduler);

	}
/**
Thread entry point for the test thread.  Creates a CTrapCleanup and
calls  DoIdleScanningTimerRunErrorTestL to carry out the test

@param		aDiscTest The CDiscovererTest object used to carry out the test
*/
LOCAL_C TInt IdleScanningTimerRunErrorThreadEntry(TAny* aDiscTest)
	{

	CTrapCleanup* tc = CTrapCleanup::New();

	CDiscovererTest *discTest = static_cast<CDiscovererTest*>(aDiscTest);

	TRAPD(err,DoIdleScanningTimerRunErrorTestL(discTest));

	delete tc;

	return err;

	}


/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3165
@SYMTestCaseDesc 	Check that the CIdleScanningTimer::RunError() works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Create a new thread which will call RunError.  Wait for the
					thread to exit and check the thread exit type and reason
					to verify behaviour
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF094675
*/
LOCAL_C void IdleScanningTimer_RunErrorTest()
	{
	__UHEAP_MARK;

	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3165 CIdleScanningTimer RunError test "));

	_LIT(KStartThreadName,"CIdleScanningTimer RunError Thread");

	//Disable JIT so that the Panic doesn't bring up a dialog
	//and stop the test
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	//Create a CDiscovererTest object to pass into the test thread
	CDiscovererTest* discTest = CDiscovererTest::NewL();
	CleanupStack::PushL(discTest);

	//Create a new thread to run the test
	RThread testThread;
	testThread.Create(KStartThreadName, IdleScanningTimerRunErrorThreadEntry,
					KDefaultStackSize,KMinHeapSize,KMinHeapSize,discTest);
	TRequestStatus status;
	testThread.Logon(status);


	testThread.Resume();

	//Wait for the thread to exit
	User::WaitForRequest(status);

	//Obtain exit type and reason for test thread
	TExitType exitType = testThread.ExitType();
	TInt exitReason = testThread.ExitReason();

	//close the thread handle
	testThread.Close();

	CleanupStack::PopAndDestroy(discTest);

	//Set JIT back to original state
	User::SetJustInTime(jitEnabled);

	//Verify the exit reason and exit code
	test(exitType == EExitPanic);
	test(exitReason == EEComPanic_CDiscoverer_CIdleScanningTimer_RunError);

	__UHEAP_MARKEND;
	}

/**
Creates and installs active scheduler for this thread and calls
CDiscovererTest::DirChangeNotifierRunErrorL

@param		aDiscTest The CDiscovererTest object used to carry out the test
*/
LOCAL_C void DoDirChangeNotifierRunErrorTestL(CDiscovererTest* aDiscTest)
	{

	// create and install the active scheduler we need
	CActiveScheduler* scheduler=new(ELeave) CActiveScheduler;
	CleanupStack::PushL(scheduler);

	CActiveScheduler::Install(scheduler);

	//call the RunErrorL method which should panic
	aDiscTest->DirChangeNotifierRunErrorL();

	// Cleanup CDiscovererTest, TheFs and scheduler
	CleanupStack::PopAndDestroy(scheduler);

	}

/**
Thread entry point for the test thread.  Creates a CTrapCleanup and
calls  DoDirChangeNotifierRunErrorTestL to carry out the test

@param		aDiscTest The CDiscovererTest object used to carry out the test
*/
LOCAL_C TInt DirChangeNotifierRunErrorThreadEntry(TAny* aDiscTest)
	{

	CTrapCleanup* tc = CTrapCleanup::New();

	CDiscovererTest *discTest = static_cast<CDiscovererTest*>(aDiscTest);

	TRAPD(err,DoDirChangeNotifierRunErrorTestL(discTest));

	delete tc;

	return err;

	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3166
@SYMTestCaseDesc 	Check that the CDirChangeNotifier::RunError() works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Create a new thread which will call RunError.  Wait for the
					thread to exit and check the thread exit type and reason
					to verify behaviour
@SYMTestExpectedResults The test must not fail.
@SYMDEF DEF094675
*/
LOCAL_C void DirChangeNotifier_RunErrorTest()
	{
	__UHEAP_MARK;

	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3166 CDirChangeNotifier RunError test "));

	_LIT(KStartThreadName,"CDirChangeNotifier RunError Thread");

	//Disable JIT so that the Panic doesn't bring up a dialog
	//and stop the test
	TBool jitEnabled = User::JustInTime();
	User::SetJustInTime(EFalse);

	//Create a CDiscovererTest object to pass into the test thread
	CDiscovererTest* discTest = CDiscovererTest::NewL();
	CleanupStack::PushL(discTest);

	//Create a new thread to run the test
	RThread testThread;
	testThread.Create(KStartThreadName, DirChangeNotifierRunErrorThreadEntry,
					KDefaultStackSize,KMinHeapSize,KMinHeapSize,discTest);
	TRequestStatus status;
	testThread.Logon(status);
	testThread.Resume();

	//Wait for the thread to exit
	User::WaitForRequest(status);

	//Obtain exit type and reason for test thread
	TExitType exitType = testThread.ExitType();
	TInt exitReason = testThread.ExitReason();

	//close the thread handle
	testThread.Close();

	CleanupStack::PopAndDestroy(discTest);

	//Set JIT back to original state
	User::SetJustInTime(jitEnabled);

	//Verify the exit reason and exit code
	test(exitType == EExitPanic);
	test(exitReason == EEComPanic_CDiscoverer_CDirChangeNotifier_RunError);

	__UHEAP_MARKEND;
	}

TInt DoTestsL()
	{
	__UHEAP_MARK;

	// Basic tests
	CreateDeleteTestL();
	DoBasicTestL(&CDiscovererTest::ResumeSuspendTestL, _L("ResumeSuspendTestL"));
	DoBasicTestL(&CDiscovererTest::DriveMountUnmountTestL, _L("DriveMountUnmountTestL"));
	DoBasicTestL(&CDiscovererTest::ProcessEntryTestL, _L("ProcessEntryTestL"));
	DoBasicTestL(&CDiscovererTest::ValidateEntryTestL, _L("ValidateEntryTestL"));
	DoBasicTestL(&CDiscovererTest::ProcessSpiEntryTestL, _L("ProcessSpiEntryTestL"));
	DoBasicTestL(&CDiscovererTest::ValidateSpiEntryTestL, _L("ValidateSpiEntryTestL"));
	DoBasicTestL(&CDiscovererTest::ProcessEntryPlugIn3TestL, _L("ProcessEntryPlugIn3TestL"));
	DoBasicTestL(&CDiscovererTest::ValidateEntryPlugIn3TestL, _L("ValidateEntryPlugIn3TestL"));
	DoBasicTestL(&CDiscovererTest::ProcessSpiEntryPlugIn3TestL, _L("ProcessSpiEntryPlugIn3TestL"));
	DoBasicTestL(&CDiscovererTest::ValidateSpiEntryPlugIn3TestL, _L("ValidateSpiEntryPlugIn3TestL"));
	DoBasicTestL(&CDiscovererTest::ValidateSpiPluginsTestL, _L("ValidateSpiPluginsTestL"));
	DoBasicTestL(&CDiscovererTest::ScanDirectoryIncrementTestL, _L("ScanDirectoryIncrementTestL"));
	DoBasicTestL(&CDiscovererTest::StagedDiscoveryStateTransitionTestL, _L("StagedDiscoveryStateTransitionTestL"));
	DoBasicTestL(&CDiscovererTest::AllAtOnceDiscoveryStateTransitionTestL, _L("AllAtOnceDiscoveryStateTransitionTestL"));
	DoBasicTestL(&CDiscovererTest::ScanDirectoryTestL, _L("ScanDirectoryTestL"));
	DoBasicTestL(&CDiscovererTest::MultipleNotificationProcessingTestL, _L("MultipleNotificationProcessingTestL"));
	DoBasicTestL(&CDiscovererTest::LanguageChangedNotificationTestL, _L("MLanguageChangedNotificationTestL"));
	DoBasicTestL(&CDiscovererTest::SWINotificationProcessingTestL, _L("SWINotificationProcessingTestL"));

	//RunError tests
	IdleScanningTimer_RunErrorTest();
	DirChangeNotifier_RunErrorTest();

	// OOM tests
	OOMCreateDeleteTest();
	// This test is not performed because the error from the final memory allocation failures are not
	// propagated back to the calling function and cannot be dealt with. Therefore
	// this test does not complete. However the normal test is performed above.
	//DoOOMTestL(&CDiscovererTest::ResumeSuspendTestL, _L("OOM ResumeSuspendTestL"));
	DoOOMTestL(&CDiscovererTest::DriveMountUnmountTestL, _L("OOM DriveMountUnmountTestL"));
	DoOOMTestL(&CDiscovererTest::ProcessEntryTestL, _L("OOM ProcessEntryTestL"));
	DoOOMTestL(&CDiscovererTest::ProcessSpiEntryTestL, _L("OOM ProcessSpiEntryTestL"));
	DoOOMTestL(&CDiscovererTest::ValidateSpiEntryTestL, _L("OOM ValidateSpiEntryTestL"));
	DoOOMTestL(&CDiscovererTest::ValidateEntryTestL, _L("OOM ValidateEntryTestL"));
	DoOOMTestL(&CDiscovererTest::ProcessEntryPlugIn3TestL, _L("OOM ProcessEntryPlugIn3TestL"));
	DoOOMTestL(&CDiscovererTest::ProcessSpiEntryPlugIn3TestL, _L("OOM ProcessSpiEntryPlugIn3TestL"));
	DoOOMTestL(&CDiscovererTest::ValidateSpiEntryPlugIn3TestL, _L("OOM ValidateSpiEntryPlugIn3TestL"));
	DoOOMTestL(&CDiscovererTest::ValidateEntryPlugIn3TestL, _L("OOM ValidateEntryPlugIn3TestL"));
	DoOOMTestL(&CDiscovererTest::ScanDirectoryIncrementTestL, _L("OOM ScanDirectoryIncrementTestL"));
	DoOOMTestL(&CDiscovererTest::StagedDiscoveryStateTransitionTestL, _L("StagedDiscoveryStateTransitionTestL"));
	DoOOMTestL(&CDiscovererTest::AllAtOnceDiscoveryStateTransitionTestL, _L("AllAtOnceDiscoveryStateTransitionTestL"));
	DoOOMTestL(&CDiscovererTest::ScanDirectoryTestL, _L("OOM ScanDirectoryTestL"));
	// This test is not performed because the error from the final memory allocation failures are not
	// propagated back to the calling function and cannot be dealt with. Therefore
	// this test does not complete. However the normal test is performed above.
	//DoOOMTestL(&CDiscovererTest::MultipleNotificationProcessingTestL, _L("MultipleNotificationProcessingTestL"));
	// This test is not performed because the error from the final memory allocation failures are not
	// propagated back to the calling function and cannot be dealt with. Therefore
	// this test does not complete. However the normal test is performed above.
	//DoOOMTestL(&CDiscovererTest::SWINotificationProcessingTestL, _L("OOM SWINotificationProcessingTestL"));

	__UHEAP_MARKEND;
	return KErrNone;
	}

// Copy the Plugins to specific folder for testing purpose
LOCAL_C void CopyPlugins()
	{
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManCopyFileL(KNewResourceFileNameOnZ, KNewResourceFileName));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KNewExampleDllFileNameOnZ, KNewExampleDllFileName));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KPlugIn3ResourceFileNameOnZ, KPlugIn3ResourceFileName));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KPlugIn3ExampleDllFileNameOnZ, KPlugIn3ExampleDllFileName));
	test(err==KErrNone);
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteTestPlugin()
	{
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManDeleteFileL(KNewResourceFileName));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KNewExampleDllFileName));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KPlugIn3ResourceFileName));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KPlugIn3ExampleDllFileName));
	}

//
//Initialise the Active Scheduler
//
LOCAL_C void SetupL()
	{
	// Construct and install the Active Scheduler. The Active Schedular is needed
	// by components used by this test as they are ActiveObjects.
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start(_L("Discoverer Tests."));

	TheTrapCleanup = CTrapCleanup::New();

	TRAPD(err, SetupL());
	test(err == KErrNone);

	// Connect the file server instance
	User::LeaveIfError(TheFs.Connect());

	CopyPlugins();

	// The reason for the folowing delay is:
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that an active object,
	// which scans plugin directories. So the discovering service is asynchronous.
	// We have to wait some time until it finishes.
	// Otherwise ListImplementationsL could fail to find requested implementations.
	User::After(KOneSecond * 3);


	// Call the main tests
	TRAP(err, DoTestsL());
	test(err == KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugin();

	TheFs.Close();

	delete TheActiveScheduler;
	delete TheTrapCleanup;

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}

