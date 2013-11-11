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
// to test production class CRegistryData. Where necessary stubs
// are implemented to help in writing test harness using RTest.
// Test can fail if the Registry Index files are corrupted or missing.
// To solve that problem we need to run some component test harness, which
// runs by linking against ECom.lib. like t_listimplementation or t_suicide.
// 
//

#include <ecom/ecom.h>
#include "EComUidCodes.h"
#include "RegistrarObserver.h"
#include "RegistryData.h"
#include "Registrar.h"
#include "EComResolverParams.h"
#include "Discoverer.h"
#include "DiscovererObserver.h"
#include "DriveInfo.h"
#include "DowngradePath.h"
#include "../EcomTestUtils/EcomTestUtils.h"
#include "RegistryResolveTransaction.h"
#include "ParseImplementationData.h"

#include <ecom/ecompanics.h>
#include <s32file.h>
#include <e32test.h>
#include <f32file.h>
#include <bautils.h>
#include <babackup.h>
#include <hal.h>

#define UNUSED_VAR(a) a = a

// Used within tests
const TUid KCExampleInterfaceUid = {0x10009DC0};
const TUid KCExampleInterfaceImp = {0x10009DC3};
const TUid KCDummyUid = {0x00000000};
const TUid KCInvalidUid = {0x10009999};

const TInt KOneSecond = 1000000;

// Used for suppressing warning in OOM tests
#define __UNUSED_VAR(var) var = var

// Used for OOM test
#define TEST_OOM_ERR if(err == KErrNoMemory) User::Leave(err)

// The implemented structure for the registry data
typedef CRegistryData::CImplementationData* CImplementationDataPtr;
typedef RArray<CImplementationDataPtr> RImplDataArray;

// Test plugins and corresponding .rsc files that be copied to RAM for testing purpose
_LIT(KEComExDllOnZ,		"Z:\\RAMOnly\\EComExample5.dll");

_LIT(KEComExDllOnC,		"C:\\sys\\bin\\EComExample5.dll");
_LIT(KEComRscFileOnC,	"C:\\resource\\plugins\\EComExample5.rsc");
_LIT(KEComRscFileOnZ,	"Z:\\RAMOnly\\EComExample5.rsc");
_LIT(KEComExampleDllOnC,		"C:\\sys\\bin\\EComExample.dll");
_LIT(KEComExampleDllOnZSysBin,	"Z:\\sys\\bin\\EComExample.dll");
_LIT(KEComExample2DllOnC,		"C:\\sys\\bin\\EComExample2.dll");
_LIT(KEComExample3DllOnC,		"C:\\sys\\bin\\EComExample3.dll");

_LIT(KEComExampleRscOnC,		"C:\\resource\\plugins\\EComExample.rsc");
_LIT(KEComExample2RscOnC,		"C:\\resource\\plugins\\EComExample2.rsc");
_LIT(KEComExample3RscOnC,		"C:\\resource\\plugins\\EComExample3.rsc");

_LIT(KEComExampleRscOnZ,		"Z:\\RAMOnly\\EComExample.rsc");
_LIT(KEComExample2RscOnZ,		"Z:\\RAMOnly\\EComExample2.rsc");
_LIT(KEComExample3RscOnZ,		"Z:\\RAMOnly\\EComExample3.rsc");

_LIT(KEComExampleDllOnZ,			"Z:\\RAMOnly\\EComExample.dll");
_LIT(KEComExample2DllOnZ,		"Z:\\RAMOnly\\EComExample2.dll");
_LIT(KEComExample3DllOnZ,		"Z:\\RAMOnly\\EComExample3.dll");

// These RSC files/DLLs are used to test an RSC file with a SID that does not
// match it's corresponding DLL. The implementations should not be listed/created.
_LIT(KEComInvalidDllOnZ, "z:\\RAMOnly\\InvalidSIDPlugin.dll");
_LIT(KEComInvalidRscOnZ, "z:\\RAMOnly\\InvalidSIDPlugin.rsc");
_LIT(KEComInvalidDllOnC, "c:\\sys\\bin\\InvalidSIDPlugin.dll");
_LIT(KEComInvalidRscOnC, "c:\\resource\\plugins\\InvalidSIDPlugin.rsc");

// This DLL does not exist, and should fail a security check
_LIT(KEComMissingDll, "c:\\sys\\bin\\NonExistentDllName.dll");

// These RSC files/DLLs are used to test an RSC file that points to a missing DLL
_LIT(KMissingDllRomRscFile, 		"z:\\RAMOnly\\EComRomRslvrExampleOnC.rsc");
_LIT(KMissingDllRamRscFile, 		"c:\\resource\\plugins\\EComRomRslvrExampleOnC.rsc");
// When the above RSC file's security check fails, the following implementations should be rolled back to
// (even though the filenames are EComRomRslvrExampleOnZ they will be copied to and exist on C Drive)
_LIT(KRollbackForMissingDllRomDllFile, 			"z:\\RAMOnly\\EComRomRslvrExampleOnZ.dll");
_LIT(KRollbackForMissingDllRamDllFile, 			"c:\\sys\\bin\\EComRomRslvrExampleOnZ.dll");
_LIT(KRollbackForMissingDllRomRscFile, 			"z:\\RAMOnly\\EComRomRslvrExampleOnZ.rsc");
_LIT(KRollbackForMissingDllRamRscFile, 			"c:\\resource\\plugins\\EComRomRslvrExampleOnZ.rsc");

//PLUGIN1 DLL with interfaceID: 0x10009E36
_LIT(KEComExample14DllOnC,		"C:\\sys\\bin\\EComExample14.dll");
_LIT(KEComExample14DllOnZ,		"Z:\\RAMOnly\\EComExample14.dll");
_LIT(KEComExample14RscOnC,		"C:\\resource\\plugins\\EComExample14.rsc");
_LIT(KEComExample14RscOnZ,		"Z:\\RAMOnly\\EComExample14.rsc");
//PLUGIN3 DLL with interfaceID: 0x10009E36, implementation version 2
_LIT(KEComExample15DllOnC,		"C:\\sys\\bin\\EComExample15.dll");
_LIT(KEComExample15DllOnZ,		"Z:\\RAMOnly\\EComExample15.dll");
_LIT(KEComExample15RscOnC,		"C:\\resource\\plugins\\EComExample15.rsc");
_LIT(KEComExample15RscOnZ,		"Z:\\RAMOnly\\EComExample15.rsc");
//PLUGIN3 DLL with interfaceID: 0x10009E36
_LIT(KEComExample12DllOnC,		"C:\\sys\\bin\\EComExample12.dll");
_LIT(KEComExample12DllOnZ,		"Z:\\RAMOnly\\EComExample12.dll");
_LIT(KEComExample12RscOnC,		"C:\\resource\\plugins\\EComExample12.rsc");
_LIT(KEComExample12RscOnZ,		"Z:\\RAMOnly\\EComExample12.rsc");

_LIT(KEComExample10DllOnC,		"C:\\sys\\bin\\EComExample10.dll");
//PLUGIN3 Upgraded
_LIT(KEcomExample12UpgradedOnC, "C:\\resource\\plugins\\EComExample12Upgraded.rsc");
_LIT(KEcomExample12UpgradedOnZ, "Z:\\RAMOnly\\EComExample12Upgraded.rsc");
_LIT(KEcomExample12UpgradedResourceNameOnly, "EComExample12Upgraded.rsc");
//PLUGIN1 Upgraded to PLUGIN3
_LIT(KEcomExample14UpgradedOnC, "C:\\resource\\plugins\\EComExample14Upgraded.rsc");
_LIT(KEcomExample14UpgradedOnZ, "Z:\\RAMOnly\\EComExample14Upgraded.rsc");
_LIT(KEcomExample14UpgradedResourceNameOnly, "EComExample14Upgraded.rsc");
//PLUGIN3 Downgraded to PLUGIN1
_LIT(KEcomExample12DowngradedOnC, "C:\\resource\\plugins\\EComExample12Downgraded.rsc");
_LIT(KEcomExample12DowngradedOnZ, "Z:\\RAMOnly\\EComExample12Downgraded.rsc");
_LIT(KEcomExample12DowngradedResourceNameOnly, "EComExample12Downgraded.rsc");

_LIT(KDriveC, "C");

#ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
    _LIT(KEnglishLocale, "elocl.01");
    _LIT(KEComSpanishLocale, "elocl.04");
#else
    // refer base/os/kernelhwsrv/kernel/eka/euser/locmapping.cpp
    // Language
    _LIT(KEComEnglishLang,"elocl_lan.001");
    //_LIT(KEComFrenchLang,"elocl_lan.002");
    //_LIT(KEComGermanLang,"elocl_lan.003");
    _LIT(KEComSpanishLang,"elocl_lan.004");
    // Region
    _LIT(KEComEnglishReg,"elocl_reg.826");
    //_LIT(KEComFrenchReg,"elocl_reg.250");
    //_LIT(KEComGermanReg,"elocl_reg.276");
    _LIT(KEComSpanishReg,"elocl_reg.724");
    // Collation
    _LIT(KEComEnglishColl,"elocl_col.001");
    //_LIT(KEComFrenchColl,"elocl_col.002");
    //_LIT(KEComGermanColl,"elocl_col.003");
    _LIT(KEComSpanishColl,"elocl_col.001");
#endif

LOCAL_D CTrapCleanup*		TheTrapCleanup=NULL;

LOCAL_D CActiveScheduler*	TheActiveScheduler=NULL;

LOCAL_D RFs					TheFs;

LOCAL_D RTest				TheTest(_L("t_registrydata.exe"));

//TPtrC defined which will point to KEComSpiFilePathAndName or KEComSpiFilePathAndNameForNand
//according to the rom build type
LOCAL_D TPtrC				TheEComSpiFilePathAndNamePtrC;

TCapabilitySet dummycaps;

LOCAL_C void CopyPluginsL();

inline LOCAL_C void DeleteTestPlugin()
	{
	TRAPD(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComRscFileOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExampleDllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample2DllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample3DllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExampleRscOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample2RscOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample3RscOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComInvalidDllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComInvalidRscOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KMissingDllRamRscFile));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KRollbackForMissingDllRamDllFile));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KRollbackForMissingDllRamRscFile));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample12DllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample12RscOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample14DllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample14RscOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample15DllOnC));
 	TRAP(ignoreErr, EComTestUtils::FileManDeleteFileL(KEComExample15RscOnC));
	}

//
//
//Test macroses and functions
LOCAL_C void Check(TInt aValue, TInt aLine)
	{
	if(!aValue)
		{
		::DeleteTestPlugin();
		TheTest(EFalse, aLine);
		}
	}
LOCAL_C void Check(TInt aValue, TInt aExpected, TInt aLine)
	{
	if(aValue != aExpected)
		{
		RDebug::Print(_L("*** Expected error: %d, got: %d\r\n"), aExpected, aValue);
		::DeleteTestPlugin();
		TheTest(EFalse, aLine);
		}
	}
/**
Sets up the system-wide locale and language downgrade path.
*/
LOCAL_C void SetLanguageL(const TLanguage aLang[3])
	{
	TLocale locale;
	locale.SetLanguageDowngrade(0, aLang[0]);
	locale.SetLanguageDowngrade(1, aLang[1]);
	locale.SetLanguageDowngrade(2, aLang[2]);
	User::LeaveIfError(locale.Set());
	}

#define TEST(arg) ::Check((arg), __LINE__)
#define TEST2(aValue, aExpected) ::Check(aValue, aExpected, __LINE__)
//
//

/**
This class allows us to provide stub behavior to instantiate CRegistrar
*/
class TRegistrarObserverStub : public MRegistrarObserver
	{
public:
/**
The overloaded method from the MRegistrarObserver class Used for firing
notifications to the client sessions that the underlying registry data has changed.
@post		The CEComServer has notified all its client sessions
			registered for notifications that the data changed
*/
	void Notification(TInt aNotificationCode)
		{
		RDebug::Print(_L("TRegistrarObserverStub called with notification code %d.\n"), aNotificationCode);
		};
	};

/**
This friend class allows us to access private and protected members of production
code class CDiscoverer.
*/
class TDiscoverer_StateAccessor
	{
public:
	TDiscoverer_StateAccessor() {};
	void ScanDirectoryL(CDiscoverer& aDiscoverer)
		{
		aDiscoverer.ProcessSSAEventL(EStartupStateNonCritical);
		}
	void ScanDriveL(CDiscoverer& aDiscoverer, TDriveUnit& aDrive)
		{
		TBool isRO = aDiscoverer.iCachedDriveInfo->DriveIsReadOnlyInternalL(aDrive);
		aDiscoverer.iDirScanner->ScanDriveL(aDrive, isRO);
		}
	};

/**
This friend class allows us to access private and protected members of production
code class CRegistrar.
*/
class TRegistrar_StateAccessor
	{
public:
	TRegistrar_StateAccessor() {};
	void ScanDirectoryL(CRegistrar& aRegistrar)
		{
		TDiscoverer_StateAccessor discovererStateAccessor;
		discovererStateAccessor.ScanDirectoryL(*(aRegistrar.iDiscoverer));
		}
	void ScanDriveL(CRegistrar& aRegistrar, TDriveUnit& aDrive)
		{
		TDiscoverer_StateAccessor discovererStateAccessor;
		discovererStateAccessor.ScanDriveL(*(aRegistrar.iDiscoverer), aDrive);
		}
	void ParseL(CRegistrar& aRegistrar, CPluginBase*& aEntry,CRegistryData::CDllData& aDll);
	};

void TRegistrar_StateAccessor::ParseL(CRegistrar& aRegistrar, CPluginBase*& aEntry,CRegistryData::CDllData& aDll)
		{
		aRegistrar.ParseL(aEntry,aDll);
		}


/**
This friend class allows us to access private and protected members of production
code class CRegistryData.
*/
class TRegistryData_StateAccessor
	{
public:
	TBool IsIndexValid(CRegistryData& aRegData);

	TInt IndexedFind(CRegistryData& aRegData, const TUid& aImplementationUid);

	TBool IsRegisteredDriveL(CRegistryData& aRegData, TDriveUnit aDriveUnit);

	TInt RegistrationsCount(CRegistryData& aRegData);

	TInt ImplementationsCount(CRegistryData& aRegData);

	void SetRegistryChanged(CRegistryData& aRegData, TBool aChanged, TDriveUnit aDrive);

	void ClearImplementationIndexList(CRegistryData& aRegData);

	void ClearRegistrations(CRegistryData& aRegData);

	void ResetAndDestroy(CRegistryData& aRegData);

	void ConstructL(CRegistryData& aRegData);

	TBool FindAnImplementation(CRegistryData& aRegData, TUid& aInterfaceUid, TDriveUnit& aDrive);

	void GetImplementationInfoL(CRegistryData& aRegData, TUid& aInterfaceUid, RImplDataArray& aImplementationInfo);

	void AppendDriveDataL(CRegistryData& aRegData, CRegistryData::CDriveData* aDriveData);

	TInt FindImplementation(CRegistryData& aRegData, const TUid aImplUid, const TUid aInterfaceUid,
										CRegistryData::CImplementationData*& aImplData) const;
	static void ResetStaticDriveData();
	TInt GetDllListL(CRegistryData& aRegistryData, TDriveUnit aDriveUnit, TDll*& aDllList);
	TBool IsLanguageInitChanged(CRegistryData& aRegistryData);
	TInt DrivesRemoved(CRegistryData& aRegistryData);
	};

/**
Constructs the registry

*/
void TRegistryData_StateAccessor::ConstructL(CRegistryData& aRegData)
	{
	aRegData.ConstructL();
	}

/**
Resets the registry

*/
void TRegistryData_StateAccessor::ResetAndDestroy(CRegistryData& aRegData)
	{
	aRegData.iRegistrations->ResetAndDestroy();
	delete aRegData.iRegistrations;
	}

/**
Clears the Implementations
@param		aRegData RegistryData object under test
*/
void TRegistryData_StateAccessor::ClearImplementationIndexList(CRegistryData& aRegData)
	{
	for(TInt index = 0; index < aRegData.iInterfaceImplIndex.Count(); ++index)
		{
		aRegData.iInterfaceImplIndex[index].Reset();
		}
	aRegData.iInterfaceImplIndex.Reset();
	aRegData.iImplIndex.Reset();
	}


/**
Clears the Registrations
@param		aRegData RegistryData object under test
*/
void TRegistryData_StateAccessor::ClearRegistrations(CRegistryData& aRegData)
	{
	if(aRegData.iRegistrations)
		{
		aRegData.iRegistrations->ResetAndDestroy();
		}
	}

/**
Indicates whether the registry index is currently valid. The
index will not be valid if discoveries are currently taking place.
@param		aRegData RegistryData object under test
@return		ETrue if index is valid EFalse otherwise
*/
TBool TRegistryData_StateAccessor::IsIndexValid(CRegistryData& aRegData)
	{
	return aRegData.IndexValid();
	}

/**
Tries to find the index entry of the passed implementation uid
@param		aRegData RegistryData object under test
@param		aImplementationUid the implementation Uid of the implementation to locate
@return		Index of the implementation, KErrNotFound otherwise
*/
TInt TRegistryData_StateAccessor::IndexedFind(CRegistryData& aRegData,
											  const TUid& aImplementationUid)
	{
	return aRegData.IndexedFind(aImplementationUid);
	}

/**
Checks if the drive is already registered
@param		aRegData RegistryData object under test
@param		aDriveUnit the drive on which plugins are registered
@return		ETrue if drive is registered, EFalse otherwise
*/
TBool TRegistryData_StateAccessor::IsRegisteredDriveL(CRegistryData& aRegData,
													 TDriveUnit aDriveUnit)
	{
	TInt driveIndex = KErrNotFound;
	CRegistryData::CDriveData* aDriveData=NULL;
	driveIndex = aRegData.FindDriveL(aDriveUnit, aDriveData);
	// Return True if the driveindex has some valid value i.e. not KErrNotFound
	return (driveIndex != KErrNotFound);
	}

/**
Obtains count of plugin registrations
@param		aRegData RegistryData object under test
@return		Number of registrations
*/
TInt TRegistryData_StateAccessor::RegistrationsCount(CRegistryData& aRegData)
	{
	return aRegData.iRegistrations->Count();
	}

/**
Obtains count of plugin registrations
@param		aRegData RegistryData object under test
@return		Number of registrations
*/
TInt TRegistryData_StateAccessor::ImplementationsCount(CRegistryData& aRegData)
	{
	return aRegData.iInterfaceImplIndex.Count();
	}

/**
Sets Registry Changed flag to the given value
@param		aRegData RegistryData object under test
@param		aChanged True if the registry is changed
@param		aDrive the drive under test
*/
void TRegistryData_StateAccessor::SetRegistryChanged(CRegistryData& aRegData, TBool aChanged, TDriveUnit aDrive)
	{
	for(TInt i = 0; i <aRegData.iRegistrations->Count(); i++)
		{
		if ((*aRegData.iRegistrations)[i]->iDrive == aDrive)
			{
			(*aRegData.iRegistrations)[i]->iRegistryChanged = aChanged;
			}
		}
	}

/**
See if an implementation is discovered on the questioned drive
@param		aRegData RegistryData object under test
@param		aInterfaceUid Interface UID for the implementation
@param		aDrive Drive to search the implementation on
@return 	ETrue if an implementation exists
*/
TBool TRegistryData_StateAccessor::FindAnImplementation(CRegistryData& aRegData, TUid& aInterfaceUid, TDriveUnit& aDrive)
	{
	const TInt driveCount = aRegData.iRegistrations->Count();
	for(TInt driveIndex = 0; driveIndex < driveCount; ++driveIndex)
		{
		CRegistryData::CDriveData* driveData = (*aRegData.iRegistrations)[driveIndex];
		if(driveData->iDrive == aDrive)
			{
			TDll* dllList = (*aRegData.iRegistrations)[driveIndex]->iDllList;
			const TInt dllCount = dllList->Count();
			for(TInt dllIndex = 0; dllIndex < dllCount;++dllIndex)
				{
				// For each dll structure
				RInterfaceList* interfaceList = (*dllList)[dllIndex]->iIfList;
				const TInt ifCount = interfaceList->Count();
				for(TInt ifIndex = 0; ifIndex < ifCount; ++ifIndex)
					{
					CRegistryData::TInterfaceIndex index;

					// For each interface structure
					TUid interfaceUid = (*interfaceList)[ifIndex]->iInterfaceUid;
					if(interfaceUid == aInterfaceUid)
						{
						return ETrue;
						}
					}
				}
			}
		}

	return EFalse;
	}

/**
Get implementation info from the index
@param		aRegData RegistryData object under test
@param		aInterfaceUid Interface UID for the implementation
@param  	aImplementationData Reference of array of Implementation Data
*/
void TRegistryData_StateAccessor::GetImplementationInfoL(CRegistryData& aRegData, TUid& aInterfaceUid, RImplDataArray& aImplementationData)
	{
	aRegData.ListImplementationsL(aInterfaceUid, aImplementationData);
	}

