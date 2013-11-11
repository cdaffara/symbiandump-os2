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
// Implementation of the registrar class
// 
//

/**
 @internalComponent
 @file 
*/

#include <barsc2.h> // CResourceFile
#include <barsread2.h> // RResourceReader
#include <bautils.h>
#include <e32uid.h>
#include <startup.hrh> // for EStartupStateNonCritical

#include "ecompanics.h"
#include "EComDebug.h"
#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include "TestUtilities.h"	// For __FILE__LINE__
#include "Registrar.h"
#include "Discoverer.h"
#include "BackupNotifier.h"
#include "RegistryData.h"
#include "EComUidCodes.h"
#include "RegistrarObserver.h"
#include "DriveInfo.h"
#include "ParseImplementationData.h"
#include "EComInternalErrorCodes.h"
	
const TInt KMinBuffAllocSize = 1;


#define UNUSED_VAR(a) a = a


CRegistrar* CRegistrar::NewL(CRegistryData& aRegistry, MRegistrarObserver& aRegistrarObserver, RFs& aFs)
	{
	CRegistrar* self = new(ELeave) CRegistrar(aRegistry,aRegistrarObserver,aFs);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

// Default d'tor

CRegistrar::~CRegistrar()
	{
	delete iBackupNotifier;
	delete iDiscoverer;
	delete iCachedDriveInfo;
	}

// Default c'tor

CRegistrar::CRegistrar(CRegistryData& aRegistry, MRegistrarObserver& aRegistrarObserver, RFs& aFs)
: CBase(), iRegistry(aRegistry), iRegistrarObserver(aRegistrarObserver), iFs(aFs), iState(EReg_Undefined)
	{
	}


void CRegistrar::ConstructL()
	{
	iCachedDriveInfo = CEComCachedDriveInfo::NewL(iFs);

	// Instantiate the discoverer
	iDiscoverer = CDiscoverer::NewL(*this, iFs);
	InitialiseEvent();
	}


TInt CRegistrar::Disable(TUid aImplementationUid)
	{
	return iRegistry.SetEnabledState(aImplementationUid, EFalse);
	}


TInt CRegistrar::Enable(TUid aImplementationUid)
	{
	return iRegistry.SetEnabledState(aImplementationUid, ETrue);
	}


TInt CRegistrar::Resume()
	{
	return iDiscoverer->Resume();
	}


TInt CRegistrar::Suspend()
	{
	return iDiscoverer->Suspend();
	}


void CRegistrar::DriveRemovedL(TDriveUnit aDrive)
	{
	iRegistry.TemporaryUninstallL(aDrive);
	}


void CRegistrar::DriveReinstatedL(TDriveUnit aDrive)
	{
	iRegistry.UndoTemporaryUninstallL(aDrive);
	}
	
TBool CRegistrar::IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive)const
	{
	return iRegistry.IsAnyDllRegisteredWithDriveL(aDrive);
	}

void CRegistrar::RegisterDiscoveryL(const TDriveName& aDrive,CPluginBase*& aDirEntry, TBool aAnyDllDiscovered)
	{
	TBool found = EFalse;
	TBool update = EFalse;
	TInt  registryDriveIndex = KErrNotFound;

	// Find the appropriate drive entry
	TChar driveLetter = User::UpperCase(aDrive[0]) - 'A';
	TDriveUnit driveUnit = EDriveA + driveLetter;
	CRegistryData::CDriveData* drive =NULL;
	
	// Get the drive data in the registry.
	registryDriveIndex = iRegistry.FindDriveL(driveUnit, drive);
	if(registryDriveIndex == KErrNotFound)
		{
		User::Leave(KEComErrDriveNotFound);
		}
		

	// Get the registry to return enough data so that the Add and Update routines
	// can be optimised to avoid rescanning the registry data.
	// This means that registryDriveIndex MUST remain valid until after
	// the ParseRegistrationDataL call below.
	
	// IsRegisteredWithDate will be called if the this drive is NOT
	// under its initial discovery. If any Dll is discovered in the drive, then
	// this drive is NOT in its initial discovery.
	
	// In read-only drive with SPI enable, aAnyDllDiscovered will be always false. 
	// If language downgrade path has been changed, IsRegisteredWithDate will be
	// called to check whether current localised file need to be updated. 
	if((aAnyDllDiscovered) || (iRegistry.HasLanguageChanged()))
		{
		found = iRegistry.IsRegisteredWithDate(aDirEntry->iDllThirdUid, aDirEntry->iDllModifiedTime, update, drive);
		}
	
	if(!found || update || iRegistry.HasLanguageChanged())
		{
		if(found && !update && iRegistry.HasLanguageChanged())
			{
			// If we got here because the language has changed
			// ensure the existing registry data is updated rather than a new entry added
			update = ETrue;
			}
		ParseRegistrationDataL(aDirEntry,driveUnit, update, registryDriveIndex, drive);
		}
	}


