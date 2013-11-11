// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// to test CDiscoverer will not discover plug-ins on C: and E:
// if the two drives are disabled by the patchable constants.
// 
//

/**
 @file
 @internalComponent
*/
#include "RegistryData.h"
#include "Discoverer.h"
#include "ImplementationInformation.h"
#include "RegistryData.h"
#include "Registrar.h"
#include "RegistrarObserver.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include "DriveInfo.h"
#include "EComPatchDataConstantv2.h"

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <startup.hrh>

_LIT (KDataDir, "C:\\private\\10009D8F\\ECom*");

// plugins to discover
_LIT(KNewResourceFileNameC, "C:\\resource\\plugins\\EComExample5.rsc");
_LIT(KNewDllFileNameC, "C:\\sys\\bin\\EComExample5.dll");
_LIT(KNewResourceFileNameZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KNewDllFileNameZ,"z:\\RAMOnly\\EComExample5.dll");

const TInt KOneSecond = 1000000;

LOCAL_D RTest test(_L("Disable drives"));
LOCAL_D RFs					TheFs;
LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;

class CDerivedActiveScheduler;
LOCAL_D CDerivedActiveScheduler* TheActiveScheduler	= NULL;

LOCAL_D TInt oomStep;

//It is used by some test methods which are called two times:
//from normal test and from OOM test.
static void LeaveIfErrNoMemoryL(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		User::Leave(aError);
		}
	}

// Copies the Plugins to specific folder for testing purpose
LOCAL_C void CopyPlugins()
	{
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManCopyFileL(KNewResourceFileNameZ, KNewResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KNewDllFileNameZ, KNewDllFileNameC));
	test(err==KErrNone);
	}

// Deleting plugin from the RAM for cleanup purpose
LOCAL_C void DeleteTestPlugin(TAny* /* aUnused */)
	{
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManDeleteFileL(KNewResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KNewDllFileNameC));
	}

// flags indicating a set of pre-conditions to fullfil before running
// CDisableDrivesTest.
enum TPreTestSetup
	{
	EPreTest_UnitializeCachedDriveInfo = 0x1,
	EPreTest_EnableAllDrives = 0x2,
	EPreTest_CopyPlugins = 0x4
	};

const TUint32 KStartupDiscoveryDrivesDisabledMask =
	EPreTest_UnitializeCachedDriveInfo | EPreTest_CopyPlugins;

const TUint32 KStartupDiscoveryDrivesEnabledMask =
	EPreTest_EnableAllDrives | EPreTest_CopyPlugins;

const TUint32 KRediscoveryDrivesDisabledMask =
	EPreTest_UnitializeCachedDriveInfo;

const TUint32 KRediscoveryDrivesEnabledMask =
	EPreTest_EnableAllDrives;

/**
TRegistryData_StateAccessor class allows access to private and protected
members of production code class CRegistryData, as its a friend class.
*/
class TRegistryData_StateAccessor
	{
public:
	TInt FindImplementation(CRegistryData& aRegistryData,
				const TUid aImplUid,
				const TUid aInterfaceUid,
				CRegistryData::CImplementationData*& aImplData) const;
	};

/** TRegistrar_StateAccessor class allows access to private members
of CRegistrar. */
class TRegistrar_StateAccessor
	{
	static void SetCompletedStateL(CRegistrar& aRegistrar,
								   TStartupStateIdentifier aKnownState);
	};

/** TDiscoverer_StateAccessor allows manipulation of CDiscoverer
private members. */
class TDiscoverer_StateAccessor
	{
	static void SetCompletedStateL(CDiscoverer& aDiscoverer,
								   CDiscoverer::TDiscovererState aState);
	};

/** TDriveInfo_StateAccessor class allows access to private members
of CEComCachedDriveInfo. */
class TDriveInfo_StateAccessor
	{
public:
	static void ClearCachedDriveInfo();

	static void SetDriveDisableMaskL(TUint32 aMask);
	};