/**
Append DriveSata to iRegistrations
@param		aRegData RegistryData object under test
@param		aInterfaceUid Interface UID for the implementation
@return 	Array of Implementations
*/
void TRegistryData_StateAccessor::AppendDriveDataL(CRegistryData& aRegData, CRegistryData::CDriveData* aDriveData)
	{
	aRegData.iRegistrations->AppendL(aDriveData);
	}

/**
@param		aRegistryData The CRegistryData class object
@param		aImplUid The implementation to find.
@param		aInterfaceUid If greater than 0 the interface associated with the
			implementation to find.
@param		aImplData The found implementation data.
@return		KErrNone if found otherwise KErrNotFound
*/
TInt TRegistryData_StateAccessor::FindImplementation(CRegistryData& aRegistryData,
												const TUid aImplUid,
												const TUid aInterfaceUid,
												CRegistryData::CImplementationData*& aImplData) const
	{
	return aRegistryData.FindImplementation(aImplUid, aInterfaceUid, aImplData);
	}

/**
@param		aRegistryData The CRegistryData class object
@param		aDriveUnit A drive unit
@param		aDllList The dll list returned on a specified drive.
@return		Index of the specified drive in registryData if found otherwise KErrNotFound
*/
TInt TRegistryData_StateAccessor::GetDllListL(CRegistryData& aRegistryData, TDriveUnit aDriveUnit, TDll*& aDllList)
{
	CRegistryData::CDriveData* drive =NULL;

	// Get the drive data in the registry.
	TInt registryDriveIndex = aRegistryData.FindDriveL(aDriveUnit, drive);
	if(registryDriveIndex != KErrNotFound)
		{
		aDllList = (*aRegistryData.iRegistrations)[registryDriveIndex]->iDllList;
		}
	return registryDriveIndex;
}

/**
Indicates whether the language downgrade path has been changed during start-up time.
@param		aRegData RegistryData object under test
@return		ETrue if index is valid EFalse otherwise
*/
TBool TRegistryData_StateAccessor::IsLanguageInitChanged(CRegistryData& aRegistryData)
{
	return aRegistryData.iLanguageChanged;
}

TInt TRegistryData_StateAccessor::DrivesRemoved(CRegistryData& aRegistryData)
	{
		return aRegistryData.iRemovedDrives;
	}

/** class to hold all the data required to create an instance of
CRegistryData::CImplementationData.
NB: the pointers in this struct are all owned by some other objects.
*/
_LIT8(KDefaultImplDataType, "Data Type");
_LIT8(KDefaultImplOpaqueData, "");

struct TImplCreationStruct
	{
	TImplCreationStruct(TDriveNumber aDriveNumber,
						TUid  aInterfaceUid,
						TUid  aImplUid,
						const TDesC& aDllPathName,
						const TDesC& aImplDisplayName,
						const TDesC8& aImplDataType = KDefaultImplDataType,
						const TDesC8& aImplOpaqueData = KDefaultImplOpaqueData,
						TInt  aImplVersion = 1,
						TBool aRomBased = EFalse,
						TBool aRomOnly = EFalse);

	TDriveUnit		iDriveUnit;
	TUid			iInterfaceUid;
	TUid			iImplUid;
	const TDesC&	iDllPathName;
	const TDesC&	iImplDisplayName;
	const TDesC8&	iImplDataType;
	const TDesC8&	iImplOpaqueData;
	TInt			iDriveIndex;
	CRegistryData::CDriveData*		iDriveData;
	CRegistryData::CDllData*		iDllData;
	CRegistryData::CInterfaceData*	iInterfaceData;
	CRegistryData::CImplementationData* iImplData;
	TInt			iImplVersion;
	TBool			iRomBased;
	TBool			iRomOnly;
	};

// TImplCreationStruct Constructor
TImplCreationStruct::TImplCreationStruct(TDriveNumber aDriveNumber,
										TUid  aInterfaceUid,
										TUid  aImplUid,
										const TDesC& aDllPathName,
										const TDesC& aImplDisplayName,
										const TDesC8& aImplDataType,
										const TDesC8& aImplOpaqueData,
										TInt  aImplVersion,
										TBool aRomBased,
										TBool aRomOnly)
	: iDriveUnit(aDriveNumber), iInterfaceUid(aInterfaceUid),
	iImplUid(aImplUid), iDllPathName(aDllPathName),
	iImplDisplayName(aImplDisplayName), iImplDataType(aImplDataType),
	iImplOpaqueData(aImplOpaqueData), iDriveIndex(KErrNotFound),
	iDriveData(NULL), iDllData(NULL), iInterfaceData(NULL),
	iImplData(NULL), iImplVersion(aImplVersion), iRomBased(aRomBased),
	iRomOnly(aRomOnly)
	{
	}

/**
Test class encloses necessary members that aid to test CRegistryData
*/
class CRegistryDataTest: public CBase
	{
public:
	static CRegistryDataTest* NewL(TBool aIsFullDiscovery);

	virtual ~CRegistryDataTest();

	void AddDllDataPlugIn3TestL();
	void AddDllDataTestL();

	void IsRegisteredWithDateUpdateDllDataTestL();
	void CopyOrgPlugInL();
	void UpdateDllDataTest1L();
	void UpdateDllDataTest2L();
	void UpdateDllDataTest3L();

	void SetEnabledStateTestL();

	void FindImplementationTestL();
	//Test for ListImplementationsL() in CRegistryData
	void ListImplementationsL();
	//Test for ListImplementationsL() in CRegistryResolverTransaction
	void ListImplementationsTestT1L();
	void ListImplementationsTestT2L();
	void ListImplementationsTestT3L();
	void ListImplementationsTestT4L();
	void ListImplementationsTestT5L();

	//Test for ListExtendedInterfacesL() in CRegistryData
	void ListExtendedInterfacesTestL();

	void DoUndoTemporaryUninstallTestL();

	void DiscoveriesBeginCompleteTestL();

	void IndexedFindTestL();

	void GetImplementationDllInfoForServerTest();

	void GetImplementationDllInfoForClientTestL();

	void GetImplementationDllInfoForServerPlugIn3Test();

	void GetImplementationDllInfoForClientPlugIn3TestL();

	void VerifySelectNewPluginTypeL();

	void AllAtOnceDiscoveryStateTransitionTestL();

	void StagedDiscoveryStateTransitionTestL();

	void ImplementationDataTestL();

	void InsertIntoIndexRemoveFromIndexTestL();

	void RollbackTestL();

	void ProcessSecurityCheckTestL();

	void FailedSecurityCheckTestListImplementationsL();

	void FailedSecurityCheckTestGetImplementationInfoL();

	void IsAnyDllDiscoveredTestL();
	void IsAnyDllDiscoveredTestNoSpiL();
	void IsAnyDllDiscoveredTestWithSpiL();

	void CheckDuplicatPluginsOnCL();

	void CheckNoPluginsOnCL();

	void DuplicateImplUidTestL();
	void TestDuplicatedImplPrecedenceL(const TImplCreationStruct& highprecedence,
									const TImplCreationStruct& lowprecedence);

	static TBool SpiFileExists();

	void LanguageChangedTestL();

	void DrivesRemovedTestL();
private:
	CRegistryDataTest();

	void ConstructL(TBool aIsFullDiscovery);

	void CreateImplL(TImplCreationStruct& aCreationData);
public:
	/** The instance of the class under test */
	CRegistryData*						iRegistryData;

	/** Registrar observer test class */
	TRegistrarObserverStub* iRegistrarObserver;

	/** CRegistrar required for populating the CRegistryData */
	CRegistrar*							iRegistrar;
	/** CRegistrar state accessor */
	TRegistrar_StateAccessor* iRegistrarAccessor;

	/** A list of returned UIDs from the list methods */
	RImplInfoArray*						iUidList;

	/** A list of implementation data from the list methods */
	RImplDataArray						iImplDataList;

	/** Friend class pointer used for accessing private members */
	TRegistryData_StateAccessor*		iStateAccessor;

	/** The current drive unit identifier */
	TDriveUnit							iDriveUnit;

	/** UID for an implementation of an interface */
	TUid								iImplementationUid;

	/** UID to identify a particular interface */
	TUid								iInterfaceUid;

	/** The directory entry data for a dll */
	TEntry								iDllEntry;

	/** Flag to indicate if the registry entry requires an update */
	TBool								iUpdate;

	/** The drive index returned by IsRegistered... so we know which drive to update on */
	TInt								iFoundDriveIndex;

	};

/**
Creates a new CRegistryDataTest object
@param		aIsFullDriveCDiscovery whether the constructor make full discovery on Drive C
@return		A pointer to the newly created class.
*/
CRegistryDataTest* CRegistryDataTest::NewL(TBool aIsFullDriveCDiscovery)
	{
	CRegistryDataTest* self = new (ELeave) CRegistryDataTest();
	CleanupStack::PushL(self);
	self->ConstructL(aIsFullDriveCDiscovery);
	CleanupStack::Pop();
	return self;
	}

/**
Standardized default constructor
@post		CRegistryDataTest is fully constructed.
*/
CRegistryDataTest::CRegistryDataTest()
:	CBase()
	{
	iDriveUnit = EDriveC;

	TUid thirdUid = {0x101F847B};

	iDllEntry.iType = TUidType(KNullUid, KNullUid, thirdUid);
	iDllEntry.iName = KEComExDllOnC;

	iInterfaceUid.iUid		= KCExampleInterfaceUid.iUid;
	iImplementationUid.iUid	= KCExampleInterfaceImp.iUid;
	}

/**
Standardized 2nd(Initialization) phase of two phase construction.
Completes the safe construction of the CRegistryDataTest object
@param		aIsFullDriveCDiscovery whether the constructor make full discovery on Drive C
@post		CRegistryDataTest is fully constructed.
@leave		KErrNoMemory.
*/
void CRegistryDataTest::ConstructL(TBool aIsFullDriveCDiscovery)
	{
	iUidList = new(ELeave) RImplInfoArray;
	iStateAccessor= new(ELeave) TRegistryData_StateAccessor;
	iRegistryData = CRegistryData::NewL(TheFs);
	iRegistrarObserver=new (ELeave) TRegistrarObserverStub;
	iRegistrarAccessor=new (ELeave) TRegistrar_StateAccessor;
	iRegistrar=CRegistrar::NewL(*iRegistryData, *iRegistrarObserver, TheFs);
	if (aIsFullDriveCDiscovery)
		{
		TDriveUnit driveUnitC = EDriveC;
		iRegistrarAccessor->ScanDriveL(*iRegistrar,driveUnitC);
		}
	}

/**
Standard destructor
*/
CRegistryDataTest::~CRegistryDataTest()
	{
	delete iUidList;
	delete iStateAccessor;
	delete iRegistrarAccessor;
	delete iRegistrar;
	delete iRegistrarObserver;
	delete iRegistryData;
	}

/**
 @return ETrue if the ECom.spi file exists on Z drive, EFalse if it does not
*/
TBool CRegistryDataTest::SpiFileExists()
	{
	TEntry entry;
	if (TheFs.Entry(TheEComSpiFilePathAndNamePtrC, entry) == KErrNone)
		{
		return ETrue;
		}
	return EFalse;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3712
@SYMTestCaseDesc	    Tests for AddExtendedInterfaceL when adding duplicate extended interface Uid.
@SYMTestPriority 	    High
@SYMTestActions  	    Call CImplementationInformation::AddExtendedInterfaceL() with duplicate extended
						interface Uid.
						AddExtendedInterfacePanicL() and ThreadPanicTest() are used for this test case.
@SYMTestExpectedResults Ensure that the AddExtendedInterfaceL panics in debug mode, but duplicate extended
						interface is ignored in release mode.
@SYMDEF                 DEF111196
*/
LOCAL_C void AddExtendedInterfaceTestL()
	{
	
	// Test Starts...
	TUid testImplUid;
	testImplUid.iUid = KCExampleInterfaceUid.iUid;
	TInt testImplVersion = 2;
	_LIT(KTestImplName, "Implementation");
	_LIT8(KTestImplDataType, "Data Type");
	_LIT8(KTestImplOpaqueData, "Opaque Data");
	TUid testExtendedInterfaceUid1;
	TUid testExtendedInterfaceUid2;
	testExtendedInterfaceUid1.iUid = 0x10009E44;
	testExtendedInterfaceUid2.iUid = 0x10009E45;

	TDriveUnit testImplDrive = EDriveC;
	TBool testImplRomOnly = ETrue;
	TBool testImplRomBased = EFalse;

	CRegistryData* registryData = CRegistryData::NewL(TheFs);
	CleanupStack::PushL(registryData);
	CRegistryData::CDriveData* driveData = CRegistryData::CDriveData::NewLC(testImplDrive, registryData);
	CRegistryData::CDllData* dllData = CRegistryData::CDllData::NewLC(driveData);
	CRegistryData::CInterfaceData* interfaceData = CRegistryData::CInterfaceData::NewLC(dllData);

	// NewL test
	HBufC* name = KTestImplName().AllocLC();
	HBufC8* dataType = KTestImplDataType().AllocLC();
	HBufC8* opaqueData = KTestImplOpaqueData().AllocLC();

	RArray<TUid>* extendedInterfaces = new(ELeave) RArray<TUid>;

	CleanupStack::PushL(TCleanupItem(CloseAndDeleteArray, extendedInterfaces));

	CRegistryData::CImplementationData* implementationData;
	implementationData = CRegistryData::CImplementationData::NewL(interfaceData,
															testImplUid,
															testImplVersion,
															name,
															dataType,
															opaqueData,
															testImplDrive,
															testImplRomOnly,
															testImplRomBased,
															extendedInterfaces);

	CleanupStack::Pop(extendedInterfaces);
	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);

	CleanupStack::PushL(implementationData);
	TEST(interfaceData == implementationData->iParent);

	//Add extended interface
	implementationData->iImplInfo->AddExtendedInterfaceL(testExtendedInterfaceUid1);
	implementationData->iImplInfo->AddExtendedInterfaceL(testExtendedInterfaceUid2);
	const RArray<TUid>* extendedInterfacesList = implementationData->iImplInfo->GetExtendedInterfaceList();
	TEST(extendedInterfacesList->Count() == 2);
	RDebug::Printf("Number of extended interfaces = %d",extendedInterfacesList->Count());
	// Add duplicate extended interface, will panic in debug mode, but duplicate extended interface
	// is ignored in release mode
	implementationData->iImplInfo->AddExtendedInterfaceL(testExtendedInterfaceUid2);

#ifdef NDEBUG
	const RArray<TUid>* extendedInterfacesList1 = implementationData->iImplInfo->GetExtendedInterfaceList();
	TEST(extendedInterfacesList1->Count() == 2);
	RDebug::Printf("Number of extended interfaces after adding duplicate extended interface = %d",extendedInterfacesList->Count());
#endif
	// Tidy up
	CleanupStack::PopAndDestroy(5, registryData);

	// Test Ends...
	}

LOCAL_C TInt AddExtendedInterfacePanicL(TAny*)
	{
	__UHEAP_MARK;
	CTrapCleanup* threadcleanup = CTrapCleanup::New();
	TRAPD(err,AddExtendedInterfaceTestL());
	TEST(err==KErrNone);
	delete threadcleanup;
	__UHEAP_MARKEND;
	return KErrNone;
	}

/**
The test code is used for capturing the PANIC that occurs as a result of calling AddExtendedInterfaceL
with duplicate extended interface Uid .
*/
LOCAL_C void ThreadPanicTest(const TDesC& aName,TThreadFunction aFunction)
	{
	TRequestStatus threadStatus;
	RThread thread;
	TBool jit;
	jit=User::JustInTime();
	User::SetJustInTime(EFalse);

	// Default stack size was doubled for X86, so reduce this multiplier to prevent panic
#ifdef __X86GCC__
	TInt err=thread.Create(aName,aFunction,KDefaultStackSize*4,KMinHeapSize,0x100000,0);
#else
	TInt err=thread.Create(aName,aFunction,KDefaultStackSize*8,KMinHeapSize,0x100000,0);
#endif	
	TEST(err==KErrNone);
	thread.Logon(threadStatus)	;
	thread.Resume();

	User::WaitForRequest(threadStatus);

	//Now check why the thread Exit
	// There is Assert_Debug check for the duplicate extended interface Uid for AddExtendedInterfaceL(),
	// Therefore, the panic reason would be KErrAlreadyExists for debug mode.
	// in release mode, the duplicate extended interface is ignored.
#ifdef _DEBUG
	RDebug::Printf("Thread status: %d", thread.ExitType());
	TEST(thread.ExitType()==EExitPanic);
	TEST(thread.ExitReason()==EEComPanic_CImlpementationInfromation_DuplicateExIf);
#else
	RDebug::Printf("Thread status: %d", thread.ExitType());
	TEST(thread.ExitType()==EExitKill);
#endif
	CLOSE_AND_WAIT(thread);
	User::SetJustInTime(jit);
	}


/**
Plugins to be used in this test
Interface UID	DLL	UID		Implementation UID	Version		DLL Type	Extended Interfaces
--------------------------------------------------------------------------------
0x10009E36		0x10009E3E	0x10009E39				1		PLUGIN3		{0x10009E44, 0x10009E45, 0x10009E46}
				0x10009E3E	0x10009E3C				1		PLUGIN3		{}
				0x10009E3E	0x10009E3D				1		PLUGIN3		{0x10009E44}
0x10009DC0		0x101F847B	0x101F847C				1		PLUGIN1		NULL

@SYMTestCaseID          SYSLIB-ECOM-UT-3575
@SYMTestCaseDesc	    Tests for CImplementationInformation::GetExtendedInterfaceList() and
						CImplementationInformation::GetExtendedInterfaceListL() functions. Makes sure that
						results from both of the functions are identical.
@SYMTestPriority 	    High
@SYMTestActions  	    List the extended interfaces for a valid implementation UID
						List the extended interfaces for a non-existing implementation UID
						List the extended interfaces for a valid PLUGIN1 implementation UID
@SYMTestExpectedResults Expected extended interfaces are returned.
@SYMEC                  EC43
@SYMDEF					DEF111196
*/
void CRegistryDataTest::ListExtendedInterfacesTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3575 "));
	TUid ImpUid = {0x10009E39};
	TUid ImpUid1 = {0x10009E3C};
	TUid ImpUidPLUGIN1 = {0x101F847C};
 	TUid extendedInterfacesUid[] = {0x10009E44,0x10009E45,0x10009E46};
 	TEntry dllinfo;
 	TInt err;
 	TClientRequest clntRq;

 	//Gets the Implementation Information for a valid implementation UID with extended interfaces
 	CImplementationInformation* implementationInfo=NULL;
 	TRAP(err,iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid, KCDummyUid, dllinfo, implementationInfo, ETrue));
 	TEST_OOM_ERR;
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid);

 	//Gets the list of extended Interfaces
 	RArray<TUid>* extendedInterfaces = implementationInfo->GetExtendedInterfaceList();

 	//Checks the extended interfaces list
 	TEST(extendedInterfaces != NULL);
	for (TInt i = 0; i < extendedInterfaces->Count(); i++)
		{
		TEST((*extendedInterfaces)[i] == extendedInterfacesUid[i]);
		}

	// Uses the GetExtendedInterfaceListL API to get the extended interface.
	RArray<TUid> extendedInterfacesAPI;
	CleanupClosePushL(extendedInterfacesAPI);
	implementationInfo->GetExtendedInterfaceListL(extendedInterfacesAPI);
	// Makes sure that the list return is the same as the one return from GetExtendedInterfaceList.
	TEST(extendedInterfacesAPI.Count() == extendedInterfaces->Count());
	for (TInt i = 0; i < extendedInterfaces->Count(); i++)
		{
		TEST((*extendedInterfaces)[i] == extendedInterfacesAPI[i]);
		}
	CleanupStack::PopAndDestroy(&extendedInterfacesAPI);

	//Gets the Implementation Information for a valid implementation UID without extended interfaces
 	TRAP(err,iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid1, KCDummyUid, dllinfo, implementationInfo, ETrue));
 	TEST_OOM_ERR;
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid1);

 	//Gets the list of extended Interfaces
	extendedInterfaces = implementationInfo->GetExtendedInterfaceList();
 	TEST(extendedInterfaces->Count() == 0);

	// Uses the GetExtendedInterfaceListL API to get the extended interface.
	RArray<TUid> extendedInterfacesAPI1;
	CleanupClosePushL(extendedInterfacesAPI1);
	implementationInfo->GetExtendedInterfaceListL(extendedInterfacesAPI1);
	// Makes sure that the list return is the same as the one return from GetExtendedInterfaceList.
	TEST(extendedInterfacesAPI1.Count() == extendedInterfaces->Count());
	for (TInt i = 0; i < extendedInterfaces->Count(); i++)
		{
		TEST((*extendedInterfaces)[i] == extendedInterfacesAPI1[i]);
		}
	CleanupStack::PopAndDestroy(&extendedInterfacesAPI1);

	//Gets the Implementation Information for a non-existing implementation UID
	TRAP(err,iRegistryData->GetImplementationDllInfoForClientL(clntRq, KCInvalidUid, KCDummyUid, dllinfo, implementationInfo, ETrue));
	TEST_OOM_ERR;
	TEST2(err,KErrNone);
	TEST(implementationInfo == NULL);

	//Gets the Implementation Information for a valid PLUGIN1 implementation UID without extended interfaces
 	TRAP(err,iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUidPLUGIN1, KCDummyUid, dllinfo, implementationInfo, ETrue));
 	TEST_OOM_ERR;
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUidPLUGIN1);

 	//Gets the list of extended Interfaces
	extendedInterfaces = implementationInfo->GetExtendedInterfaceList();
 	TEST(extendedInterfaces == NULL);

	// Uses the GetExtendedInterfaceListL API to get the extended interface.
	RArray<TUid> extendedInterfacesAPI2;
	CleanupClosePushL(extendedInterfacesAPI2);
	implementationInfo->GetExtendedInterfaceListL(extendedInterfacesAPI2);
	// Makes sure that the list return is the same as the one return from GetExtendedInterfaceList.
	TEST(extendedInterfacesAPI2.Count() == 0);
	CleanupStack::PopAndDestroy(&extendedInterfacesAPI2);
	}


