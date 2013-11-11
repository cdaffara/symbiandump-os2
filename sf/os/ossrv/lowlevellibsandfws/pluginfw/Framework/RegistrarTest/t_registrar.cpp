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
// to test production class CRegistrar. Where necessary stubs
// are implemented to help in writing test harness using RTest.
// 
//

#include "RegistryData.h"
#include "EComErrorCodes.h"
#include "EComUidCodes.h"
#include "ImplementationInformation.h"
#include "RegistryData.h"
#include "Registrar.h"
#include "RegistrarObserver.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include "EComEntryBase.h"
#include "DriveInfo.h"

#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <startup.hrh>

#include "baspi.h"

_LIT (KDataDir, "C:\\private\\10009D8F\\ECom*");

//We need two different entry RSC and the SPI to fully test CRegistrar
//------------------------------NEW TYPE----------------------------------------
const TUid NewDllUid={0x101F847B};
_LIT(KNewResourceFileNameOnly,"EComExample5.rsc");
_LIT(KNewResourceFileNameC, "C:\\resource\\plugins\\EComExample5.rsc");
_LIT(KNewDllFileNameC, "C:\\sys\\bin\\EComExample5.dll");
_LIT(KNewResourceFileNameZ, "z:\\RAMOnly\\EComExample5.rsc");
_LIT(KNewDllFileNameZ,"z:\\RAMOnly\\EComExample5.dll");

//We need IIC type 3 RSC to fully test CRegistrar
//------------------------------IIC TYPE 3----------------------------------------
const TUid Type3DllUid = {0x10009E3E};
_LIT(KType3ResourceFileNameOnly,"EComExample12.rsc");
_LIT(KType3ResourceFileNameC, "C:\\resource\\plugins\\EComExample12.rsc");
_LIT(KType3DllFileNameC, "C:\\sys\\bin\\EComExample12.dll");
_LIT(KType3ResourceFileNameZ, "z:\\RAMOnly\\EComExample12.rsc");
_LIT(KType3DllFileNameZ,"z:\\RAMOnly\\EComExample12.dll");

//We need IIC type 3 RSC with data exceed maximum to fully test CRegistrar
//------------------------------IIC TYPE 3----------------------------------------
_LIT(KType3BadDataResourceFileNameOnly,"EComExampleBadData.rsc");
_LIT(KType3BadDataResourceFileNameC, "C:\\resource\\plugins\\EComExampleBadData.rsc");
_LIT(KType3BadDataDllFileNameC, "C:\\sys\\bin\\EComExampleBadData.dll");
_LIT(KType3BadDataResourceFileNameZ, "z:\\RAMOnly\\EComExampleBadData.rsc");
_LIT(KType3BadDataDllFileNameZ,"z:\\RAMOnly\\EComExampleBadData.dll");

_LIT(KType3BadData1ResourceFileNameOnly,"EComExampleBadData1.rsc");
_LIT(KType3BadData1ResourceFileNameC, "C:\\resource\\plugins\\EComExampleBadData1.rsc");
_LIT(KType3BadData1DllFileNameC, "C:\\sys\\bin\\EComExampleBadData1.dll");
_LIT(KType3BadData1ResourceFileNameZ, "z:\\RAMOnly\\EComExampleBadData1.rsc");
_LIT(KType3BadData1DllFileNameZ,"z:\\RAMOnly\\EComExampleBadData1.dll");

_LIT(KType3BadData2ResourceFileNameOnly,"EComExampleBadData2.rsc");
_LIT(KType3BadData2ResourceFileNameC, "C:\\resource\\plugins\\EComExampleBadData2.rsc");
_LIT(KType3BadData2DllFileNameC, "C:\\sys\\bin\\EComExampleBadData2.dll");
_LIT(KType3BadData2ResourceFileNameZ, "z:\\RAMOnly\\EComExampleBadData2.rsc");
_LIT(KType3BadData2DllFileNameZ,"z:\\RAMOnly\\EComExampleBadData2.dll");

//We need IIC type 2 RSC to fully test CRegistrar
//------------------------------IIC TYPE 2----------------------------------------
_LIT(KType2ResourceFileNameOnly,"EComExample14.rsc");
_LIT(KType2ResourceFileNameC, "C:\\resource\\plugins\\EComExample14.rsc");
_LIT(KType2DllFileNameC, "C:\\sys\\bin\\EComExample14.dll");
_LIT(KType2ResourceFileNameZ, "z:\\RAMOnly\\EComExample14.rsc");
_LIT(KType2DllFileNameZ,"z:\\RAMOnly\\EComExample14.dll");

//------------------------------LEGACY TYPE-------------------------------------

// spi test file
_LIT(KEComSpiTestFilePathAndName, "Z:\\Test\\Data\\EcomTest.spi");
// spi test file for testing IIC Type3
_LIT(KEComSpiTestFileType3PathAndName, "Z:\\Test\\Data\\ecomtesttype3.spi");

const TInt KOneSecond = 1000000;

const TUid interfaceUidExample12 = {0x10009E36};
const TUid extendedInterfaceUid1 = {0x10009E44};
const TUid extendedInterfaceUid2 = {0x10009E45};
const TUid extendedInterfaceUid3 = {0x10009E46};


LOCAL_D RTest test(_L("t_Registrar.exe"));

LOCAL_D RFs					TheFs;

LOCAL_D CTrapCleanup* 		TheTrapCleanup 		= NULL;

LOCAL_D CActiveScheduler*	TheActiveScheduler	= NULL;

// Used for supressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

//It is used by some test methods which are called two times:
//from normal test and from OOM test.
static void LeaveIfErrNoMemory(TInt aError)
	{
	if(aError == KErrNoMemory)
		{
		User::Leave(aError);
		}
	}

/**
TRegistrar_StateAccessor class allows access to private and protected
members of production code class CRegistrar, as its a friend class.
*/
class TRegistrar_StateAccessor
	{
public:
	void DiscoveriesBegin(CRegistrar& aRegistrar);

	void DiscoveriesComplete(CRegistrar& aRegistrar, TBool aSuccessful);

	void DriveRemovedL(CRegistrar& aRegistrar, TDriveUnit aDriveUnit);

	void DriveReinstatedL(CRegistrar& aRegistrar, TDriveUnit aDriveUnit);

	void RegisterDiscoveryL(CRegistrar& aRegistrar,const TDriveName& aDrive, CPluginBase*& aEntry,TBool aDatFileExists);

	void ParseL(CRegistrar& aRegistrar,
				CPluginBase*& aEntry,
				CRegistryData::CDllData& aDll);

	void ParseRegistrationDataL(CRegistrar& aRegistrar,
				CPluginBase*& aEntry,
				TDriveUnit& aDriveUnit,
				TBool aUpdate,
				TInt aRegistryDriveIndex,
				CRegistryData::CDriveData* aDriveData);

	CRegistrar::TRegistrarState State(CRegistrar& aRegistrar);
	};

/**
Parses the resource file associated with an
Interface Implementation Collection for the plugin entry.

@param		aRegistrar The CRegistrar class object under test
@param		aEntry an ecom entry discovered
@param		aDll CRegistrar::CDllData object in which parsed data needs to be stored
@pre 		CRegistrar should be fully constructed.
@post		The resource file is parsed and the data is added to 'aDll'.
 */
void TRegistrar_StateAccessor::ParseL(CRegistrar& aRegistrar,
									  CPluginBase*& aEntry,
									  CRegistryData::CDllData& aDll)

	{
	aRegistrar.ParseL(aEntry,aDll);
	}