/**
@return		KErrNone if found otherwise KErrNotFound
@param		aRegistryData The CRegistryData class object
@param		aImplUid The implementation to find.
@param		aInterfaceUid If greater than 0 the interface associated with the
			implementation to find.
@param		aImplData The found implementation data.
@pre 		CRegistrar is fully constructed
*/
TInt TRegistryData_StateAccessor::FindImplementation(CRegistryData& aRegistryData,
												const TUid aImplUid,
												const TUid aInterfaceUid,
												CRegistryData::CImplementationData*& aImplData) const
	{
	return aRegistryData.FindImplementation(aImplUid, aInterfaceUid, aImplData);
	}

/** Mark the static drive info array uninitialized. Then the
next instantiation of CEComCachedDriveInfo will need to setup
the drive info again.
*/
void TDriveInfo_StateAccessor::ClearCachedDriveInfo()
	{
	CEComCachedDriveInfo::iInitialized = EFalse;
	}

/** Instantiate a CEComCachedDriveInfo object with a special drive
mask. The object is destroyed right away but the static drive info
array is fixed to the special mask.
@param aMask the drive disable mask to set.
*/
void TDriveInfo_StateAccessor::SetDriveDisableMaskL(TUint32 aMask)
	{
	// Set this bool to false otherwise ConstructL will do nothing.
	CEComCachedDriveInfo::iInitialized = EFalse;

	CEComCachedDriveInfo* ptr = new (ELeave) CEComCachedDriveInfo();
	CleanupStack::PushL(ptr);
	ptr->ConstructL(TheFs, aMask);
	CleanupStack::PopAndDestroy();
	}

/** Need a CActive to wait for CDiscoverer dir change notifiers. */
class CSimpleTimer : public CTimer
	{
public:
	inline CSimpleTimer(TInt aPriority);
	inline void ConstructL();
	inline void StartTimer(TTimeIntervalMicroSeconds32 aTimeInterval);

private:
	void RunL();
	};

inline CSimpleTimer::CSimpleTimer(TInt aPriority)
	: CTimer(aPriority)
	{
	CActiveScheduler::Add(this);
	}

inline void CSimpleTimer::ConstructL()
	{
	CTimer::ConstructL();
	}

inline void CSimpleTimer::StartTimer(TTimeIntervalMicroSeconds32 aTimeInterval)
	{
	After(aTimeInterval);
	}

void CSimpleTimer::RunL()
	{
	CActiveScheduler::Stop();
	}

/** Avoid E32User::Case 47 panic in OOM test */
class CDerivedActiveScheduler : public CActiveScheduler
	{
public:
	virtual void Error(TInt aError) const;
	};

void CDerivedActiveScheduler::Error(TInt aError) const
	{
	Halt(aError);
	}

/**
The implementation of the abstract Registrar Observer class,
used for recieving notifications of registry changes.
Stub class(for CEComServer) used for the creation of CRegistrar class object.
CEComServer class acts as observer for CRegistrar.
*/
class CTestRegistrarObserver : public MRegistrarObserver // codescanner::missingcclass
	{
public:
	// This function is used by RegistrarObserver (i.e.CEComServer) to notify its
	// clients(REComSession objects) that some change has happened in Registry.
	// Here we have no clients to notify, so no implementaion.
	void Notification( TInt /*aNotification*/ ) {}
	};