/**
The interface Uid 0x10009E36 has 5 implementations on different DLLs(EComExample12.dll,EComExample14.dll and EComExample15.dll).
0x10009E3D is said to be ROM-Only, it will not be created. 0x10009E39 has higher version in EComExample15.
Interface UID	DLL	UID		Implementation UID	Version		DLL Type	Extended Interfaces
--------------------------------------------------------------------------------
0x10009E36		0x10009E40	0x10009E48				1		PLUGIN1		None
				0x10009E3E	0x10009E39				1		PLUGIN3		{0x10009E44, 0x10009E45, 0x10009E46}
				0x10009E3E	0x10009E3C				1		PLUGIN3		{}
				0x10009E3E	0x10009E3D				1		PLUGIN3		{0x10009E44}
				0x10009E42	0x10009E39				2		PLUGIN3		{0x10009E44, 0x10009E45, 0x10009E46}

@SYMTestCaseID          SYSLIB-ECOM-UT-3580
@SYMTestCaseDesc	    Tests for CRegistryResolveTransaction::ListImplementationsL() function with matches
						that fall into a PLUGIN1 Dll and PLUGIN3. The extended interface list will not be populated
@SYMTestPriority 	    High
@SYMTestActions  	    List the implementations for an interface UID and a set of extended interfaces.
						Check the VID of every implementation matches the VID of the DLL which the implementation
                        belongs to.
@SYMTestExpectedResults Expected matches are returned from both PLUGIN1 and PLUGIN3.
@SYMEC                  EC43
*/
void CRegistryDataTest::ListImplementationsTestT5L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3580 "));
	const TUid KInterfaceUid = {0x10009E36};

	TEST(iRegistryData->IndexValid());
	// Set up client request and extended interfaces
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TBool capability= ETrue;
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,extendedInterfaces,clientReq,capability);
	CleanupStack::PushL(registryResolveTransaction);
	RImplInfoArray* iUidListPtr = NULL;
	TRAPD(err, iUidListPtr = &(registryResolveTransaction->ListImplementationsL(KInterfaceUid)));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	// Prints UIDs of implementations
	_LIT(KUIDFormat,"Uid = 0x%08x\n");
	_LIT(KLists,"There are %d Implementations \n");

	TheTest.Printf(KLists, iUidListPtr->Count());
	// Tests for expected number of implementations.
	TEST(iUidListPtr->Count() == 3);

	for(TInt index = 0; index < iUidListPtr->Count(); ++index)
		{
		// check VID of every implementation
		TEST(((*(iUidListPtr))[index]->VendorId()).iId == 0x70000001);
		TUid uid = (*(iUidListPtr))[index]->ImplementationUid();
		TheTest.Printf(KUIDFormat, uid);
		}
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}
/**
The interface Uid 0x10009E36 has 5 implementations on different DLLs(EComExample12.dll,EComExample14.dll and EComExample15.dll).
0x10009E3D is said to be ROM-Only, it will not be created. 0x10009E39 has higher version in EComExample15.
Interface UID	DLL	UID		Implementation UID	Version	DLL Type	Extended Interfaces
-------------------------------------------------------------------------------------------------------
0x10009E36		0x10009E40	0x10009E48			1		PLUGIN1		None
				0x10009E3E	0x10009E39			1		PLUGIN3		{0x10009E44, 0x10009E45, 0x10009E46}
				0x10009E3E	0x10009E3C			1		PLUGIN3		{}
				0x10009E3E	0x10009E3D			1		PLUGIN3		{0x10009E44}
				0x10009E42	0x10009E39			2		PLUGIN3		{0x10009E44, 0x10009E45, 0x10009E46}

@SYMTestCaseID          SYSLIB-ECOM-UT-3579
@SYMTestCaseDesc	    Tests for CRegistryResolveTransaction::ListImplementationsL() function with matches
						that fall into a PLUGIN1 Dll and PLUGIN3. The extended interface list will be populated
@SYMTestPriority 	    High
@SYMTestActions  	    List the implementations for an interface UID and a set of extended interfaces.
						Check the VID of every implementation match the VID of the DLL which the implementation
                        belongs to.
@SYMTestExpectedResults No PLUGIN1 implementations should be returned.
@SYMEC                  EC43
*/
void CRegistryDataTest::ListImplementationsTestT4L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3579 "));
	const TUid KInterfaceUid = {0x10009E36};
	const TUid KExtendedInterfaceUid = {0x10009E44};

	TEST(iRegistryData->IndexValid());
	//Set up client request and extended interfaces
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	extendedInterfaces.AppendL(KExtendedInterfaceUid);
	TBool capability= ETrue;
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,extendedInterfaces,clientReq,capability);
	CleanupStack::PushL(registryResolveTransaction);
	RImplInfoArray* iUidListPtr = NULL;
	TRAPD(err, iUidListPtr = &(registryResolveTransaction->ListImplementationsL(KInterfaceUid)));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	// Prints UIDs of implementations
	_LIT(KUIDFormat,"Uid = 0x%08x\n");
	_LIT(KLists,"There are %d Implementations \n");

	TheTest.Printf(KLists, iUidListPtr->Count());
	// Tests for expected number of implementations.
	TEST(iUidListPtr->Count() == 1);

	for(TInt index = 0; index < iUidListPtr->Count(); ++index)
		{
		// check VID of every implementation
		TEST(((*(iUidListPtr))[index]->VendorId()).iId == 0x70000001);
		TUid uid = (*(iUidListPtr))[index]->ImplementationUid();
		TheTest.Printf(KUIDFormat, uid);
		}
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3578
@SYMTestCaseDesc	    Tests for CRegistryResolveTransaction::ListImplementationsL() function with invalid interface id.
@SYMTestPriority 	    High
@SYMTestActions  	    List the implementations for an invalid interface UID and valid extended interfaces.
@SYMTestExpectedResults No implementation is returned.
@SYMEC                  EC43
*/
void CRegistryDataTest::ListImplementationsTestT3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3578 "));
	const TUid KExtendedInterfaceUid = {0x10009E44};

	TEST(iRegistryData->IndexValid());
	//Set up client request and extended interfaces
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	extendedInterfaces.AppendL(KExtendedInterfaceUid);
	TBool capability= ETrue;
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,extendedInterfaces,clientReq,capability);
	CleanupStack::PushL(registryResolveTransaction);
	TRAPD(err, RImplInfoArray*	iUidListPtr = &(registryResolveTransaction->ListImplementationsL(KCInvalidUid)));

	TEST_OOM_ERR;
	TEST2(err, KEComErrNoInterfaceIdentified);
	TheTest.Printf(_L("No Implementation is found.\n"));
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}

/**
The interface Uid 0x10009E36 has 5 implementations (EComExample12.dll,EComExample14.dll and EComExample15.dll), but only 1 will
be picked up as 2 of them do not match the extended interfaces criteria (0x10009E3C and 0x10009E48),
1 of them is said to be ROM only but it is on RAM(0x10009E3D), and 1 of them has older version(0x10009E39).

Interface UID	DLL	UID		Implementation UID	Version		Type	Extended Interfaces
--------------------------------------------------------------------------------------
0x10009E36		0x10009E3E	0x10009E39			1			PlugIn3	{0x10009E44, 0x10009E45, 0x10009E46}
				0x10009E3E	0x10009E3C			1			PlugIn3	{}
				0x10009E3E	0x10009E3D			1			PlugIn3	{0x10009E44}
				0x10009E40	0x10009E48			1			PLUGIN1	None
				0x10009E42	0x10009E39			2			PlugIn3	{0x10009E44, 0x10009E45, 0x10009E46}

@SYMTestCaseID          SYSLIB-ECOM-UT-3577
@SYMTestCaseDesc	    Tests for CRegistryResolveTransaction::ListImplementationsL() function with matches
						that fall into a PLUGIN3 Dll.
@SYMTestPriority 	    High
@SYMTestActions  	    List the implementations for an existing interface UID and a set of extended interfaces.
                        Test for expected number of implementations. Check the VID of every implementation match
                        the VID of the DLL which the implementation belongs to.
@SYMTestExpectedResults Expected number of implementations are returned.
@SYMEC                  EC43
*/
void CRegistryDataTest::ListImplementationsTestT2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3577 "));
	const TUid KInterfaceUid = {0x10009E36};
	const TUid KExtendedInterfaceUid = {0x10009E44};

	TEST(iRegistryData->IndexValid());
	// Set up client request and extended interfaces
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	extendedInterfaces.AppendL(KExtendedInterfaceUid);
	TBool capability= ETrue;
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,extendedInterfaces,clientReq,capability);
	CleanupStack::PushL(registryResolveTransaction);
	RImplInfoArray* iUidListPtr = NULL;
	TRAPD(err, iUidListPtr = &(registryResolveTransaction->ListImplementationsL(KInterfaceUid)));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	// Prints UIDs of implementations
	_LIT(KUIDFormat,"Uid = 0x%08x\n");
	_LIT(KLists,"There are %d Implementations \n");

	TheTest.Printf(KLists, iUidListPtr->Count());
	// Tests for expected number of implementations.
	TEST(iUidListPtr->Count() == 1);

	for(TInt index = 0; index < iUidListPtr->Count(); ++index)
		{
		// checks VID of every implementation
		TEST(((*(iUidListPtr))[index]->VendorId()).iId == 0x70000001);
		TUid uid = (*(iUidListPtr))[index]->ImplementationUid();
		TheTest.Printf(KUIDFormat, uid);
		}
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}


/**
The interface Uid 0x10009DC0 has 6 implementations on different DLLs but only 4 will
be picked up as 2 of them are of older version(Impl ID 0x10009DC3 and 0x10009DC4 with version 1)

Interface UID	DLL	UID		Implementation UID	Version
-------------------------------------------------------
0x10009DC0		0x10009DB1	0x10009DC3			1
				0x10009DB1	0x10009DC4			1
				0x10009DB3	0x10009DC3			2
				0x10009DB3	0x10009DC4			2
				0x101F8477	0x101F8478			1
				0x101F847B	0x101F847C			1

@SYMTestCaseID          SYSLIB-ECOM-UT-3576
@SYMTestCaseDesc	    Tests for CRegistryResolveTransaction::ListImplementationsL() function with matches
						that fall into a PLUGIN1 Dll.
@SYMTestPriority 	    High
@SYMTestActions  	    List the implementations for an existing interface UID and
                        test for expected number of implementations.Check the VID of
                        every implementation match the VID of the DLL which the implementation
                        belongs to.
@SYMTestExpectedResults Expected number of implementations are returned.
@SYMEC                  EC43
*/
void CRegistryDataTest::ListImplementationsTestT1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3576 "));
	TEST(iRegistryData->IndexValid());
	// Set up client request and extended interfaces
	TClientRequest clientReq;
	RArray<TUid> extendedInterfaces;
	CleanupClosePushL(extendedInterfaces);
	TBool capability= ETrue;
	CRegistryResolveTransaction* registryResolveTransaction = CRegistryResolveTransaction::NewL(*iRegistryData,extendedInterfaces,clientReq,capability);
	CleanupStack::PushL(registryResolveTransaction);
	RImplInfoArray* iUidListPtr = NULL;
	TRAPD(err, iUidListPtr = &(registryResolveTransaction->ListImplementationsL(iInterfaceUid)));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	// Prints UIDs of implementations
	_LIT(KUIDFormat,"Uid = 0x%08x\n");
	_LIT(KLists,"There are %d Implementations \n");

	TheTest.Printf(KLists, iUidListPtr->Count());
	// Tests for expected number of implementations.
	TEST(iUidListPtr->Count() == 4);

	for(TInt index = 0; index < iUidListPtr->Count(); ++index)
		{
		// checks VID of every implementation
		TEST(((*(iUidListPtr))[index]->VendorId()).iId == 0x70000001);
		TUid uid = (*(iUidListPtr))[index]->ImplementationUid();
		TheTest.Printf(KUIDFormat, uid);
		}
	CleanupStack::PopAndDestroy(registryResolveTransaction);
	CleanupStack::PopAndDestroy(&extendedInterfaces);
	}

/** In order to create an instance of CImplementationData, the test
has to create the chain of: drive data, dll data, and interface data.
Use this method to safe the repetitive work.

This method will not new the CImplementationData's parents if they
have been set. Hence can be used to add an implementation to an
existing interface data.
*/
void CRegistryDataTest::CreateImplL(TImplCreationStruct& aCreationData)
	{
	if (aCreationData.iImplData)
	  {
	  // Should not call this method. There is nothing to create.
	  User::Leave(KErrGeneral);
	  }

	if (! aCreationData.iDriveData || (KErrNotFound == aCreationData.iDriveIndex))
		{
        aCreationData.iDriveIndex = iRegistryData->FindDriveL(
            aCreationData.iDriveUnit, aCreationData.iDriveData);
        if (KErrNotFound == aCreationData.iDriveIndex)
            {
            aCreationData.iDriveData = CRegistryData::CDriveData::NewLC(
                aCreationData.iDriveUnit, iRegistryData);
            iStateAccessor->AppendDriveDataL(*iRegistryData,
                                             aCreationData.iDriveData);
            CleanupStack::Pop(aCreationData.iDriveData);

            aCreationData.iDriveIndex = iRegistryData->FindDriveL(
            	aCreationData.iDriveUnit,aCreationData.iDriveData);
            }
		}

	if (! aCreationData.iDllData)
		{
		if (aCreationData.iInterfaceData)
			{
			User::Leave(KErrGeneral); // can not have child without parent
			}
		TEntry e;
		e.iName = aCreationData.iDllPathName;
		TParse fileparse;
		fileparse.Set(e.iName,NULL,NULL);
		aCreationData.iDllData = CRegistryData::CDllData::NewLC(
			fileparse.NameAndExt(),e.iModified,KNullUid,KNullUid, aCreationData.iDriveData);
		aCreationData.iDllData->iSecurityChecked = ETrue;
		aCreationData.iDriveData->AddL(aCreationData.iDllData);
		CleanupStack::Pop(aCreationData.iDllData);
		}

	if (!aCreationData.iInterfaceData)
		{
		aCreationData.iInterfaceData = CRegistryData::CInterfaceData::NewLC(
			aCreationData.iInterfaceUid, aCreationData.iDllData);
		aCreationData.iDllData->AddL(aCreationData.iInterfaceData);
		CleanupStack::Pop(aCreationData.iInterfaceData);
		}

	HBufC* name = aCreationData.iImplDisplayName.AllocLC();
	HBufC8* dataType = aCreationData.iImplDataType.AllocLC();
	HBufC8* opaqueData = aCreationData.iImplOpaqueData.AllocLC();

	aCreationData.iImplData = CRegistryData::CImplementationData::NewL(
		aCreationData.iInterfaceData,
		aCreationData.iImplUid,
		aCreationData.iImplVersion,
		name,
		dataType,
		opaqueData,
		aCreationData.iDriveUnit,
		aCreationData.iRomOnly,
		aCreationData.iRomBased);

	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);

	CleanupStack::PushL(aCreationData.iImplData);
	aCreationData.iInterfaceData->AddL(aCreationData.iImplData);
	CleanupStack::Pop(aCreationData.iImplData);
	}


/**
@SYMTestCaseID			SYSLIB-ECOM-CT-0290
@SYMTestCaseDesc		Unit tests for CRegistryData::CImplementationData
@SYMTestPriority			High
@SYMTestActions			Create instances using NewLC and NewL. Check that the parent supplied is
						correctly set.
						Externalize instance created by NewL.
						Internalize data set by previous step. Check implementation data is identical.
						Check compare uid returns correct values.
						Check for no crash and memory leaks.
@SYMTestExpectedResults	The test must not fail.
@SYMPREQ				PREQ277
*/
void  CRegistryDataTest::ImplementationDataTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0290 "));
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	// Test Data
	TUid testImplUid;
	testImplUid.iUid = KCExampleInterfaceUid.iUid;
	TInt testImplVersion = 2;
	_LIT(KTestImplName, "Implementation");
	_LIT8(KTestImplDataType, "Data Type");
	_LIT8(KTestImplOpaqueData, "Opaque Data");
	TUid testExtendedInterfaceUid1;
	TUid testExtendedInterfaceUid2;
	testExtendedInterfaceUid1.iUid = 0x10009E44;
	testExtendedInterfaceUid2.iUid = 0x10009E45;

	TDriveUnit testImplDrive = EDriveC;
	TBool testImplRomOnly = ETrue;
	TBool testImplRomBased = EFalse;

	CRegistryData* registryData = CRegistryData::NewL(TheFs);
	CleanupStack::PushL(registryData);
	CRegistryData::CDriveData* driveData = CRegistryData::CDriveData::NewLC(testImplDrive, registryData);
	CRegistryData::CDllData* dllData = CRegistryData::CDllData::NewLC(driveData);
	CRegistryData::CInterfaceData* interfaceData = CRegistryData::CInterfaceData::NewLC(dllData);

	// Start tests...
	// NewLC test
	CRegistryData::CImplementationData* implementation1;
	implementation1 = CRegistryData::CImplementationData::NewLC(interfaceData);
	TEST(interfaceData == implementation1->iParent);

	// NewL test
	HBufC* name = KTestImplName().AllocLC();
	HBufC8* dataType = KTestImplDataType().AllocLC();
	HBufC8* opaqueData = KTestImplOpaqueData().AllocLC();

	RArray<TUid>* extendedInterfaces = new(ELeave) RArray<TUid>;

	CleanupStack::PushL(TCleanupItem(CloseAndDeleteArray, extendedInterfaces));

	CRegistryData::CImplementationData* implementation2;
	implementation2 = CRegistryData::CImplementationData::NewL(interfaceData,
															testImplUid,
															testImplVersion,
															name,
															dataType,
															opaqueData,
															testImplDrive,
															testImplRomOnly,
															testImplRomBased,
															extendedInterfaces);

	CleanupStack::Pop(extendedInterfaces);
	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);

	CleanupStack::PushL(implementation2);
	TEST(interfaceData == implementation1->iParent);

	//Add extended interface
	implementation2->iImplInfo->AddExtendedInterfaceL(testExtendedInterfaceUid1);
	implementation2->iImplInfo->AddExtendedInterfaceL(testExtendedInterfaceUid2);

	// Externalize test
	_LIT(KFileName,"c:\\ecomimpldata.dat");
	RFile file;
	TFileName fileName(KFileName);
	TEST2(file.Replace(TheFs,fileName,EFileWrite|EFileRead), KErrNone);
	RFileBuf buf;
	CleanupClosePushL(buf);
	buf.Attach(file);
	RWriteStream stream(&buf);

	implementation2->ExternalizeL(stream);
	buf.SynchL();
	CleanupStack::PopAndDestroy(&buf);

	// Internalize test
	TInt err = file.Open(TheFs,fileName,EFileRead);
	TEST2(err, KErrNone);
	RFileBuf buf2;
	CleanupClosePushL(buf2);
	buf2.Attach(file);
	RReadStream stream2(&buf2);

	implementation1->InternalizeL(stream2);

	TEST(implementation1->iImplInfo->ImplementationUid().Uid == testImplUid.Uid);
	TEST(implementation1->iImplInfo->Version() == testImplVersion);
	TEST2(KErrNone, name->Compare(implementation1->iImplInfo->DisplayName()));
	TEST2(KErrNone, dataType->Compare(implementation1->iImplInfo->DataType()));
	TEST2(KErrNone, opaqueData->Compare(implementation1->iImplInfo->OpaqueData()));
	TEST(implementation1->iImplInfo->Drive() == testImplDrive);
	TEST(implementation1->iImplInfo->RomOnly() == testImplRomOnly);
	TEST(implementation1->iImplInfo->RomBased() == testImplRomBased);
	//Get extended interface and check
	RArray<TUid>* extendedInterfaceList = implementation1->iImplInfo->GetExtendedInterfaceList();
	TEST((*extendedInterfaceList)[0] == testExtendedInterfaceUid1);
	TEST((*extendedInterfaceList)[1] == testExtendedInterfaceUid2);

	CleanupStack::PopAndDestroy(&buf2);

	// Setup data for Compare Implementation Uid
	name = KTestImplName().AllocLC();
	dataType = KTestImplDataType().AllocLC();
	opaqueData = KTestImplOpaqueData().AllocLC();
	TUid compareTestImplUid;
	compareTestImplUid.iUid = 0x10009DC3;
	CRegistryData::CImplementationData* implementation3;
	implementation3 = CRegistryData::CImplementationData::NewL(interfaceData,
															compareTestImplUid,
															testImplVersion,
															name,
															dataType,
															opaqueData,
															testImplDrive,
															testImplRomOnly,
															testImplRomBased);
	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);

	CleanupStack::PushL(implementation3);
	TEST(interfaceData == implementation3->iParent);

	// Compare Implementation Uid test
	TEST(CRegistryData::CImplementationData::CompareImplUid(*implementation3, *implementation2) > 0);
	TEST(CRegistryData::CImplementationData::CompareImplUid(*implementation1, *implementation3) < 0);
	TEST(CRegistryData::CImplementationData::CompareImplUid(*implementation3, *implementation3) == 0);

	// Tidy up
	CleanupStack::PopAndDestroy(7, registryData);
	TheFs.Delete(KFileName);