void CRegistrar::DiscoveriesBegin()
	{
	// We are about to receive registrations so inform the registry that its index
	// is about to be invalid
	iRegistry.DiscoveriesBeginning();
	}


void CRegistrar::DiscoveriesComplete(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType)
	{
	
	TBool doesRegChanged = EFalse;

	// Successfully completed, if registry data has been changed, update 
	TRAPD(error,iRegistry.DiscoveriesCompleteL(aSuccessful, aProcessingType,doesRegChanged));
    UNUSED_VAR(error);
	// Notify if the registry data has been changed.
	if(doesRegChanged)
		{
	// Notify that we have updated the registry
		iRegistrarObserver.Notification(KErrNone);
		}
	}

void CRegistrar::SetDiscoveryFlagL(const TDriveUnit& aDriveUnit)
	{
	//set the drive flag to indicate it has changes.
	iRegistry.SetDiscoveryFlagL(aDriveUnit);
	}
	
TBool CRegistrar::NotifiedWithErrorCode(TInt aError)
	{
	// Test the safe error codes
	return (aError == KErrNotReady ||		// Drive removed?
			aError == KErrPathNotFound);	// Directory deleted?
	}

void CRegistrar::ParseRegistrationDataL(CPluginBase*& aEntry,
										const TDriveUnit& aDrive, 
										TBool aUpdate, 
										TInt aRegistryDriveIndex,
										CRegistryData::CDriveData* aDriveData)
	{
	// Ok we intend installing this item so 
	// create the Interface Implementation Collection entry first
	CRegistryData::CDllData* dll = CRegistryData::CDllData::NewLC(*(aEntry->iDllName),aEntry->iDllModifiedTime,aEntry->iDllSecondUid,aEntry->iDllThirdUid,aDriveData);

#ifdef ECOM_TRACE
	static int counter = 0; counter++;
	TFileName fullFileName(aDrive.Name());
	fullFileName.Append(_L("\\sys\\bin\\"));
	fullFileName.Append(*(aEntry->iDllName));
	__ECOM_TRACE2("ECOM: Plugin discovered (%04d) %S", counter, &fullFileName);
#endif

	// update resource ext info
	HBufC* resourceExt = aEntry->RscFileExt();
	if(resourceExt)
		dll->SetResourceExtL(*resourceExt);
	
	// Don't automatically leave during the parse of the implementation
	// collection's registration data.
	// If we did then there would be a cascade of leaves which stop the 
	// entire registration process for all discoveries remaining on the list.
	// Check to see if there is a problem specifically with the registration data
	// OR if its a general problem. In the latter case leave.
	TInt error=KErrNone;
			
	TRAP(error, ParseL(aEntry,*dll));
	if (error==KErrNoMemory)
		User::LeaveNoMemory();
	if(error == KErrNone)
		{
		if(aUpdate)
			{	
			iRegistry.UpdateDllDataL(aDrive, aRegistryDriveIndex, dll);
			}
		else
			{	
			iRegistry.AddDllDataL(aDrive, aRegistryDriveIndex, dll);
			}
		// Remove dll from CleanupStack as ownership has been passed in one of
		// the two functions above
		CleanupStack::Pop(dll);
		}
	else
		{
		// This interface implementation collection 
		// cannot be registered correctly.
		CleanupStack::PopAndDestroy(dll);
		}
	}

void CRegistrar::GetResourceFormatVersionL(RResourceReader& aReader, TInt& aResourceFormatVersion)
	{
	aResourceFormatVersion = 1;
	TUid dllUid(KNullUid);
	TUid uid = {aReader.ReadInt32L()};
	if(uid==KUidEComResourceFormatV2)
		{
		aResourceFormatVersion = 2;
		dllUid.iUid = aReader.ReadInt32L();
		}
	else if(uid==KUidEComResourceFormatV3)
		{
		aResourceFormatVersion = 3;
		dllUid.iUid = aReader.ReadInt32L();
		}		
	else
		dllUid = uid;
	}

	