/**
Determines if the directory entry for an Interface Implementation
Collection requires registration. Parses and registers the plugin entry
associated with the resource file if entry is not already registered.

@param		aRegistrar The CRegistrar class object under test
@param		aEntry Directory entry for the Interface Impplementation Collection.
@param		aRscFileName Resource file from which to extract the data
@param		aDrive Drive on which registry data has been found on
@param		aUpdate Flag indicating if this is an update, or new entry.
@param		aRegistryDriveIndex Index within the registry data
			for the branch that the registry entry was found within.
@pre 		CRegistrar is fully constructed.
@post		The Interface Implementation Collection entry has been processed appropriately.
*/
void TRegistrar_StateAccessor::ParseRegistrationDataL(CRegistrar& aRegistrar,
														CPluginBase*& aEntry,
														TDriveUnit& aDrive,
														TBool aUpdate,
														TInt aRegistryDriveIndex,
														CRegistryData::CDriveData* aDriveData)
	{
	aRegistrar.ParseRegistrationDataL(aEntry, aDrive,aUpdate, aRegistryDriveIndex, aDriveData);
	}

/**
Overload of the MDiscovererObserver callback method.
The cue that a registry update is about to occur.

@param		aRegistrar The CRegistrar class object under test
@pre 		CRegistrar is fully constructed
@post		The Registry data manager has been informed that its internal
			data is out of date.
*/
void TRegistrar_StateAccessor::DiscoveriesBegin(CRegistrar& aRegistrar)
	{
	aRegistrar.DiscoveriesBegin();
	}

/**
Overload of the MDiscovererObserver callback method.
The cue that a registry update is done.

@param		aRegistrar The CRegistrar class object under test
@param		aSuccessful The registry data has been updated successfully
@pre 		CRegistrar is fully constructed
@post		The current registry data and the internal access indexes have been rebuilt.
*/
void TRegistrar_StateAccessor::DiscoveriesComplete(CRegistrar& aRegistrar,
												   TBool aSuccessful)
	{
	aRegistrar.DiscoveriesComplete(aSuccessful, EPluginProcessingTypeAll);
	}

/**
Overload of the MDiscovererObserver callback method.
Signals registry that the registered Interface Implementation
Collections stored upon the specified drive are no
longer available for use.

@param		aRegistrar The CRegistrar class object under test
@param		aDrive	The drive identifier.
@pre 		CRegistrar is fully constructed.
@post		The registered Interface Implementation Collections
			stored upon the specified drive are no
			longer available for use.
*/
void TRegistrar_StateAccessor::DriveRemovedL(CRegistrar& aRegistrar,
											 TDriveUnit aDrive)
	{
	aRegistrar.DriveRemovedL(aDrive);
	}

/**
Overload of the MDiscovererObserver callback method.
Signals registry that the registered Interface
Implementation Collections stored upon the specified drive are again
available for use.

@param		aRegistrar The CRegistrar class object under test
@param		aDrive	The drive identifier.
@pre 		CRegistrar is fully constructed.
@post		The registered Interface Implementation Collections
			stored upon the specified drive are again
			made available for use.
*/
void TRegistrar_StateAccessor::DriveReinstatedL(CRegistrar& aRegistrar,
												   TDriveUnit aDrive)
	{
	aRegistrar.DriveReinstatedL(aDrive);
	}

/**
Overload of the MDiscovererObserver callback method.
Adds the specified Interface Implementation Collection Entry to the registry.

@param		aRegistrar The CRegistrar class object under test
@param		aDirEntry The plugin entry to register.
@pre 		CRegistrar is fully constructed
@post		The plugin entry is registered.
*/
void TRegistrar_StateAccessor::RegisterDiscoveryL(CRegistrar& aRegistrar,
												  const TDriveName& aDrive,
												  CPluginBase*& aEntry,
												  TBool aDatFileExists )
	{
	aRegistrar.RegisterDiscoveryL(aDrive,aEntry,aDatFileExists);
	}

/**
Retrieves the object's current state.

@return 	TCRegistarState the CRegistrar's current state.
@pre		CRegistrar is fully constructed.
@post 		the CRegistrar's current state has been returned.
*/
CRegistrar::TRegistrarState TRegistrar_StateAccessor::State(CRegistrar& aRegistrar)
	{
	return aRegistrar.State();
	}

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
class CRegistrarTest : public CBase
	{
public:

	static CRegistrarTest* NewL(TBool aIsFullDiscovery);
	virtual ~CRegistrarTest();
	const CImplementationInformation * GetImplementationDataL();

	void EnableDisableTestL();
	void ResumeSuspendTestL();
	void DiscoveriesBegin_Register_CompleteTestL();
	void DriveRemovedReinstatedTestL();
	void ParseTestL();
	void ParseType2TestL();
	void ParseType3TestL();
	void ParseType3BadDataTestL();
	void ParseRegistrationDataTestL();
	void ParseRegistrationDataType3TestL();
	void StateTransitionTestL();
	void ParseSpiRegistrationDataTestL();
	void ParseSpiRegistrationDataType3TestL();
	void ParseSpiTestL();
	void ParseSpiType3TestL();

private:
	CRegistrarTest();
	void ConstructL(TBool aIsFullDiscovery);
	void ParseBadDataTestL(const TDesC& aRscFullName, const TDesC& aRscNameOnly);
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
	};

/**
Standardised safe construction which
leaves nothing on the cleanup stack.

@param		aIsFullDiscovery whether the constructor make full discovery
@post		CRegistrarTest is fully constructed and initialised.
*/
CRegistrarTest* CRegistrarTest::NewL(TBool aIsFullDiscovery)
	{
	CRegistrarTest* self = new (ELeave) CRegistrarTest();
	CleanupStack::PushL(self);
	self->ConstructL(aIsFullDiscovery);
	CleanupStack::Pop(self);
	return self;
	}

/**
Standardized default c'tor

@post		CRegistrarTest is fully constructed.
*/
CRegistrarTest::CRegistrarTest()
:	CBase(),
	iDriveUnit(EDriveC)
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
CRegistrarTest::~CRegistrarTest()
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

@param		aIsFullDiscovery whether the constructor make full discovery
@post		CRegistrarTest is fully constructed.
*/
void CRegistrarTest::ConstructL(TBool aIsFullDiscovery)
	{
	CRegistryData::CDriveData* driveData=NULL;
	iRegistrarObserver = new (ELeave) CTestRegistrarObserver;
	iStateAccessor	   = new (ELeave) TRegistrar_StateAccessor;
	iRegistryDataStateAccessor	   = new (ELeave) TRegistryData_StateAccessor;
	iRegistryData	   = CRegistryData::NewL(TheFs);
	iRegistrar		   = CRegistrar::NewL(*iRegistryData, *iRegistrarObserver, TheFs);
	if(aIsFullDiscovery)
		{
		iRegistrar->ProcessSSAEventL(EStartupStateNonCritical);
		}
	iDllData		   = CRegistryData::CDllData::NewLC(driveData);
	CleanupStack::Pop(iDllData);
	}


/**
Returns information of a particular interface implementation.

@return		Information on the requested implementation
@pre 		This object is fully constructed
*/
const CImplementationInformation * CRegistrarTest::GetImplementationDataL()
	{
	CRegistryData::CImplementationData *implementationData=NULL;
	TUid dummyUid;
	dummyUid.iUid = 0;
	User::LeaveIfError(iRegistryDataStateAccessor->FindImplementation(*iRegistryData, iImplementationUid,
																dummyUid, implementationData));
	return implementationData->iImplInfo;
	}