// Check for open handles
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-CT-0291
@SYMTestCaseDesc		Tests for CRegistryData::InsertIntoIndexRemoveFromIndexTestL
						See CR65BCA3.
@SYMTestPriority			High
@SYMTestActions			Create 2 identical implementations, 1 for the Z drive and 1 for the C drive,
						including parents and add to the registry.
						Create a 3rd identical implementation, except for the name, for the C drive.
						Check there are no implementations in the Implementation Index List.
						Insert the Z implementations into the Implementation Index List and check.
						Insert the 1st C implementation with security check on and check the upgrade
						took place.
						Remove the implementation. Check.
						Add the Z drive implementation. Check.
						Insert the 1st C implementation with security check off and check the upgrade
						took place.
						Remove the implementation. Check.
						Add the Z drive implementation. Check.
						Insert the 2nd C implementation with security check on and check the upgrade
						did not take place.
						Insert the 2nd C implementation with security check off and check the upgrade
						did took place.
						Remove the implementation. Check.
						Check for no crash and memory leaks.
@SYMTestExpectedResults	The test must not fail.
@SYMPREQ				PREQ277
*/
void  CRegistryDataTest::InsertIntoIndexRemoveFromIndexTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0291 "));
	_LIT(KTestImplName, "Implementation");
	_LIT8(KTestImplDataType, "Data Type");
	_LIT8(KTestImplOpaqueData, "Opaque Data");

	// create Z drive data
	TDriveUnit implDriveZ = EDriveZ;
	CRegistryData::CDriveData* driveDataZ = CRegistryData::CDriveData::NewLC(implDriveZ,  iRegistryData);
	iStateAccessor->AppendDriveDataL(*iRegistryData, driveDataZ);
	CleanupStack::Pop(driveDataZ);

	// Create Implementation 1 on Z drive
	_LIT(KEComUpgradeExample1OnZ,		"Z:\\sys\\bin\\EComUpgradeExampleOne.dll");
	TEntry upgradeExample1OnZ;
	upgradeExample1OnZ.iName = KEComUpgradeExample1OnZ;
	TParse fileparse;
	fileparse.Set(KEComUpgradeExample1OnZ,NULL,NULL);

	CRegistryData::CDllData* dllDataZ = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),upgradeExample1OnZ.iModified,KNullUid, KNullUid, driveDataZ);
	dllDataZ->iSecurityChecked = ETrue;
	driveDataZ->AddL(dllDataZ);
	CleanupStack::Pop(dllDataZ);
	CRegistryData::CInterfaceData* interfaceDataZ = CRegistryData::CInterfaceData::NewLC(iInterfaceUid, dllDataZ);
	dllDataZ->AddL(interfaceDataZ);
	CleanupStack::Pop(interfaceDataZ);
	HBufC* name = KTestImplName().AllocLC();
	HBufC8* dataType = KTestImplDataType().AllocLC();
	HBufC8* opaqueData = KTestImplOpaqueData().AllocLC();

	CRegistryData::CImplementationData* implementation1;
	implementation1 = CRegistryData::CImplementationData::NewL(interfaceDataZ,
															iImplementationUid,
															1,
															name,
															dataType,
															opaqueData,
															implDriveZ,
															EFalse,
															EFalse);
	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);
	CleanupStack::PushL(implementation1);
	interfaceDataZ->AddL(implementation1);
	CleanupStack::Pop(implementation1);

	// create C drive data
	TDriveUnit implDriveC = EDriveC;
	CRegistryData::CDriveData* driveDataC = CRegistryData::CDriveData::NewLC(implDriveC,  iRegistryData);
	iStateAccessor->AppendDriveDataL(*iRegistryData, driveDataC);
	CleanupStack::Pop(driveDataC);

	// Create Implementation 1 on C drive
	_LIT(KEComUpgradeExample1OnC,		"C:\\sys\\bin\\EComUpgradeExampleOne.dll");
	TEntry upgradeExample1OnC;
	upgradeExample1OnC.iName = KEComUpgradeExample1OnC;
	fileparse.Set(KEComUpgradeExample1OnC,NULL,NULL);

	CRegistryData::CDllData* dllDataC1 = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),upgradeExample1OnC.iModified,KNullUid, KNullUid, driveDataC);
	dllDataC1->iSecurityChecked = ETrue;
	driveDataC->AddL(dllDataC1);
	CleanupStack::Pop(dllDataC1);
	CRegistryData::CInterfaceData* interfaceDataC1 = CRegistryData::CInterfaceData::NewLC(iInterfaceUid, dllDataC1);
	dllDataC1->AddL(interfaceDataC1);
	CleanupStack::Pop(interfaceDataC1);
	name = KTestImplName().AllocLC();
	dataType = KTestImplDataType().AllocLC();
	opaqueData = KTestImplOpaqueData().AllocLC();

	CRegistryData::CImplementationData* implementation2;
	implementation2 = CRegistryData::CImplementationData::NewL(interfaceDataC1,
															iImplementationUid,
															2,
															name,
															dataType,
															opaqueData,
															implDriveC,
															EFalse,
															EFalse);
	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);
	CleanupStack::PushL(implementation2);
	interfaceDataC1->AddL(implementation2);
	CleanupStack::Pop(implementation2);

	// Create Implementation 2 on C drive
	_LIT(KEComUpgradeExample2OnC,		"C:\\sys\\bin\\EComUpgradeExampleTwo.dll");
	TEntry upgradeExample2OnC;
	upgradeExample2OnC.iName = KEComUpgradeExample2OnC;
	fileparse.Set(KEComUpgradeExample2OnC,NULL,NULL);

	CRegistryData::CDllData* dllDataC2 = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),upgradeExample2OnC.iModified,KNullUid,KNullUid, driveDataC);
	dllDataC2->iSecurityChecked = ETrue;
	driveDataC->AddL(dllDataC2);
	CleanupStack::Pop(dllDataC2);
	CRegistryData::CInterfaceData* interfaceDataC2 = CRegistryData::CInterfaceData::NewLC(iInterfaceUid, dllDataC2);
	dllDataC2->AddL(interfaceDataC2);
	CleanupStack::Pop(interfaceDataC2);
	name = KTestImplName().AllocLC();
	dataType = KTestImplDataType().AllocLC();
	opaqueData = KTestImplOpaqueData().AllocLC();

	CRegistryData::CImplementationData* implementation3;
	implementation3 = CRegistryData::CImplementationData::NewL(interfaceDataC2,
															iImplementationUid,
															2,
															name,
															dataType,
															opaqueData,
															implDriveC,
															EFalse,
															EFalse);
	CleanupStack::Pop(opaqueData);
	CleanupStack::Pop(dataType);
	CleanupStack::Pop(name);
	CleanupStack::PushL(implementation3);
	interfaceDataC2->AddL(implementation3);
	CleanupStack::Pop(implementation3);

//===========================================================================

	// Check index is empty
	TEST(iStateAccessor->ImplementationsCount(*iRegistryData) == 0);

//===========================================================================

	// Add Implementation from Z drive to index
	TBool result = iRegistryData->InsertIntoIndexL(implementation1, ETrue);
	TEST2(result, ETrue);

	RImplDataArray impls;
	CleanupClosePushL(impls);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);

//===========================================================================

	// Add Implementation from C drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(implementation2, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);

//===========================================================================

	// remove existing implementation which causes rollback to Z drive impl
	iRegistryData->RemoveFromIndexL(dllDataC1);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);

//===========================================================================

	// Add Implementation from C drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(implementation2, EFalse);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);
//===========================================================================

	// remove existing implementation which causes rollback to Z drive impl
	iRegistryData->RemoveFromIndexL(dllDataC1);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);
//===========================================================================

	// Add Implementation with different name from C drive to index
	// this should NOT upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(implementation3, ETrue);
	TEST2(result, EFalse);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);
//===========================================================================

	// Add Implementation with different name from C drive to index
	// this should upgrade existing implementation as security check is OFF
	result = iRegistryData->InsertIntoIndexL(implementation3, EFalse);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);
//===========================================================================

	// remove existing implementation which causes rollback to Z drive impl
	iRegistryData->RemoveFromIndexL(dllDataC2);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);
	CleanupStack::PopAndDestroy(&impls);
	}

/**
Test case for Defect ECOM Index must rollback	to original Impl. if upgrade impl. is removed

@SYMTestCaseID			SYSLIB-ECOM-CT-1373
@SYMTestCaseDesc		Test case for defect number DEF065559 ECOM Index must rollback
						to original Impl. if upgrade impl. is removed.
@SYMTestPriority			High
@SYMTestActions			Create 3 identical implementations, 1 for the Z drive, 1 for the C drive and
						1 for the D drive, including parents and add to the registry.
						Insert the Z implementation and check.
						Insert the C implementation and check the upgrade took place.
						Insert the D implementation and check the upgrade took place.
						Remove the D implementation and check rollback to C occurred.
						Remove the C implementation and check rollback to Z occurred.
						Insert the C implementation and check the upgrade took place.
						Insert the D implementation and check the upgrade took place.
						Remove the C implementation and check no rollback occurred. i.e D impl available
						Remove the D implementation and check rollback to Z occurred.
						Insert the D implementation and check the upgrade took place.
						Insert the C implementation and check no upgrade took place. i.e D impl available
						Remove the D implementation and check rollback to C occurred.
						Remove the C implementation and check rollback to Z occurred.
						Check for no crash and memory leaks.
@SYMTestExpectedResults	The test must not fail.
@SYMDEF				DEF065559
*/
void  CRegistryDataTest::RollbackTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1373 "));
	_LIT(KTestImplName, "Implementation");
	_LIT8(KTestImplDataType, "Data Type");
	_LIT8(KTestImplOpaqueData, "Opaque Data");

	// Create Implementation 1 on Z drive
	_LIT(KEComUpgradeExample1OnZ,		"Z:\\sys\\bin\\EComUpgradeExampleOne.dll");
	TImplCreationStruct zDllData(EDriveZ,
								iInterfaceUid,
								iImplementationUid,
								KEComUpgradeExample1OnZ,
								KTestImplName,
								KTestImplDataType,
								KTestImplOpaqueData,
								1,
								ETrue,  // Z: is ROMBased
								EFalse);
	CreateImplL(zDllData);

	// Create Implementation 1 on C drive
	_LIT(KEComUpgradeExample1OnC,		"C:\\sys\\bin\\EComUpgradeExampleOne.dll");
	TImplCreationStruct cDllData(EDriveC,
								iInterfaceUid,
								iImplementationUid,
								KEComUpgradeExample1OnC,
								KTestImplName,
								KTestImplDataType,
								KTestImplOpaqueData,
								2);
	CreateImplL(cDllData);

	// create drive data on third drive.

	// find a valid drive which is not Z and not C.
	TEComCachedDriveInfoIterator iter(*iRegistryData->iCachedDriveInfo);
	iter.Last(); // positioned at Z:
	iter.Prev(); // the drive before Z:
	for (; iter.InRange(); iter.Prev())
		{
		if (iter.DriveNumber() != EDriveC)
			{
			break;
			}
		}

	TEST(iter.InRange() && iter.DriveNumber() != EDriveC);

	TDriveName drvName = iter.DriveUnit().Name();

	_LIT(KEComUpgradeExample1On3,		"_:\\sys\\bin\\EComUpgradeExampleOne.dll");
	TBuf<36> dllName3(KEComUpgradeExample1On3);
	dllName3[0] = drvName[0];
	TImplCreationStruct thirdDllData(iter.DriveNumber(),
									iInterfaceUid,
									iImplementationUid,
									dllName3,
									KTestImplName,
									KTestImplDataType,
									KTestImplOpaqueData,
									3);
	CreateImplL(thirdDllData);
//===========================================================================

	// Check index is empty
	TEST(iStateAccessor->ImplementationsCount(*iRegistryData) == 0);

//===========================================================================
	TDriveUnit implDriveZ = EDriveZ;
	TDriveUnit implDriveC = EDriveC;

	// Add Implementation from Z drive to index
	TBool result = iRegistryData->InsertIntoIndexL(zDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	RImplDataArray impls;
	CleanupClosePushL(impls);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);

//===========================================================================

	// Add Implementation from C drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(cDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);

//===========================================================================

	// Add Implementation from third drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(thirdDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == iter.DriveUnit());

//===========================================================================

	// remove implementation from third drive which causes rollback to C drive impl
	iRegistryData->RemoveFromIndexL(thirdDllData.iDllData);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);

//===========================================================================

	// remove implementation from C which causes rollback to Z drive impl
	iRegistryData->RemoveFromIndexL(cDllData.iDllData);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);

//===========================================================================

	// Add Implementation from C drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(cDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);

//===========================================================================

	// Add Implementation from third drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(thirdDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == iter.DriveUnit());

//===========================================================================

	// remove implementation from C which should cause no rollback
	iRegistryData->RemoveFromIndexL(cDllData.iDllData);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == iter.DriveUnit());

//===========================================================================

	// remove implementation from third drive which causes rollback to Z drive impl
	iRegistryData->RemoveFromIndexL(thirdDllData.iDllData);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);

//===========================================================================

	// Add Implementation from third drive to index - this should upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(thirdDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == iter.DriveUnit());

//===========================================================================

	// Add Implementation from C drive to index - this should not upgrade existing implementation
	result = iRegistryData->InsertIntoIndexL(cDllData.iImplData, ETrue);
	TEST2(result, ETrue);

	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == iter.DriveUnit());

//===========================================================================

	// remove implementation from third drive which causes rollback to C drive impl
	iRegistryData->RemoveFromIndexL(thirdDllData.iDllData);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveC);

//===========================================================================

	// remove implementation from C which causes rollback to Z drive impl
	iRegistryData->RemoveFromIndexL(cDllData.iDllData);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, iInterfaceUid, impls);
	TEST(impls.Count() == 1);
	TEST(impls[0]->iImplInfo->Drive() == implDriveZ);
	CleanupStack::PopAndDestroy(&impls);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0737
@SYMTestCaseDesc	    Tests the creation and deletion of CRegistryData
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete CRegistryData object,checks for open handles
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void CreateDeleteTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0737 Create and Delete Test "));
	// Set up for heap leak checking
	__UHEAP_MARK;

	// and leaking thread handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;

	// Test Starts...
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CRegistryDataTest* theTest = NULL;
	// Create the object
	TRAPD(err, theTest = CRegistryDataTest::NewL(EFalse));
	TEST2(err, KErrNone);
	// Delete the object
	delete theTest;

	// Check for open handles
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);
	TEST(startThreadHandleCount == endThreadHandleCount);

	// Test Ends...
	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3572
@SYMTestCaseDesc	    Tests for CRegistryData::UpdateDllDataL() function when an existing registered
						PLUGIN3 is downgraded with an updated PLUGIN1 resource file.
@SYMTestPriority 	    High
@SYMTestActions  	    Update the data for a dll in the registry.
                        Tests for OOM error.
@SYMTestExpectedResults The data for the dll in the registry is updated.
@SYMEC                  EC43
*/
void CRegistryDataTest::UpdateDllDataTest3L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3572 "));
	// Checks if drive is registered
	TEST(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit)!= KErrNotFound);
	CRegistryData::CDriveData* driveData = NULL;
	// Fetches index of drive, in iFoundDriveIndex
	iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData);

	TEntry dllEntry;
	TUid example12ThirdUid = {0x10009E3E};
	dllEntry.iType = TUidType(KNullUid, KNullUid, example12ThirdUid);
	dllEntry.iName = KEComExample12DllOnC;
	dllEntry.iModified.HomeTime();

	TBool registered = EFalse;

	registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData);
	TEST(registered);
	// Log message if implementations need update
	if (!iUpdate)
		{
		TheTest.Printf(_L("Specified implementation group is already registered and does not need an update\n"));
		}
	//If the implementation needs an update, update it using UpdateDllDataL
	else
		{
		TheTest.Printf(_L("Specified implementation group is not registered with date and needs an update\n"));
		// Checks Implementation 0x10009E48 registered in the registry is version 1, display name is "Example 12 PlugIn3"
		// A reference to the implementation entry in the registry
		const TUid implementationUid = {0x10009E3C};
		CRegistryData::CImplementationData*	implementationData = NULL;
		TInt err = iStateAccessor->FindImplementation(*iRegistryData, implementationUid, KCDummyUid, implementationData);
		TEST2(err,KErrNone);
		CImplementationInformation* implementationInfo = implementationData->iImplInfo;
		TEST(implementationInfo->Version() == 1);
		TEST(implementationInfo->DisplayName().CompareF(_L("Example 12 PlugIn3"))==0);

		// Dll Data object to be updated
		CRegistryData::CDllData* updateDllData = NULL;
		TParse fileparse;
		fileparse.Set(dllEntry.iName,NULL,NULL);
		updateDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),dllEntry.iModified,dllEntry[1], dllEntry[2],driveData);

		//Copies the downgraded resource file to C:
		TRAP(err, EComTestUtils::FileManCopyFileL(KEcomExample12DowngradedOnZ, KEcomExample12DowngradedOnC));
		TEST2(err, KErrNone);
		//Creats the entry using downgraded resource file
		TEntry rscBasedEntry;
		rscBasedEntry.iName = KEcomExample12DowngradedResourceNameOnly;
		TParse rscPath;
		rscPath.Set(KEcomExample12DowngradedOnC,NULL,NULL);
		CPluginBase* entryBase = NULL;
		entryBase=CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), EFalse);
		CleanupStack::PushL(entryBase);
		// Updates resource ext info
		HBufC* resourceExt = entryBase->RscFileExt();
		if(resourceExt)
			{
			updateDllData->SetResourceExtL(*resourceExt);
			}
		// parses the downgraded resource file
		TInt error = KErrNone;
		TRAP(error, iRegistrarAccessor->ParseL(*iRegistrar, entryBase, *updateDllData));
		if (error == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		CleanupStack::PopAndDestroy(entryBase);
		TDriveUnit aDrive;
		// Updates DLL data with a new dll data
		TRAP(err, iRegistryData->UpdateDllDataL(aDrive, iFoundDriveIndex, updateDllData));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		CleanupStack::Pop(updateDllData);
		// Checks registry data is updated with new data
		// Implementation 0x10009E48 is downgraded from version 1 to version 0, display name is changed to "Example 12 PlugIn"
		implementationData = NULL;
		err = iStateAccessor->FindImplementation(*iRegistryData, implementationUid, KCDummyUid, implementationData);
		TEST2(err,KErrNone);
		implementationInfo = implementationData->iImplInfo;
		TEST(implementationInfo->Version() == 0);
		TEST(implementationInfo->DisplayName().CompareF(_L("Example 12 PlugIn")) == 0);
		TheTest.Printf(_L("DLL data is updated \n"));
		//deletes the downgraded resource file
		TRAP(err,EComTestUtils::FileManDeleteFileL(KEcomExample12DowngradedOnC));
		}
	registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData);
	TEST(registered);
	TEST(!iUpdate);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3571
@SYMTestCaseDesc	    Tests for CRegistryData::UpdateDllDataL() function when an existing registered
						PLUGIN1 is upgraded with an updated PLUGIN3 resource file.
@SYMTestPriority 	    High
@SYMTestActions  	    Update the data for a dll in the registry.
                        Tests for OOM error.