/**
Test class for object CRegistrar.
This class provides the parameters and behaviour that
allows this class to behave normally under a test
scenario.
*/
class CDisableDrivesTest : public CBase
	{
public:
	static CDisableDrivesTest* NewL(TBool aDrivesEnabled);
	virtual ~CDisableDrivesTest();
	void startupDiscoveryL();
	void RediscoveryL();

private:
	CDisableDrivesTest(TBool aDrivesEnabled);
	void ConstructL();
	void WaitForDiscovererAOL();

public:
	/** The instance of the class under test */
	CRegistrar* iRegistrar;

	/** The instance of the State Accessor class */
	TRegistrar_StateAccessor* iStateAccessor;

	/** The registry data instance required to construct a CRegistrar object */
	CRegistryData* iRegistryData;

	/** The instance of the Registry Data State Accessor class */
	TRegistryData_StateAccessor* iRegistryDataStateAccessor;

	/** The instance of the observer of CRegistrar class */
	MRegistrarObserver* iRegistrarObserver;

	/** The destination for the data discovered during a parse */
	CRegistryData::CDllData* iDllData;

	/** Unique Id of the ECOM dll */
	TUid    iDllUid;

	/** Unique Id of an interface implementation */
	TUid	iImplementationUid;

	/** Unique Id of an interface */
	TUid	iInterfaceUid;

	/** The drive on which interface implementations can be found */
	TDriveUnit	iDriveUnit;

	/** Ecom plugin which contains interface implementations. Used in Registration APIs.*/
	TEntry		iDllEntry;

	TBool		iDrivesEnabled;
	};

/**
Standardised safe construction which
leaves nothing on the cleanup stack.

@post		CDisableDrivesTest is fully constructed and initialised.
*/
CDisableDrivesTest* CDisableDrivesTest::NewL(TBool aDrivesEnabled)
	{
	CDisableDrivesTest* self = new (ELeave) CDisableDrivesTest(aDrivesEnabled);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/**
Standardized default c'tor

@post		CDisableDrivesTest is fully constructed.
*/
CDisableDrivesTest::CDisableDrivesTest(TBool aDrivesEnabled)
:	CBase(),
	iDriveUnit(EDriveC),
	iDrivesEnabled(aDrivesEnabled)
	{
	iDllUid.iUid			= 0x101F847B; // Dlluid for Ecom plugin EComExample5.dll
	iInterfaceUid.iUid		= 0x10009DC0; // Interface uid for interface contained in above plugin
	iImplementationUid.iUid = 0x101f847C; // Implementaion uid for above interface
	TUid uid1 = {0};
	TUid uid2 = {0};
	iDllEntry.iType = TUidType(uid1, uid2, iDllUid);//Setting Dlluid to plugin entry
	}

/**
Destructor.

@post		This object is properly destroyed.
*/
CDisableDrivesTest::~CDisableDrivesTest()
	{
	delete iDllData;
	delete iRegistrar;
	delete iRegistryData;
	delete iRegistrarObserver;
	delete iStateAccessor;
	delete iRegistryDataStateAccessor;
	}

/**
Standardized 2nd(Initialization) phase of two phase construction.

@post		CDisableDrivesTest is fully constructed.
*/
void CDisableDrivesTest::ConstructL()
	{
	CRegistryData::CDriveData* driveData=NULL;
	iRegistrarObserver = new (ELeave) CTestRegistrarObserver;
	iStateAccessor	   = new (ELeave) TRegistrar_StateAccessor;
	iRegistryDataStateAccessor	   = new (ELeave) TRegistryData_StateAccessor;
	iRegistryData	   = CRegistryData::NewL(TheFs);
	iRegistrar		   = CRegistrar::NewL(*iRegistryData, *iRegistrarObserver, TheFs);
	iDllData		   = CRegistryData::CDllData::NewLC(driveData);
	CleanupStack::Pop(iDllData);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3541
@SYMTestCaseDesc 	Verify ECOM's patchdata scheme works.
@SYMTestPriority 	High
@SYMTestActions  	Check the value of the constant KDiscoveryDisabledDriveList.
@SYMTestExpectedResults It should be 0x14 in hw.
@SYMCR CR1049
*/
void PatchableConstantTest()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3541 Patch constant value "));

	// check the patchable constant has the patched value.
#ifdef __ARMCC__
	test(KDiscoveryDisabledDriveList == 0x14);
#else
	// On emulator the constant is not patchable. Thus the RTest links
	// with a different .cpp file.
	test(KDiscoveryDisabledDriveList == 0x00800004);
#endif
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-4014
@SYMTestCaseDesc 	Check that the patchables for custom resolver caching
    can be patched.
@SYMTestPriority 	High
@SYMTestActions  	Check the values of KCustomResolverCacheSize and
	KCustomResolverCacheTimeout
@SYMTestExpectedResults disabledrivestest.hby sets KCustomResolverCacheSize
	to 1182 and KCustomResolverCacheTimeout to 4001182. Note that this test
	only runs on hw.
@SYMCR CR1182
*/
void CR1182PatchableConstantTest()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-4014 CR1182 patchable constants "));

	// check the patchable constants have the patched values.
#ifdef __ARMCC__
	test(KCustomResolverCacheSize == 1182);
	test(KCustomResolverCacheTimeout == 4001182);
#else
	test.Printf(_L("This test only runs in armv5."));
#endif
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3542
@SYMTestCaseDesc 	Check that CDiscoverer will not discover plug-ins in disabled drives at boot time.
@SYMTestPriority 	High
@SYMTestActions  	Use CRegistrar::ProcessSSAEventL to trigger boot up discovery.
@SYMTestExpectedResults No plugins registered if C: drive is disabled. In the
	control test, C: is enabled and the plugin is registered.
@SYMCR CR1049
*/
void CDisableDrivesTest::startupDiscoveryL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3542 "));

	// Do not scan ReadOnly internal drive in OOM. Will take an hour.
	__UHEAP_SETFAIL(RHeap::ENone, 0);

	iRegistrar->ProcessSSAEventL(EStartupStateCriticalStatic);

	if (oomStep)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, oomStep);
		}

	iRegistrar->ProcessSSAEventL(EStartupStateNonCritical);

	CRegistryData::CImplementationData *implementationData=NULL;
	TUid dummyUid;
	dummyUid.iUid = 0;

	TInt err = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, dummyUid, implementationData);
	::LeaveIfErrNoMemoryL(err);

	if (iDrivesEnabled)
		{
		test(err == KErrNone);
		}
	else
		{
		test(err == KErrNotFound);
		}
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-3543
@SYMTestCaseDesc 	Check that CDiscoverer will not discover plug-ins via dir
	change notification if drive is disabled.