/**
The test executes by first Disabling the Implementation then by
Enabling it.

@SYMTestCaseID          SYSLIB-ECOM-CT-0727
@SYMTestCaseDesc	    Tests for enabling and disabling the Implementation
@SYMTestPriority 	    High
@SYMTestActions  	    Fetch the Implementation information for testing for iImplementationUid
                        Disable and enable the implementation for the unique UID
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistrarTest::EnableDisableTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0727 "));
	const CImplementationInformation *implimentationInfo = NULL;

	// Get the Implementation information for testing for iImplementationUid.
	// Implementation for iImplementationUid should be registered to
	// run this test successfully.
	TRAPD(err, implimentationInfo = GetImplementationDataL());
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	test(implimentationInfo != NULL);

	// Disable the implementaion
	err = iRegistrar->Disable(iImplementationUid);
	::LeaveIfErrNoMemory(err);
	test(KErrNone == err);
	test(implimentationInfo->Disabled());//It should be disabled


	// Enable the implementaion
	err = iRegistrar->Enable(iImplementationUid);
	::LeaveIfErrNoMemory(err);
	test(KErrNone == err);
	test(!implimentationInfo->Disabled());//It should be enabled
	}

/**
The test executes by first suspending the discoverer then by resuming it.

@SYMTestCaseID          SYSLIB-ECOM-CT-0728
@SYMTestCaseDesc	    Tests for overloaded MBackupNotifierObserver's Suspend and Resume functions
@SYMTestPriority 	    High
@SYMTestActions  	    The test executes by first calling CRegistrar's suspend and then by resume.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistrarTest::ResumeSuspendTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0728 "));
	TInt err = iRegistrar->Suspend();
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	err = iRegistrar->Resume();
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	}

/**
The test executes by calling DiscoveriesBegin, RegisterDiscoveryL and
DiscoveriesComplete in sequence to register the plugin.

@SYMTestCaseID          SYSLIB-ECOM-CT-0729
@SYMTestCaseDesc	    Tests for registering the DLL interfaces
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up DiscoveriesBegin, RegisterDiscoveryL and
						DiscoveriesComplete in sequence to register the plugin and
						verifies that DLL's have been registered succesfully.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistrarTest::DiscoveriesBegin_Register_CompleteTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0729 "));
	// The following three steps need to be executed to successfully
	// register the plugin
	iStateAccessor->DiscoveriesBegin(*iRegistrar);

//-----------------REGISTRATION PART------------------------------------
	CPluginBase* entryBase=NULL;
	TInt err=KErrNone;
	TBool update = EFalse;
	TInt  driveIndex = KErrNotFound;		//Used as return parameter used below
	TBool isDriveRegistered = EFalse;		//Used as return parameter used below
	CRegistryData::CDriveData* driveData = NULL;


	//Testing a RSc based entry using EComExample5.dll and EComExample5.rsc
	TEntry rscBasedEntry;
	rscBasedEntry.iName=KNewResourceFileNameOnly;
	err = TheFs.Modified(KNewResourceFileNameC, rscBasedEntry.iModified);
	test(err==KErrNone);
	TParse rscPath;
	rscPath.Set(KNewResourceFileNameC,NULL,NULL);
	entryBase=CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), ETrue);
	CleanupStack::PushL(entryBase);

	//Now register this Rsc Entry with the registrar
	TRAP(err, iStateAccessor->RegisterDiscoveryL(*iRegistrar,rscPath.Drive(), entryBase,ETrue));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	CleanupStack::PopAndDestroy(entryBase);
	entryBase=NULL;
	TRAP(err, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
	::LeaveIfErrNoMemory(err);
	test(driveIndex != KErrNotFound);
	//Verify weather dll interfaces got registered
	// Check whether Rsc based entry is already registered in CRegistryData

	TRAP(err, isDriveRegistered = iRegistryData->IsRegisteredWithDate(
																		NewDllUid,
																		rscBasedEntry.iModified,
																		update,
																		driveData));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);
	test(isDriveRegistered);
	test(!update);// As plugin is just registered, it does not require update.

	//THE REASON WHY THIS FUNCTION IS MOVED TO THE BACK AS IT PREVENT THE TESTING
	//OF THE LEGACY PLUGINS BEING REGISTERED AS DURING THE VALIDATE REGISTRY
	//THE LEGACY ENTRY WILL BE WIPED OUT
	iStateAccessor->DiscoveriesComplete(*iRegistrar, EFalse);
	test(iRegistrar != NULL);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0730
@SYMTestCaseDesc	    Tests for CRegistrar::DriveRemovedL,DriveReinstatedL functions
@SYMTestPriority 	    High
@SYMTestActions  	    The test executes by first making the test drive uninstall and
						then testing for Install and Uninstall.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistrarTest::DriveRemovedReinstatedTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0730 "));
	// We are checking with F Drive.
	// The test executes by first removing and then reinstalling
	const TTime modified(0); //Dummy parameter...
	TBool update = EFalse;   //Dummy parameter...
	TBool isDriveRegistered = EFalse;
	TInt  driveIndex = KErrNotFound;
	CRegistryData::CDriveData* driveData = NULL;

	iDriveUnit = EDriveF;
	TInt error = KErrNone;
	TRAP(error, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
	::LeaveIfErrNoMemory(error);
	TInt err = KErrNone;
	if(driveIndex != KErrNotFound)
	{
	TRAP(err, iRegistryData->IsRegisteredWithDate(
													iDllUid,
													modified,
													update,

													driveData));

	::LeaveIfErrNoMemory(err);
	}

	isDriveRegistered = (driveIndex != KErrNotFound);

	//Remove the drive from registry if registered
	if(isDriveRegistered)
		{
		TRAP(err, iStateAccessor->DriveRemovedL( *iRegistrar, iDriveUnit));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);

		//Call CRegistryData::IsRegisteredWithDate again to check its removed now
		driveIndex = KErrNotFound;
		TRAP(err, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
		//Drive should not be present
		::LeaveIfErrNoMemory(err);
		test(driveIndex == KErrNotFound);

		}


	// Install the drive in registry
	TRAP(err, iStateAccessor->DriveReinstatedL(*iRegistrar, iDriveUnit));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);

	//Call CRegistryData::IsRegisteredWithDate to check its Reinstalled now
	driveIndex = KErrNotFound;
	TRAP(err, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
	::LeaveIfErrNoMemory(err);
	//Drive should be present now
	test(driveIndex != KErrNotFound);
	TRAP(err, iRegistryData->IsRegisteredWithDate(
													iDllUid,
													modified,
													update,

													driveData));
	::LeaveIfErrNoMemory(err);
	test(err == KErrNone);


	//Remove the drive from registry if it was not registered before
	if(!isDriveRegistered)
		{
		TRAP(err, iStateAccessor->DriveRemovedL( *iRegistrar, iDriveUnit));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);

		//Call CRegistryData::IsRegisteredWithDate again to check its removed now
		driveIndex = KErrNotFound;
		TRAP(err, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
		//Drive should not be present
		::LeaveIfErrNoMemory(err);
		test(driveIndex == KErrNotFound);
		}
	}

/**
The test executes by parsing the Interface implementaion information
from the given resource file.

@SYMTestCaseID          SYSLIB-ECOM-CT-0731
@SYMTestCaseDesc	    Tests for verification of Interface implementaion information
@SYMTestPriority 	    High
@SYMTestActions  	    Parse the resource information associated with an
					    Interface Implementation Collection for entry into the registry
					    information.Check for OOM error or parse error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistrarTest::ParseTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0731 "));
	TInt error=KErrNone;
	CPluginBase* entryBase=NULL;
	CRegistryData::CDllData* dllData=NULL;
	CRegistryData::CInterfaceData* ifData =NULL;
	CRegistryData::CDriveData* driveData = NULL;

	//Testing a RSC based entry using EComExample5.dll and EComExample5.rsc
	//(101F847B.RSS)
	TEntry rscBasedEntry;
	rscBasedEntry.iName=KNewResourceFileNameOnly;
	error = TheFs.Modified(KNewResourceFileNameC, rscBasedEntry.iModified);
	test(error==KErrNone);
	TParse rscPath;
	rscPath.Set(KNewResourceFileNameC,NULL,NULL);
	TDriveUnit driveUnit(rscPath.Drive());
	error=iRegistryData->FindDriveL(driveUnit,driveData);
	test(error!=KErrNotFound);
	entryBase=CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), ETrue);
	CleanupStack::PushL(entryBase);
	dllData=CRegistryData::CDllData::NewLC(*(entryBase->iDllName),entryBase->iDllModifiedTime,entryBase->iDllSecondUid,entryBase->iDllThirdUid,driveData);

	//Now testing the parseL function
	TRAP(error, iStateAccessor->ParseL(*iRegistrar,entryBase,*dllData));
	::LeaveIfErrNoMemory(error);
	test(error == KErrNone);
	//Now checking the content of the DllData
	test(dllData->iIfList->Count()==1);
	ifData=(*(dllData->iIfList))[0];
	test(ifData->iInterfaceUid.iUid==0x10009DC0);
	test(ifData->iImplementations->Count()==1);

	CleanupStack::PopAndDestroy(dllData);
	CleanupStack::PopAndDestroy(entryBase);
	entryBase=NULL;
	dllData=NULL;
	ifData=NULL;
	}

/**
The test executes by parsing the Interface implementation information with type 2 entries
from the given resource file.

@SYMTestCaseID          SYSLIB-ECOM-UT-3563
@SYMTestCaseDesc	    Tests for verification of Interface implementation information with type 2 entries
@SYMTestPriority 	    High
@SYMTestActions  	    Parse the resource information associated with an
					    Interface Implementation Collection type 2 for entry into the registry
					    information.Check for OOM error or parse error.
@SYMTestExpectedResults CRegistrar::ParseL() function parses resource file without error.
						Data is interpreted correctly.
@SYMEC                  EC43
*/
void CRegistrarTest::ParseType2TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3563 "));
	TInt error = KErrNone;
	CPluginBase* entryBase = NULL;
	CRegistryData::CDllData* dllData = NULL;
	CRegistryData::CInterfaceData* ifData = NULL;
	CRegistryData::CDriveData* driveData = NULL;
	TUid interfaceUidExample14 = {0x10009E36};

	//Tests an RSC based entry using EComExample14.dll and EComExample14.rsc
	//(EcomExample14.RSS)
	TEntry rscBasedEntry;
	rscBasedEntry.iName = KType2ResourceFileNameOnly;
	error = TheFs.Modified(KType2ResourceFileNameC, rscBasedEntry.iModified);
	test(error == KErrNone);
	TParse rscPath;
	rscPath.Set(KType2ResourceFileNameC,NULL,NULL);
	TDriveUnit driveUnit(rscPath.Drive());
	error = iRegistryData->FindDriveL(driveUnit,driveData);
	test(error != KErrNotFound);
	entryBase = CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), ETrue);
	CleanupStack::PushL(entryBase);
	dllData = CRegistryData::CDllData::NewLC(*(entryBase->iDllName),entryBase->iDllModifiedTime,entryBase->iDllSecondUid,entryBase->iDllThirdUid,driveData);

	//tests the parseL function
	TRAP(error, iStateAccessor->ParseL(*iRegistrar,entryBase,*dllData));
	::LeaveIfErrNoMemory(error);
	test(error == KErrNone);
	//checks the content of the DllData
	test(dllData->iIfList->Count() == 1);
	ifData = (*(dllData->iIfList))[0];
	test(ifData->iInterfaceUid == interfaceUidExample14);
	CleanupStack::PopAndDestroy(dllData);
	CleanupStack::PopAndDestroy(entryBase);
	}