@SYMTestExpectedResults The data for the dll in the registry is updated.
@SYMEC                  EC43
*/
void CRegistryDataTest::UpdateDllDataTest2L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3571 "));
	// Checks if drive is registered
	TEST(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit)!= KErrNotFound);
	CRegistryData::CDriveData* driveData = NULL;
	// Fetches index of drive, in iFoundDriveIndex
	iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData);

	TEntry dllEntry;
	TUid example14ThirdUid = {0x10009E40};
	dllEntry.iType = TUidType(KNullUid, KNullUid, example14ThirdUid);
	dllEntry.iName = KEComExample14DllOnC;
	dllEntry.iModified.HomeTime();

	TBool registered = EFalse;

	registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData);
	TEST(registered);
	// Log message if implementations need update
	if (!iUpdate)
		{
		TheTest.Printf(_L("Specified implementation group is already registered and does not need an update\n"));
		}
	//If the implementation needs an update, update it using UpdateDllDataL
	else
		{
		TheTest.Printf(_L("Specified implementation group is not registered with date and needs an update\n"));

		// Checks Implementation 0x10009E48 registered in the registry is version 1, display name is "Example 14 PlugIn"
		// A reference to the implementation entry in the registry
		const TUid implementationUid = {0x10009E48};
		CRegistryData::CImplementationData*	implementationData = NULL;
		TInt err = iStateAccessor->FindImplementation(*iRegistryData, implementationUid, KCDummyUid, implementationData);
		TEST2(err,KErrNone);
		CImplementationInformation* implementationInfo = implementationData->iImplInfo;
		TEST(implementationInfo->Version() == 1);
		TEST(implementationInfo->DisplayName().CompareF(_L("Example 14 PlugIn"))==0);

		// Dll Data object to be updated
		CRegistryData::CDllData* updateDllData = NULL;
		TParse fileparse;
		fileparse.Set(dllEntry.iName,NULL,NULL);
		updateDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),dllEntry.iModified,dllEntry[1], dllEntry[2],driveData);

		//copies the upgraded resource file to C:
		TRAP(err, EComTestUtils::FileManCopyFileL(KEcomExample14UpgradedOnZ, KEcomExample14UpgradedOnC));
		TEST2(err, KErrNone);
		//Creats the entry using upgraded resource file
		TEntry rscBasedEntry;
		rscBasedEntry.iName = KEcomExample14UpgradedResourceNameOnly;
		TParse rscPath;
		rscPath.Set(KEcomExample14UpgradedOnC,NULL,NULL);
		CPluginBase* entryBase = NULL;
		entryBase=CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), EFalse);
		CleanupStack::PushL(entryBase);
		// Updates resource ext info
		HBufC* resourceExt = entryBase->RscFileExt();
		if(resourceExt)
			{
			updateDllData->SetResourceExtL(*resourceExt);
			}
		// parses the upgraded resource file
		TInt error = KErrNone;
		TRAP(error, iRegistrarAccessor->ParseL(*iRegistrar, entryBase, *updateDllData));
		if (error == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		CleanupStack::PopAndDestroy(entryBase);

		TDriveUnit aDrive;
		// Updates DLL data with a new dll data
		TRAP(err, iRegistryData->UpdateDllDataL(aDrive, iFoundDriveIndex, updateDllData));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		CleanupStack::Pop(updateDllData);
		// Checks registry data is updated with new data
		// Implementation 0x10009E48 is upgraded from version 1 to version 2, display name is changed to "Example 14 PlugIn3"
		implementationData = NULL;
		err = iStateAccessor->FindImplementation(*iRegistryData, implementationUid, KCDummyUid, implementationData);
		TEST2(err,KErrNone);
		implementationInfo = implementationData->iImplInfo;
		TEST(implementationInfo->Version() == 2);
		TEST(implementationInfo->DisplayName().CompareF(_L("Example 14 PlugIn3"))==0);
		TheTest.Printf(_L("DLL data is updated \n"));
		//deletes the upgraded resource file
		TRAP(err,EComTestUtils::FileManDeleteFileL(KEcomExample14UpgradedOnC));
		}
	registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData);
	TEST(registered);
	TEST(!iUpdate);
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3570
@SYMTestCaseDesc	    Tests for CRegistryData::UpdateDllDataL() function when an existing registered
						PLUGIN3 is upgraded with an updated PLUGIN3 resource file.
@SYMTestPriority 	    High
@SYMTestActions  	    Update the data for a dll in the registry.
                        Tests for OOM error.
@SYMTestExpectedResults The data for the dll in the registry is updated.
@SYMEC                  EC43
*/

void CRegistryDataTest::UpdateDllDataTest1L()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3570 "));
	// Checks if drive is registered
	TEST2(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit),ETrue);
	CRegistryData::CDriveData* driveData = NULL;
	// Fetches index of drive, in iFoundDriveIndex
	iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData);

	TEntry dllEntry;
	TUid example12ThirdUid = {0x10009E3E};
	dllEntry.iType = TUidType(KNullUid, KNullUid, example12ThirdUid);
	dllEntry.iName = KEComExample12DllOnC;
	dllEntry.iModified.HomeTime();

	TBool registered = EFalse;
	registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData);
	TEST(registered);
	// Log message if implementations need update
	if (!iUpdate)
		{
		TheTest.Printf(_L("Specified implementation group is already registered and does not need an update\n"));
		}
	//If the implementation needs an update, update it using UpdateDllDataL
	else
		{
		TheTest.Printf(_L("Specified implementation group is not registered with date and needs an update\n"));

		// Checks Implementation 0x10009E3C registered in the registry is version 1
		// A reference to the implementation entry in the registry
		const TUid implementationUid = {0x10009E3C};
		CRegistryData::CImplementationData*	implementationData = NULL;
		TInt err = iStateAccessor->FindImplementation(*iRegistryData, implementationUid, KCDummyUid, implementationData);
		TEST2(err,KErrNone);
		CImplementationInformation* implementationInfo = implementationData->iImplInfo;
		TEST(implementationInfo->Version() == 1);
		// Dll Data object to be updated
		CRegistryData::CDllData* updateDllData = NULL;
		TParse fileparse;
		fileparse.Set(dllEntry.iName,NULL,NULL);
		updateDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),dllEntry.iModified,dllEntry[1], dllEntry[2],driveData);

		//copies the upgraded resource file to C:
		TRAP(err, EComTestUtils::FileManCopyFileL(KEcomExample12UpgradedOnZ, KEcomExample12UpgradedOnC));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		//Creats the entry using upgraded resource file
		TEntry rscBasedEntry;
		rscBasedEntry.iName = KEcomExample12UpgradedResourceNameOnly;
		TParse rscPath;
		rscPath.Set(KEcomExample12UpgradedOnC,NULL,NULL);
		CPluginBase* entryBase = NULL;
		entryBase=CSecurePlugin::NewL(TheFs,rscBasedEntry,rscPath.Drive(), EFalse);
		CleanupStack::PushL(entryBase);
		// Updates resource ext info
		HBufC* resourceExt = entryBase->RscFileExt();
		if(resourceExt)
			{
			updateDllData->SetResourceExtL(*resourceExt);
			}
		// parses the upgraded resource file
		TInt error = KErrNone;
		TRAP(error, iRegistrarAccessor->ParseL(*iRegistrar, entryBase, *updateDllData));
		if (error == KErrNoMemory)
			{
			User::LeaveNoMemory();
			}
		CleanupStack::PopAndDestroy(entryBase);
		TDriveUnit aDrive;
		// Updates DLL data with a new dll data
		TRAP(err, iRegistryData->UpdateDllDataL(aDrive, iFoundDriveIndex, updateDllData));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		CleanupStack::Pop(updateDllData);
		// Checks registry data is updated with new data
		// Implementation 0x10009E3C is upgraded from version 1 to version 2
		implementationData = NULL;
		err = iStateAccessor->FindImplementation(*iRegistryData, implementationUid, KCDummyUid, implementationData);
		TEST_OOM_ERR;
		TEST2(err,KErrNone);
		implementationInfo = implementationData->iImplInfo;
		TEST(implementationInfo->Version() == 2);
		TheTest.Printf(_L("DLL data is updated \n"));
		//cleans the upgraded resource file
		TRAP(err,EComTestUtils::FileManDeleteFileL(KEcomExample12UpgradedOnC));
		}
	registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData);
	TEST(registered);
	TEST(!iUpdate);
	}



/**
@SYMTestCaseID          SYSLIB-ECOM-UT-3569
@SYMTestCaseDesc	    Tests for CRegistryData::AddDllDataL() function using Dll type PLUGIN3
@SYMTestPriority 	    High
@SYMTestActions  	    Adds data on a specific dll to the registry.
                        Tests for OOM error,checks if drive is registered.
@SYMTestExpectedResults Dll data is added to the registry.
@SYMEC                  EC43
*/
void CRegistryDataTest::AddDllDataPlugIn3TestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3569 "));
	// Checks if drive is registered
	TEST(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit)!= KErrNotFound);
	CRegistryData::CDriveData* driveData = NULL;
	// Fetches index of drive, in iFoundDriveIndex
	iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData);

	TEntry dllEntry;
	TUid example10ThirdUid = {0x10009E37};
	dllEntry.iType = TUidType(KNullUid, KNullUid, example10ThirdUid);
	dllEntry.iName = KEComExample10DllOnC;
	TBool registered = EFalse;
	TRAPD(err, registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TEST(!registered);
	// Dll Data object to be added
	CRegistryData::CDllData* addDllData = NULL;
	TParse fileparse;
	fileparse.Set(dllEntry.iName,NULL,NULL);
	addDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),dllEntry.iModified,dllEntry[1], dllEntry[2],driveData);

	// Adds DLL data of an existing DLL
	TRAP(err, iRegistryData->AddDllDataL(iDriveUnit, iFoundDriveIndex, addDllData));

	TEST_OOM_ERR;
	CleanupStack::Pop(addDllData);
	TEST2(err, KErrNone);


	TRAP(err, iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TRAP(err, registered = iRegistryData->IsRegisteredWithDate(
										 dllEntry[2],
										 dllEntry.iModified,
										 iUpdate,
										 driveData));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TEST(registered);
	// Do not delete addDllData as ownership is with the iRegistryData
	}


/**
Adds the dll data on a perticular drive.

@SYMTestCaseID          SYSLIB-ECOM-CT-0738
@SYMTestCaseDesc	    Tests for CRegistryData::AddDllDataL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Adds data on a specific dll to the registry.
                        Tests for OOM error,checks if drive is registered.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::AddDllDataTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0738 "));
	// Check if drive is registered
	TEST(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit)!= KErrNotFound);
	CRegistryData::CDriveData* driveData = NULL;
	// Fetch index of drive, in iFoundDriveIndex
	iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData);

	iRegistryData->IsRegisteredWithDate(
										 iDllEntry[2],
										 iDllEntry.iModified,
										 iUpdate,

										 driveData);
	// Dll Data object to be added
	CRegistryData::CDllData*			addDllData=NULL;
	TParse fileparse;
	fileparse.Set(iDllEntry.iName,NULL,NULL);
	addDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),iDllEntry.iModified,iDllEntry[1], iDllEntry[2],driveData);

	// Add DLL data of an existing DLL
	TRAPD(err, iRegistryData->AddDllDataL(iDriveUnit, iFoundDriveIndex, addDllData));

	TEST_OOM_ERR;
	CleanupStack::Pop(addDllData);
	TEST2(err, KErrNone);

	TBool registered = EFalse;
	TRAP(err, iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TRAP(err, registered = iRegistryData->IsRegisteredWithDate(
										 iDllEntry[2],
										 iDllEntry.iModified,
										 iUpdate,

										 driveData));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TEST(registered);
	// Do not delete addDllData as ownership is with the iRegistryData
	}

/**
Determines if the specified implementation is already registered and
updates the data for the dll in the registry if it needs an update

@SYMTestCaseID          SYSLIB-ECOM-CT-0739
@SYMTestCaseDesc	    Tests for CRegistryData::UpdateDllDataL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Update the data for a dll in the registry if it needs an update.
                        Tests for OOM error.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::IsRegisteredWithDateUpdateDllDataTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0739 "));
	TBool result=EFalse;
	CRegistryData::CDriveData* driveData = NULL;
	TInt err = KErrNone;
	// Here we are doing positive testing. We are testing with valid entry.
	// If that is not registered, register it using UpdateDllDataL
	TRAP(err, iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	TRAP(err, result = iRegistryData->IsRegisteredWithDate(
					  										 iDllEntry[2],
															 iDllEntry.iModified,
															 iUpdate,

															 driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	// Log message if implementations need update
	if(result)
		{
		if(!iUpdate)
			{
			TEST(!iUpdate);
			TheTest.Printf(_L("Specified implementation group is already registered and does not need an update\n"));
			}
		else
			{
			TheTest.Printf(_L("Specified implementation group is not registered with date and needs an update\n"));

			// Dll Data object to be updated
			CRegistryData::CDllData*			updateDllData=NULL;
			TParse fileparse;
			fileparse.Set(iDllEntry.iName,NULL,NULL);
			updateDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),iDllEntry.iModified,iDllEntry[1],iDllEntry[2],driveData);
			TDriveUnit aDrive;

			// Updates DLL data with a new dll data
			TRAP(err, iRegistryData->UpdateDllDataL(aDrive, iFoundDriveIndex, updateDllData));
			TEST_OOM_ERR;
			CleanupStack::Pop(updateDllData);
			TEST2(err, KErrNone);
			// Print to that dll data is updated else we won't know we did.
			TheTest.Printf(_L("DLL data is updated \n"));
			}
		}

	TUid thirdUid = {0x101F8400};
	iDllEntry.iType = TUidType(KNullUid, KNullUid, thirdUid);
	// Here we are doing negative testing. We are testing with invalid entry.
	// The result should be false
	TRAP(err, iFoundDriveIndex = iRegistryData->FindDriveL(iDriveUnit,driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TRAP(err, result = iRegistryData->IsRegisteredWithDate(
					  										iDllEntry[2],
															iDllEntry.iModified,
															iUpdate,

															driveData));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TEST(!result);
	}

/**
Tests that an implementation is disabled and enabled by setting the state

@SYMTestCaseID          SYSLIB-ECOM-CT-0740
@SYMTestCaseDesc	    Tests for CRegistryData::SetEnabledState() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for enable or disable the specified interface implementation
						within the registry.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::SetEnabledStateTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0740 "));
	// A reference to the implementation entry in the registry
	CRegistryData::CImplementationData*	implementationData = NULL;

	// Pass the state and enable or disable the implementaion
	TInt err;
	err = iStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, KCDummyUid, implementationData);
	TEST2(err, KErrNone);

	const CImplementationInformation* implementationInfo = implementationData->iImplInfo;

	// SetEnabledState() will disable the implementation if enablestate parameter is EFalse.
	// If the  disablestate parameter is false the reverse will happen.
	TBool result=EFalse;
	TBool enableState=EFalse;
	TRAP(err, result = iRegistryData->SetEnabledState(iImplementationUid, enableState));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TEST(result == KErrNone);

	//Implementation should be disabled
	TEST(implementationInfo->Disabled()==(!enableState));

	// enable it
	enableState = ETrue;
	TRAP(err, result = iRegistryData->SetEnabledState(iImplementationUid, enableState));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TEST(result == KErrNone);

	// Implementation should be enabled
	TEST(implementationInfo->Disabled()==(!enableState));
	}

/**
Obtains Implementation data and prints to log

@SYMTestCaseID		SYSLIB-ECOM-CT-0741
@SYMTestCaseDesc	Tests for CRegistryData::FindImplementation() functions
@SYMTestPriority		High
@SYMTestActions		Fetch the information on a particular implementation (UID)
					Tests for the validation of the information fetched.
					Fetch the information on a particular implementation (UID) and interface (UID)
					Tests for the validation of the information fetched.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::FindImplementationTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0741 "));
	// A reference to the implementation entry in the registry
	CRegistryData::CImplementationData*	implementationData = NULL;

	// find implementation with interface uid unknown
	TInt err = iStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, KCDummyUid, implementationData);
	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	const CImplementationInformation* implementationInfo = implementationData->iImplInfo;

	TheTest.Printf((implementationInfo->DisplayName()));
	_LIT(KUIDFormat,"Uid = 0x%08X\n");
	TheTest.Printf(KUIDFormat, iImplementationUid);
	TheTest.Printf(KUIDFormat, implementationInfo->ImplementationUid());
	// Check for if its the same implementation
	TEST(iImplementationUid == implementationInfo->ImplementationUid());

	// find implementation with interface uid known
	err = iStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, iInterfaceUid, implementationData);
	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	implementationInfo = implementationData->iImplInfo;

	TheTest.Printf((implementationInfo->DisplayName()));
	TheTest.Printf(KUIDFormat, iImplementationUid);
	TheTest.Printf(KUIDFormat, implementationInfo->ImplementationUid());
	TheTest.Printf(KUIDFormat, iInterfaceUid);
	TheTest.Printf(KUIDFormat, implementationData->iParent->iInterfaceUid);

	// Check for if its the same implementation
	TEST(iImplementationUid == implementationInfo->ImplementationUid());
	// Check for if its the same interface
	TEST(iInterfaceUid == implementationData->iParent->iInterfaceUid);

	// attempt to find unknown implementation with dummy interface uid
	err = iStateAccessor->FindImplementation(*iRegistryData, KCInvalidUid, KCDummyUid, implementationData);
	TEST_OOM_ERR;
	TEST2(err, KErrNotFound);

	// attempt to find implementation for unknown interface uid
	err = iStateAccessor->FindImplementation(*iRegistryData, iImplementationUid, KCInvalidUid, implementationData);
	TEST_OOM_ERR;
	TEST2(err, KErrNotFound);
	}

/**
Tests functionality of ListImplementationsL for an interface id and listing all
the implementations.
The interface Uid 0x10009DC0 has 6 implementations on different DLLs but only 4 will
be picked as as 2 of them are of older version(Impl ID 0x10009DC3 and 0x10009DC4 with version 1)

Interface UID	DLL	UID		Implementation UID	Version
-------------------------------------------------------
0x10009DC0		0x10009DB1	0x10009DC3			1
				0x10009DB1	0x10009DC4			1
				0x10009DB3	0x10009DC3			2
				0x10009DB3	0x10009DC4			2
				0x101F8477	0x101F8478			1
				0x101F847B	0x101F847C			1

@SYMTestCaseID          SYSLIB-ECOM-CT-0742
@SYMTestCaseDesc	    Tests for CRegistryData::ListImplementationsL() function
@SYMTestPriority 	    High
@SYMTestActions  	    List the implementations for an existing interface UID and
                        test for expected number of implementations.Check the VID of
                        every implementation match the VID of the DLL which the implementation
                        belongs to.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000, REQ6111
*/
void CRegistryDataTest::ListImplementationsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0742 "));
	// List the implementations for an existing interface UID
	TEST(iRegistryData->IndexValid());

	TRAPD(err, iRegistryData->ListImplementationsL(iInterfaceUid,iImplDataList));
	CleanupClosePushL(iImplDataList);

	TEST_OOM_ERR;
	TEST2(err, KErrNone);

	// Print UIDs of implementations
	_LIT(KUIDFormat,"Uid = 0x%08x\n");
	_LIT(KLists,"There are %d Implementations \n");

	TheTest.Printf(KLists, iImplDataList.Count());
	// Test for expected number of implementations
	TEST(iImplDataList.Count() == 4);

	for(TInt index = 0; index < iImplDataList.Count(); ++index)
		{
		CImplementationInformation* implInfo = iImplDataList[index]->iImplInfo;
		TUid uid = implInfo->ImplementationUid();
		TheTest.Printf(KUIDFormat, uid);
		}
	// Empty the array of implementation data
	CleanupStack::PopAndDestroy(&iImplDataList);
	}