void CRegistrar::ParseL(CPluginBase*& aEntry,CRegistryData::CDllData& aDll)
	{
// Read the resource file starting at offset:0 section size:0
	CResourceFile* regFile =aEntry->RscFile();
//	Note : There may be an issue here with resource reading when the 
//  file is not in the ROM. The solution would be to call
//	regFile.ConfirmSignatureL(regFile.SignatureL());
//	However for 6.2 interface implementation collections will ALL
//	be within the ROM. and future development will move to an
//	XML parsed solution @ build time.
//	IF this situation changes, place the fix here.
 	RResourceReader theReader;
 	theReader.OpenLC(regFile, KMinBuffAllocSize);
	TInt resourceFormatVersion = 0;
	GetResourceFormatVersionL(theReader,resourceFormatVersion);
	
	TDriveUnit drive(aDll.iParent->iDrive);
	TBool romBased = iCachedDriveInfo->DriveIsReadOnlyInternalL(drive);
	const TInt numInterfaces = theReader.ReadInt16L();
	if (resourceFormatVersion == 3 && numInterfaces > KMaxInterfacesInResV3)
		{
		User::Leave(KErrNotSupported);
		}
	for(TInt iface = 0; iface < numInterfaces; ++iface)
		{
		// Read the interface uid
		const TUid interfaceUid = {theReader.ReadInt32L()};
		CRegistryData::CInterfaceData* interfaceList = CRegistryData::CInterfaceData::NewLC(interfaceUid,&aDll);
		aDll.AddL(interfaceList);
		CleanupStack::Pop(interfaceList);		// Now owned by aDll
		const TInt numImplementations = theReader.ReadInt16L();
		if (resourceFormatVersion == 3 && numImplementations > KMaxImplementationsForEachInterfacesInResV3)
			{
			User::Leave(KErrNotSupported);
			}
		for(TInt index = 0; index < numImplementations; ++index)
			{
			TUid implementationUid;
			TInt versionNo;
			TInt infoFormat;
			TBool romOnly;
			RExtendedInterfacesArray* extendedInterfaces = NULL;
			HBufC* name = NULL;
			HBufC8* defaultData = NULL;
			HBufC8* opaqueData = NULL;
			CParseImplementationData* parseImplementationData = CParseImplementationData::NewLC(resourceFormatVersion);
			parseImplementationData->ParseL(theReader,
											infoFormat,
											implementationUid,
											versionNo,
									   	  	name,
									      	defaultData,
									      	opaqueData,
									      	extendedInterfaces,
									      	romOnly);
			CleanupStack::PopAndDestroy(parseImplementationData);									      	
			
			CleanupStack::PushL(TCleanupItem(CloseAndDeleteArray, extendedInterfaces));
			CleanupStack::PushL(name);
			CleanupStack::PushL(defaultData);
			CleanupStack::PushL(opaqueData);
			if (romOnly && !(romBased))
				{
				// pop and destroy opaquedata, defaultdata,name and extendedInterfaces
				CleanupStack::PopAndDestroy(4,extendedInterfaces); 
				}
			else
				{
				CRegistryData::CImplementationData* implData = CRegistryData::CImplementationData::NewL(interfaceList,
																										implementationUid, 
																				 						versionNo, 
																				 						name, 
																						 				defaultData,
																										opaqueData,
																					 					drive,
																					 					romOnly,
																				 						romBased,
																				 						extendedInterfaces);
				CleanupStack::Pop(4,extendedInterfaces); // opaqueData,defaultData,name,extendedInterfaces owned by implData
				CleanupStack::PushL(implData);
				interfaceList->AddL(implData);	// Now owned by interfaceList	
				CleanupStack::Pop(implData);
				}
			}
		}
	CleanupStack::PopAndDestroy(&theReader);		
	}

	
CRegistrar::TRegistrarState CRegistrar::State()const
	{
	return iState;
	}

void CRegistrar::ProcessSSAEventL(TStartupStateIdentifier aKnownState)
	{
	iDiscoverer->ProcessSSAEventL(aKnownState); 
	if(aKnownState == EStartupStateNonCritical && iState == EReg_StartupInProgess)
		{
		__ECOM_TRACE("ECOM: CRegistrar::ProcessSSAEventL(): EStartupStateNonCritical is reached.");

		iState = EReg_StartupComplete;
		iBackupNotifier = CBackupNotifier::NewL(*this);
		}
	}
void CRegistrar::InitialiseEvent()
	{
	iState = EReg_StartupInProgess;
	}

void CRegistrar::LanguageChangedL(TBool& aLanguageChanged)
	{
	iRegistry.LanguageChangedL(aLanguageChanged);
	}

void CRegistrar::InstallSwiEventCallBack(const TCallBackWithArg& aCallBack)
	{
	iDiscoverer->SetSwiChangeCallBack(aCallBack);
	}

void CRegistrar::InstallBurEventCallBack(const TCallBackWithArg& aCallBack)
	{
	iDiscoverer->SetBurChangeCallBack(aCallBack);
	}