/**
The test executes by parsing the Interface implementation information with type 3 entries
from the given resource file.

@SYMTestCaseID          SYSLIB-ECOM-UT-3564
@SYMTestCaseDesc	    Tests for verification of Interface implementation information with type 3 entries
@SYMTestPriority 	    High
@SYMTestActions  	    Parse the resource information associated with an
					    Interface Implementation Collection type 3 for entry into the registry
					    information.Check for OOM error or parse error.
@SYMTestExpectedResults CRegistrar::ParseL() function parses resource file without error.
						Data is interpreted correctly.
@SYMEC                  EC43
*/
void CRegistrarTest::ParseType3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3564 "));
	TInt error = KErrNone;
	CPluginBase* entryBase = NULL;
	CRegistryData::CDllData* dllData = NULL;
	CRegistryData::CInterfaceData* ifData = NULL;
	CRegistryData::CDriveData* driveData = NULL;
	CRegistryData::CImplementationData* implData = NULL;
	CImplementationInformation* implInfo = NULL;

	//Tests an RSC based entry using EComExample12.dll and EComExample12.rsc
	//(EcomExample12.RSS)
	TEntry rscBasedEntry;
	rscBasedEntry.iName = KType3ResourceFileNameOnly;
	error = TheFs.Modified(KType3ResourceFileNameC, rscBasedEntry.iModified);
	test(error == KErrNone);
	TParse rscPath;
	rscPath.Set(KType3ResourceFileNameC,NULL,NULL);
	TDriveUnit driveUnit(rscPath.Drive());
	error = iRegistryData->FindDriveL(driveUnit,driveData);
	test(error != KErrNotFound);
	entryBase = CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), ETrue);
	CleanupStack::PushL(entryBase);
	dllData = CRegistryData::CDllData::NewLC(*(entryBase->iDllName),entryBase->iDllModifiedTime,entryBase->iDllSecondUid,entryBase->iDllThirdUid,driveData);

	// Tests the parseL function
	TRAP(error, iStateAccessor->ParseL(*iRegistrar,entryBase,*dllData));
	::LeaveIfErrNoMemory(error);
	test(error == KErrNone);
	// Checks the content of the DllData
	test(dllData->iIfList->Count() == 1);
	ifData = (*(dllData->iIfList))[0];
	test(ifData->iInterfaceUid == interfaceUidExample12);
	//Currently there are 3 implementations for this interface.
	//But the implementation(UID=0x10009E3D) is ROM-only but is actually in RAM will not be created.
	//So, actually number of implementations created for this interface is 2.
	test(ifData->iImplementations->Count() == 2);
	implData = (*(ifData->iImplementations))[0];
	implInfo = implData->iImplInfo;

	// Fetches the list of extended interfaces for the first implementation
	RArray<TUid>* extendedInterfaceList = implInfo->GetExtendedInterfaceList();
	test(extendedInterfaceList->Count()==3);
	test((*extendedInterfaceList)[0] == extendedInterfaceUid1);
	test((*extendedInterfaceList)[1] == extendedInterfaceUid2);
	test((*extendedInterfaceList)[2] == extendedInterfaceUid3);
	CleanupStack::PopAndDestroy(dllData);
	CleanupStack::PopAndDestroy(entryBase);
	}


/** Wrapper function for testing ParseL function with type 3 entries
and data in the resource file exceed maximum
*/
void CRegistrarTest::ParseBadDataTestL(const TDesC& aRscFullName, const TDesC& aRscNameOnly)
	{
	TInt error = KErrNone;
	CPluginBase* entryBase = NULL;
	CRegistryData::CDllData* dllData = NULL;
	CRegistryData::CDriveData* driveData = NULL;

	TEntry rscBasedEntry;
	rscBasedEntry.iName = aRscNameOnly;
	error = TheFs.Modified(aRscFullName, rscBasedEntry.iModified);
	test(error == KErrNone);
	TParse rscPath;
	rscPath.Set(aRscFullName,NULL,NULL);
	TDriveUnit driveUnit(rscPath.Drive());
	error = iRegistryData->FindDriveL(driveUnit,driveData);
	test(error != KErrNotFound);
	entryBase = CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), ETrue);
	CleanupStack::PushL(entryBase);
	dllData = CRegistryData::CDllData::NewLC(*(entryBase->iDllName),entryBase->iDllModifiedTime,entryBase->iDllSecondUid,entryBase->iDllThirdUid,driveData);

	//Tests the parseL function
	TRAP(error, iStateAccessor->ParseL(*iRegistrar,entryBase,*dllData));
	test(error == KErrNotSupported);

	CleanupStack::PopAndDestroy(dllData);
	CleanupStack::PopAndDestroy(entryBase);
	}