/**
Tests functionality of TemporaryUninstallL and UndoTemporaryUninstallL
Test does temporarily 1.install 2.uninstall and 3.install the specified drive

@SYMTestCaseID          SYSLIB-ECOM-CT-0743
@SYMTestCaseDesc	    Tests for CRegistryData::UndoTemporaryUninstallL(),TemporaryUninstallL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for temporarily install,uninstall,install the specified drive
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::DoUndoTemporaryUninstallTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0743 "));
	// We are testing with F drive.
	iDriveUnit = EDriveF;
	TBool driveregistered = iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit);
	// Check that the drive is registered. If its not there add it by UndoTemporaryUninstallL
	if (!driveregistered)
		{
		TRAPD(err, iRegistryData->UndoTemporaryUninstallL(iDriveUnit));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		}

	// Test drive should be present
	TEST(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit));

	// Uninstall the test drive
	TRAPD(err, iRegistryData->TemporaryUninstallL(iDriveUnit));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	// Test drive should not be present
	TEST(!(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit)));

	// Put the test drive back to work if it was registered before
	if (driveregistered)
		{
		TRAP(err, iRegistryData->UndoTemporaryUninstallL(iDriveUnit));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		// Test drive should be present
		TEST(iStateAccessor->IsRegisteredDriveL(*iRegistryData, iDriveUnit));
		}
	}

/**
Tests functionality of DiscoveriesBeginning and DiscoveriesCompleteL
Tests registry data discovery process.

@SYMTestCaseID          SYSLIB-ECOM-CT-0745
@SYMTestCaseDesc	    Tests for CRegistryData::DiscoveriesBeginning(),CRegistryData::DiscoveriesCompleteL() functions
@SYMTestPriority 	    High
@SYMTestActions  	    Tests for registry data discovery process.
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::DiscoveriesBeginCompleteTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0745 "));
	// Issue discoveries begin and complete requests
	// and confirm the same

	// DiscoveriesBegin marks the registry indexes as invalid as
	// the discovery process is in progress
	TRAPD(err, iRegistryData->DiscoveriesBeginning());
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	// Check Registry is invalid
	TEST(!iStateAccessor->IsIndexValid(*(iRegistryData)));

	// DiscoveriesCompleteL marks the registry indexes valid as
	// the discovery process is complete
	TBool regChanged = EFalse;
	TRAP(err, iRegistryData->DiscoveriesCompleteL(EFalse, EPluginProcessingTypeAll,regChanged));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	// Check Registry is valid
	TEST(iStateAccessor->IsIndexValid(*(iRegistryData)));
	}

/**
Tests finding index for an entry for implementation Uid

@SYMTestCaseID          SYSLIB-ECOM-CT-0747
@SYMTestCaseDesc	    Tests for CRegistryData::IndexedFindTestL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Tests to find the index entry for aImplementationUid and log it
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
void CRegistryDataTest::IndexedFindTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0747 "));
	// Find the index entry for aImplementationUid and print it
	TInt index=0;

	TRAPD(err, index = iStateAccessor->IndexedFind(*(iRegistryData), iInterfaceUid));
	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	TheTest.Printf(_L("The index of interface 0x%08x is %d \n"), iInterfaceUid, index);
	// Index should have some valid value
	TEST(index >= 0);
	}


/**
Plugins to be used in this test
Interface UID	DLL	UID		Implementation UID	Version		DLL File
--------------------------------------------------------------------------------
0x10009E36		0x10009E3E	0x10009E39				1		C:\\..\\EComExample12.dll
				0x10009E3E	0x10009E3C				1		C:\\..\\EComExample12.dll
				0x10009E3E	0x10009E3D				1		C:\\..\\EComExample12.dll
				0x10009E42	0x10009E39				2		C:\\..\\EComExample15.dll


@SYMTestCaseID			SYSLIB-ECOM-UT-3573
@SYMTestCaseDesc		Tests for CRegistryData::GetImplementationDllInfoForClientL() function for DLL type PLUGIN3.
@SYMTestPriority		High
@SYMTestActions			Get the plugin information of the implementation UID and validate the information.
						Get the plugin information of the implementation UID and Interface UID and validate
						the information.
						Tests for that the implementation returned will have the highest version
@SYMTestExpectedResults	Implementation information is returned correctly.
@SYMEC					EC43
*/
void CRegistryDataTest::GetImplementationDllInfoForClientPlugIn3TestL()
 	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3573 "));
	TUid ImpUid15 = {0x10009E39};
 	TUid ImpUid12 = {0x10009E3C};
 	TUid InterfaceUid = {0x10009E36};
 	TEntry dllinfo;
 	TInt err;
 	TClientRequest clntRq;

 	//Tests the CImplementationInformation is correct
 	//and the DllInfo contains correct implementation info
 	//Implementation uid used=0x10009E3C
 	CImplementationInformation* implementationInfo = NULL;
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid12, KCDummyUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid12);
 	TEST(implementationInfo->Version() == 1);
 	TEST(dllinfo.iName.CompareF(KEComExample12DllOnC) == 0);

 	// ... again but with interface uid
  	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid12, InterfaceUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid12);
 	TEST(implementationInfo->Version() == 1);
 	TEST(dllinfo.iName.CompareF(KEComExample12DllOnC) == 0);

 	//Tests that the implementation returned will have the highest version
 	//EComExample12.dll  contains Impl 0x10009E39 with version 1
 	//EComExample15.dll contains Impl 0x10009E39 with version 2
 	//Ensure that choose the implementation in EComExample15.dll
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid15, KCDummyUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid15);
 	TEST(implementationInfo->Version() == 2);
 	TEST(dllinfo.iName.CompareF(KEComExample15DllOnC) == 0);

 	// ... again but with interface uid
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid15, InterfaceUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid15);
 	TEST(implementationInfo->Version() == 2);
 	TEST(dllinfo.iName.CompareF(KEComExample15DllOnC) == 0);
 	}

/**
Plugins to be used in this test
Interface UID	DLL	UID		Implementation UID	Version		DLL File
--------------------------------------------------------------------------------
0x10009E36		0x10009E3E	0x10009E39				1		C:\\..\\EComExample12.dll
				0x10009E3E	0x10009E3C				1		C:\\..\\EComExample12.dll
				0x10009E3E	0x10009E3D				1		C:\\..\\EComExample12.dll
				0x10009E42	0x10009E39				2		C:\\..\\EComExample15.dll

@SYMTestCaseID			SYSLIB-ECOM-UT-3574
@SYMTestCaseDesc		Tests for CRegistryData::GetImplementationDllInfoForServer() function for DLL type PLUGIN3.
@SYMTestPriority		High
@SYMTestActions			Get the plugin information of the implementation UID and validate the information.
						Get the plugin information of the implementation UID and Interface UID and validate
						the information.
						Tests for loading implementation from RAM
						Tests for that the implementation returned will have the highest version
@SYMTestExpectedResults	Implementation information is returned correctly.
@SYMEC					EC43
*/
void CRegistryDataTest::GetImplementationDllInfoForServerPlugIn3Test()
 	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3574 "));
  	TUid ImpUid15={0x10009E39};
 	TUid ImpUid12={0x10009E3C};
 	TUid InterfaceUid={0x10009E36};
 	TEntry dllinfo;
 	TInt err;
	TBool dummyBool;

 	//Tests the CImplementationInformation is correct
 	//and the DllInfo contains correct implementation info
 	//Implementation uid used=0x10009E3C
 	CImplementationInformation* implementationInfo=NULL;
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid12, KCDummyUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid12);
 	TEST(implementationInfo->Version() == 1);
 	TEST(dllinfo.iName.CompareF(KEComExample12DllOnC) == 0);

 	// ... again but with interface uid
  	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid12, InterfaceUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid12);
 	TEST(implementationInfo->Version() == 1);
 	TEST(dllinfo.iName.CompareF(KEComExample12DllOnC) == 0);

 	//Tests that the implementation returned will have the highest version
 	//EComExample12.dll  contains Impl 0x10009E39 with version 1
 	//EComExample15.dll contains Impl 0x10009E39 with version 2
 	//Ensure that choose the implementation in EComExample15.dll
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid15, KCDummyUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid15);
 	TEST(implementationInfo->Version() == 2);
 	TEST(dllinfo.iName.CompareF(KEComExample15DllOnC) == 0);

 	// ... again but with interface uid
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid15, InterfaceUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid() == ImpUid15);
 	TEST(implementationInfo->Version() == 2);
 	TEST(dllinfo.iName.CompareF(KEComExample15DllOnC) == 0);
 	}

/**
Tests for the CRegistryData::GetImplementationDllInfoForClientTestL.
Plugins to be used in this test
Interface UID	DLL	UID		Implementation UID	Version		DllFile
-------------------------------------------------------------------
0x10009DC0		0x10009DB1	0x10009DC3			1		C:\\..\\EComExample.dll
				0x10009DB1	0x10009DC4			1		C:\\..\\EComExample.dll
				0x10009DB3	0x10009DC3			2		C:\\..\\EComExample2.dll
				0x10009DB3	0x10009DC4			2		C:\\..\\EComExample2.dll
				0x101F8477	0x101F8478			1		C:\\..\\EComExample3.dll
				0x101F847B	0x101F847C			1		C:\\..\\EComExample5.dll


@SYMTestCaseID			SYSLIB-ECOM-CT-1374
@SYMTestCaseDesc		Tests for CRegistryData::GetImplementationDllInfoForClientL() function.
@SYMTestPriority			High
@SYMTestActions			Get the plugin information of the implementation UID and validate the information.
						Get the plugin information of the implementation UID and Interface UID and validate
						the information.
						Tests for  the CImplementationInformation is correct
						Tests for loading implementation from RAM
						Tests for that the implementation returned will have the highest version
						Tests for for invalid ImplementationUid
@SYMTestExpectedResults	The test must not fail.
@SYMREQ				DEF065552
*/
void CRegistryDataTest::GetImplementationDllInfoForClientTestL()
 	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1374 "));
 	_LIT(KEComExample2,"C:\\sys\\bin\\EComExample2.dll");
 	_LIT(KEComExample3,"C:\\sys\\bin\\EComExample3.dll");
 	_LIT(KEComExample5,"C:\\sys\\bin\\EComExample5.dll");

 	TUid ImpUid={0x10009DC3};
 	TUid ImpUid3={0x101F8478};
 	TUid ImpUid5={0x101F847C};
 	TEntry dllinfo;
 	TInt err;
 	TClientRequest clntRq;

 	//Testing the CImplementationInformation is correct
 	//and the DllInfo contains correct implementation info
 	//Implementation uid used=0x101F8478
 	CImplementationInformation* implementationInfo=NULL;
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid3, KCDummyUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid3);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample3)==0);

 	// ... again but with interface uid
  	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid3, iInterfaceUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid3);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample3)==0);

 	//Testing loading implementation from RAM(EComExample5.dll)
 	//Implementation uid used=0x101F847C
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid5, KCDummyUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid5);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample5)==0);

 	// ... again but with interface uid
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid5, iInterfaceUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid5);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample5)==0);

 	//Testing that the implementation returned will have the highest version
 	//EComExample.dll  contains Impl 0x10009DC3 with version 1
 	//EComExample2.dll contains Impl 0x10009DC3 with version 2
 	//Ensure that choose the implementation in EComExample2.dll
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid, KCDummyUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid);
 	TEST(implementationInfo->Version()==2);
 	TEST(dllinfo.iName.CompareF(KEComExample2)==0);

 	// ... again but with interface uid
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid, iInterfaceUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid);
 	TEST(implementationInfo->Version()==2);
 	TEST(dllinfo.iName.CompareF(KEComExample2)==0);

 	//Testing for invalid ImplementationUid
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, KCInvalidUid, KCDummyUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNotFound);
 	TEST(implementationInfo==0);

 	//Testing for invalid InterfaceUid
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, ImpUid, KCInvalidUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNotFound);
 	TEST(implementationInfo==0);

 	//Testing for invalid ImplementationUid and invalid InterfaceUid
 	err = iRegistryData->GetImplementationDllInfoForClientL(clntRq, KCInvalidUid, KCInvalidUid, dllinfo, implementationInfo, ETrue);
 	TEST2(err, KErrNotFound);
 	TEST(implementationInfo==0);
 	}

/**
Tests for the CRegistryData::GetImplementationDllInfoForServer.
This new API is introduced in the Clientside loading of plugins(ECOM Platform security)
where the server will return the plugin information(aEntry) of the implementation Uid
requested by the client.
Plugins to be used in this test
Interface UID	DLL	UID		Implementation UID	Version		DllFile
-------------------------------------------------------------------
0x10009DC0		0x10009DB1	0x10009DC3			1		C:\\..\\EComExample.dll
				0x10009DB1	0x10009DC4			1		C:\\..\\EComExample.dll
				0x10009DB3	0x10009DC3			2		C:\\..\\EComExample2.dll
				0x10009DB3	0x10009DC4			2		C:\\..\\EComExample2.dll
				0x101F8477	0x101F8478			1		C:\\..\\EComExample3.dll
				0x101F847B	0x101F847C			1		C:\\..\\EComExample5.dll


@SYMTestCaseID			SYSLIB-ECOM-CT-0749
@SYMTestCaseDesc		Tests for CRegistryData::GetImplementationDllInfoForServer() function.
@SYMTestPriority			High
@SYMTestActions			Get the plugin information of the implementation UID and validate the information.
						Get the plugin information of the implementation UID and Interface UID and validate
						the information.
						Tests for  the CImplementationInformation is correct
						Tests for loading implementation from RAM
						Tests for that the implementation returned will have the highest version
						Tests for for invalid ImplementationUid
@SYMTestExpectedResults	The test must not fail.
@SYMREQ				REQ0000
*/
void CRegistryDataTest::GetImplementationDllInfoForServerTest()
 	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0749 "));
 	_LIT(KEComExample2,"C:\\sys\\bin\\EComExample2.dll");
 	_LIT(KEComExample3,"C:\\sys\\bin\\EComExample3.dll");
 	_LIT(KEComExample5,"C:\\sys\\bin\\EComExample5.dll");

 	TUid ImpUid={0x10009DC3};
 	TUid ImpUid3={0x101F8478};
 	TUid ImpUid5={0x101F847C};
 	TEntry dllinfo;
 	TInt err;
	TBool dummyBool;

 	//Testing the CImplementationInformation is correct
 	//and the DllInfo contains correct implementation info
 	//Implementation uid used=0x101F8478
 	CImplementationInformation* implementationInfo=NULL;
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid3, KCDummyUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid3);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample3)==0);

 	// ... again but with interface uid
  	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid3, iInterfaceUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid3);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample3)==0);

 	//Testing loading implementation from RAM(EComExample5.dll)
 	//Implementation uid used=0x101F847C
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid5, KCDummyUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid5);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample5)==0);

 	// ... again but with interface uid
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid5, iInterfaceUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid5);
 	TEST(implementationInfo->Version()==1);
 	TEST(dllinfo.iName.CompareF(KEComExample5)==0);

 	//Testing that the implementation returned will have the highest version
 	//EComExample.dll  contains Impl 0x10009DC3 with version 1
 	//EComExample2.dll contains Impl 0x10009DC3 with version 2
 	//Ensure that choose the implementation in EComExample2.dll
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid, KCDummyUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid);
 	TEST(implementationInfo->Version()==2);
 	TEST(dllinfo.iName.CompareF(KEComExample2)==0);

 	// ... again but with interface uid
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid, iInterfaceUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNone);
 	TEST(implementationInfo->ImplementationUid()==ImpUid);
 	TEST(implementationInfo->Version()==2);
 	TEST(dllinfo.iName.CompareF(KEComExample2)==0);

 	//Testing for invalid ImplementationUid
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, KCInvalidUid, KCDummyUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNotFound);
 	TEST(implementationInfo==0);

 	//Testing for invalid InterfaceUid
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, ImpUid, KCInvalidUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNotFound);
 	TEST(implementationInfo==0);

 	//Testing for invalid ImplementationUid and invalid InterfaceUid
 	err = iRegistryData->GetImplementationDllInfoForServer(dummycaps, KCInvalidUid, KCInvalidUid, dllinfo, implementationInfo, dummyBool);
 	TEST2(err, KErrNotFound);
 	TEST(implementationInfo==0);
 	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0750
@SYMTestCaseDesc	    OOM test for the creation and deletion of CRegistryData
@SYMTestPriority 	    High
@SYMTestActions  	    Create and delete CRegistryData object,checks for handles leaked
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
LOCAL_C void OOMCreateDeleteTest()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0750 OOM CreateDeleteTest "));
	TInt err;
	TInt failAt = 1;
	__UNUSED_VAR(failAt);

	CRegistryDataTest* theTest = NULL;

	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		__UHEAP_SETFAIL(RHeap::EDeterministic, failAt++);

		TRAP(err, theTest = CRegistryDataTest::NewL(EFalse));

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		delete theTest;
		theTest = NULL;

		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		}
	while(err == KErrNoMemory);

	TheTest.Printf(_L("- Succeeded at heap failure rate of %i\n"), failAt);
 	TEST2(err, KErrNone);
	}

/**
@SYMTestCaseID			SYSLIB-ECOM-UT-1408
@SYMTestCaseDesc		Unit tests for CRegistryData::IsAnyDiscoveredL
@SYMTestPriority		High
@SYMTestActions			Calls each of the actions that should cause IsAnyDiscovered to be set. Because the behavior is different with and without SPI, the tests are separated into a test with SPI and a test without
@SYMTestExpectedResults	IsAnyDiscovered should be false when CDriveData is created, and false when the drive is internalized with no dat file.
						It should be true when the drive is internalized and there is a dat file.
@SYMPREQ				PREQ1192
*/
void  CRegistryDataTest::IsAnyDllDiscoveredTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1408 "));
	if (SpiFileExists())
		{
		IsAnyDllDiscoveredTestWithSpiL();
		}
	IsAnyDllDiscoveredTestNoSpiL();
	}

void  CRegistryDataTest::IsAnyDllDiscoveredTestWithSpiL()
	{
	//
	// Test DatFileExists for dat files on ROM when there is a SPI file
	//

	iStateAccessor->ClearRegistrations(*iRegistryData);

	// when a CDriveData is created DatFileExists should be false
	CRegistryData::CDriveData* driveDataZ = CRegistryData::CDriveData::NewLC(EDriveZ, iRegistryData);
	iStateAccessor->AppendDriveDataL(*iRegistryData, driveDataZ);
	CleanupStack::Pop(driveDataZ);
	TEST(!iRegistryData->IsAnyDllRegisteredWithDriveL(EDriveZ));
	}

void  CRegistryDataTest::IsAnyDllDiscoveredTestNoSpiL()
	{
	//
	// Test IsAnyDiscovered for dat files on ROM when there is no SPI file
	//

	iStateAccessor->ClearRegistrations(*iRegistryData);

	// when a CDriveData is created there should be no DLL entries so IsAnyDiscoveredL should be false
	CRegistryData::CDriveData* driveDataC = CRegistryData::CDriveData::NewLC(EDriveC, iRegistryData);
	iStateAccessor->AppendDriveDataL(*iRegistryData, driveDataC);
	CleanupStack::Pop(driveDataC);
	TEST(!iRegistryData->IsAnyDllRegisteredWithDriveL(EDriveC));
	}

/**
Adds the dll data on a particular drive.

@SYMTestCaseID          SYSLIB-ECOM-UT-1407
@SYMTestCaseDesc	    Tests for CRegistryData::CDllData::ProcessSecurityCheckL() function
@SYMTestPriority 	    High
@SYMTestActions  	    Creates CDllData objects and calls ProcessSecurityCheckL on them
                        The CDllData with a missing DLL should return EFalse, the CDllData with invalid
                        SID should return EFalse, and a valid CDllData should return ETrue and ensure the VID
                        match the VID of the valid DLL.
@SYMTestExpectedResults ProcessSecurityCheckL returns false when a plugin's dll doesn't exist or has an invalid SID, true otherwise.
@SYMREQ                 PREQ1192, REQ6111
*/
void CRegistryDataTest::ProcessSecurityCheckTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1407 "));
	TParse fileparse;
	TDriveUnit driveUnitC(EDriveC);
	CRegistryData *registryData = CRegistryData::NewL(TheFs);
	CleanupStack::PushL(registryData);
	CRegistryData::CDriveData *driveCData = CRegistryData::CDriveData::NewLC(driveUnitC, registryData);

	//
	// Test that a missing DLL causes ProcessSecurityCheckL to return EFalse
	//
	TEntry missingDllEntry;
	missingDllEntry.iName = KEComMissingDll;
	fileparse.Set(KEComMissingDll,NULL,NULL);
	CRegistryData::CDllData* missingDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),missingDllEntry.iModified,KNullUid,KNullUid, driveCData);
	TEST(!missingDllData->ProcessSecurityCheckL());
	CleanupStack::PopAndDestroy(missingDllData);

	//
	// Test that an invalid SID causes ProcessSecurityCheckL to return EFalse
	//
	TEntry mismatchedSidDllEntry;
	// The actual secure Id for this DLL is 0x10009DB1, here the corresponding ID is set to 0x01010101
	TUidType mismatchedSidUidType(KCInvalidUid, KCInvalidUid, KCInvalidUid);
	mismatchedSidDllEntry.iName = KEComExampleDllOnZSysBin;
	mismatchedSidDllEntry.iType = mismatchedSidUidType;
	fileparse.Set(KEComExampleDllOnZSysBin,NULL,NULL);
	CRegistryData::CDllData* mismatchedSidDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),mismatchedSidDllEntry.iModified,mismatchedSidUidType[1],mismatchedSidUidType[2], driveCData);
	TEST(!mismatchedSidDllData->ProcessSecurityCheckL());
	CleanupStack::PopAndDestroy(mismatchedSidDllData);

	//
	// Test that a valid entry causes ProcessSecurityCheckL to return ETrue
	//
	TUid KEcomExampleSecureId = {0x10009DB1};
	TEntry validDllEntry;
	TUidType validUidType(KCInvalidUid, KUidInterfaceImplementationCollection, KEcomExampleSecureId);
	validDllEntry.iName = KEComExampleDllOnZSysBin;
	validDllEntry.iType = validUidType;
	fileparse.Set(KEComExampleDllOnZSysBin(),NULL,NULL);
	CRegistryData::CDllData* validDllData = CRegistryData::CDllData::NewLC(fileparse.NameAndExt(),validDllEntry.iModified,validUidType[1],validUidType[2], driveCData);
	// Because ProcessSecurityCheckL is handled differently the first and second times it is called for a given DLL
	// ProcessSecurityCheckL is called twice here to test both cases (the fist time the security check is performed, and the second time
	// a cached value is used)
	TEST(validDllData->ProcessSecurityCheckL());
	TEST(validDllData->ProcessSecurityCheckL());
	// check the VID of the DLL.
	TEST((validDllData->iVid).iId == 0x70000001);
	CleanupStack::PopAndDestroy(validDllData);


	// Clean up
	CleanupStack::PopAndDestroy(driveCData);
	CleanupStack::PopAndDestroy(registryData);
	}