@SYMTestPriority 	High
@SYMTestActions  	After startup discovery completed, copy .rsc to
	C:\resource\plugins and the .dll to C:\sys\bin.
@SYMTestExpectedResults The plugin is not registered if C: is disabled. In the
	control test, C: is enabled and the plugin is registered.
@SYMCR CR1049
*/
void CDisableDrivesTest::RediscoveryL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3543 "));
	// Do not scan ReadOnly internal drive in OOM. Will take an hour.
	__UHEAP_SETFAIL(RHeap::ENone, 0);

	iRegistrar->ProcessSSAEventL(EStartupStateNonCritical);

	if (oomStep)
		{
		__UHEAP_SETFAIL(RHeap::EDeterministic, oomStep);
		}

	CopyPlugins();
	CleanupStack::PushL(TCleanupItem(DeleteTestPlugin, NULL));

	WaitForDiscovererAOL();

	CRegistryData::CImplementationData *implementationData=NULL;
	TUid dummyUid = KNullUid;

	TInt err = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, dummyUid, implementationData);
	::LeaveIfErrNoMemoryL(err);

	CleanupStack::PopAndDestroy(1);

	if (iDrivesEnabled)
		{
		test(err == KErrNone);
		}
	else
		{
		test(err == KErrNotFound);
		}
	}

void CDisableDrivesTest::WaitForDiscovererAOL()
	{
	// Yield priority to CDiscoverer's AO
	CSimpleTimer* timer = new(ELeave) CSimpleTimer(CActive::EPriorityLow);
	CleanupStack::PushL(timer);
	timer->ConstructL();
	timer->StartTimer(KOneSecond * 2);
	CActiveScheduler::Start();
	CleanupStack::PopAndDestroy(timer);
	}

typedef void (CDisableDrivesTest::*ClassFuncPtrL) (void);