/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3565
@SYMTestCaseDesc	    Tests for verification of Interface implementation information with type 3 entries
						and data in the resource file exceed maximum.
@SYMTestPriority 	    High
@SYMTestActions  	    Parse the resource information associated with an
					    Interface Implementation Collection type 3 for entry into the registry
					    information.
@SYMTestExpectedResults CRegistrar::ParseL() function parses resource file with KErrNotSupported leave occurs.
@SYMEC                  EC43
@SYMDEF                 DEF111196
*/
void CRegistrarTest::ParseType3BadDataTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3565 "));
	// Tests for number of strings of default data exceeds maximum
	// Uses EComExampleBadData.dll and EComExampleBadData.rsc (EcomExampleBadData.RSS)
	ParseBadDataTestL(KType3BadDataResourceFileNameC,KType3BadDataResourceFileNameOnly);

	// Tests for number of interfaces exceeds maximum
	// Uses EComExampleBadData1.dll and EComExampleBadData1.rsc (EcomExampleBadData1.RSS)
	ParseBadDataTestL(KType3BadData1ResourceFileNameC,KType3BadData1ResourceFileNameOnly);

	// Tests for number of implementation in one interfaces exceeds maximum
	// Uses EComExampleBadData2.dll and EComExampleBadData2.rsc (EcomExampleBadData2.RSS)
	ParseBadDataTestL(KType3BadData2ResourceFileNameC,KType3BadData2ResourceFileNameOnly);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0089
@SYMTestCaseDesc 	Check that the ParseL for spi data works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecomtest.spi can be parsed successfully
					and no leave occurs.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CRegistrarTest::ParseSpiTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0089 "));
	TEntry spiEntry;
	TParse spiPath;
	if (TheFs.Entry(KEComSpiTestFilePathAndName, spiEntry) != KErrNone)
		return;

	spiPath.Set(KEComSpiTestFilePathAndName,NULL,NULL);
	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFilePathAndName);
	CleanupClosePushL(resourceArchive);

	// Checks SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	// there is only 1 resource file
	CPluginBase* entryBase=NULL;
	CRegistryData::CDllData* dllData=NULL;
	CRegistryData::CDriveData* driveData = NULL;

	TDriveUnit driveUnit(spiPath.Drive());
	TInt error=iRegistryData->FindDriveL(driveUnit,driveData);
	test(error!=KErrNotFound);

	while(!resourceArchive.End())
		{
		entryBase=CSpiPlugin::NewL(resourceArchive);
		CleanupStack::PushL(entryBase);
		dllData=CRegistryData::CDllData::NewLC(*(entryBase->iDllName),entryBase->iDllModifiedTime,entryBase->iDllSecondUid,entryBase->iDllThirdUid,driveData);
		TRAPD(err,iStateAccessor->ParseL(*iRegistrar,entryBase,*dllData));

		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);
		CleanupStack::Pop(dllData);
		CleanupStack::PopAndDestroy(entryBase);
		entryBase=NULL;
		}

	CleanupStack::PopAndDestroy(&resourceArchive);

	// Currently only one interface definition in the resource file
	const TInt ifCount = dllData->iIfList->Count();
	test(ifCount == 1);

	// Currently only one implemention for the above interface in the resource file
	CRegistryData::CInterfaceData* ifData = (*(dllData->iIfList))[0];
	const TInt impCount = ifData->iImplementations->Count();
	test(impCount == 1);
	// Interface UID verification
	test(ifData->iInterfaceUid == iInterfaceUid);

	delete dllData;
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-3566
@SYMTestCaseDesc 	Check that the ParseL for spi data with IIC Type 3 entry works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecomtesttype3.spi can be parsed successfully
					and no leave occurs.
@SYMTestExpectedResults CRegistrar::ParseL() function parses SPI file without error.
						Data is interpreted correctly.
@SYMEC                  EC43
*/
void CRegistrarTest::ParseSpiType3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3566 "));
	TEntry spiEntry;
	TParse spiPath;
	//"Z:\\Test\\Data\\ecomtesttype3.spi" is used for testing. The only resource file included
	//in the spi file is EComExample12.RSC
	if (TheFs.Entry(KEComSpiTestFileType3PathAndName, spiEntry) != KErrNone)
		return;

	spiPath.Set(KEComSpiTestFileType3PathAndName,NULL,NULL);
	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFileType3PathAndName);
	CleanupClosePushL(resourceArchive);

	// Checks SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	CRegistryData::CDriveData* driveData = NULL;

	TDriveUnit driveUnit(spiPath.Drive());
	TInt error=iRegistryData->FindDriveL(driveUnit,driveData);
	test(error!=KErrNotFound);
	// there is only 1 resource file in the spi file
	while(!resourceArchive.End())
		{
		CPluginBase* entryBase = NULL;
		entryBase=CSpiPlugin::NewL(resourceArchive);
		CleanupStack::PushL(entryBase);
		CRegistryData::CDllData* dllData = NULL;
		dllData = CRegistryData::CDllData::NewLC(*(entryBase->iDllName),entryBase->iDllModifiedTime,entryBase->iDllSecondUid,entryBase->iDllThirdUid,driveData);
		TRAPD(err,iStateAccessor->ParseL(*iRegistrar,entryBase,*dllData));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);
		// Currently only one interface definition in the resource file
		// Checks the content of the DllData
		test(dllData->iIfList->Count() == 1);
		CRegistryData::CInterfaceData* ifData = NULL;
		ifData = (*(dllData->iIfList))[0];
		//There are 3 implementations for the above interface in the spi file.
		//Since ecomtesttype3.spi is under Z: drive which is ROM based,
		//the implementation (UID=0x10009E3D) will be created during Parsing.
		//So, the total implementations is 3.
		test(ifData->iImplementations->Count() == 3);
		// Interface UID verification
		test(ifData->iInterfaceUid == interfaceUidExample12);
		// Fetches the list of extended interfaces for the second implementation
		//Currently we have no extended Interface for the second implementation
		CRegistryData::CImplementationData* implData = NULL;
		implData = (*(ifData->iImplementations))[1];
		CImplementationInformation* implInfo = NULL;
		implInfo = implData->iImplInfo;

		RArray<TUid>* extendedInterfaceList = implInfo->GetExtendedInterfaceList();
		test(extendedInterfaceList->Count() == 0);
		CleanupStack::PopAndDestroy(dllData);
		CleanupStack::PopAndDestroy(entryBase);
		}
	CleanupStack::PopAndDestroy(&resourceArchive);
	}