/**
Tests functionality of ListImplementationsL for an interface ID where the security checks
for DLLs implementing the given interface fail

@SYMTestCaseID          SYSLIB-ECOM-UT-1405
@SYMTestCaseDesc	    Tests for CRegistryData::ListImplementationsL() function
@SYMTestPriority 	    High
@SYMTestActions  		Makes sure the failed DLL security checks are handled properly
						An RSC file with an incorrect SID is tested, followed by an RSC file with a missing DLL
@SYMTestExpectedResults ECom plugin's with invalid DLLs are not returned.
@SYMREQ                 PREQ1192
*/
void CRegistryDataTest::FailedSecurityCheckTestListImplementationsL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1405 "));
	// Set up for heap leak checking
	__UHEAP_MARK;

	//
	// Test that mismatching SIDs cause a plug-in to not be returned through ListImplementationsL
	// InvalidSIDPlugin.rsc and InvalidSIDPlugin.dll are on the C drive, but their SIDs do not match
	// This will cause the DLL security check to fail, and since there are no other implementations of this interface,
	// no implementations should be returned
	//
	TUid ifUid1 = {0x101FE392};	// the interface UID for InvalidSIDPlugin

	TRAPD(err, iRegistryData->ListImplementationsL(ifUid1,iImplDataList));

	TEST_OOM_ERR;
	// Since there are no other plug-ins implementing {0x101FE392}, there should be no implementations found
	TEST2(err, KEComErrNoInterfaceIdentified);

	//
	// Test that a missing DLL causes a plug-in to not be returned through ListImplementationsL
	// and that other valid implementations are rolled back to when this happens
	//
	// EComRomRslvrExampleOnC.rsc is on the C drive, and has an implementation that upgrades EComRomRslvrExampleOnZ.rsc
	// The DLL corresponding to EComRomRslvrExampleOnC.rsc is missing, so will fail the security check
	// This means Ecom should fall back to the implementations from EComRomRslvrExampleOnZ
	//
	TUid ifUid3 = {0x10009DC8};  // the interface UID for EComRomRslvrExampleDllOnZ and EComRomRslvrExampleDllOnC
	TRAP(err, iRegistryData->ListImplementationsL(ifUid3,iImplDataList));

	TEST_OOM_ERR;
	TEST2(err, KErrNone);
	CleanupClosePushL(iImplDataList);

	// Print UIDs of implementations
	_LIT(KUIDFormat,"Uid = 0x%08x\n");
	_LIT(KLists,"There are %d Implementations \n");

	TheTest.Printf(KLists, iImplDataList.Count());

	// Go through each of the implementations returned, and find the ones that match up with EComRomRslvrExampleRscOnZ
	// (not the ones from EComRomRslvrExampleRscOnC because it's DLL is missing)
	// EComRomRslvrExampleOnZ contains the implementation 0x10009DC7v2 and 0x10009DC6v1 (these should be found)
	// EComRomRslvrExampleOnC contains the implementation 0x10009DC7v1 and 0x10009DC6v2 (these should NOT be returned becuase of the missing DLL)
	TBool foundRightImplementation1 = EFalse;
	TBool foundRightImplementation2 = EFalse;
	for(TInt index = 0; index < iImplDataList.Count(); ++index)
		{
		CImplementationInformation* implInfo = iImplDataList[index]->iImplInfo;
		TUid uid = implInfo->ImplementationUid();
		TInt version = implInfo->Version();
		if (uid.iUid == 0x10009DC6)
			{
			TEST(version == 1);
			// The implementation of 0x10009DC6 from EComRomRslvrExampleOnZ was found
			foundRightImplementation1 = ETrue;
			}
		if (uid.iUid == 0x10009DC7)
			{
			TEST(version == 2);
			// The implementation of 0x10009DC7 from EComRomRslvrExampleOnZ was found
			foundRightImplementation2 = ETrue;
			}
		TheTest.Printf(KUIDFormat, uid);
		}
	// Test that both implementations were found
	TEST(foundRightImplementation1 && foundRightImplementation2);


	CleanupStack::PopAndDestroy(&iImplDataList);
	__UHEAP_MARKEND;
	}

/**
Tests functionality of ListImplementationsL for an interface id where the corresponding
DLL fails the se

@SYMTestCaseID          SYSLIB-ECOM-UT-1406
@SYMTestCaseDesc	    Tests for CRegistryData::GetImplementationDllInfoForClientL() function
@SYMTestPriority 	    High
@SYMTestActions  		Makes sure the plug-ins that will fail a security check are not returned through GetImplementationDllInfoForClientL
						InvalidSIDPlugin.rsc and InvalidSIDPlugin.dll are on the C drive, with non-matching SIDs, so will fail a security check
						Because the security check fails, no implementation should be returned
						EComRomRslvrExampleOnC is on the C drive, without its associated DLL so will fail a security check
						Because the DLL is not there, Ecom should fall back to the implementations on the Z drive in EComRomRslvrExampleOnZ
@SYMTestExpectedResults ECom plugin's with invalid DLLs are not returned.
@SYMREQ                 PREQ1192
*/
void CRegistryDataTest::FailedSecurityCheckTestGetImplementationInfoL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-1406 "));
	// Set up for heap leak checking
	__UHEAP_MARK;

	//
	// Test that mismatching SIDs cause a plug-in to not be returned through GetImplementationDllInfoForClientL
	//
	TUid implUid1 = {0x101FE393};  // the implementation UID for InvalidSIDPlugin
	TUid ifUid1 = {0x101FE392};  // the interface UID for InvalidSIDPlugin
	TClientRequest clientReq;
	TEntry dllInfo;
	CImplementationInformation *implementationInfo = NULL;
	// Test for GetImplementatioDllInfoForClientL when security check is not done.
	TInt err = iRegistryData->GetImplementationDllInfoForClientL(clientReq, implUid1, ifUid1, dllInfo, implementationInfo, EFalse);
	TEST2(err, KErrNone);
	TEST(dllInfo.iName.Compare(_L("C:\\sys\\bin\\InvalidSIDPlugin.dll")) == 0);
	// Test for GetImplementatioDllInfoForClientL when security check is done.
	err = iRegistryData->GetImplementationDllInfoForClientL(clientReq, implUid1, ifUid1, dllInfo, implementationInfo, ETrue);
	// Because the security check failed for InvalidSIDPlugin and there are no other implementations of implementation UID 0x101FE393, KErrNotFound is returned
	TEST2(err, KErrNotFound);
	//
	// Test that a missing DLL causes a plug-in to not be returned through GetImplementationDllInfoForClientL
	// and that other valid implementations are rolled back to when this happens
	//
	TUid implUid3 = {0x10009DC7};  // EComRomRslvrExampleDllOnZ
	TUid ifUid3 = {0x10009DC8};  // EComRomRslvrExampleDllOnZ
	err = iRegistryData->GetImplementationDllInfoForClientL(clientReq, implUid3, ifUid3, dllInfo, implementationInfo, ETrue);
	TEST2(err, KErrNone);
	// Because the security check failed for EComRomRslvrExampleDllOnC the returned implementation should rollback to the implementation on the Z drive
	TEST(dllInfo.iName.Compare(_L("C:\\sys\\bin\\EComRomRslvrExampleOnZ.dll")) == 0);

	__UHEAP_MARKEND;
	}

/**
Test case for Defect ECOM non-spi handling of multiple localised resource files with same name but different extension.
Only one dll is created.

@SYMTestCaseID		SYSLIB-ECOM-CT-1806
@SYMTestCaseDesc 	This function is used to check no duplication DLL is created when
					there are multiple resource files with same name but different extensions.
@SYMTestPriority 	High
@SYMTestActions  	Use CDiscoverer::CDirScanner::ScanDriveL to start discovery plugins on C:.
					Check that the dll list only has one dll existing with specified dll and interface UID.
@SYMTestExpectedResults The test must not fail.
@SYMDEF	DEF87787
*/
void CRegistryDataTest::CheckDuplicatPluginsOnCL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1806 "));

	//clean up all resource files in C:
	DeleteTestPlugin();

	TDriveUnit driveC = TDriveUnit(KDriveC);

	TRegistrarObserverStub* registrarObsv = new(ELeave) TRegistrarObserverStub;
	CleanupStack::PushL(registrarObsv);

	CRegistryData* registryData = CRegistryData::NewL(TheFs);
	CleanupStack::PushL(registryData);

	CRegistrar* registrar = CRegistrar::NewL(*registryData, *registrarObsv, TheFs);
	CleanupStack::PushL(registrar);

	TRegistryData_StateAccessor* stateAccessor= new(ELeave) TRegistryData_StateAccessor;
	CleanupStack::PushL(stateAccessor);

	//After construction check that: registry is empty
	TEST(stateAccessor->RegistrationsCount(*registryData) == 0);

    //Copies multiple resource file with same name but different extensions on C:.
    TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
 	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileOnC));
 	TEST2(err, KErrNone);
 	_LIT(KEComRscFileA01OnC,	"C:\\resource\\plugins\\EComExample5.A01");
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileA01OnC));
 	TEST2(err, KErrNone);
 	_LIT(KEComRscFileA02OnC,	"C:\\resource\\plugins\\EComExample5.A02");
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileA02OnC));
 	TEST2(err, KErrNone);

	//Scan C: drive
	TRegistrar_StateAccessor registrarStateAccessorStub;
	registrarStateAccessorStub.ScanDriveL(*registrar, driveC);


	TDll *DllList = NULL;
	TInt indx = stateAccessor->GetDllListL(*registryData, driveC, DllList);
	TEST(indx == 0);
	TEST(DllList->Count() == 1);
	CRegistryData::CDllData *dllData = (*DllList)[0];

	//Testing loading implementation from RAM(EComExample5.dll) with specified Dll and interface UID.
 	TUid thirdDllUid = {0x101F847B};
	TEST(dllData->iDllEntry->GetThirdUid() == thirdDllUid);
 	TUid InterfaceUid5={0x10009DC0};
	TEST2(stateAccessor->FindAnImplementation(*registryData, InterfaceUid5, driveC), ETrue);

	//Clean up
	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileA01OnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileA02OnC));

 	CleanupStack::PopAndDestroy(stateAccessor);
    CleanupStack::PopAndDestroy(registrar);
    CleanupStack::PopAndDestroy(registryData);
    CleanupStack::PopAndDestroy(registrarObsv);

    //back to orignal state before the test
    CopyPluginsL();
 	}

/**
Test case for Defect ECOM non-spi handling of multiple localised resource files but without
a proper RSC file based on language downgrade path.

@SYMTestCaseID		SYSLIB-ECOM-CT-1829
@SYMTestCaseDesc 	This function is used to check no DLL is created when multiple resource files
					exists but no proper RSC file based on language downgrade path.
@SYMTestPriority 	High
@SYMTestActions  	Use CDiscoverer::CDirScanner::ScanDriveL to start discovery plugins on C:.
					Check that the dll list is empty after scanning.
@SYMTestExpectedResults The test must not fail.
@SYMDEF	DEF87787
*/

void CRegistryDataTest::CheckNoPluginsOnCL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-1829 "));

	//clean up all resource files in C:
	DeleteTestPlugin();

	TDriveUnit driveC = TDriveUnit(KDriveC);

	TRegistrarObserverStub* registrarObsv = new(ELeave) TRegistrarObserverStub;
	CleanupStack::PushL(registrarObsv);

	CRegistryData* registryData = CRegistryData::NewL(TheFs);
	CleanupStack::PushL(registryData);

	CRegistrar* registrar = CRegistrar::NewL(*registryData, *registrarObsv, TheFs);
	CleanupStack::PushL(registrar);

	TRegistryData_StateAccessor* stateAccessor= new(ELeave) TRegistryData_StateAccessor;
	CleanupStack::PushL(stateAccessor);

	//After construction check that registry is empty
	TEST(stateAccessor->RegistrationsCount(*registryData) == 0);

    //Copies only EComExample5.A01 and EComExample5.A02 to C:, but no EComExample5.RSC
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
 	TEST2(err, KErrNone);
 	_LIT(KEComRscFileA01OnC,	"C:\\resource\\plugins\\EComExample5.A01");
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileA01OnC));
 	TEST2(err, KErrNone);
 	_LIT(KEComRscFileA02OnC,	"C:\\resource\\plugins\\EComExample5.A02");
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileA02OnC));
 	TEST2(err, KErrNone);

	//Scan C: drive
	TRegistrar_StateAccessor registrarStateAccessorStub;
	registrarStateAccessorStub.ScanDriveL(*registrar, driveC);

	//check dll list. No dll is created.
	TDll *DllList = NULL;
	TInt indx = stateAccessor->GetDllListL(*registryData, driveC, DllList);
	TEST(indx == 0);
	TEST(DllList->Count() == 0);

 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComExDllOnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileA01OnC));
 	TRAP(err, EComTestUtils::FileManDeleteFileL(KEComRscFileA02OnC));

 	CleanupStack::PopAndDestroy(stateAccessor);
    CleanupStack::PopAndDestroy(registrar);
    CleanupStack::PopAndDestroy(registryData);
    CleanupStack::PopAndDestroy(registrarObsv);

    //back to orignal state before the test
    CopyPluginsL();

	}

/**
Test case for implementations with same implementation UID but different
interface UIDs. Implementation UIDs should be globally unique. So this
is an error condition. But to maintain backward compatibility,
ECom will allow it. List operation and most of the time, create
should work. Problem arises if client uses the create request which
does not supply the interface UID. In that case ECom applies a set of
rules to determine which of the duplicate to pick. The returned
impl. may not be the one the client is expecting. The rules from high
precedence to low are:
1. ROM based > non-ROM based,
2. High drive letter > low drive letter
3. Low I/F UID > High I/F UID.
Rule number 1 is tested in SYSLIB-ECOM-CIT-3161.
This testcase tests rule number 2 and number 3.

@SYMTestCaseID		SYSLIB-ECOM-CT-3157
@SYMTestCaseDesc 	This function verifies that ECom handles duplicated
					implementation UID as expected:
					- List operations always return the correct implementations.
					- Create request which does not specify the I/F UID would get
					  the implementation with the lowest I/F UID.
					- Create request specifying both I/F UID and impl UID would
					  get the correct implementation.
@SYMTestPriority 	High
@SYMTestActions  	1. Add two plugins with duplicated impl. UID but different I/F UID.
					2. Perform list operation on each interface. Check that the
					correct implementation is returned in each case.
					3. Do a search by implementation UID only (I/F UID is zero).
					Check that ECom returns the implementation with the lower
					I/F UID.
					4. Do another search specifying both I/F UID and impl. UID.
					Check that ECom returns the correct implementation.
					5. Remove the implementation with lower I/F UID from the
					indexes. Do search by impl UID only. ECom should return the high
					implementation.
@SYMTestExpectedResults The test must not fail.
@SYMDEF				INC080992
*/
void CRegistryDataTest::DuplicateImplUidTestL()
	{
	// Do not run this test in OOM mode. Main part of this test is
	// inserting implementation in iInterfaceImplIndex and iImplIndex.
	// This operation does not cleanup properly if OOM because it
	// relies on server exit.

	// This test uses 2 interfaces: KCExampleInterfaceUid (10009DC0) and
	//                              KCInvalidUid (10009999).
	// They have a common implementationUID KCExampleInterfaceImp.
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-3157 "));

	_LIT(KHighDllPath, "C:\\resource\\plugins\\interfacehigh.dll");
	_LIT(KLowDllPath, "C:\\resource\\plugins\\interfacelow.dll");

	_LIT(KHighDisplayName, "dupl impl. of 10009DC0");
	_LIT(KLowDisplayName, "dupl impl. of 10009999");

	// Create implementation with high interface UID
	TImplCreationStruct highIfUid(EDriveC,
							 KCExampleInterfaceUid,
							 KCExampleInterfaceImp,
							 KHighDllPath,
							 KHighDisplayName);
	CreateImplL(highIfUid);

	// Create implementation with low interface UID
	TImplCreationStruct lowIfUid(EDriveC,
							KCInvalidUid,
							KCExampleInterfaceImp,
							KLowDllPath,
							KLowDisplayName);
	CreateImplL(lowIfUid);

	TheTest.Printf(_L("Testing low I/F UID overrides high I/F UID on same drive\n"));
	TestDuplicatedImplPrecedenceL(lowIfUid, highIfUid);

	_LIT(KHighDriveDllPath, "K:\\resource\\plugins\\highdriveletter.dll");
	_LIT(KHighDriveDisplayName, "K: impl. of i/f 10009DC0");
	TImplCreationStruct highDriveLetter(EDriveK,
							 KCExampleInterfaceUid,
							 KCExampleInterfaceImp,
							 KHighDriveDllPath,
							 KHighDriveDisplayName);
	CreateImplL(highDriveLetter);

	// I/f UID of the dll in K: is same as highIfUid above.
	// But K: take precedence over C: by virtue of higher drive number.
	TheTest.Printf(_L("Testing high drive letter overrides low drive letter"));
	TestDuplicatedImplPrecedenceL(highDriveLetter, lowIfUid);
	}

/** Helper of DuplicateImplUidTestL. This function does the real
work while DuplicateImplUidTestL just setup the test parameters.

@param highprecedence - data of the implementation of higher precedence.
@param lowprecedence - the other implementation of lower precedence.
*/
void CRegistryDataTest::TestDuplicatedImplPrecedenceL(
	const TImplCreationStruct& highprecedence,
	const TImplCreationStruct& lowprecedence)
	{
	TUid commonImplUid = highprecedence.iImplUid;
	TEST2(lowprecedence.iImplUid.iUid, highprecedence.iImplUid.iUid);

	//=============================================================
	// Check index is empty
	TEST2(iStateAccessor->ImplementationsCount(*iRegistryData), 0);

	//=============================================================
	// Add implementation of lowprecedence to indexes
	TBool result = iRegistryData->InsertIntoIndexL(lowprecedence.iImplData,
												   ETrue);
	TEST2(result, ETrue);

	//=============================================================
	// Add implementation of highprecedence to indexes
	result = iRegistryData->InsertIntoIndexL(highprecedence.iImplData,
											 ETrue);
	TEST2(result, ETrue);

	//=============================================================
	// Check the two plugins are inserted in the indexes and list
	// operation works OK
	TUid tempUid = lowprecedence.iInterfaceUid;
	RImplDataArray impls;
	CleanupClosePushL(impls);
	iStateAccessor->GetImplementationInfoL(*iRegistryData, tempUid, impls);
	TEST2(impls.Count(), 1);
	TEST(impls[0]->iImplInfo->DisplayName() == lowprecedence.iImplDisplayName);

	// Next do a list on the other I/F.
	tempUid = highprecedence.iInterfaceUid;
	iStateAccessor->GetImplementationInfoL(*iRegistryData, tempUid, impls);
	TEST2(impls.Count(), 1);
	TEST(impls[0]->iImplInfo->DisplayName() == highprecedence.iImplDisplayName);

	//=============================================================
	// If not specify interface UID, FindImplementation should return
	// the implementation of high precedence
	CRegistryData::CImplementationData*	searchResult = NULL;
	TInt err = iStateAccessor->FindImplementation(*iRegistryData,
											 commonImplUid,
											 KNullUid,
											 searchResult);
	TEST2(err, KErrNone);
	TEST(searchResult == highprecedence.iImplData);

	//=============================================================
	// FindImplementation should return the correct implementation
	// if the interface UID is specified.
	err = iStateAccessor->FindImplementation(*iRegistryData,
											 commonImplUid,
											 lowprecedence.iInterfaceUid,
											 searchResult);
	TEST2(err, KErrNone);
	TEST(searchResult == lowprecedence.iImplData);

	// Repeat FindImplementation test on the highprecedence interface UID
	err = iStateAccessor->FindImplementation(*iRegistryData,
											 commonImplUid,
											 highprecedence.iInterfaceUid,
											 searchResult);
	TEST2(err, KErrNone);
	TEST(searchResult == highprecedence.iImplData);

	//=============================================================
	// Remove the implementation of higher precedence.
	TRAP(err, iRegistryData->RemoveFromIndexL(highprecedence.iDllData));
	TEST2(err, KErrNone);

	//=============================================================
	// Now only the implementation with low precedence is in the indexes
	err = iStateAccessor->FindImplementation(*iRegistryData,
											 commonImplUid,
											 KNullUid,
											 searchResult);
	TEST2(err, KErrNone);
	TEST(searchResult == lowprecedence.iImplData);

	// Before exit, make sure the indexes are empty.
	TRAP(err, iRegistryData->RemoveFromIndexL(lowprecedence.iDllData));
	TEST2(err, KErrNone);

	// Check index is empty
	TEST2(iStateAccessor->ImplementationsCount(*iRegistryData), 0);
	CleanupStack::PopAndDestroy(&impls);
	}