/**
Wrapper function to call all test functions

@param	aTestFuncL pointer to test function
@param	aUseZeroMask if false, let the test use the patchable constant else
	initialize the static drive info array with zero, i.e. enabled all.
@param	aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL aTestFuncL, const TUint32 aTaskMask, const TDesC& aTestDesc)
	{
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	// A series of tasks to perform before calling the test method.

	// Delete the previous data files to ensure rediscover from scratch
	TRAP_IGNORE(EComTestUtils::FileManDeleteFileL(KDataDir));

	// The two flags below are mutually exclusive.
	if (aTaskMask & EPreTest_UnitializeCachedDriveInfo)
		{
		TDriveInfo_StateAccessor::ClearCachedDriveInfo();
		}
	else if (aTaskMask & EPreTest_EnableAllDrives)
		{
		TDriveInfo_StateAccessor::SetDriveDisableMaskL(0);
		}

	if (aTaskMask & EPreTest_CopyPlugins)
		{
		CopyPlugins();
		User::After(KOneSecond);
		CleanupStack::PushL(TCleanupItem(DeleteTestPlugin, NULL));
		}

	// All set to start the test
	TBool drivesEnabled = (aTaskMask & EPreTest_EnableAllDrives) != 0;
	CDisableDrivesTest* disableDrvTest = CDisableDrivesTest::NewL(drivesEnabled);
	CleanupStack::PushL(disableDrvTest);

	(disableDrvTest->*aTestFuncL)();

	CleanupStack::PopAndDestroy(disableDrvTest);

	if (aTaskMask & EPreTest_CopyPlugins)
		{
		CleanupStack::PopAndDestroy(1);
		}

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
Wrapper function to call all OOM test functions

@param aTestFuncL pointer to OOM test function
@param aTaskMask  indicates tasks need to be done before entering
	the OOM loop.
@param	aTestDesc test function name
*/
//LOCAL_C void DoOOMTestL(ClassFuncPtrL aTestFuncL,
//						const TUint32 aTaskMask,
//						const TDesC& aTestDesc)
//	{
//	test.Next(aTestDesc);

//	TInt err(KErrNone);
	// A series of tasks to perform before calling the test method.

	// Delete the previous data files to ensure rediscover from scratch
//	TRAP(err, EComTestUtils::FileManDeleteFileL(KDataDir));

	// The two flags below are mutually exclusive.
//	if (aTaskMask & EPreTest_UnitializeCachedDriveInfo)
//		{
//		TDriveInfo_StateAccessor::ClearCachedDriveInfo();
//		}
//	else if (aTaskMask & EPreTest_EnableAllDrives)
//		{
//		TDriveInfo_StateAccessor::SetDriveDisableMaskL(0);
//		}

//	if (aTaskMask & EPreTest_CopyPlugins)
//		{
//		CopyPlugins();
//		User::After(KOneSecond);
//		CleanupStack::PushL(TCleanupItem(DeleteTestPlugin, NULL));
//		}

//	TBool drivesEnabled = (aTaskMask & EPreTest_EnableAllDrives) != 0;
//	oomStep = 0;

//	do
//		{
//		__UHEAP_MARK;
  		// find out the number of open handles
//		TInt startProcessHandleCount;
//		TInt startThreadHandleCount;
//		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		// Need to create object here as ECom has embedded TRAPs
		// that will fail the test if memory not available
//		CDisableDrivesTest* disableDrvTest = CDisableDrivesTest::NewL(drivesEnabled);
//		CleanupStack::PushL(disableDrvTest);

		// Setting Heap failure for OOM test
//		__UHEAP_SETFAIL(RHeap::EDeterministic, ++oomStep);

//		TRAP(err, (disableDrvTest->*aTestFuncL)());

//		__UHEAP_SETFAIL(RHeap::ENone, 0);

//		CleanupStack::PopAndDestroy(disableDrvTest);
//		disableDrvTest = NULL;

		// check that no handles have leaked