/**
The test executes by registering the Interface implementaion information
for an plugin entry.

@SYMTestCaseID          SYSLIB-ECOM-CT-0732
@SYMTestCaseDesc	    Tests for CRegistrar::ParseRegistrationDataL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Checks for directory entry for an Interface Implementation
						Collection entry requires registration.Parses the associated resource file information where necessary
						Checks for OOM or parsing error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistrarTest::ParseRegistrationDataTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0732 "));
	TInt error=KErrNone;
	TTime modified(0);							//Dummy parameter...
	TBool update			= EFalse;			//Dummy parameter...
	TInt  driveIndex		= KErrNotFound;
	CRegistryData::CImplementationData *implementationData = NULL;
	CPluginBase* entryBase=NULL;
	TUid implementationUid;
	CRegistryData::CDriveData* driveData = NULL;

	// Calling  RegistryData->IsRegisteredWithDate to get the drive index,
	// that will be used in the ParseRegistrationDataL call
	// The drive should have been installed before registering any plugin
	TRAP(error, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
	::LeaveIfErrNoMemory(error);
	test(driveIndex != KErrNotFound);
	TRAP(error, iRegistryData->IsRegisteredWithDate(
																		NewDllUid,
																		modified,
																		update,

																		driveData));
	::LeaveIfErrNoMemory(error);
	test(error == KErrNone);

	//Creating the entry
	TEntry rscBasedEntry;
	rscBasedEntry.iName=KNewResourceFileNameOnly;
	error = TheFs.Modified(KNewResourceFileNameC, rscBasedEntry.iModified);
	test(error==KErrNone);
	TParse rscPath;
	rscPath.Set(KNewResourceFileNameC,NULL,NULL);
	entryBase=CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), EFalse);
	CleanupStack::PushL(entryBase);

	//Trying to parse the registration data
	update=ETrue;
	TRAP(error,iStateAccessor->ParseRegistrationDataL(*iRegistrar,entryBase,iDriveUnit,update,driveIndex, driveData));
	::LeaveIfErrNoMemory(error);
	test(error == KErrNone);
	CleanupStack::PopAndDestroy(entryBase);

	// Calling DiscoveriesComplete for building the registry indexes
	// as registration of new plugin has happened.
	iStateAccessor->DiscoveriesComplete(*iRegistrar, EFalse);

	//Now checking whether the implementation is already registered in the CRegistryData
	//The only implementation Uid in EComExample5. see 101F847B.RSS
	implementationUid=TUid::Uid(0x101F847C);
	TUid dummyUid;
	dummyUid.iUid = 0;
	error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNone);
	test(implementationData != NULL);

	}

/**
The test executes by registering the Interface implementation information
for an plugin with type 3 entry.

@SYMTestCaseID          SYSLIB-ECOM-UT-3567
@SYMTestCaseDesc	    Tests for CRegistrar::ParseRegistrationDataL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Checks for directory entry for an Interface Implementation
						Collection with Type 3 entry requires registration.Parses the associated resource file information where necessary
@SYMTestExpectedResults CRegistrar::ParseRegistrationDataL() function parses resource file and registers
						implementations correctly.
@SYMEC                  EC43
*/
void CRegistrarTest::ParseRegistrationDataType3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3567 "));
	TInt error=KErrNone;
	TTime modified(0);							//Dummy parameter...
	TBool update			= EFalse;			//Dummy parameter...
	TInt  driveIndex		= KErrNotFound;
	CRegistryData::CImplementationData *implementationData = NULL;
	CPluginBase* entryBase = NULL;
	TUid implementationUid;
	CRegistryData::CDriveData* driveData = NULL;

	// Calls iRegistryData->FindDriveL to get the drive index,
	// that will be used in the ParseRegistrationDataL call.
	// The drive should have been installed before registering any plugin
	TRAP(error, driveIndex = iRegistryData->FindDriveL(iDriveUnit, driveData));
	::LeaveIfErrNoMemory(error);
	test(driveIndex != KErrNotFound);
	// Calls iRegistryData->IsRegisteredWithDate to determine if the entry in the registry needs an update.
	iRegistryData->IsRegisteredWithDate(Type3DllUid, modified, update, driveData);

	//Creates the entry
	TEntry rscBasedEntry;
	rscBasedEntry.iName = KType3ResourceFileNameOnly;
	error = TheFs.Modified(KType3ResourceFileNameC, rscBasedEntry.iModified);
	test(error == KErrNone);
	TParse rscPath;
	rscPath.Set(KType3ResourceFileNameC,NULL,NULL);
	entryBase = CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), EFalse);
	CleanupStack::PushL(entryBase);

	//Try to parse the registration data
	TRAP(error,iStateAccessor->ParseRegistrationDataL(*iRegistrar,entryBase,iDriveUnit,update,driveIndex, driveData));
	::LeaveIfErrNoMemory(error);
	test(error == KErrNone);
	CleanupStack::PopAndDestroy(entryBase);

	// Calls DiscoveriesComplete for building the registry indexes
	// as registration of new plugin has happened.
	iStateAccessor->DiscoveriesComplete(*iRegistrar, EFalse);

	// Checks whether the implementations are already registered in the CRegistryData
	//The implementation Uids in EComExample12 are 0x10009E39, 0x10009E3C and 0x10009E3D
	// Checks implementation 0x10009E39
	implementationUid = TUid::Uid(0x10009E39);
	TUid dummyUid;
	dummyUid.iUid = 0;
	error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNone);
	test(implementationData != NULL);
	// Checks implementation 0x10009E3C
	implementationUid = TUid::Uid(0x10009E3C);
	dummyUid.iUid = 0;
	error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNone);
	test(implementationData != NULL);
	// Checks implementation 0x10009E3D
	// This implementation is said it's ROM-only but is actually in RAM.
	// So, this implementation will not be created.
	implementationUid = TUid::Uid(0x10009E3D);
	dummyUid.iUid = 0;
	error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNotFound);
	test(implementationData == NULL);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0090
@SYMTestCaseDesc 	Check that the ParseL for spi data works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecomtest.spi can be parsed successfully
					and no leave occurred.
@SYMTestExpectedResults The test must not fail.
@SYMREQ REQ3655
*/
void CRegistrarTest::ParseSpiRegistrationDataTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0090 "));
	TDriveUnit driveUnit(EDriveZ);
	TEntry spiEntry;
	test(TheFs.Entry(KEComSpiTestFilePathAndName, spiEntry) == KErrNone);
	TParse spiPath;
	spiPath.Set(KEComSpiTestFilePathAndName, NULL, NULL);

	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFilePathAndName);
	CleanupClosePushL(resourceArchive);

	// Checks SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	CPluginBase* entryBase=NULL;
	// there is only 1 resource file
	while(!resourceArchive.End())
		{
		const TTime modified(0);					//Dummy parameter...
		TBool update			= EFalse;			//Dummy parameter...
		TInt  driveIndex		= KErrNotFound;
		CRegistryData::CDriveData* driveData = NULL;

		// Calls RegistryData->IsRegisteredWithDate to get the drive index,
		// that will be used in the ParseRegistrationDataL call
		// The drive should have been installed before registering any plugin
		TRAPD(error, driveIndex = iRegistryData->FindDriveL(driveUnit, driveData));
		::LeaveIfErrNoMemory(error);
		test(driveIndex != KErrNotFound);
		TRAPD(err, (void)iRegistryData->IsRegisteredWithDate(

																	iDllUid,
																	modified,
																	update,

																	driveData));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);


		entryBase=CSpiPlugin::NewL(resourceArchive);
		CleanupStack::PushL(entryBase);

		TRAP(err, iStateAccessor->ParseRegistrationDataL(*iRegistrar,
														 entryBase,
														 driveUnit,
														 update,
														 driveIndex,
														 driveData));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);
		CleanupStack::PopAndDestroy(entryBase);
		entryBase = NULL;
		}

	CleanupStack::PopAndDestroy(&resourceArchive);

	// Calls DiscoveriesComplete for building the registry indexes
	// as registration of new plugin has happened.
	iStateAccessor->DiscoveriesComplete(*iRegistrar, EFalse);

	CRegistryData::CImplementationData *implementationData = NULL;
	TUid dummyUid;
	dummyUid.iUid = 0;
	TInt err = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, dummyUid, implementationData);
	test(err == KErrNone);
	test(implementationData != NULL);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-3568