/**
Test case for making sure the flag for indication of language switch set properly when
language downgrade change happens during the running time.

@SYMTestCaseID		SYSLIB-ECOM-UT-3171
@SYMTestCaseDesc 	This function verifies that ECom set the flag for indicating
					language change properly:
					- False when there is no language switch activity.
					- True when language downgrade path is changed from non-localise to localise.
					- True when first language changed only as opposed to  complete language downgrade path
					- False when there is language switch activity, however, the language downgrade
					  path has no change.
@SYMTestPriority 	High
@SYMTestActions  	1. construct language downgrade path
					2. Sets up the system-wide locale and language downgrade path.
					3. Check flag whether set properly.
@SYMTestExpectedResults The test must not fail.
@SYMDEF				CR0902
*/
void CRegistryDataTest::LanguageChangedTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3171 "));
	//exclude those changes for language downgrade path during start-up time
	if(!iStateAccessor->IsLanguageInitChanged(*iRegistryData))
		{
		// Language files used for locale settings
		_LIT(KEComEnglishLocale, "elocl.01");
		_LIT(KEComSpanishLocale, "elocl.04");

		TLanguage downgradePathSetup[3];
		TLocale locale;
		TExtendedLocale extendedLocale;

		TBool isLanguageChange = EFalse;
		//There is no language switch activity, expect EFalse
		TRAPD(err, iRegistryData->LanguageChangedL(isLanguageChange));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		TEST2(isLanguageChange, EFalse);

		//change language downgrade path from non localise to localise.
		extendedLocale.LoadSystemSettings();
        #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            extendedLocale.LoadLocale(KEComEnglishLocale);
        #else
            extendedLocale.LoadLocale(KEComEnglishLang,KEComEnglishReg,KEComEnglishColl);
        #endif  
 		extendedLocale.SaveSystemSettings();

		downgradePathSetup[0] = ELangEnglish;
		downgradePathSetup[1] = ELangFrench;
		downgradePathSetup[2] = ELangGerman;
		SetLanguageL(downgradePathSetup);

		TRAP(err, iRegistryData->LanguageChangedL(isLanguageChange));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		TEST2(isLanguageChange, ETrue);
		//Change first language from English to Spanish..
		extendedLocale.LoadSystemSettings();
        #ifndef SYMBIAN_DISTINCT_LOCALE_MODEL
            extendedLocale.LoadLocale(KEComSpanishLocale);
        #else
            extendedLocale.LoadLocale(KEComSpanishLang,KEComSpanishReg,KEComSpanishColl);
        #endif  
		extendedLocale.SaveSystemSettings();
		downgradePathSetup[0] =ELangSpanish;
		SetLanguageL(downgradePathSetup);
		TRAP(err, iRegistryData->LanguageChangedL(isLanguageChange));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		TEST2(isLanguageChange, ETrue);

		//use same language downgrade path setting.
		SetLanguageL(downgradePathSetup);
		TRAP(err, iRegistryData->LanguageChangedL(isLanguageChange));
		TEST_OOM_ERR;
		TEST2(err, KErrNone);
		TEST2(isLanguageChange, EFalse);
		}
	}

/**
@SYMTestCaseID		SYSLIB-ECOM-UT-3670
@SYMTestCaseDesc 	This test verifies that ECom sets the iDrivesRemoved correctly when drives
					are added and removed
@SYMTestPriority 	High
@SYMTestActions  	Call UndoTemporaryUninstallL to install some drives
					Remove selected drives by calling TemporaryUninstallL
					Use the state accessor to verify that the iDrivesRemoved flag
					correctly stores the removed drives
@SYMTestExpectedResults The iDrivesRemoved flag should match the removed drives
@SYMDEF				INC110470
*/
void CRegistryDataTest::DrivesRemovedTestL()
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3670 "));

	//Ensure that the following drives are all installed
	iRegistryData->UndoTemporaryUninstallL(EDriveA);
	iRegistryData->UndoTemporaryUninstallL(EDriveO);
	iRegistryData->UndoTemporaryUninstallL(EDriveF);
	iRegistryData->UndoTemporaryUninstallL(EDriveZ);

	//Test that there are no drives removed
	TEST2(iStateAccessor->DrivesRemoved(*iRegistryData), 0);

	//Remove a drive and check that the state indicates the correct drive
	//has been removed
	iRegistryData->TemporaryUninstallL(EDriveZ);
	TInt drivesRemoved = iStateAccessor->DrivesRemoved(*iRegistryData);
	TEST2(drivesRemoved, 0x2000000);

	//remove another drive then re-add the first and make sure the
	//correct drive is removed
	iRegistryData->TemporaryUninstallL(EDriveA);
	iRegistryData->UndoTemporaryUninstallL(EDriveZ);
	drivesRemoved = iStateAccessor->DrivesRemoved(*iRegistryData);
	TEST2(drivesRemoved, 0x1);

	//Remove all existing drives  plus that hasnt been mounted
	//and check the result
	iRegistryData->TemporaryUninstallL(EDriveA);
	iRegistryData->TemporaryUninstallL(EDriveO);
	iRegistryData->TemporaryUninstallL(EDriveF);
	iRegistryData->TemporaryUninstallL(EDriveZ);
	iRegistryData->TemporaryUninstallL(EDriveX);

	drivesRemoved = iStateAccessor->DrivesRemoved(*iRegistryData);
	TEST2(drivesRemoved, 0x2004021);

	//Add some existing and some new drives check the result
	iRegistryData->UndoTemporaryUninstallL(EDriveF);
	iRegistryData->UndoTemporaryUninstallL(EDriveZ);
	iRegistryData->UndoTemporaryUninstallL(EDriveB);
	iRegistryData->UndoTemporaryUninstallL(EDriveR);

	drivesRemoved = iStateAccessor->DrivesRemoved(*iRegistryData);
	TEST2(drivesRemoved, 0x0004001);

	}

// Type definition for pointer to member function.
// Used in calling the CRegistryDataTest member function for testing.
typedef void (CRegistryDataTest::*ClassFuncPtrL) (void);

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0751
@SYMTestCaseDesc	    Function to call all test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up all test functions related to CRegistryData
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all test functions
@param		testFunc pointer to test function
@param		aTestDesc test function name
@param		aIsFullDriveCDiscovery whether the constructor make full discovery on drive C
*/
LOCAL_C void DoBasicTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc, TBool aIsFullDriveCDiscovery)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0751 "));
	TheTest.Next(aTestDesc);

	__UHEAP_MARK;
  	// find out the number of open handles
	TInt startProcessHandleCount;
	TInt startThreadHandleCount;
	RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

	CRegistryDataTest* theTest = CRegistryDataTest::NewL(aIsFullDriveCDiscovery);
	CleanupStack::PushL(theTest);

	(theTest->*testFuncL)();

	CleanupStack::PopAndDestroy(theTest);

	// check that no handles have leaked
	TInt endProcessHandleCount;
	TInt endThreadHandleCount;
	RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

	TEST(startProcessHandleCount == endProcessHandleCount);
	TEST(startThreadHandleCount  == endThreadHandleCount);

	__UHEAP_MARKEND;
	}

/**
@SYMTestCaseID          SYSLIB-ECOM-CT-0752
@SYMTestCaseDesc	    Function to call all OOM test functions
@SYMTestPriority 	    High
@SYMTestActions  	    Calls up all OOM test functions related to CRegistryData
@SYMTestExpectedResults The test must not fail.
@SYMREQ                 REQ0000
*/
/**
Wrapper function to call all OOM test functions
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
@param		aIsFullDriveCDiscovery whether the constructor make full discovery on drive C
*/
LOCAL_C void DoOOMTestL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc, TBool aIsFullDriveCDiscovery)
	{
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-CT-0752 "));
	TheTest.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CRegistryDataTest* theTest = CRegistryDataTest::NewL(aIsFullDriveCDiscovery);
		CleanupStack::PushL(theTest);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (theTest->*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(theTest);
		theTest = NULL;
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TEST2(err, KErrNone);
	TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}

/**
Wrapper function for OOM test of function UpdateDllDataTest1L,UpdateDllDataTest2L,UpdateDllDataTest3L
@param		testFuncL pointer to OOM test function
@param		aTestDesc test function name
@param		aIsFullDriveCDiscovery whether the constructor makes full discovery on drive C
@param		aTestNumber The number identifies the test case of update dll data.
*/
LOCAL_C void DoOOMTestUpdateDllDataL(ClassFuncPtrL testFuncL, const TDesC& aTestDesc, TBool aIsFullDriveCDiscovery,TInt aTestNumber)
	{
	TheTest.Next(aTestDesc);

	TInt err;
	TInt tryCount = 0;
	do
		{
		__UHEAP_MARK;
		switch (aTestNumber)
			{
				case 1:
					{
					TRAP(err,EComTestUtils::FileManDeleteFileL(KEcomExample12UpgradedOnC));
					}
				case 2:
					{
					TRAP(err,EComTestUtils::FileManDeleteFileL(KEcomExample14UpgradedOnC));
					}
				case 3:
					{
					TRAP(err,EComTestUtils::FileManDeleteFileL(KEcomExample12DowngradedOnC));
					}
			}
  		// find out the number of open handles
		TInt startProcessHandleCount;
		TInt startThreadHandleCount;
		RThread().HandleCount(startProcessHandleCount, startThreadHandleCount);

		CRegistryDataTest* theTest = CRegistryDataTest::NewL(aIsFullDriveCDiscovery);
		CleanupStack::PushL(theTest);

		__UHEAP_SETFAIL(RHeap::EDeterministic, ++tryCount);

		TRAP(err, (theTest->*testFuncL)());

		__UHEAP_SETFAIL(RHeap::ENone, 0);

		CleanupStack::PopAndDestroy(theTest);
		theTest = NULL;
		// check that no handles have leaked
		TInt endProcessHandleCount;
		TInt endThreadHandleCount;
		RThread().HandleCount(endProcessHandleCount, endThreadHandleCount);

		TEST(startProcessHandleCount == endProcessHandleCount);
		TEST(startThreadHandleCount  == endThreadHandleCount);

		__UHEAP_MARKEND;
		} while(err == KErrNoMemory);

 	TEST2(err, KErrNone);
	TheTest.Printf(_L("- server succeeded at heap failure rate of %i\n"), tryCount);
	}


LOCAL_C void DoTestsL()
	{
	__UHEAP_MARK;
	// Create backup server session here. When deleted it deletes safely.
	// Registrar object creates BackUpNotifier object which
	// internally creates CBaBackupSessionWrapper, an interface to BackUpSErver session opens 3 handles.
	// but when Registrar object is deleted it deletes one handle and two are left open.

	CBaBackupSessionWrapper *BackupSession = CBaBackupSessionWrapper::NewL();

	// Basic tests
	CreateDeleteTestL();
	DoBasicTestL(&CRegistryDataTest::FailedSecurityCheckTestListImplementationsL,_L("FailedSecurityCheckTestListImplementations Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::FailedSecurityCheckTestGetImplementationInfoL,_L("FailedSecurityCheckTestGetImplementationInfo Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ProcessSecurityCheckTestL, _L("ProcessSecurityCheckTest Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::IsAnyDllDiscoveredTestL,_L("IsAnyDllDiscoveredTest Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ImplementationDataTestL,_L("ImplementationData Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::InsertIntoIndexRemoveFromIndexTestL,_L("InsertIntoIndexRemoveFromIndex Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::RollbackTestL,_L("Rollback Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::AddDllDataTestL, _L("AddDllData Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::AddDllDataPlugIn3TestL, _L("AddDllData PlugIn3 Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::UpdateDllDataTest1L, _L("UpdateDllData Test 1"), ETrue);
	DoBasicTestL(&CRegistryDataTest::UpdateDllDataTest2L, _L("UpdateDllData Test 2"), ETrue);
	DoBasicTestL(&CRegistryDataTest::UpdateDllDataTest3L, _L("UpdateDllData Test 3"), ETrue);
	DoBasicTestL(&CRegistryDataTest::IsRegisteredWithDateUpdateDllDataTestL, _L("IsRegisteredWithDate and UpdateDllData Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::SetEnabledStateTestL, _L("SetEnabledState Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::FindImplementationTestL, _L("FindImplementation Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListImplementationsL, _L("ListImplementations Test for CRegistryData"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListImplementationsTestT1L, _L("ListImplementations Test for CRegistryResolveTransaction 1"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListImplementationsTestT2L, _L("ListImplementations Test for CRegistryResolveTransaction 2"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListImplementationsTestT3L, _L("ListImplementations Test for CRegistryResolveTransaction 3"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListImplementationsTestT4L, _L("ListImplementations Test for CRegistryResolveTransaction 4"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListImplementationsTestT5L, _L("ListImplementations Test for CRegistryResolveTransaction 5"), ETrue);
	DoBasicTestL(&CRegistryDataTest::ListExtendedInterfacesTestL, _L("ListExtendedInterfaces Test"), ETrue);

	DoBasicTestL(&CRegistryDataTest::DoUndoTemporaryUninstallTestL, _L("TemporaryUninstall and UndoTemporaryUninstall Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::DiscoveriesBeginCompleteTestL, _L("DiscoveriesBegin and DiscoveriesComplete Test"), ETrue);

	DoBasicTestL(&CRegistryDataTest::IndexedFindTestL, _L("IndexedFind Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::GetImplementationDllInfoForClientTestL,_L("GetImplementationDllInfoForClientL Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::GetImplementationDllInfoForServerTest,_L("GetImplementationDllInfoForServer Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::GetImplementationDllInfoForClientPlugIn3TestL,_L("GetImplementationDllInfoForClientPlugIn3L Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::GetImplementationDllInfoForServerPlugIn3Test,_L("GetImplementationDllInfoForServerPlugIn3 Test"), ETrue);
	DoBasicTestL(&CRegistryDataTest::CheckDuplicatPluginsOnCL,_L("CheckDuplicatPluginsOnCL Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::CheckNoPluginsOnCL,_L("CheckNoPluginsOnCL Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::DuplicateImplUidTestL,_L("DuplicateImplUidTestL Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::LanguageChangedTestL,_L("LanguageChangedTestL Test"), EFalse);
	DoBasicTestL(&CRegistryDataTest::DrivesRemovedTestL,_L("DrivesRemovedTestL Test"), EFalse);

	//OOM tests

	OOMCreateDeleteTest();
	DoOOMTestL(&CRegistryDataTest::FailedSecurityCheckTestListImplementationsL,_L("OOM FailedSecurityCheckTestListImplementations Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::FailedSecurityCheckTestGetImplementationInfoL,_L("OOM FailedSecurityCheckTestGetImplementationInfo Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ProcessSecurityCheckTestL, _L("OOM ProcessSecurityCheckTest Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::IsAnyDllDiscoveredTestL, _L("OOM IsAnyDllDiscoveredTestL Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ImplementationDataTestL, _L("OOM ImplementationData Test"), EFalse);
	DoOOMTestL(&CRegistryDataTest::InsertIntoIndexRemoveFromIndexTestL, _L("OOM InsertIntoIndexRemoveFromIndex Test"), EFalse);
	DoOOMTestL(&CRegistryDataTest::RollbackTestL, _L("OOM Rollback Test"), EFalse);
	DoOOMTestL(&CRegistryDataTest::AddDllDataTestL, _L("OOM AddDllData Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::AddDllDataPlugIn3TestL, _L("OOM AddDllData PlugIn3 Test"), ETrue);
	DoOOMTestUpdateDllDataL(&CRegistryDataTest::UpdateDllDataTest1L, _L("OOM UpdateDllData Test 1"), ETrue, 1);
	DoOOMTestUpdateDllDataL(&CRegistryDataTest::UpdateDllDataTest2L, _L("OOM UpdateDllData Test 2"), ETrue, 2);
	DoOOMTestUpdateDllDataL(&CRegistryDataTest::UpdateDllDataTest3L, _L("OOM UpdateDllData Test 3"), ETrue, 3);
	DoOOMTestL(&CRegistryDataTest::IsRegisteredWithDateUpdateDllDataTestL, _L("IsRegisteredWithDate and UpdateDllData Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::SetEnabledStateTestL, _L("OOM SetEnabledState Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::FindImplementationTestL, _L("OOM FindImplementation Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ListImplementationsL, _L("OOM ListImplementations Test for CRegistryData"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ListImplementationsTestT1L, _L("OOM ListImplementations Test for CRegistryResolveTransaction 1"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ListImplementationsTestT2L, _L("OOM ListImplementations Test for CRegistryResolveTransaction 2"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ListImplementationsTestT3L, _L("OOM ListImplementations Test for CRegistryResolveTransaction 3"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ListImplementationsTestT4L, _L("OOM ListImplementations Test for CRegistryResolveTransaction 4"), ETrue);
	DoOOMTestL(&CRegistryDataTest::ListImplementationsTestT5L, _L("OOM ListImplementations Test for CRegistryResolveTransaction 5"), ETrue);
    DoOOMTestL(&CRegistryDataTest::ListExtendedInterfacesTestL, _L("OOM ListExtendedInterfaces Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::DoUndoTemporaryUninstallTestL, _L("OOM Test for TemporaryUninstall and UndoTemporaryUninstall"), ETrue);
	DoOOMTestL(&CRegistryDataTest::DiscoveriesBeginCompleteTestL, _L("OOM Test for DiscoveriesBegin and DiscoveriesComplete"), ETrue);
	DoOOMTestL(&CRegistryDataTest::IndexedFindTestL, _L("OOM IndexedFind Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::GetImplementationDllInfoForClientTestL,_L("OOM Test for GetImplementationDllInfoForClientL Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::GetImplementationDllInfoForServerTest,_L("OOM Test for GetImplementationDllInfoForServer Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::GetImplementationDllInfoForClientTestL,_L("OOM Test for GetImplementationDllInfoForClientPlugIn3L Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::GetImplementationDllInfoForServerTest,_L("OOM Test for GetImplementationDllInfoForServerPlugIn3L Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::LanguageChangedTestL,_L("OOM Test for LanguageChangedTestL Test"), ETrue);
	DoOOMTestL(&CRegistryDataTest::DrivesRemovedTestL,_L("OOM Test for DrivesRemovedTestL Test"), ETrue);

	// OOM test can not be performed on API StoreL nad RestoreL, since registry index file gets corrupted when its
	// data is not completely written, causing to fail in next call.
	delete BackupSession;

	// Panic Test
	TheTest.Next(_L(" @SYMTestCaseID:SYSLIB-ECOM-UT-3712 "));
	ThreadPanicTest(_L("AddExtendedInterface Panic Testing"),AddExtendedInterfacePanicL);

	__UHEAP_MARKEND;
	}

/**
Copies the Plugins to specific folder for testing purpose
*/
LOCAL_C void CopyPluginsL()
    {
	// Copy the dlls and .rsc files on to RAM
	TRAPD(err, EComTestUtils::FileManCopyFileL(KEComExDllOnZ, KEComExDllOnC));
 	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComRscFileOnZ, KEComRscFileOnC));
 	TEST2(err, KErrNone);

  	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExampleDllOnZ, KEComExampleDllOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample2DllOnZ, KEComExample2DllOnC));
	TEST2(err, KErrNone);
 	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample3DllOnZ, KEComExample3DllOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample12DllOnZ, KEComExample12DllOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample14DllOnZ, KEComExample14DllOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample15DllOnZ, KEComExample15DllOnC));
	TEST2(err, KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExampleRscOnZ, KEComExampleRscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample2RscOnZ, KEComExample2RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample3RscOnZ, KEComExample3RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample12RscOnZ, KEComExample12RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample14RscOnZ, KEComExample14RscOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComExample15RscOnZ, KEComExample15RscOnC));
	TEST2(err, KErrNone);

	// Test plug-in resource file SID that doesn't match the DLL
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComInvalidDllOnZ, KEComInvalidDllOnC));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KEComInvalidRscOnZ, KEComInvalidRscOnC));
	TEST2(err, KErrNone);

	TRAP(err, EComTestUtils::FileManCopyFileL(KMissingDllRomRscFile, KMissingDllRamRscFile));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KRollbackForMissingDllRomDllFile, KRollbackForMissingDllRamDllFile));
	TEST2(err, KErrNone);
	TRAP(err, EComTestUtils::FileManCopyFileL(KRollbackForMissingDllRomRscFile, KRollbackForMissingDllRamRscFile));
	TEST2(err, KErrNone);

	// Wait, so that ECom server looks for plugins copied from Z: to C drive
	// ECOM server could be already started. It means that when we copy some
	// ECOM plugins from Z: to C: drive - ECOM server should look for and
	// find the new ECOM plugins. The ECOM server uses for that CDiscoverer::CIdleScanningTimer
	// which is an active object. So the discovering service is asynchronous. We have to
	// wait some time until it finishes. Otherwise ListImplementationsL could fail to find
	// requested implementations.
	User::After(KOneSecond * 3);
	}

//Initialise the Active Scheduler
LOCAL_C void SetupL()
    {
	// Construct and install the Active Scheduler. The Active Schedular is needed
	// by components used by this test as they are ActiveObjects.
	TheActiveScheduler = new(ELeave)CActiveScheduler;
	CActiveScheduler::Install(TheActiveScheduler);
	ECOMTU_KILL_SERVER_L(TheTest);
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;

	TheTest.Printf(_L("\n"));
	TheTest.Title();
	TheTest.Start(_L("Registry Data Tests"));

	TheTrapCleanup = CTrapCleanup::New();

	User::LeaveIfError(TheFs.Connect());

	TRAPD(err, SetupL());
	TEST2(err, KErrNone);

	CopyPluginsL();

	TRAP(err, DoTestsL());
	TEST2(err, KErrNone);

	// Cleanup files. If the cleanup fails that is no problem,
	// as any subsequent tests will replace them. The only downside
	// would be the disk not being tidied
	DeleteTestPlugin();
	TheFs.Close();

	delete TheTrapCleanup;
	delete TheActiveScheduler;
	TheTest.End();
	TheTest.Close();

	__UHEAP_MARKEND;
	return(KErrNone);
	}