//		TInt endProcessHandleCount;
//		TInt endThreadHandleCount;
//		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

//		test(startProcessHandleCount == endProcessHandleCount);
//		test(startThreadHandleCount  == endThreadHandleCount);

//		__UHEAP_MARKEND;
//		} while(err == KErrNoMemory);

//	if (aTaskMask & EPreTest_CopyPlugins)
//		{
//		CleanupStack::PopAndDestroy(1);
//		}

//	test(err == KErrNone);
//	test.Printf(_L("- server succeeded at heap failure rate of %i\n"), oomStep);
//	}

LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	// Basic tests
	PatchableConstantTest();
	CR1182PatchableConstantTest();

	DoBasicTestL(&CDisableDrivesTest::startupDiscoveryL,
				KStartupDiscoveryDrivesDisabledMask,
				_L("startupDiscoveryL scan disabled"));

	// Control step to proof that the method is sound.
	DoBasicTestL(&CDisableDrivesTest::startupDiscoveryL,
				KStartupDiscoveryDrivesEnabledMask,
				_L("startupDiscoveryL scan enabled"));

	DoBasicTestL(&CDisableDrivesTest::RediscoveryL,
				KRediscoveryDrivesDisabledMask,
				_L("RediscoveryL scan disabled"));

	// Control step to proof that the method is sound.
	DoBasicTestL(&CDisableDrivesTest::RediscoveryL,
				KRediscoveryDrivesEnabledMask,
				_L("RediscoveryL scan enabled"));

	// OOM tests are left in here as instructed by the component owner.
	// In case we need to investigate OOM issues in the future then the
	// SWE can follow the instructions below to run OOM test manually:
	//
	// CDiscoverer::CDirScanner::DoScanDriveL in discoverer.cpp ignores
	// all errors from RFs::GetDir().
	// Go there and manually add if (error == KErrNoMemory) User::LeaveNoMemory()
	// Also the three RunError of the CActive in discoverer.cpp need to return
	// KErrNoMemory instead of panic.
	// NB: after adding the above changes, some OOM tests in t_discoverer and
	// t_registrar will timeout in ONB (RTests are given 40 minutes to run but OOM
	// test on discovery takes couple hours).
	// But if these changes are manual and not submitted in perforce, then
	// there is no effect on ONB.

//	DoOOMTestL(&CDisableDrivesTest::startupDiscoveryL,
//				KStartupDiscoveryDrivesDisabledMask,
//				_L("OOM startupDiscoveryL scan disabled"));

//	DoOOMTestL(&CDisableDrivesTest::startupDiscoveryL,
//				KStartupDiscoveryDrivesEnabledMask,
//				_L("OOM startupDiscoveryL scan enabled"));

//	DoOOMTestL(&CDisableDrivesTest::RediscoveryL,
//				KRediscoveryDrivesDisabledMask,
//				_L("OOM RediscoveryL scan disabled"));

	// Control step to proof that the method is sound.
//	DoOOMTestL(&CDisableDrivesTest::RediscoveryL,
//				KRediscoveryDrivesEnabledMask,
//				_L("OOM RediscoveryL scan enabled"));
	__UHEAP_MARKEND;
	}

//Initialise the Active Scheduler
LOCAL_C void SetupL()
	{
	// Construct and install the Active Scheduler. The Active Schedular is needed
	// by components used by this test as they are ActiveObjects.
	TheActiveScheduler = new(ELeave)CDerivedActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	test.Printf(_L("\n"));
	test.Title();
	test.Start( _L("Disable scanning on selected drives via patchable constant") );

	TheTrapCleanup = CTrapCleanup::New();

	// Connect the file server instance
	User::LeaveIfError(TheFs.Connect());

	TRAPD(err, SetupL());
	test(err == KErrNone);

	// Call the main tests
	TRAP(err, DoTestsL());
	test(err == KErrNone);

	delete TheActiveScheduler;
	delete TheTrapCleanup;

	TheFs.Close();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