@SYMTestCaseDesc 	Check that the ParseL for spi data works correctly.
@SYMTestPriority 	High
@SYMTestActions  	Ensure ecomtesttype3.spi can be parsed successfully
					and no leave occurs.
@SYMTestExpectedResults CRegistrar::ParseRegistrationDataL() function parses SPI file and registers
						implementations correctly.
@SYMEC                  EC43
*/
void CRegistrarTest::ParseSpiRegistrationDataType3TestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3568 "));
	TDriveUnit driveUnit(EDriveZ);
	TEntry spiEntry;
	//"Z:\\Test\\Data\\ecomtesttype3.spi" is used for testing. The only resource file included
	//in the spi file is EComExample12.RSC
	test(TheFs.Entry(KEComSpiTestFileType3PathAndName, spiEntry) == KErrNone);
	TParse spiPath;
	spiPath.Set(KEComSpiTestFileType3PathAndName, NULL, NULL);

	RResourceArchive resourceArchive;
	resourceArchive.OpenL(TheFs, KEComSpiTestFileType3PathAndName);
	CleanupClosePushL(resourceArchive);

	// Checks SPI file type
	TUid type = resourceArchive.Type();
	test(type == KEcomSpiFileTypeUid);

	CPluginBase* entryBase = NULL;
	// there is only 1 resource file in the spi file
	while(!resourceArchive.End())
		{
		const TTime modified(0);					//Dummy parameter...
		TBool update			= EFalse;			//Dummy parameter...
		TInt  driveIndex		= KErrNotFound;
		CRegistryData::CDriveData* driveData = NULL;

		// Calls RegistryData->FindDriveL to get the drive index,
		// that will be used in the ParseRegistrationDataL call.
		// The drive should have been installed before registering any plugin
		TRAPD(error, driveIndex = iRegistryData->FindDriveL(driveUnit, driveData));
		::LeaveIfErrNoMemory(error);
		test(driveIndex != KErrNotFound);
		iRegistryData->IsRegisteredWithDate(Type3DllUid,modified,update,driveData);

		entryBase=CSpiPlugin::NewL(resourceArchive);
		CleanupStack::PushL(entryBase);

		TRAPD(err, iStateAccessor->ParseRegistrationDataL(*iRegistrar,
														 entryBase,
														 driveUnit,
														 update,
														 driveIndex,
														 driveData));
		::LeaveIfErrNoMemory(err);
		test(err == KErrNone);
		CleanupStack::PopAndDestroy(entryBase);
		entryBase = NULL;
		}

	CleanupStack::PopAndDestroy(&resourceArchive);

	// Calls DiscoveriesComplete for building the registry indexes
	// as registration of new plugin has happened.
	iStateAccessor->DiscoveriesComplete(*iRegistrar, EFalse);

	CRegistryData::CImplementationData *implementationData=NULL;
	// Checks whether the implementations are already registered in the CRegistryData
	// The implementation Uids in EComExample12 are 0x10009E39, 0x10009E3C and 0x10009E3D
	// Checks implementation 0x10009E39
	TUid implementationUid = TUid::Uid(0x10009E39);
	TUid dummyUid;
	dummyUid.iUid = 0;
	TInt error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNone);
	test(implementationData != NULL);
	// Checks implementation 0x10009E3C
	implementationUid = TUid::Uid(0x10009E3C);
	dummyUid.iUid = 0;
	error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNone);
	test(implementationData != NULL);
	// Checks implementation 0x10009E3D
	// Since ecomtesttype3.spi is under Z: drive which is ROM based,
	// the implementation (UID=0x10009E3D) will be created during Parsing.
	implementationUid = TUid::Uid(0x10009E3D);
	dummyUid.iUid = 0;
	error = iRegistryDataStateAccessor->FindImplementation(*iRegistryData, implementationUid, dummyUid, implementationData);
	test(error == KErrNone);
	test(implementationData != NULL);
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-CT-0184
@SYMTestCaseDesc 	Check that SSA related states transition as expected when registration is staged
@SYMTestPriority 	High
@SYMTestActions  	Use CRegistrar::ProcessSSAEventL to start registration in stages i.e. rom only
then non rom only. Check that the
CRegistrar state is transitioning correctly at every step.
@SYMTestExpectedResults The test must not fail.
@SYMPREQ PREQ967
*/
void CRegistrarTest::StateTransitionTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0184 "));
	//After construction check that initial state is EReg_StartupInProgess
	test(iStateAccessor->State(*iRegistrar) == CRegistrar::EReg_StartupInProgess);

	//Call ProcessSSAEventL with EStartupStateUndefined. Check that state
	// is not changed. i.e. still EReg_StartupInProgess
	iRegistrar->ProcessSSAEventL(EStartupStateUndefined);
	test(iStateAccessor->State(*iRegistrar) == CRegistrar::EReg_StartupInProgess);

	//Call ProcessSSAEventL with EStartupStateCriticalStatic. Check that state
	// is not changed. i.e. still EReg_StartupInProgess
	iRegistrar->ProcessSSAEventL(EStartupStateCriticalStatic);
	test(iStateAccessor->State(*iRegistrar) == CRegistrar::EReg_StartupInProgess);

	//Call ProcessSSAEventL with EStartupStateCriticalDynamic. Check that state
	// is not changed. i.e. still EReg_StartupInProgess
	iRegistrar->ProcessSSAEventL(EStartupStateCriticalDynamic);
	test(iStateAccessor->State(*iRegistrar) == CRegistrar::EReg_StartupInProgess);

	//Call ProcessSSAEventL with EStartupStateNonCritical. Check that state
	//is changed to EReg_StartupComplete
	iRegistrar->ProcessSSAEventL(EStartupStateNonCritical);
	test(iStateAccessor->State(*iRegistrar) == CRegistrar::EReg_StartupComplete);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0733
@SYMTestCaseDesc	    Tests the creation and deletion of CRegistrar
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete CRegistrar object,checks for open handles
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDeleteTestL()
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0733 CreateDeleteTestL "));
	// Set up for heap leak checking
	__UHEAP_MARK;
	//Check Thread handles leak
	TInt startProcessHandleCount = 0;
	TInt startThreadHandleCount = 0;
	TInt endProcessHandleCount = 0;
	TInt endThreadHandleCount = 0;

	RThread rThread;
	rThread.HandleCount(startProcessHandleCount, startThreadHandleCount);

	//CREATE DELETE TEST START//

	CRegistrarTest* registrarTest = CRegistrarTest::NewL(EFalse);

	test(registrarTest != NULL);

	delete registrarTest;

	//CREATE DELETE TEST END//

	// Check for open handles
	rThread.HandleCount(endProcessHandleCount, endThreadHandleCount);
	test(startThreadHandleCount == endThreadHandleCount);

	//Test ends
	__UHEAP_MARKEND;
	}
/*
LOCAL_C void OOMCreateDeleteTest()
	{
	//
	// Out of memory test
	// ------------------------------------------------------------------
	//
	test.Next(_L("OOM CreateDeleteTest"));
	TInt err;
	TInt failAt = 0;
	__UNUSED_VAR(failAt);

	CRegistrarTest* registrarTest = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		// Setting Heap failure for OOM test
		__UHEAP_SETFAIL(RHeap::EDeterministic, ++failAt);

		TRAP(err, registrarTest = CRegistrarTest::NewL(EFalse));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete registrarTest;
		registrarTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		test(startProcessHandleCount == endProcessHandleCount);
		test(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}
	while (err == KErrNoMemory);

	test.Printf(_L("- Succeeded at heap failure rate of %i\n"), failAt);
	test(err == KErrNone);
	}
*/
// Type definition for pointer to member function.
// Used in calling the CDiscovererTest member function for testing.
typedef void (CRegistrarTest::*ClassFuncPtrL) (void);

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0735
@SYMTestCaseDesc	    Wrapper function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up test functions of CRegistrarTest.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions

@param		testFuncL pointer to test function
@param		aTestDesc test function name
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc, TBool aIsFullDiscovery)
	{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0735 "));
	test.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CRegistrarTest* registrarTest = CRegistrarTest::NewL(aIsFullDiscovery);
	CleanupStack::PushL(registrarTest);

	(registrarTest->*testFuncL)();

	CleanupStack::PopAndDestroy(registrarTest);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	test(startProcessHandleCount == endProcessHandleCount);
	test(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0736
@SYMTestCaseDesc	    Function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up all OOM test functions of CRegistrarTest.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Utility function to continually invoke a test function and cause memory allocation failures

@param		testFuncL pointer to OOM test function
@param	 	tryCount specifies what value to start memory allocation failures at
@param		increment how much to increase the point at which memory allocation will fail on each test attempt
@param		stopCount the value of memory allocation failure to stop testing at
*/
TInt RunTestUnderOOMCondition(ClassFuncPtrL testFuncL, TBool aIsFullDiscovery, TInt tryCount, TInt increment, TInt stopCount)
{
	test.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0736 "));
	TInt err = KErrNone;

	do
	{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		// Need to create object here as ECom has embedded TRAPs
		// that will fail the test if memory not available
		CRegistrarTest* registrarTest = CRegistrarTest::NewL(aIsFullDiscovery);
		CleanupStack::PushL(registrarTest);

		// Setting Heap failure for OOM test
 		__UHEAP_SETFAIL(RHeap::EDeterministic, tryCount);

		TRAP(err, (registrarTest->*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(registrarTest);
		registrarTest = NULL;

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
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc, TBool aIsFullDiscovery)
	{
	test.Next(aTestDesc);

	TInt startCount = 1;
	TInt increment = 1;
	TInt successRate = -1;
	TInt stopCount = 256;

 	successRate = RunTestUnderOOMCondition(testFuncL, aIsFullDiscovery, startCount, increment, stopCount);

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
 		successRate = RunTestUnderOOMCondition(testFuncL, aIsFullDiscovery, startCount, increment, stopCount);
 	test(successRate > 0);

 	if (successRate > 256)
 	{
 		startCount = successRate - 256;
 		increment = 1;
 		stopCount = -1;
 		successRate = RunTestUnderOOMCondition(testFuncL, aIsFullDiscovery, startCount, increment, stopCount);
 	}

 	test(successRate > 0);

	}

LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;

	// Basic tests
	CreateDeleteTestL();

	DoBasicTestL(&CRegistrarTest::EnableDisableTestL, _L("EnableDisableTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ResumeSuspendTestL, _L("ResumeSuspendTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::DriveRemovedReinstatedTestL, _L("DriveRemovedReinstatedTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::DiscoveriesBegin_Register_CompleteTestL, _L("DiscoveriesBegin_Register_CompleteTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseTestL, _L("ParseTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseType2TestL, _L("ParseType2TestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseType3TestL, _L("ParseType3TestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseType3BadDataTestL, _L("ParseType3BadDataTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseRegistrationDataTestL, _L("ParseRegistrationDataTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseRegistrationDataType3TestL, _L("ParseRegistrationDataType3TestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseSpiTestL, _L("ParseSpiTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseSpiType3TestL, _L("ParseSpiType3TestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseSpiRegistrationDataTestL, _L("ParseSpiRegistrationDataTestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::ParseSpiRegistrationDataType3TestL, _L("ParseSpiRegistrationDataType3TestL"), ETrue);
	DoBasicTestL(&CRegistrarTest::StateTransitionTestL, _L("StateTransitionTestL"), EFalse);
	// OOM tests
	// The OOM test for CRegistrar takes nearly 1 hour on hardware as it is reading the entire ecom.spi file
 	// Since ECOMSERVERTEST OOM test covered the full CRegistar OOM part, we will not run this the following
 	// test as that means 2 hours for ecom tests.
	//OOMCreateDeleteTest();
	DoOOMTestL(&CRegistrarTest::EnableDisableTestL, _L("OOM EnableDisableTestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::ResumeSuspendTestL, _L("OOM ResumeSuspendTestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::DriveRemovedReinstatedTestL, _L("OOM DriveRemovedReinstatedTestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::DiscoveriesBegin_Register_CompleteTestL, _L("OOM DiscoveriesBegin_Register_CompleteTestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::ParseTestL, _L("OOM ParseTestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::ParseType2TestL, _L("OOM ParseType2TestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::ParseType3TestL, _L("OOM ParseType3TestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::ParseSpiTestL, _L("OOM ParseSpiTestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::ParseSpiType3TestL, _L("OOM ParseSpiType3TestL"), ETrue);
	DoOOMTestL(&CRegistrarTest::StateTransitionTestL, _L("OOM StateTransitionTestL"), EFalse);
	// Not performing the OOM on ParseRegistrationDataL because of TRAPs present inside this function.
	// The OOM error is trapped inside during the creation of the DllData but this error is not propagated to
	// the calling funtion. Hence does not give the right results. However the normal test is performed above.
	//DoOOMTestL(&CRegistrarTest::ParseRegistrationDataTestL,_L("OOM ParseRegistrationDataTestL"));
	__UHEAP_MARKEND;
	}

// Copies the Plugins to specific folder for testing purpose
LOCAL_C void CopyPlugins()
	{
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManCopyFileL(KNewResourceFileNameZ, KNewResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KNewDllFileNameZ, KNewDllFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3ResourceFileNameZ, KType3ResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3DllFileNameZ, KType3DllFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3BadDataResourceFileNameZ, KType3BadDataResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3BadDataDllFileNameZ, KType3BadDataDllFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3BadData1ResourceFileNameZ, KType3BadData1ResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3BadData1DllFileNameZ, KType3BadData1DllFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3BadData2ResourceFileNameZ, KType3BadData2ResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType3BadData2DllFileNameZ, KType3BadData2DllFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType2ResourceFileNameZ, KType2ResourceFileNameC));
	test(err==KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KType2DllFileNameZ, KType2DllFileNameC));
	test(err==KErrNone);
	}

// Deleting plugin from the RAM for cleanup purpose
inline LOCAL_C void DeleteTestPlugin()
	{
	TInt err=KErrNone;
	TRAP(err, EComTestUtils::FileManDeleteFileL(KNewResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KNewDllFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3ResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3DllFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3BadDataResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3BadDataDllFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3BadData1ResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3BadData1DllFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3BadData2ResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType3BadData2DllFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType2ResourceFileNameC));
	TRAP(err, EComTestUtils::FileManDeleteFileL(KType2DllFileNameC));
	}

//Initialise the Active Scheduler
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
	test.Start( _L("Registrar Tests.") );

	TheTrapCleanup = CTrapCleanup::New();

	// Connect the file server instance
	User::LeaveIfError(TheFs.Connect());

	TRAPD(err, SetupL());
	test(err == KErrNone);

	// We need to clean up the registry files to ensure a fresh discovery that
	// has the effect of taking certain pathways within the code. This
	// tests the memory cleanup
	// Delete the prevous data files so we rediscover from scratch

	TRAP(err, EComTestUtils::FileManDeleteFileL(KDataDir));
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

	delete TheActiveScheduler;
	delete TheTrapCleanup;

	TheFs.Close();

	test.End();
	test.Close();

	__UHEAP_MARKEND;
	return (KErrNone);
	}
