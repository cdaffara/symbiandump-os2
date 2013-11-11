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
// This file contains the implementations of the classes
// which manage the internal structure of the registry.
// 
//

#include <e32base.h>
#include <e32uid.h>
#include <s32stor.h>
#include <s32file.h>
#include <startup.hrh> // for EStartupStateNonCritical
#include <e32ldr.h> // for hash checking
#include <e32ldr_private.h> // for RLoader
#include <bautils.h> // for BaflUtils::FileExists

#include "EComDebug.h"
#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include "RegistryData.h"
#include "DowngradePath.h"
#include "DriveInfo.h"
#include "FileUtils.h"
#include "EComUidCodes.h"
#include "EComInternalErrorCodes.h"
#define UNUSED_VAR(a) a = a

const TInt KRegVersion = -9999;
const TInt KDllExtensionLength=4;
_LIT(KDllExtension,".dll");
//
//CRegistryData::TInterfaceStruct

/**
Constructor for TInterfaceStruct
@post			Its member variable is granulated as (1)
*/
CRegistryData::TInterfaceStruct::TInterfaceStruct():
	iImpData(1)
	{
	// Do nothing here
	}

/**
This method determins the order of two TInterfaceIndex objects
@param			indexEntry1 first TInterfaceIndex object
@param			indexEntry2 second TInterfaceIndex object
@return			integer indicating the order of these two
@pre			This object is fully constructed
*/
TInt CRegistryData::TInterfaceStruct::CompareInfUid(const TInterfaceIndex& indexEntry1,
													const TInterfaceIndex& indexEntry2)
	{
	return CompareTUidValues(indexEntry1.iInterfaceUid.iUid,indexEntry2.iInterfaceUid.iUid);
	}

//
//CRegistryData::TImplStruct

/**
Constructor for TImplStruct
@post			Its member variables are initialised
*/
CRegistryData::TImplStruct::TImplStruct():
	iCurrentImpl(NULL),
	iUnusedImpls(1)
	{
	// Do nothing here
	}
	
/**
This method determins the order of two TImplStruct objects
@param			aEntry1 first TImplStruct object
@param			aEntry2 second TImplStruct object
@return			integer indicating the order of these two
@pre			This object is fully constructed
*/
 TInt CRegistryData::TImplStruct::CompareImplStructUid(const TImplStruct& aEntry1,
													const TImplStruct& aEntry2)
	{
	return CompareTUidValues(aEntry1.iCurrentImpl->iImplInfo->ImplementationUid().iUid,
						aEntry2.iCurrentImpl->iImplInfo->ImplementationUid().iUid);
	}
	
/** Comparer to determine the order of a TUid object (implementation UID) in relation to and a TImplStruct object.
@param aUid the Implementation UID to compare with.
@param aEntry the TImplStruct object to compare aUid against.
@return integer indicating the order of the two.
*/
TInt CRegistryData::TImplStruct::CompareUidAgainstImplStruct(
	const TUid* aUid,
	const TImplStruct& aEntry)
	{
	return CompareTUidValues(aUid->iUid,aEntry.iCurrentImpl->iImplInfo->ImplementationUid().iUid);
	}

//
// CRegistryData::CImplementationData class

/**
Creates a new CImplemenatationData object. Note that CImplementationInformation needs to be constructed too,
for this object to be fully constructed.
@param			aParent A pointer to the parent instance of CInterfaceData
@return			A pointer to the newly created object.
@post			This object is partly constructed and initialized and is on 
				the CleanupStack.
*/
CRegistryData::CImplementationData* CRegistryData::CImplementationData::NewLC(CInterfaceData* aParent)
	{
	CImplementationData* self=new(ELeave) CImplementationData(aParent);
	CleanupStack::PushL(self);
	return self;
	}

/**
Creates a new CImplemenatationData object. It takes parameters to create a CImplementationInformation object,
during 2nd phase construction and initializes iImplInfo to reference to this CImplementationInformation object.
@param			aParent A pointer to the parent instance of CInterfaceData
@param			aUid The unique Id of the new implementation
@param			aVersion The version number of the new implementation
@param			aName The display name of the new implementation
@param			aDataType The data type supported by the new implementation
@param			aOpaqueData Data for the new implementation which is not used by the ECom framework
@param			aDrive The drive that the new implementation is on
@param			aRomOnly The flag recording whether the new implementation may be loaded from ROM only
@param			aRomBased The flag recording whether the new implementation is on ROM or is a later version of one on ROM
@return			A pointer to the newly created object
@post			This object is fully constructed and initialized.
*/
CRegistryData::CImplementationData* CRegistryData::CImplementationData::NewL(CInterfaceData* aParent,
																				TUid	aUid,
																				TInt	aVersion, 
																				HBufC*  aName,
																				HBufC8* aDataType,
																				HBufC8* aOpaqueData,
																				TDriveUnit aDrive,
																				TBool aRomOnly,
																				TBool aRomBased)
	{
	CImplementationData* self=new(ELeave) CImplementationData(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aUid,
					aVersion,
					aName,
					aDataType,
					aOpaqueData,
					aDrive,
					aRomOnly,
					aRomBased);
	CleanupStack::Pop(self);
	return self;
	}

/**
Creates a new CImplemenatationData object. It takes parameters to create a CImplementationInformation object,
during 2nd phase construction and initializes iImplInfo to reference to this CImplementationInformation object.
@param			aParent A pointer to the parent instance of CInterfaceData
@param			aUid The unique Id of the new implementation
@param			aVersion The version number of the new implementation
@param			aName The display name of the new implementation
@param			aDataType The data type supported by the new implementation
@param			aOpaqueData Data for the new implementation which is not used by the ECom framework
@param			aDrive The drive that the new implementation is on
@param			aRomOnly The flag recording whether the new implementation may be loaded from ROM only
@param			aRomBased The flag recording whether the new implementation is on ROM or is a later version of one on ROM
@param			aExtendedInterfaces The pointer to the array recording the extended interfaces supported by this implementation.
				NULL is available for PLUGIN without extended interfaces support.
@return			A pointer to the newly created object
@post			This object is fully constructed and initialized.
*/
CRegistryData::CImplementationData* CRegistryData::CImplementationData::NewL(CInterfaceData* aParent,
																				TUid	aUid,
																				TInt	aVersion, 
																				HBufC*  aName,
																				HBufC8* aDataType,
																				HBufC8* aOpaqueData,
																				TDriveUnit aDrive,
																				TBool aRomOnly,
																				TBool aRomBased,
																				RExtendedInterfacesArray* aExtendedInterfaces)
	{
	CImplementationData* self=new(ELeave) CImplementationData(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aUid,
					aVersion,
					aName,
					aDataType,
					aOpaqueData,
					aDrive,
					aRomOnly,
					aRomBased,
					aExtendedInterfaces);
	CleanupStack::Pop(self);
	return self;
	}

/**
Constructor for CImplementationData
@param			aParent The parent interface data of this implementation
@post			Its member variables are initialised
*/
CRegistryData::CImplementationData::CImplementationData( CInterfaceData* aParent):
	CBase(),
	iImplInfo(NULL),
	iParent(aParent)
	{
	// Do nothing here
	}
	
/**
Destructor of CImplementationData
*/	
CRegistryData::CImplementationData::~CImplementationData()
	{
	if(iImplInfo)
		{
		delete iImplInfo;
		iImplInfo = NULL;	
		}	
	iParent = NULL;		
	}

/**
The object's memory has been allocated.
@param			aUid The unique Id of the new implementation
@param			aVersion The version number of the new implementation
@param			aName The display name of the new implementation
@param			aDataType The data type supported by the new implementation
@param			aOpaqueData Data for the new implementation which is not used by the ECom framework
@param			aDrive The drive that the new implementation is on
@param			aRomOnly The flag recording whether the new implementation may be loaded from ROM only
@param			aRomBased The flag recording whether the new implementation is on ROM or is a later version of one on ROM
@pre 			This object is fully constructed.
@post			This object is fully constructed and initialized.
*/
void CRegistryData::CImplementationData::ConstructL(TUid	aUid,
													TInt	aVersion, 
													HBufC*  aName,
													HBufC8* aDataType,
													HBufC8* aOpaqueData,
													TDriveUnit aDrive,
													TBool aRomOnly,
													TBool aRomBased)
	{
	CImplementationInformation* newImpl = CImplementationInformation::NewL(aUid, 
													 						aVersion, 
													 						aName, 
															 				aDataType,
																			aOpaqueData,
														 					aDrive,
														 					aRomOnly,
													 						aRomBased);
	this->iImplInfo = newImpl;
	}

/**
The object's memory has been allocated.
@param			aUid The unique Id of the new implementation
@param			aVersion The version number of the new implementation
@param			aName The display name of the new implementation
@param			aDataType The data type supported by the new implementation
@param			aOpaqueData Data for the new implementation which is not used by the ECom framework
@param			aDrive The drive that the new implementation is on
@param			aRomOnly The flag recording whether the new implementation may be loaded from ROM only
@param			aRomBased The flag recording whether the new implementation is on ROM or is a later version of one on ROM
@param			aExtendedInterfaces The pointer to the array recording the extended interfaces supported by this implementation.
				NULL is available for PLUGIN without extended interfaces support.
@pre 			This object is fully constructed.
@post			This object is fully constructed and initialized.
*/
void CRegistryData::CImplementationData::ConstructL(TUid	aUid,
													TInt	aVersion, 
													HBufC*  aName,
													HBufC8* aDataType,
													HBufC8* aOpaqueData,
													TDriveUnit aDrive,
													TBool aRomOnly,
													TBool aRomBased,
													RExtendedInterfacesArray* aExtendedInterfaces)
	{
	CImplementationInformation* newImpl = CImplementationInformation::NewL(aUid, 
													 						aVersion, 
													 						aName, 
															 				aDataType,
																			aOpaqueData,
														 					aDrive,
														 					aRomOnly,
													 						aRomBased,
													 						aExtendedInterfaces);
	this->iImplInfo = newImpl;
	}

/**
Initialises member variable with the CImplementationInformation state specified in aStore.
@param			aStore The stream to read the data from.
@pre 			This object is full constructed.
@post			This object is set to the state specified in aStore.
*/
void CRegistryData::CImplementationData::InternalizeL(RReadStream& aStore)
	{
	if (iImplInfo)
		{
		delete iImplInfo;
		iImplInfo = NULL;
		}
	CImplementationInformation* implInfo = 0;
	implInfo=CImplementationInformation::NewLC(EFalse,aStore);
	//as we never store the drive name we need to get this from the parent
	implInfo->SetDrive(iParent->iParent->iParent->iDrive);
	CleanupStack::Pop(1);
	iImplInfo = implInfo;
	implInfo = 0;
	}


/**
Writes out the state of this member variable of type CImplementationInformation to aStore.
@param			aStore The stream to store the data in.
@pre 			This object is fully constructed.
*/
void CRegistryData::CImplementationData::ExternalizeL(RWriteStream& aStore) const
	{
	iImplInfo->ExternalizeL(EFalse,aStore);
	}

/**
This method determines the order of two CImplementationData objects.
For backward compatibility reason, ECom allows different I/Fs using
the same implementation UID for when the device has an implementation UID
that is not unique. Although this is not a valid or supported situation ECOM 
server should be robust.
@see FindImplementation
@see CompareTUidValues
@param			aImpl1 first reference to CImplementationData object 
@param			aImpl2 second reference to CImplementationData object
@return			integer indicating the order of these two
@pre			This object is fully constructed
*/	
TInt CRegistryData::CImplementationData::CompareImplUid(const CImplementationData& aImpl1,
													const CImplementationData& aImpl2)
	{
	TInt ret = CompareTUidValues(aImpl1.iImplInfo->ImplementationUid().iUid, aImpl2.iImplInfo->ImplementationUid().iUid);
	if (ret != 0)
		{
		return ret;
		}

	// Implementation UIDs are equal, use I/F UID as tie breaker
	return CompareTUidValues(aImpl1.iParent->iInterfaceUid.iUid, aImpl2.iParent->iInterfaceUid.iUid);
	}
	
/**
Similar to CompareImplUid above. This comparer only compare the Impl UID
and ignore the I/F UID part.
@param			aImpl1 first reference to CImplementationData object 
@param			aImpl2 second reference to CImplementationData object
@return			integer indicating the order of these two
@pre			This object is fully constructed
*/	
TInt CRegistryData::CImplementationData::CompareImplUidIgnoreIfUid(const CImplementationData& aImpl1,
													const CImplementationData& aImpl2)
	{
	return CompareTUidValues(aImpl1.iImplInfo->ImplementationUid().iUid,
						aImpl2.iImplInfo->ImplementationUid().iUid);
	}
	
/**
This method determines the order of an CImplementationData object in relation
to aUid.
@param	aUid is really TUid. Cast it back before compare.
@param	aImplData reference to CImplementationData object
@return	integer indicating the order of the two.
*/	
TInt CRegistryData::CImplementationData::CompareUidAgainstImplData(
											const CImplementationData& aUid,
											const CImplementationData& aImplData)
	{
	// The first argument aUid is really TUid.
	const TUid* ImplUid = reinterpret_cast<const TUid*>(&aUid);
	return CompareTUidValues(ImplUid->iUid,
						aImplData.iImplInfo->ImplementationUid().iUid);
	}

//
// CRegistryData::CInterfaceData class

/**
Creates a new CInterfaceData object and leave it on the CleanupStack
@param			aInterface The unique Id of this interface.
@param			aParent A pointer to the parent dll data 
@return			A pointer to the newly created class.
@post			This object is fully constructed and initialized and is on 
				the CleanupStack.
*/
CRegistryData::CInterfaceData* CRegistryData::CInterfaceData::NewLC(TUid aInterfaceUid,CDllData* aParent)
	{
	CInterfaceData* self=new(ELeave) CInterfaceData(aInterfaceUid,aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


/**
Creates a new CInterfaceData object using the supplied interface id 
and leave it on the CleanupStack
@param			aParent A pointer to the parent dll data
@return			A pointer to the newly created class.
@post			This object is fully constructed and initialized and is on 
				the CleanupStack.
*/
CRegistryData::CInterfaceData* CRegistryData::CInterfaceData::NewLC(CDllData* aParent)
	{
	CInterfaceData* self=new(ELeave) CInterfaceData(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}


CRegistryData::CInterfaceData::~CInterfaceData()
	{
	if(iImplementations)
		{
		iImplementations->ResetAndDestroy();
		delete iImplementations;
		iImplementations = NULL;
		}
	iParent = NULL;
	}

/**
Adds the specified implementation to this interface in the registry.
@param			aImplementation The implementation to add to this interface.
@pre			This object is fully constructed.
@post			aImplementation is added to the list of implementations for this interface.
*/
void CRegistryData::CInterfaceData::AddL(const CImplementationData* aImplementation)
	{
#ifdef ECOM_TRACE
	static int counter = 0; counter++;
	__ECOM_TRACE6("ECOM: Implementation discovered (%04d) UID:0x%X interfaceUID:0x%X version:%d on drive:%d \"%S\"", counter, aImplementation->iImplInfo->ImplementationUid().iUid, aImplementation->iParent->iInterfaceUid.iUid, aImplementation->iImplInfo->Version(), (TInt)(aImplementation->iImplInfo->Drive()), &(aImplementation->iImplInfo->DisplayName()));

#endif

	User::LeaveIfError(iImplementations->Append(aImplementation));
	}

/**
Sets the uid of this interface to aInterfaceUid.
@param			aInterfaceUid The Uid which this object should take.
@pre			This object is fully constructed.
@post			The Uid of this interface is set to aInterfaceUid
*/
void CRegistryData::CInterfaceData::SetInterfaceUid(TUid aInterfaceUid)
	{
	iInterfaceUid = aInterfaceUid;
	}

/**
Writes out this CInterfaceData to aStore.
@param			aStore The stream to store the data in.
@pre 			The state of this object is stored in the stream aStore.
*/
void CRegistryData::CInterfaceData::ExternalizeL(RWriteStream& aStore) const
	{
	aStore.WriteInt32L(iInterfaceUid.iUid);

	if(iImplementations)
		{
		const TInt entryCount = iImplementations->Count();
		aStore.WriteInt32L(entryCount);
		for(TInt i = 0; i < entryCount; ++i)
			((*iImplementations)[i])->ExternalizeL(aStore);
		}
	else
		aStore.WriteInt32L(0);
	}

/**
Restores this CInterfaceData to the state specified in aStore.
@param			aStore The stream to read the data from.
@param			aPresent A boolean indicating whether the dll is still present
@pre 			This object is full constructed.
@post			This object is set to the state specified in aStore.
*/
void CRegistryData::CInterfaceData::InternalizeL(RReadStream& aStore)
	{
	iInterfaceUid.iUid = aStore.ReadInt32L();

	const TInt entryCount = aStore.ReadInt32L();
    if(entryCount < 0)
        {
        User::Leave(KErrCorrupt);
        }
	
	for(TInt i = 0; i < entryCount; ++i)
		{
		CImplementationData* implementation = CImplementationData::NewLC(this);
		implementation->InternalizeL(aStore);
		AddL(implementation);
		CleanupStack::Pop(implementation);		//now owned by this interface
		}
	}

/**
@param			aParent A pointer to the parent dll data
*/
CRegistryData::CInterfaceData::CInterfaceData(CDllData* aParent) :
	CBase(),
	iParent(aParent)
	{
	// Do nothing here
	}

/**
@param			aInterfaceUid The unique Id of this interface
@param			aParent A pointer to the parent dll data			
*/
CRegistryData::CInterfaceData::CInterfaceData(TUid aInterfaceUid, CDllData* aParent) :
	CBase(),
	iInterfaceUid(aInterfaceUid),
	iParent(aParent)
	{
	// Do nothing here
	}

/**
Standard second phase construction function
@pre 			This object is fully constructed.
@post			This object is fully constructed and initialized.
*/
void CRegistryData::CInterfaceData::ConstructL()
	{
	iImplementations = new(ELeave) RPointerArray<CImplementationData>;
	}

//
// CRegistryData::CDllData class

/**
Creates a new CDllData object using aEntry and leaves it on the CleanupStack
@param			aDllName the name of this dll
@param			aDllModTime the modified time of this dll
@param 			aSecondUid Identifies type of the DLL. (PLUGIN or PLUGIN3)
@param  			aThirdUid Identifies a component uniquely. 	
@param			aParent A pointer to the parent drive data
@return			A pointer to the newly created object.
@post			This object is fully constructed and initialized and on the CleanupStack.
*/
CRegistryData::CDllData* CRegistryData::CDllData::NewLC(const TDesC& aDllName,const TTime& aDllModTime,const TUid& aSecondUid,const TUid& aThirdUid,CDriveData* aParent)
	{
	CDllData* self=new(ELeave) CDllData(aParent);
	CleanupStack::PushL(self);
	self->ConstructL(aDllName,aDllModTime,aSecondUid,aThirdUid);
	return self;
	}

/**
Creates a new CDllData object using aParent and leaves it on the CleanupStack
@param			aParent A pointer to the parent drive data
@return			A pointer to the newly created object.
@post			This object is fully constructed and initialized and on the CleanupStack.
*/
CRegistryData::CDllData* CRegistryData::CDllData::NewLC( CDriveData* aParent)
	{
	CDllData* self=new(ELeave) CDllData(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}
	
CRegistryData::CDllData::~CDllData()
	{
	if(iIfList)
		{
		// Clear the interface list and destroy its objects
		iIfList->ResetAndDestroy();
		delete iIfList;
		iIfList = NULL;
		}

	// Unload this implementation dll.
	delete iDllEntry;
	iDllEntry = NULL;
	iParent = NULL;
	delete iRscFileExtension;
	}

/**
Adds the specified interface to this dll in the registry.
@param			aInterface The interface to add to this dll
@pre 			This object is fully constructed.
@post			aInterface is added to the list of interfaces in this dll.
*/
void CRegistryData::CDllData::AddL(const CInterfaceData* aInterface)
	{
	User::LeaveIfError(iIfList->Append(aInterface));
	}

/**
Sets the resource extension for the plugin. Not set for read only internal drives.
@param			aExt The resource extension to set
@pre 			This object is fully constructed.
@post			aExt is added to the object.
*/
void CRegistryData::CDllData::SetResourceExtL(const TDesC& aExt)
	{
	delete iRscFileExtension;
	iRscFileExtension = NULL;
	iRscFileExtension = aExt.AllocL();
	}
	
/**
Set the capability,the VID and do the Security check for this DLL.
@pre 			This object is fully constructed.
@return			ETrue if the security check is done successfully. Otherwise EFalse is returned. 
*/
TBool CRegistryData::CDllData::SaveSecurityInfoL()
	{
	iSecurityChecked = EFalse;

	//need to construct the full filename i.e. appending with the
	//preconstructed drivepath name in CDriveData
	TFileName dllFullName;
	dllFullName.Append(iParent->iDrive.Name());
	dllFullName.Append(_L("\\sys\\bin\\"));
	dllFullName.Append(iDllEntry->GetName());
	RLibrary::TInfoBuf infoBuf;
	TInt ret = RLibrary::GetInfo(dllFullName, infoBuf);
	if(ret != KErrNone)
		{
		return EFalse;
		}
	// Set the DLL's capability
	iCapSet = infoBuf().iSecurityInfo.iCaps;
	// Set the DLL's VID
	iVid = infoBuf().iSecurityInfo.iVendorId;
	// Now verify that SID identified in the resource file matches the SID of the Dll file
	TBool match=iDllEntry->GetThirdUid() ==infoBuf().iSecurityInfo.iSecureId;
	if (!match)
		{
#ifdef __ECOM_TRACE 
		__ECOM_TRACE1("ERROR: Plugin SID Mismatch ERROR for %S.", &dllFullName);
#endif
		return EFalse;
		}

	// Now verify the two DLL's second Uid are of the same Uid type (used for distinguising between collection/collection3)
	match=iDllEntry->GetSecondUid() ==infoBuf().iUids[1];
	if (!match)
		{
#ifdef __ECOM_TRACE 
		__ECOM_TRACE1("ERROR: Plugin UID2 Mismatch ERROR for %S.", &iDllEntry->GetName());
#endif
		return EFalse;
		}

	// On the emulator RLoader::CheckLibraryHash() returns KErrNotSupported.
	// Also on the emulator RLoader does no hash checking for DLL's on removable drives.
	// Therefore to be consistent ECOM does not do any hash checking itself on the emulator.
	// Hence code is removed for emulator builds.
#if !defined(__WINSCW__)
	// Verify hash is available
	if(iParent->iParent->iCachedDriveInfo->DriveIsRemovableL(iParent->iDrive))
		{
		RLoader loader;
		TInt err = loader.Connect();
		if(err != KErrNone)
			{
			return EFalse;
			}
		err = loader.CheckLibraryHash(dllFullName, ETrue);
		loader.Close();
		if(err != KErrNone)
			{
#ifdef __ECOM_TRACE 
		__ECOM_TRACE2("INFO: Hash Check Failed for %S with error %d.", &dllFullName, err);
#endif
			return EFalse;
			}
		}
#endif
	iSecurityChecked = ETrue;
	return ETrue;
	}
	
/**
Check whether security check has been performed if not go retrieve it.
@pre 			This object is fully constructed.
@return			ETrue if the security check is done successfully. Otherwise EFalse is returned. 
*/
TBool CRegistryData::CDllData::ProcessSecurityCheckL()
	{
	if(iSecurityChecked)
		{
		return ETrue;
		}
	return SaveSecurityInfoL();
	}


/**
Writes out the state of this CDllData to aStore.
@param			aStore The stream to store the data in.
@pre 			This object is fully constructed.
*/
void CRegistryData::CDllData::ExternalizeL(RWriteStream& aStore) const
   	{
 	const CEComEntry& dllEntryData = *iDllEntry;
	TInt size=dllEntryData.GetName().Length()-KDllExtensionLength;
 	aStore.WriteUint32L(size);
	aStore.WriteL(dllEntryData.GetName(),size);

	aStore.WriteInt32L(dllEntryData.GetSecondUid().iUid);	
  	aStore.WriteInt32L(dllEntryData.GetThirdUid().iUid);
    TPckg<TTime> modified(dllEntryData.GetModified());	
 	aStore.WriteL(modified);
	
	if(!iParent->iParent->iCachedDriveInfo->DriveIsReadOnlyInternalL(iParent->iDrive))
		{
		if(iRscFileExtension)
			{
			size = iRscFileExtension->Length();		
			aStore.WriteUint32L(size);
			aStore.WriteL(iRscFileExtension->Des(),size);
			}
		else
			{
			aStore.WriteUint32L(0);		
			}
		}

   	if(iIfList)
   		{
   		const TInt entryCount = iIfList->Count();
   		aStore.WriteInt32L(entryCount);
   		for(TInt i = 0; i < entryCount; ++i)
   			((*iIfList)[i])->ExternalizeL(aStore);
   		}
   	else
   		aStore.WriteInt32L(0);
	}

/**
Restores this CDllData to the state specified in aStore.
@param			aStore The stream to read the data from.
@pre 			This object is fully constructed.
@post			The state of this object is restored to that specified in aStore.
*/
void CRegistryData::CDllData::InternalizeL(RReadStream& aStore)
 	{	
	TInt size=aStore.ReadUint32L();
    //The next "if" checks if size < 0 not size <= 0 because it seems the ECOM server externalizes
    //the last file name as a string with length 0. If the fix is <= 0, then it makes it
    //incompatible with the existing applications
    if(size < 0 || size > KMaxFileName)
        {
        User::Leave(KErrCorrupt);
        }
	HBufC* name = HBufC::NewLC(size+KDllExtensionLength);
	TPtr ptr=name->Des();
	aStore.ReadL(ptr,size);
	ptr.Append(KDllExtension);

  	TUid secondUid=TUid::Uid(aStore.ReadInt32L());
  	TUid thirdUid=TUid::Uid(aStore.ReadInt32L());
  	TTime dllModifiedTime;  	
   	TPckg<TTime> modified(dllModifiedTime);	
 	aStore.ReadL(modified);

	if(!iParent->iParent->iCachedDriveInfo->DriveIsReadOnlyInternalL(iParent->iDrive))
		{
		size = aStore.ReadUint32L();
		if(size < 0 || size > KMaxFileName)
		        {
       		 User::Leave(KErrCorrupt);
       		 }
		if(size)
			{
			iRscFileExtension = HBufC::NewL(size);
			TPtr extPtr = iRscFileExtension->Des();
			aStore.ReadL(extPtr,size);
			}
		}
	
   	const TInt entryCount = aStore.ReadInt32L();
	if(entryCount < 0)
		{
		User::Leave(KErrCorrupt);
		}
	
   	// Security check is deferred until the DLL is needed.
	for(TInt i = 0; i < entryCount; ++i)
	   	{
	   	CInterfaceData* interface = CInterfaceData::NewLC(this);
		interface->InternalizeL(aStore);
		AddL(interface);
	   	CleanupStack::Pop(interface);	// now owned by dll	
		}  	
	iDllEntry = CEComEntry::NewL(*name,secondUid,thirdUid);
	iDllEntry->SetModified(dllModifiedTime);
 	CleanupStack::PopAndDestroy(name);
	}
	
/**
@param			aParent The parent drive data of this implementation
*/	
CRegistryData::CDllData::CDllData( CDriveData* aParent) :
	CBase(),
	iParent(aParent)	
	{
	// Do nothing here
	//Initialize empty capabilities here
	iCapSet.SetEmpty();
	}

/**
Standard second phase construction function.
@pre 			This object is fully constructed.
@post			This object is fully constructed and initialized.
*/
void CRegistryData::CDllData::ConstructL()
	{
	iIfList = new(ELeave) RInterfaceList(2);
	}

/**
Standard second phase construction function with parameter.
@param			aDllName the name of this dll
@param			aDllModTime the modified time of this dll
@param 			aSecondUid Distinguishes between components having the same UID1 (which distinguishes between EXEs and DLLs) 
				UID2 identifies Interface Implementation Collections (collection and collection 3)
@param			aThirdUid Identifies a component uniquely. In order to ensure that each binary that needs a distinguishing 
				UID is assigned a genuinely unique value. Symbian manages UID allocation through central database.	
@pre 			This object is fully constructed.
@post			This object is fully constructed and initialized.
*/
void CRegistryData::CDllData::ConstructL(const TDesC& aDllName,const TTime& aDllModTime,const TUid& aSecondUid,const TUid& aThirdUid)
	{
	iIfList = new(ELeave) RInterfaceList(2);
	iDllEntry=CEComEntry::NewL(aDllName,aSecondUid,aThirdUid);
	iDllEntry->SetModified(aDllModTime);
	}
	
	
/**
Populate a caller-supplied TEntry instance with the data from this dll.
@param			aEntry An entry to be populated (destination)
*/
void CRegistryData::CDllData::PopulateAnEntry(TEntry& aEntry) const
	{
	TPtr bufPtr=aEntry.iName.Des();
	bufPtr.Zero();
	bufPtr.Append(iParent->iDrive.Name());
	bufPtr.Append(_L("\\sys\\bin\\"));
	bufPtr.Append(iDllEntry->GetName());
	aEntry.iType	= TUidType(KDynamicLibraryUid,iDllEntry->GetSecondUid(),iDllEntry->GetThirdUid());
	aEntry.iModified= iDllEntry->GetModified();
	}


//
// CRegistryData::CDriveData class


/**
Creates a new CDriveData object and places it on the CleanupStack
@param			aDrive Information on this drive
@param			aParent A pointer to the parent registry data
@return			A pointer to the newly created class.
@post			This object is fully constructed and initialized and on the CleanupStack.
*/
CRegistryData::CDriveData* CRegistryData::CDriveData::NewLC(TDriveUnit aDrive, CRegistryData* aParent)
	{
	CDriveData* self=new(ELeave) CDriveData(aDrive,aParent);  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(); // Complete the 'construction'.
	return self;
	}

/**
Destructor of CDriveData
*/
CRegistryData::CDriveData::~CDriveData()
	{
	if(iDllList)
		{
		// Clear the interface list and destroy its objects
		iDllList->ResetAndDestroy();
		delete iDllList;
		iDllList = NULL;
		}
	iParent=NULL;
	}

/**
Adds the specified CDllData to this drive data in the registry.
@param			aDll The Dll to add to this drive
@pre 			This object is fully constructed.
@post			aDll is added to the list of dll on this drive.
*/
void CRegistryData::CDriveData::AddL(const CDllData* aDll)
	{
	User::LeaveIfError(iDllList->Append(aDll));
	}

/**
Returns the index of the DLL with this Uid in the Dll list.
@param			aDllUid the UID of to be found DLL.
@return  		The index of this DLL in the DLL list. KErrNotFound if not found.
@pre			This object is fully constructed.
*/
TInt CRegistryData::CDriveData::FindDllIndex(const TUid aDllUid) const
	{
	const TInt dllCount = iDllList->Count();
	for(TInt j = 0; j < dllCount; ++j)
		{
		// For each dll structure check the dll UID
		CDllData* dll = (*iDllList)[j];
		if (aDllUid == dll->iDllEntry->GetThirdUid())
			{
			// it is a match 
 			return j;	 				
			}
		}
	return KErrNotFound;
	}
	
/**
Writes out the state of this CDriveData to file.
@param			aFs A handle to an open file server session.
@param			aDatFileName the dat file to persist to
@pre 			This object is fully constructed.
*/
void CRegistryData::CDriveData::ExternalizeL(RFs& aFs,const TDesC& aDatFileName) 
	{
	// attempt to create the folders if they don't already exist
	TInt mkDirError = aFs.MkDirAll(aDatFileName);
		
	if((mkDirError == KErrNone) || (mkDirError == KErrAlreadyExists))
		{
		RFileWriteStream registryStream;
		if(registryStream.Replace(aFs,aDatFileName, EFileWrite) == KErrNone)
			{
			CleanupClosePushL(registryStream);
			// Write the version number as the first thing in the file stream.
			registryStream.WriteInt32L(KRegVersion);				
			if(iDllList)
				{
				TInt entryCount = iDllList->Count();
				registryStream.WriteInt32L(entryCount);
				// Now stream out the data
				for(TInt i = 0; i < entryCount; ++i)
					((*iDllList)[i])->ExternalizeL(registryStream);
				}
			else
				registryStream.WriteInt32L(0);
			CleanupStack::PopAndDestroy();	// registryStream
			}
		}
	else
		{
		User::Leave(mkDirError);
		}
	}

/**
Restores the state of this CDriveData from file.
@param			aFs A handle to an open file server session.
@param			aDatFileName the dat file name to internalize from
@pre 			This object is fully constructed.
@post			The state of this object is restored to that specified
				in the file on this drive.
*/

void CRegistryData::CDriveData::InternalizeL(RFs& aFs,const TDesC& aDatFileName)
	{
	DoInternalizeL(aFs, aDatFileName);  
	}
/**
The method internalizes the data from aRegistryStream.
@param aFileName The ECOM registry file name.
@leave KErrNoMemory
@leave Some other system-wide error codes as well.
*/
void CRegistryData::CDriveData::DoInternalizeL(RFs& aFs, const TDesC& aFileName)
	{
	RFileReadStream registryStream;
	User::LeaveIfError(registryStream.Open(aFs, aFileName, EFileRead));
	CleanupClosePushL(registryStream);	
	// Read in version number. If its not what expected - the file is treated as corrupted.
	const TInt version = registryStream.ReadInt32L();
	if(version != KRegVersion)
		{
		User::Leave(KErrCorrupt);
		}	
	const TInt entryCount = registryStream.ReadInt32L();
    if(entryCount < 0)
        {
        User::Leave(KErrCorrupt);
        }
	for(TInt i = 0; i < entryCount; ++i)
		{
		CDllData* dll = CDllData::NewLC(this);
		dll->InternalizeL(registryStream);
		// always add the DLL, DLL security check will be deferred 
		// until the DLL is needed.		
		AddL(dll);
		CleanupStack::Pop(dll);	// now owned by drive	
		}
	CleanupStack::PopAndDestroy();//registryStream
	}
	
/**	
Constructor for CDriveData
@param			aDrive Information on this drive
@param			aParent A pointer to the parent registry data
*/	
CRegistryData::CDriveData::CDriveData(TDriveUnit aDrive, CRegistryData* aParent) :
	CBase(), 

	iDrive(aDrive),
	iParent(aParent),
	iDriveChanged(ETrue),
	iRegistryChanged(EFalse)
	{
	}

/**
Standard second phase construction function.
@pre 			This object is fully constructed.
@post			This object is fully constructed and initialized.
*/
void CRegistryData::CDriveData::ConstructL()
	{
	iDllList = new(ELeave)TDll;
	}
	
//
// CRegistryData class

/**
Standardized safe construction which leaves nothing on the cleanup stack.
This overload is used by CEComImplIndexPerfTest which plots 
discovery time vs. granularity settings.
@leave 			KErrNoMemory.	
@param 			aFs The open file session.
@param			aInterfaceImplIndexGranularity granularity of main index.
@param			aImplIndexGranularity granularity of auxiliary index.
@return			The newly created instance of the registry data.
@post			CRegistryData is fully constructed, and initialized.
*/
CRegistryData* CRegistryData::NewL(RFs& aFs, TInt aInterfaceImplIndexGranularity, TInt aImplIndexGranularity)
	{
	CRegistryData* self=new(ELeave) CRegistryData(aFs, aInterfaceImplIndexGranularity, aImplIndexGranularity);  // calls c'tor
	CleanupStack::PushL(self);	// Make the construction safe by using the cleanup stack
	self->ConstructL(); // Complete the 'construction'.
	CleanupStack::Pop(self);
	return self;
	}

/**
Standardized safe construction which leaves nothing on the cleanup stack.
This overload uses default granularities for the two indexes. These
default values were found to provide reasonably good performance at
the time of testing.
@leave 			KErrNoMemory.	
@param 			aFs The open file session.
@return			The newly created instance of the registry data.
@post			CRegistryData is fully constructed, and initialized.
*/
CRegistryData* CRegistryData::NewL(RFs& aFs)
	{
	return NewL(aFs, KDefaultInterfaceImplIndexGranularity, KDefaultImplIndexGranularity);
	}

CRegistryData::~CRegistryData()
	{
	for(TInt index = 0; index < iInterfaceImplIndex.Count(); ++index)
		{
		iInterfaceImplIndex[index].Reset();
		}
	iInterfaceImplIndex.Reset();
	
	// Reset only. It does not own the pointers!
	iImplIndex.Reset();

	if(iRegistrations)
		{
		iRegistrations->ResetAndDestroy();
		delete iRegistrations;
		iRegistrations = NULL;
		}
	// reset the cached language settings
	RDowngradePath::Reset();

	delete iCachedDriveInfo;
	}

/**
Lists the implementations given the interface UID. The filtered list is returned in the client provided
RImplDataArray parameter.
@param			aInterfaceUid The Uid of the interface which the implementations should provide
@param			aImplementationData Return value. The filtered list.
*/
void CRegistryData::ListImplementationsL(TUid aInterfaceUid,
										 RImplDataArray& aImplementationData) const
	{
	// If discoveries are taking place then the index is invalid so we cannot do
	// this request
	if(iCurrentlyDiscovering)
		User::Leave(KEComErrListCurrentlyUnavailable);
    
	TBool securityPassed = EFalse;
	//Do the security check
	TInt index = 0;
	while (!securityPassed)
		{
		aImplementationData.Reset();
		index = IndexedFind(aInterfaceUid);
		if(index == KErrNotFound)
			{
			User::Leave(KEComErrNoInterfaceIdentified);
			}
		User::LeaveIfError(index);
	
		TImplContainerArray& implementationList = iInterfaceImplIndex[index].iImpData;
		for (TInt j = 0; j < implementationList.Count(); j++)
			{
			CImplementationData* currentImplementation = implementationList[j].iCurrentImpl;
			CDllData* dll = currentImplementation->iParent->iParent;
			
			securityPassed = dll->ProcessSecurityCheckL();
			if(!securityPassed)
				{
				// remove the implementations of the DLL from iInterfaceImplIndex
				// and remove the DLL from its parent DLL list.
				DeleteDllL(dll);
				delete dll;
				dll = NULL;
				// don't continue processing implementations after we have deleted the DLL
				// because the implementations list will have changed, so we need to 
				// re-do the IndexedFind
				break;	
				}
			else
				{
				User::LeaveIfError(aImplementationData.Append(currentImplementation));
				}	
			}
		}
	}

/**
Enables or disables the specified interface implementation within the registry.
@param			aImplementationUid The interface implementation to change.
@param			aState ETrue to enable the implementation, EFalse to disable it
@return			KErrNone, KEComErrEnableFailed or KEComErrDisableFailed
@pre 			CRegistry is fully constructed,
@post			The implementation is now enabled or disabled as described 
				by aState.
*/
TInt CRegistryData::SetEnabledState(TUid aImplementationUid, TBool aState)
	{
	CImplementationData* impData = NULL;
	TUid dummy={0x00000000};
	
	(void)FindImplementation(aImplementationUid, dummy, impData);

	if(impData != NULL)
		{
		impData->iImplInfo->SetDisabled((aState) ? EFalse : ETrue);
		return KErrNone;
		}

    return (aState) ? KEComErrEnableFailed : KEComErrDisableFailed;
	}


/**
Temporarily uninstalls the interface implementation groups upon the specified drive.
(ie this can be undone later).
@param			aDrive	The identifier of the drive to uninstall.
@pre 			CRegistry is fully constructed
@post			The registered implementation groupings
				stored upon the specified drive are no 
				longer available for use.
*/
void CRegistryData::TemporaryUninstallL(const TDriveUnit& aDrive)
	{
	// Find the appropriate drive entry and remove it...
	// Note : the drive may have already been taken offline, so no save is possible!
	CDriveData* drive = NULL;
	TInt driveIndex = FindDriveL(aDrive, drive);
	if(driveIndex != KErrNotFound)
		{
    	TInt dllIndex = drive->iDllList->Count();
		while(dllIndex > 0)
			{
			--dllIndex;
			RemoveFromIndexL((*drive->iDllList)[dllIndex]);
			}       
		iRegistrations->Remove(driveIndex);
		delete drive;

		DriveChanged(aDrive, ETrue);
		}
	// The flag iRegistryChanged has been moved to each drive.
	}

/**
Undoes a temporary uninstall of the interface 
implementation groups upon the specified drive.
(ie this can be undone later).
@leave			KErrNoMemory, KErrReinstallFailed.
@param			aDrive	The identifier of the drive to reinstate.
@pre 			CRegistry is fully constructed
@post			The registered implementation groupings
				stored upon the specified drive are again 
				made available for use.
*/
void CRegistryData::UndoTemporaryUninstallL(const TDriveUnit& aDrive)
	{
	CDriveData* driveData = NULL;
	// If the drive was not found then install it, otherwise do nothing because it
	// is already there.  NB. We could leave here but the problem with that is the 
	// use case where the drive has been mounted during registry load but the registrar 
	// is started afterwards, and makes calls to this method as it discovers drives 
	// for the first time. 
	if(FindDriveL(aDrive, driveData) == KErrNotFound)
		{
		// Add the drive entry
		CDriveData* drive = CDriveData::NewLC(aDrive,this);
		AddImplDataL(drive);
		User::LeaveIfError(iRegistrations->Append(drive));
		CleanupStack::Pop(drive);		// now owned by iRegistrations

		DriveChanged(aDrive, EFalse);

		// NB We DO need to set iRegistryChanged = ETrue because the idx file needs to be
		// rewritten to include the reinstalled drive
		}
	}
	
/** 
Update the staus of drives in the system
@param 	aDrive The drive that has changed
@param 	aDriveRemoved Indicates whether a drive has been removed or added
@pre 	CRegistry is fully constructed.
@post	The state of the drive has been stored
*/
void CRegistryData::DriveChanged(const TDriveUnit& aDrive, TBool aDriveRemoved)
	{
	if(aDriveRemoved)
		{
		iRemovedDrives |= (0x1 << aDrive);
		}
	else
		{
		iRemovedDrives &= ~(0x1 << aDrive);
		}
	}

/** 
Find if any Dll is regsitered in the drive unit.
@param aDrive The identifier of the drive to find if any Dll is registered.
@return ETrue if any Dll is registered in the drive, otherwise EFalse.
@pre 			CRegistry is fully constructed.
@post			If any Dll is registered in the drive is returned if successfully.
*/
TBool CRegistryData::IsAnyDllRegisteredWithDriveL(const TDriveUnit& aDrive)const
	{
	CDriveData* driveData = NULL;
	if(FindDriveL(aDrive, driveData) == KErrNotFound)
		{
		User::Leave(KEComErrDriveNotFound);	
		}
	return driveData->iDllList->Count() > 0;
	}
	
/**
Determine if the specified implementation grouping already registered, but needs an update.
@param			aDllUid The interface implementation collection data to add. 
				CRegistryData takes ownership of this object
				so it should not be on the CleanupStack.
@param			aModified The date of the last modification of the Interface 
				Implementation Collection. 
@param			aUpdate An output to indicate if the registry entry requires an update. 
				(Only valid if the return is ETrue).
@param			aDriveData The drive data used to find DLL.
@return			ETrue if the Interface Implementation Collection already has a registry entry.
				EFalse otherwise.
@pre 			This object is fully constructed and there is also a valid drive entry
				in the registry for aDrive
@post			The new data is added to the registry
*/
TBool CRegistryData::IsRegisteredWithDate (TUid aDllUid, 
										   	const TTime& aModified, 
										   	TBool& aUpdate, 
										   	CDriveData* aDriveData)
	{
	// Find the appropriate drive entry for this 
	// Interface Implementation Collection
	TBool found = EFalse;
	TInt index = aDriveData->FindDllIndex(aDllUid);
	if(index != KErrNotFound)
		{
		TDll* dllList = aDriveData->iDllList;
		CDllData* dll = (*dllList)[index];
		const CEComEntry& dllEntry =*(dll->iDllEntry);	
		aUpdate = dllEntry.GetModified().Int64() < aModified.Int64(); 
		found = ETrue;
		}
	return found;
	}

/**
Adds data on a specific dll to the registry. The data to be added
is parsed by CRegistryParser.
@leave			KErrNoMemory If the item could not be appended to the registry
@leave			KEComErrDriveNotFound If aDrive is not a valid drive
@param			aDrive The drive the registry data has been found on 
@param			aFoundDriveIndex The index of the registry data for the drive 
				the interface implementation collection data has been found on.
@param			aDllData The dll data to add. CRegistryData takes ownership of this object
				so it should not be on the CleanupStack.
@pre 			This object is fully constructed and there is also a valid drive entry
				in the registry for aDrive
@post			The new data is added to the registry
*/
void CRegistryData::AddDllDataL(const TDriveUnit& aDrive, TInt aFoundDriveIndex, CRegistryData::CDllData* aDllData)
	{
	const TInt regCount = iRegistrations->Count();
	if(regCount == 0)
		User::Leave(KEComErrDriveNotFound);

	// Find the appropriate drive entry
	CDriveData* drive = NULL;
	if(aFoundDriveIndex == KErrNotFound)
		{
		if(FindDriveL(aDrive, drive) == KErrNotFound)
			User::Leave(KEComErrDriveNotFound);
		}
	else
		drive = (*iRegistrations)[aFoundDriveIndex];
	
	// Append the new data to the list
	TDll* dllList = drive->iDllList;
	
	// NOTE : This function MUST NOT leave after the following line because 
	// aDllData will potentially be 'owned' in two places (dllList and the caller) 
	// until we return and the caller can Pop.
	TBool checkNeeded = ETrue;
	TInt ifListCount=aDllData->iIfList->Count();
	for(TInt ifListCounter=0; ifListCounter < ifListCount; ifListCounter++)
		{
		// For each interface structure
		CInterfaceData* interface = (*aDllData->iIfList)[ifListCounter];
		for(TInt impNum = 0; impNum < interface->iImplementations->Count(); ++impNum)
			{
			CImplementationData* implData = (*interface->iImplementations)[impNum];
			if(!InsertIntoIndexL(implData,checkNeeded))
				{
				interface->iImplementations->Remove(impNum); //remove from implementation array
				delete implData; //delete object
				}	
			}
		}
	//add filtered list for legitimate implementations into registry
	User::LeaveIfError(dllList->Append(aDllData)); 
		
	drive->iRegistryChanged = ETrue;	
	}

/**
Updates the data for a dll in the registry.
@leave			KErrNoMemory If the indexes cannot be rebuilt
@param			aFoundDriveIndex The index of the registry data for the drive 
				the interface implementation collection data has been found on.
@param			aDllData The updated interface implementation collection data. 
				CRegistryData takes ownership of this object
				so it should not be on the CleanupStack.
@pre 			This object is fully constructed
@post			The dll data is updated in the registry
*/
void CRegistryData::UpdateDllDataL(const TDriveUnit& aDrive,TInt aFoundDriveIndex, CDllData* aDllData)
	{
	const TInt regCount = iRegistrations->Count();
	if(regCount == 0)
		User::Leave(KEComErrDriveNotFound);

	// Find the appropriate drive entry
	CDriveData* drive = (*iRegistrations)[aFoundDriveIndex];
	
	TInt index = drive->FindDllIndex(aDllData->iDllEntry->GetThirdUid());
	
	// find the dll entry and clear that too
	if(index != KErrNotFound)
		{
		// Ok its a match, so remove this registry entry and call AddDllData()
		//to add the new dll
		TDll* dllList = drive->iDllList;
		CDllData* olddll = (*dllList)[index];
		//remove 'old'implementations from iInterfaceImplIndex
 		RemoveFromIndexL(olddll);
 		//now remove old dll from the array and registry
 		dllList->Remove(index);
   		delete olddll;
   		AddDllDataL(aDrive,aFoundDriveIndex,aDllData);
   		drive->iRegistryChanged = ETrue;

		}
	if (index == KErrNotFound)
	    {
        __ECOM_TRACE("ECOM: PANIC in CRegistryData::UpdateDllDataL(), expected DLL data missing from ECOM registry");
	    __ASSERT_DEBUG(EFalse, User::Panic (KEComServerPanicCategory, EEComPanic_CRegistryData_UpdateDllDataL_DllRegistrationMissing));
	    }
	}

/**
The cue that newly discovered Dlls are about to be registered, 
and, therefore, the registry index will be out of date until 
DiscoveriesCompleteL is called.
@pre 			CRegistrar is fully constructed
@post			iCurrentlyDiscovering is set and the index list is cleared.
*/
void CRegistryData::DiscoveriesBeginning() const
	{
	iCurrentlyDiscovering = ETrue;
	// The registry is about to start changing so the index is now invalid
	}

/**
The cue to rebuild the registry indexes
@leave			KErrNoMemory
@see 			CStore
@param			aSuccessful Indicates whether discovery completed fully or not
@param			aProcessingType indicates the type of processing for plugins
				for ensuring that plugins are not processed multiple times
				during start-up phase
@param			aHasRegistryChanged The flag to indicate whether registry data has been changed 
@pre 			CRegistrar is fully constructed
@post			The internal access indexes have been rebuilt
*/
void CRegistryData::DiscoveriesCompleteL(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType, TBool& aHasRegistryChanged)
	{
	iCurrentlyDiscovering = EFalse;
	if (!aSuccessful)
		{
		return;
		}
	//if the rediscovery is a result of iLanguageChanged, we should reset it to false
	iLanguageChanged=EFalse;		
	// Warning: this method may be called from the CEComServer destructor.
	// E.g. ecomservertest running OOM test on CEComServer::NewLC.
	// The call stack: 
	// ~CEComServer delete iRegistrar, ~CRegistrar delete iDiscoverer,
	// ~CDiscoverer calls CRegistrar::DiscoveriesComplete,
	// then here.
    // Hence if ! aSuccessful do not validate.

	if(aProcessingType != EPluginProcessingTypeCriticalOnly)
		{
		ValidateRegistryL();
		}
	if(aProcessingType==EPluginProcessingTypeNonCriticalOnly || aProcessingType==EPluginProcessingTypeAll)
		{
		//first find whether there is any registry data changed
		TInt regCount=iRegistrations->Count();
		for(TInt i = 0; i <regCount; i++)
			{
			if ((*iRegistrations)[i]->iRegistryChanged)
				{
	 			aHasRegistryChanged = ETrue;
	 			//now having cached that the registry has changed we need
	 			//to reset this flag in ALL the CDriveData, note that it is
	 			//possible that more than one drive will have the flag set
	 			(*iRegistrations)[i]->iRegistryChanged=EFalse;
				}
			}
			//Also indicate registry change if any drives have been removed
			aHasRegistryChanged |= (iRemovedDrives != 0);
			iRemovedDrives = 0;
		}
	}

/**
	@fn 			SetDiscoveryFlag(const TDriveUnit aDrive)
	Intended Usage	: Set the flag to indicate the drive has change(s)
	Error Condition	: None.
	@param 			aDrive The identifier of the drive changed.
	@pre 			The CRegistrar must be fully constructed
	@post			The flag is set.				 
*/

void CRegistryData::SetDiscoveryFlagL(const TDriveUnit& aDriveUnit)
	{
	CDriveData* drive = NULL;
	TInt driveIndex = FindDriveL(aDriveUnit, drive);
	if(driveIndex != KErrNotFound)
		{
		drive->iDriveChanged = ETrue;
		}
	
	}
/**
Indicates whether the registry index is currently valid.  The 
index will not be valid if discoveries are currently taking place.
If the index is not currently valid then calls to 
ListImplementationsL() cannot be serviced.
@return			ETrue if the index is currently valid, EFalse otherwise.
@pre 			CRegistrar is fully constructed
*/
TBool CRegistryData::IndexValid() const
	{
	return !iCurrentlyDiscovering;
	}

/**
@param			aCapabilitySet A capability set
@param			aImplUid The Uid of the implementation for which info is required
@param			aInterfaceUid The uid of the interface associated with aImplUid to find or less than
				0 if uid is unknown.
@param			aEntry Output parameter which will contain the dll information
@param			aImplInfo An output parameter. If the call succeeds it will point to the found implementation information,
				NULL otherwise.
@param			aIsOnRWDrive an output parameter. If the call is successful, this will
				be set to ETrue if the implementation is on RW drive. EFalse if the
				implementation is on ReadOnly drive.
@return			KErrNone if the call succeeds, KErrNotFound - no implementation found, 
				KErrPermissionDenied - the caller has not enough capabilities to load the plugin.
@pre			CRegistrar is fully constructed
*/
TInt CRegistryData::GetImplementationDllInfoForServer(
	const TCapabilitySet& /*aCapabilitySet*/,
	const TUid aImplUid,
	const TUid aInterfaceUid,	
	TEntry& aEntry,
	CImplementationInformation*& aImplInfo,
	TBool& aIsOnRWDrive) const
	{
	aImplInfo = NULL;
	CImplementationData* implData;
	
	TInt res = FindImplementation(aImplUid, aInterfaceUid, implData);
	if (KErrNone != res)
		{
		return res;
		}
	
	aImplInfo = implData->iImplInfo;
	const CDllData* dll = implData->iParent->iParent;
	dll->PopulateAnEntry(aEntry);

	TEComCachedDriveInfoIterator iter(*iCachedDriveInfo);
	if (! iter.SetPos(dll->iParent->iDrive))
		{
		res = KErrNotFound;
		}
	else
		{
		aIsOnRWDrive = iter.DriveIsWritable();
		}
	return res;
	}

/**
@param			aClientRequest A client request
@param			aImplUid The Uid of the implementation for which info is required
@param			aInterfaceUid The uid of the interface associated with aImplUid to find or less than
				0 if uid is unknown.
@param			aEntry Output parameter which will contain the dll information
@param			aImplInfo An output parameter. If the call succeeds it will point to the found implementation information,
				NULL otherwise.
@param			aSecurityCheckNeeded The bool value to identify whether the security check is needed here. The default value is false.				
@return			KErrNone if the call succeeds, KErrNotFound - no implementation found,
				KErrPermissionDenied - the caller has not enough capabilities to load the plugin.
@pre			CRegistrar is fully constructed
*/
TInt CRegistryData::GetImplementationDllInfoForClientL(
	const TClientRequest& aClientRequest,
	const TUid aImplUid,
	const TUid aInterfaceUid,	
	TEntry& aEntry,
	CImplementationInformation*& aImplInfo,
	TBool aSecurityCheckNeeded)const
	{
	aImplInfo = NULL;
	TInt res = KErrNotFound;
	CImplementationData* implData;
	if (!aSecurityCheckNeeded)
		{
		//The security check has been done already. All the invalid dll have been removed.
		res = FindImplementation(aImplUid, aInterfaceUid, implData);
		if (KErrNone != res)
			{
			return res;
			}
		CDllData* dll = implData->iParent->iParent;
		dll->PopulateAnEntry(aEntry);
		aImplInfo = implData->iImplInfo;
		}
	else
		{
		TBool securityPassed = EFalse;
		while(!securityPassed) // go out of this loop either Security check is passed or no DLL found.
			{
			res = FindImplementation(aImplUid, aInterfaceUid, implData);
			if (KErrNone != res)
				{
				return res;
				}
			CDllData* dll = implData->iParent->iParent;
			// security check is deferred to here.
			securityPassed = dll->ProcessSecurityCheckL();
			if(securityPassed)
				{
				if (!aClientRequest.CheckCapability(dll->iCapSet, *(implData->iImplInfo)))
					{
					return KErrPermissionDenied;
					}
				dll->PopulateAnEntry(aEntry);
				aImplInfo = implData->iImplInfo;
				}
			else
				{
				// remove the implementations of the DLL from iInterfaceImplIndex
				// and remove the DLL from its parent DLL list.
				DeleteDllL(dll);
				delete dll;
				dll = NULL;
				}
			}
		}
	return res;
	}

/**
This method removes all implementations of This Dll from the iInterfaceImplIndex,
then delete this Dll from its parent Dll list.
@param	aDllData the Dll to be cleaned.
@pre	CRegistrar is fully constructed	
*/
void CRegistryData::DeleteDllL(CDllData* aDllData) const
	{
	// find the index of the passed aDllData in its parent's Dll list.
	TInt index = aDllData->iParent->FindDllIndex(aDllData->iDllEntry->GetThirdUid());
	
	//removes all implementations of This Dll from the iInterfaceImplIndex
	RemoveFromIndexL(aDllData);
	if(index != KErrNotFound)
		{
		//now remove the dll from the array and registry
		TDll* dllList = aDllData->iParent->iDllList;
		dllList->Remove(index);
		}
	}


/**
Indicates whether the language downgrade path has changed.
This means we need to call NearestLanguageFile again
@return			ETrue if the language downgrade path has changed, EFalse otherwise.
@pre 			CRegistrar is fully constructed
*/
TBool CRegistryData::HasLanguageChanged() const
	{
	return iLanguageChanged;
	}

	
/**
@param 			aFs A handle to a connected file server.
*/
CRegistryData::CRegistryData(RFs& aFs,
							 TInt aInterfaceImplIndexGranularity,
							 TInt aImplIndexGranularity) :
	CBase(), 
	iFs(aFs),
	iInterfaceImplIndex(aInterfaceImplIndexGranularity),
	iImplIndex(aImplIndexGranularity)
	{
	}


/**
Completes the safe construction of the CRegistryData object.
@leave 			KErrNoMemory.
@pre 			This object is constructed
@post			This object is fully initialized
*/
void CRegistryData::ConstructL()
	{
	iCachedDriveInfo = CEComCachedDriveInfo::NewL(iFs);

	// Construction of the empty registration data structure here
	iRegistrations = new(ELeave)TRegistration;
	iSystemDrive=iFs.GetSystemDrive();	
	// during construction we always need to call this function in
	// order to initialise and store the language settings at boot time
	TRAPD(err,iLanguageChanged=RDowngradePath::HasChangedL(iFs))
	if (err==KErrNoMemory)
		{
		User::LeaveNoMemory();
		}
	}

/**
To find the index entry for aImplementationUid.
@return			The index of the item or KErrIndexEntryNotFound. 
@pre 			This object is fully constructed
*/
TInt CRegistryData::IndexedFind(TUid aInterfaceUid) const
	{
	// Find the correct implementation
	TInterfaceIndex key;
	key.iInterfaceUid = aInterfaceUid;
	return iInterfaceImplIndex.FindInOrder(key,TLinearOrder<TInterfaceIndex>(TInterfaceStruct::CompareInfUid));
	}

/**
Used by a TIdentityRelation to decide if two CDriveData match.
@return 		ETrue if the TDriveUnit inside the indexes match.
@param			aIndexOne The first CDriveData to compare
@param			aIndexTwo The second CDriveData to compare
*/
TBool CRegistryData::MatchOnDrive(const CRegistryData::CDriveData& aIndexOne, 
								  const CRegistryData::CDriveData& aIndexTwo)
	{
	return aIndexOne.iDrive == aIndexTwo.iDrive;
	}

TInt CRegistryData::FindDriveL(const TDriveUnit& aDrive,
							   CRegistryData::CDriveData*& aDriveData)const 
	{
	TInt index = KErrNotFound;

	// Set up the find parameters
	TIdentityRelation<CRegistryData::CDriveData> identity(MatchOnDrive);
    CDriveData* driveToMatch = CDriveData::NewLC(aDrive,const_cast<CRegistryData*>(this));

	index = iRegistrations->Find(driveToMatch, identity);
	
	CleanupStack::PopAndDestroy(driveToMatch);

	if(index != KErrNotFound)
		aDriveData = (*iRegistrations)[index];
	else
		aDriveData = NULL;

	return index;
	}

/**
Finds the instance of CImplementationData for given impl uid and interface uid if known.
@return			KErrNone if impl found, otherwise KErrNotFound.
@param			aImplUid The uid of the impl to find
@param			aInterfaceUid The uid of the interface associated with the impl to find or less than
				0 if uid is unknown
@param			aImplData Set to the instance CImplementationData found in the index or if
				not found set to NULL. This argument is always overwritten.
*/
TInt CRegistryData::FindImplementation(const TUid aImplUid, const TUid aInterfaceUid, 
												CImplementationData*& aImplData) const
	{
	aImplData = NULL;
	TInt i;
	// Index is kept up to date even when discoveries are occurring
	// therefore always search index for implementation

	// if aInterfaceUid is non zero use it to find position in index list
	if(aInterfaceUid.iUid != 0)
		{
		i = IndexedFind(aInterfaceUid);
		if(i == KErrNotFound)
			{
			return KErrNotFound;
			}
		
		TInterfaceIndex listItem = iInterfaceImplIndex[i];
		i = listItem.iImpData.FindInOrder(aImplUid,
			TImplStruct::CompareUidAgainstImplStruct);
		if (i >= 0)
			{
			aImplData = listItem.iImpData[i].iCurrentImpl;
			return KErrNone;
			}
		}
	else // not given I/F UID. Use the iImplIndex.
		{
		// NB: 1. Impl UID should be globally unique. It is an error if impls
		// have same Impl UID but different i/f UIDs. It means one of the 
		// plug-in supplier made an error in the .RSS file or it could be a
		// deliberate attach. But to maintain backward compatibility, ECOM
		// allows this error.
		// (Multiple impls can have same i/f UID and same Impl UID. That is
		//  the upgrade situation and only one of them get stored in iImplIndex.)

		// 2. Entries in iImplIndex are ordered by Impl UID, and if duplicated,
		// ordered by i/f UID. Here i/f UID is wild card. The situation is 
		// analogous to the array was built with InsertInOrderAllowRepeats.
		// RPointerArray::SpecificFindInOrder is for ordered search in array
		// with duplicates. Note: it is very expensive to instantiate
		// a CImplementationData* just for binary search. Hence reinterpret
		// cast the TUid* as CImplementationData*. CompareUidAgainstImplData
		// knows to cast the first argument back to TUid.
		i = iImplIndex.SpecificFindInOrder(
			reinterpret_cast<const CImplementationData*>(&aImplUid), 
			TLinearOrder<CImplementationData>(CImplementationData::CompareUidAgainstImplData),
			EArrayFindMode_First);
		if (i == KErrNotFound)
			{
			return KErrNotFound;
			}

		aImplData = iImplIndex[i];

		// If duplicates exist, they are sorted according to i/f UID
		// but we cannot take advantage of this order. To miniize risk
		// of malware using duplicate Impl UID as DOS attack, ECOM applies
		// the rule that ROMBased plug-in preferred over non-ROMBased,
		// higher drive letter preferred over lower drive letter, and 
		// lastly lower i/f UID preferred over higher i/f UID. Must visit
		// every duplicate to compare their ROMBasedness and driver letter

		TInt count = iImplIndex.Count();
		for (TInt j = i + 1; j < count; j++)
			{
			if ( iImplIndex[j]->iImplInfo->ImplementationUid().iUid != aImplUid.iUid )
				{
				break;
				}

			aImplData = SelectDuplicatedImpl(aImplData, iImplIndex[j]);
			}

		return KErrNone;
		}
		
	return KErrNotFound;
	}

/**
This function helps FindImplementation() to decide which of two
implementations with duplicated implementation UID to choose.
The selection rules are:
1. ROM based plugins > non-ROM based.
2. Higher drive letter > lower drive letter
3. Lower I/F UID > Higher I/F UID (for backward compatibility reason)

@param aImpl1 - one of the two implementations to compare.
@param aImpl2 - the other implementation to compare.
@return the preferred implementation.
*/
CRegistryData::CImplementationData* CRegistryData::SelectDuplicatedImpl(
	const CImplementationData* aImpl1, 
	const CImplementationData* aImpl2) const
	{

#ifdef ECOM_TRACE
	TPtrC oldName = aImpl1->iImplInfo->DisplayName().Left(60);
	TPtrC newName = aImpl2->iImplInfo->DisplayName().Left(60);
	TPtrC oldDll = aImpl1->iParent->iParent->iDllEntry->GetName();
	TPtrC newDll = aImpl2->iParent->iParent->iDllEntry->GetName();
#endif

	const TInt KRomBasedFactor = 0x100;

	TInt drive1 = aImpl1->iImplInfo->Drive();
	if (aImpl1->iImplInfo->RomBased())
		{
		drive1 |= KRomBasedFactor;
		}

	TInt drive2 = aImpl2->iImplInfo->Drive();
	if (aImpl2->iImplInfo->RomBased())
		{
		drive2 |= KRomBasedFactor;
		}

	if (drive1 > drive2)
		{
#ifdef ECOM_TRACE
		if ((drive1 & KRomBasedFactor) && !(drive2 & KRomBasedFactor))
			{
			__ECOM_TRACE5("ECOM dup impl UID resolution: rejected \"%S\" i/f UID 0x%X impl UID 0x%X, select ROM based %S over R/W %S", &newName, aImpl2->iParent->iInterfaceUid.iUid, aImpl2->iImplInfo->ImplementationUid().iUid, &oldDll, &newDll);
			}
		else
			{
			__ECOM_TRACE5("ECOM dup impl UID resolution: rejected \"%S\" i/f UID 0x%X impl UID 0x%X, select higher drive %S over %S", &newName, aImpl2->iParent->iInterfaceUid.iUid, aImpl2->iImplInfo->ImplementationUid().iUid, &oldDll, &newDll);
			}
#endif

		return const_cast<CImplementationData*>(aImpl1);
		}
	else if (drive1 < drive2)
		{
#ifdef ECOM_TRACE
		if ((drive2 & KRomBasedFactor) && !(drive1 & KRomBasedFactor))
			{
			__ECOM_TRACE5("ECOM dup impl UID resolution: rejected \"%S\" i/f UID 0x%X impl UID 0x%X, select ROM based %S over R/W %S", &oldName, aImpl1->iParent->iInterfaceUid.iUid, aImpl1->iImplInfo->ImplementationUid().iUid, &newDll, &oldDll);
			}
		else
			{
			__ECOM_TRACE5("ECOM dup impl UID resolution: rejected \"%S\" i/f UID 0x%X impl UID 0x%X, select higher drive %S over %S", &oldName, aImpl1->iParent->iInterfaceUid.iUid, aImpl1->iImplInfo->ImplementationUid().iUid, &newDll, &oldDll);
			}
#endif

		return const_cast<CImplementationData*>(aImpl2);
		}
	// They are on the same drive. Choose the one with lower I/F UID.
	else if (aImpl2->iParent->iInterfaceUid.iUid < aImpl1->iParent->iInterfaceUid.iUid) 
		{
#ifdef ECOM_TRACE
		__ECOM_TRACE5("ECOM dup impl UID resolution: rejected \"%S\" i/f UID 0x%X impl UID 0x%X, select %S with lower i/f UID 0x%X", &oldName, aImpl1->iParent->iInterfaceUid.iUid, aImpl1->iImplInfo->ImplementationUid().iUid, &newDll, aImpl2->iParent->iInterfaceUid.iUid);
#endif

		return const_cast<CImplementationData*>(aImpl2);
		}

#ifdef ECOM_TRACE
	__ECOM_TRACE5("ECOM dup impl UID resolution: rejected \"%S\" i/f UID 0x%X impl UID 0x%X, select %S with lower i/f UID 0x%X", &newName, aImpl2->iParent->iInterfaceUid.iUid, aImpl2->iImplInfo->ImplementationUid().iUid, &oldDll, aImpl1->iParent->iInterfaceUid.iUid);
#endif

	return const_cast<CImplementationData*>(aImpl1);
	}

/**
Checks each entry in the registry to ensure that both the RSC file and the 
corresponding dll exist.  If not the dll branch is removed
from the tree.  If a drive branch is found which contains no dlls
it is also removed.
@pre 			This object is fully constructed.
@post			Any out of date registry entries are removed from the tree.
*/
void CRegistryData::ValidateRegistryL()
	{
	TInt driveIndex = iRegistrations->Count();
	TFileName rscFileName;
	while(driveIndex > 0)
		{
		--driveIndex;
		CDriveData* drive = (*iRegistrations)[driveIndex];
		
		if( !iCachedDriveInfo->DriveIsReadOnlyInternalL(drive->iDrive) &&
			drive->iDriveChanged)
			{
			TInt dllIndex = drive->iDllList->Count();	
			while(dllIndex > 0)
				{
				--dllIndex;
				CDllData* dll = (*drive->iDllList)[dllIndex];
				//reset the buffer first			
				rscFileName.Zero();
				rscFileName.Append(dll->iParent->iDrive.Name());
				rscFileName.Append(KEComResourceFilePath);
				rscFileName.Append(dll->iDllEntry->GetName().Left(dll->iDllEntry->GetName().Length()-4));
				if(dll->iRscFileExtension)
					{
					rscFileName.Append(dll->iRscFileExtension->Des());
					}
			
					// check the existence of RSC file in resource\plugins\ directory.
					// RSC filename is already a full name here.
				TBool rscFileExistence = BaflUtils::FileExists(iFs, rscFileName);

  				//save the security info, this will both check existence of the dll
  				//and cache the information for future use
  				TBool success=dll->SaveSecurityInfoL();
				// If this dll is not found or the corresponding RSC file is not found, then remove it from the registry
				if(!success || !rscFileExistence)
					{
					//remove all implementations of this dll from iInterfaceImplIndex
					RemoveFromIndexL(dll);
					drive->iDllList->Remove(dllIndex);
					//set flag to indicate registry data has been changed
					drive->iRegistryChanged = ETrue;
					delete dll;
					}
				}
			}
		}
	}

/**
Determines whether the new implmentation should be preferred over the existing implementation.
Validates later version implementations from R/W drives.
@param		aOldImpl The array to append to
@param		aNewImpl The item to append
@param		aLegitimateImpl Flag, indicating whether current implementation is secure
@return		The preferred implementation
@pre		This object is fully constructed.
*/
CRegistryData::CImplementationData* CRegistryData::SelectPreferredImplL(CImplementationData* aOldImpl,
																CImplementationData* aNewImpl,
																TBool& aLegitimateImpl,
																TBool aCheckIsNeeded) const
	{
	aLegitimateImpl = ETrue;

	TBool newIsRomOnly = aNewImpl->iImplInfo->RomOnly();
	TBool oldIsRomOnly = aOldImpl->iImplInfo->RomOnly();
		
	/* 	In addition to selecting the highest version of an 
		implementation this check takes care of the following special 
		cases:
		1. Ensure that a higher-versioned RAM-based implementation 
		   cannot override a ROM-based version
		2. Allows for the case where there are two versions of the 
		   same ROM-only implementation on ROM,	thus ensuring the 
		   higher of the two versions is used.
	*/
	if(newIsRomOnly && !oldIsRomOnly)
		{
		return aNewImpl;
		}
	else if((newIsRomOnly && oldIsRomOnly))
		{
		if(aNewImpl->iImplInfo->Version() > aOldImpl->iImplInfo->Version())
			{
			return aNewImpl;
			}
		else if(aNewImpl->iImplInfo->Version() == aOldImpl->iImplInfo->Version())
			{
			//any drive from Y-A has higher priority than Z drive
            //any drive with a letter alphabetically greater has higher priority
            if((aNewImpl->iImplInfo->Drive() != EDriveZ) &&
                (aOldImpl->iImplInfo->Drive() == EDriveZ || aNewImpl->iImplInfo->Drive() > aOldImpl->iImplInfo->Drive()))
                {
				return aNewImpl;
				}
			}
		}
	//all new implementations which are not flagged'RomOnly'
	else if(!newIsRomOnly && !oldIsRomOnly)
		{
		TBool newIsReadOnly = iCachedDriveInfo->DriveIsReadOnlyInternalL(aNewImpl->iParent->iParent->iParent->iDrive);
		TBool oldIsReadOnly = iCachedDriveInfo->DriveIsReadOnlyInternalL(aOldImpl->iParent->iParent->iParent->iDrive);
			
		if(aNewImpl->iImplInfo->Version() > aOldImpl->iImplInfo->Version())
			{
			//filter for implementations on R/W drive trying to upgrade implementations on R/O drive
			if((!newIsReadOnly && oldIsReadOnly) && aCheckIsNeeded)
				{						
				//test if a later implementation version on R/W drive has the same filename as the current one before upgrading.
				//If not,...
				if(aOldImpl->iParent->iParent->iDllEntry->GetName().CompareF(aNewImpl->iParent->iParent->iDllEntry->GetName()) == 0)
					{
					return aNewImpl;
					}
				else
					{
					aLegitimateImpl = EFalse; //not secure
					return aOldImpl;
					}
				}
			else
				{
				return aNewImpl;
				}
			}
		else if(aNewImpl->iImplInfo->Version() == aOldImpl->iImplInfo->Version())
			{
			//any drive from Y-A has higher priority than Z drive OR
			//any drive with a letter alphabetically greater has higher priority
			if((aNewImpl->iImplInfo->Drive() != EDriveZ) &&
                (aOldImpl->iImplInfo->Drive() == EDriveZ || aNewImpl->iImplInfo->Drive() > aOldImpl->iImplInfo->Drive()))                 
				{
				//filename check needs to performed on implementation from R/W drives, trying to 
				//upgrade implementations on R/O drive    
				if((!newIsReadOnly && oldIsReadOnly) && aCheckIsNeeded)
					{							
					//test if a later implementation version on R/W drive has the same filename as the current one before upgrading.
					//If not,...
					if(aOldImpl->iParent->iParent->iDllEntry->GetName().CompareF(aNewImpl->iParent->iParent->iDllEntry->GetName()) == 0)
						{
						return aNewImpl;
						}
					else
						{
						aLegitimateImpl = EFalse; //not secure
						return aOldImpl;
						}		
					}
				else
					{
					return aNewImpl;
					}
				}						
			}
		}
	return aOldImpl;
	}

/**
This functions checks if a given implementation already exists in
the indexes. If it does exist, determine if the given implementation
should replace the existing one or not.
@param  aIdxArray The container array of the interface to hold the implementation.
@param  aNewImpl The implementation to filter.
@param  aInsertMode whether aNewIMpl is a newcomer of the interface, or an 
		update of an existing implementation, or a older version of an
		existing implementation.
@param  aPosOfImplInArray return the index of aNewImpl in aIdxArray,
@param  aLegitimateImpl Flag,indicating whether current implementation is secure
@pre    This object is fully constructed.
@post   none
*/
void
CRegistryData::FilterForLatestLegitimateImplL(TImplContainerArray& aIdxArray,
											  CImplementationData* aNewImpl,
											  TInsertImplMode& aInsertMode,
											  TInt&  aPosOfImplInArray,
											  TBool& aLegitimateImpl,
											  TBool aCheckIsNeeded)
	{
	aInsertMode = EInsertImplUndefinedMode;
	aLegitimateImpl = ETrue;
	TImplStruct newImplStruct;
	newImplStruct.iCurrentImpl = aNewImpl;

	TInt idxPos = aIdxArray.FindInOrder(newImplStruct, TLinearOrder<TImplStruct> (TImplStruct::CompareImplStructUid));

	aPosOfImplInArray = idxPos;

	if(idxPos != KErrNotFound)
		{
		if(aNewImpl != SelectPreferredImplL(aIdxArray[idxPos].iCurrentImpl,
											aNewImpl,
											aLegitimateImpl,
											aCheckIsNeeded))
			{
			aInsertMode = EInsertImplAsUnusedImpl;
			}
		else
			{
			aInsertMode = EInsertImplAsUpgradeOfExistingImpl;
			} 
		}
	else
		{
		aInsertMode = EInsertImplAsNewcomerOfInterface;
		}
	}

void CRegistryData::ResetTInterfaceIndex(TAny* aObject)
	{
	TInterfaceIndex* index=reinterpret_cast<TInterfaceIndex*>(aObject);
	index->Reset();
	}
void CRegistryData::RemoveImplFromImplIndexCleanUp(TAny* aPtr)
    {
    TCleanupImplIndexEntry *aCleanup = (TCleanupImplIndexEntry*)aPtr;
    aCleanup->iRegistryData->RemoveImplByAddrFromImplIndex(aCleanup->iImplEntry);
    }

/**
This method retrieves the data for security checks from the iInterfaceImplIndex
@param          aImplPtr The new item to be checked
@param			aCheckIsNeeded	Boolean indicating, whether a filename check is needed
@pre            This object is fully constructed.
@post           aImplPtr has been checked and added to the index, or not, depending on the 
				outcome of the checks.
*/
TBool CRegistryData::InsertIntoIndexL(CImplementationData* aImplPtr, TBool aCheckIsNeeded)
	{
	TBool legitimateImpl = ETrue;

	TInterfaceIndex newIndexEl;
	TInterfaceIndex* newElPtr;
	//initialise
	newIndexEl.iInterfaceUid = aImplPtr->iParent->iInterfaceUid;
	CleanupStack::PushL(TCleanupItem(ResetTInterfaceIndex,&newIndexEl));
	
	TInt indexPos = iInterfaceImplIndex.FindInOrder(newIndexEl,TLinearOrder<TInterfaceIndex>(TInterfaceStruct::CompareInfUid));

	
	if(indexPos!=KErrNotFound)
		newElPtr = &(iInterfaceImplIndex[indexPos]);
	else		
		newElPtr = &newIndexEl;	

	// For each implementation make sure we only have the latest version
	TInt implIdxInContainerArray(KErrNotFound);
	TInsertImplMode insertMode;
	FilterForLatestLegitimateImplL(newElPtr->iImpData,
									aImplPtr,
									insertMode,
									implIdxInContainerArray,
									legitimateImpl,
									aCheckIsNeeded);
	
	InsertImplInIndexesL(insertMode, 
						 indexPos,
						 *newElPtr,
						 implIdxInContainerArray,
						 aImplPtr,
						 legitimateImpl);

	CleanupStack::Pop();

	if (insertMode == EInsertImplAsUpgradeOfExistingImpl)
		{
	    TUid ImplUid = aImplPtr->iImplInfo->ImplementationUid();
		iImplUpgradeCallBack.CallBack(ECallBackId_ImplUpgrade, &ImplUid);
		}

	return legitimateImpl;
	}

/** Handle inserting a CImplementationData in iInterfaceImplIndex and
iImplIndex.

@param aNewImpl the implementation to add to the indexes.
@param aInsertMode indicates whether the implementation is a newcover
			of the interface, or is an update of existing implementation
			or is an older version of an existing implementation.
@param aIfPosInInterfaceImplIndex is the index of the interface in
		iInterfaceImplIndex.
@param aNewIfIndexEl the TInterfaceIndex object containing the implementation.
@param aImplPosInContainerArray is the index of the implementation in
		the iImpData member of aNewIfIndexEl.
@param aLegitimateImpl indicate if the implementation passed security check or not.
@leave KErrNoMemory operation fails because the system is out of memory.
@leave KErrGeneral any non-specific programming error.
@leave KErrAlreadyExists the indexes already have an entry with the same
		Impl. UID and Interface UID.
*/
void CRegistryData::InsertImplInIndexesL(TInsertImplMode aInsertMode,
										TInt aIfPosInInterfaceImplIndex,
										TInterfaceIndex& aNewIfIndexEl,
										TInt aImplPosInContainerArray,
								   		CImplementationData* aNewImpl,
								   		TBool aLegitimateImpl)
	{
	if(aInsertMode == EInsertImplUndefinedMode)
		{
		// Will not happen because if FilterForLatestLegitimateImplL
		// does not leave then insertMode is set to one of the valid
		// values. If FilterForLatestLegitimateImplL leaves ecomserver exits.
		User::Leave(KErrGeneral);
		}

	// If not a legitimate implementation, aNewImpl will be deleted.
	// Do not add it to the two indexes.

	if (!aLegitimateImpl)
		{
		return;
		}

	TImplContainerArray& implContainerArray = aNewIfIndexEl.iImpData;

	if(aInsertMode == EInsertImplAsNewcomerOfInterface)
		{
		TImplStruct newImplStruct;
		newImplStruct.iCurrentImpl = aNewImpl;
		implContainerArray.InsertInOrderL(newImplStruct, TLinearOrder<TImplStruct> (TImplStruct::CompareImplStructUid));

#ifdef _DEBUG
		// on debug build, check for duplicated implementation UID
		// owned by different interfaces.

		TInt ii = iImplIndex.SpecificFindInOrder(
			aNewImpl,
			CImplementationData::CompareImplUidIgnoreIfUid,
			EArrayFindMode_Any);
		if (ii != KErrNotFound)
			{
			CImplementationData* otherImpl = iImplIndex[ii];
			TPtrC oldName = otherImpl->iImplInfo->DisplayName().Left(60);
			TPtrC newName = aNewImpl->iImplInfo->DisplayName().Left(60);
			__ECOM_TRACE5("** ECOM: error Impl UID %X, I/F UID %X DLL %S duplicating I/F %X DLL %S",\\ 
				aNewImpl->iImplInfo->ImplementationUid().iUid, \\
				aNewImpl->iParent->iInterfaceUid.iUid, &newName,\\
				otherImpl->iParent->iInterfaceUid.iUid, &oldName);
			}
#endif
		}
	else if(aInsertMode == EInsertImplAsUpgradeOfExistingImpl)
		{
		TImplContainer& implContainer = implContainerArray[aImplPosInContainerArray];
		CImplementationData* oldImpl =  implContainer.iCurrentImpl;
		implContainer.iUnusedImpls.AppendL(oldImpl);
		RemoveImplByAddrFromImplIndex(oldImpl); // ignore return code
		implContainer.iCurrentImpl = aNewImpl;

		// We are replacing existing impl with aNewImpl.  If existing  
		// corresponds to a ROM-based plug-in then aNewImpl is an 
		// update and qualifies as ROM-based under current policy.
		if(oldImpl->iImplInfo->RomBased())
			{
			aNewImpl->iImplInfo->SetRomBased(ETrue);
			}
		__ECOM_TRACE5("ECOM: Removed old implementation: UID:0x%X interfaceUID:0x%X version:%d on drive:%d \"%S\"", oldImpl->iImplInfo->ImplementationUid().iUid, oldImpl->iParent->iInterfaceUid.iUid, oldImpl->iImplInfo->Version(), (TInt)(oldImpl->iImplInfo->Drive()), &(oldImpl->iImplInfo->DisplayName()));
		__ECOM_TRACE5("ECOM: Loaded new implementation: UID:0x%X interfaceUID:0x%X version:%d on drive:%d \"%S\"", aNewImpl->iImplInfo->ImplementationUid().iUid, aNewImpl->iParent->iInterfaceUid.iUid, aNewImpl->iImplInfo->Version(), (TInt)(aNewImpl->iImplInfo->Drive()), &(aNewImpl->iImplInfo->DisplayName()));
		}
	else if (aInsertMode == EInsertImplAsUnusedImpl)
		{
		TImplContainer& implContainer = implContainerArray[aImplPosInContainerArray];
		implContainer.iUnusedImpls.AppendL(aNewImpl);

		CImplementationData* currImpl =  implContainer.iCurrentImpl;
		__ECOM_TRACE5("ECOM: Kept old implementation: UID:0x%X interfaceUID:0x%X version:%d on drive:%d \"%S\"", currImpl->iImplInfo->ImplementationUid().iUid, currImpl->iParent->iInterfaceUid.iUid, currImpl->iImplInfo->Version(), (TInt)(currImpl->iImplInfo->Drive()), &(currImpl->iImplInfo->DisplayName()));
		__ECOM_TRACE5("ECOM: Not loaded new implementation: UID:0x%X interfaceUID:0x%X version:%d on drive:%d \"%S\"", aNewImpl->iImplInfo->ImplementationUid().iUid, aNewImpl->iParent->iInterfaceUid.iUid, aNewImpl->iImplInfo->Version(), (TInt)(aNewImpl->iImplInfo->Drive()), &(aNewImpl->iImplInfo->DisplayName()));
		// We are not replacing existing with aNewImpl.  However, 
		// if aNewImpl corresponds to a ROM-based plug-in then the 
		// existing impl is an update and qualifies as ROM-based 
		// under current policy.
		if(aNewImpl->iImplInfo->RomBased())
			{
			currImpl->iImplInfo->SetRomBased(ETrue);
			}

		return; // the trailing steps not applicable to downgrade situation
		}

	User::LeaveIfError( InsertImplIntoImplIndex(aNewImpl) );
	
	if(aIfPosInInterfaceImplIndex==KErrNotFound)
		{		
        TCleanupImplIndexEntry aCleanup(this, aNewImpl);
        CleanupStack::PushL(TCleanupItem(RemoveImplFromImplIndexCleanUp,&aCleanup));
		iInterfaceImplIndex.InsertInOrderL(aNewIfIndexEl, TLinearOrder<TInterfaceIndex>(TInterfaceStruct::CompareInfUid));
		CleanupStack::Pop();
		}
	}

/**
This method takes removes all implementations of a plugin from the iInterfaceImplIndex
@param			aDllData Reference to a fully constructed CDllData object 
@pre			This object is constructed
@post			All implementations of the CDllData object, have been removed from the index
*/
void CRegistryData::RemoveFromIndexL(CDllData* aDllData) const
	{
	//remove 'old' implementations from iInterfaceImplIndex
	TInt counter=0;
	TImplStruct implStruct;
	TInt ifListCount=aDllData->iIfList->Count();
	while(counter < ifListCount)
		{
		CInterfaceData* interface = (*aDllData->iIfList)[counter];
		TInterfaceIndex index;
		index.iInterfaceUid = interface->iInterfaceUid;
		
		TInt indexPos = iInterfaceImplIndex.FindInOrder(index,TLinearOrder<TInterfaceIndex>(TInterfaceStruct::CompareInfUid));
        TImplContainerArray* impDataArray = NULL;
		TInt implCount=0;
		
		if(indexPos!=KErrNotFound)
			{
            impDataArray = &(iInterfaceImplIndex[indexPos].iImpData);
            implCount=interface->iImplementations->Count();
			for(TInt i=0; i<implCount; i++)
				{
				implStruct.iCurrentImpl = (*interface->iImplementations)[i];
				TInt impPos = impDataArray->FindInOrder(implStruct, TLinearOrder<CRegistryData::TImplStruct> (TImplStruct::CompareImplStructUid));
				if(impPos!=KErrNotFound)
					{
					TImplContainer& implContainer = (*impDataArray)[impPos];
					// remove from unused list if exists
					TInt count = implContainer.iUnusedImpls.Count();
					while(count > 0)
						{
						--count;
						if(implContainer.iUnusedImpls[count]->iParent->iParent == aDllData)
							{
							implContainer.iUnusedImpls.Remove(count);
							}
						}

					// update current entry
					if(implContainer.iCurrentImpl->iParent->iParent == aDllData)
						{
						// do not care about the return code.
						RemoveImplByAddrFromImplIndex(implContainer.iCurrentImpl);
						
						TInt implContainerUnusedImplCount=implContainer.iUnusedImpls.Count();
						// no unused impl's therefore no rollback and remove entry
						if(implContainerUnusedImplCount == 0)
							{
							implContainer.Reset();
							impDataArray->Remove(impPos);
							}
						else
							{
							// Rollback to implementation from unused array
							// copy first from unused array
							TInt selectedPos = 0;
							implContainer.iCurrentImpl = implContainer.iUnusedImpls[selectedPos];
							
							// now check if any others in list should be preferred
							for(count = 1;count < implContainerUnusedImplCount; count++)
								{
								// determine which implementation should be used.
								// no security check required as this will have already been done when impl was
								// first added to unused list.
								TBool dummLegitimateImpl;
								implContainer.iCurrentImpl = SelectPreferredImplL(implContainer.iCurrentImpl,
																implContainer.iUnusedImpls[count],
																dummLegitimateImpl,
																EFalse);
								if(implContainer.iCurrentImpl == implContainer.iUnusedImpls[count])
									{
									selectedPos = count;
									}
								}
							implContainer.iUnusedImpls.Remove(selectedPos);
							User::LeaveIfError(InsertImplIntoImplIndex(implContainer.iCurrentImpl));
#ifdef ECOM_TRACE
							{
							CImplementationData* p = implContainer.iCurrentImpl;
							__ECOM_TRACE5("ECOM: unused implementation restored UID:0x%X interfaceUID:0x%X version:%d on drive:%d \"%S\"", p->iImplInfo->ImplementationUid().iUid, p->iParent->iInterfaceUid.iUid, p->iImplInfo->Version(), (TInt)(p->iImplInfo->Drive()), &(p->iImplInfo->DisplayName()));
							}
#endif
							}
						}
					}
				//To make sure it is removed from the implIndex no matter what and that the return code is ignored.
				//The previous removal is still required so that a subsequent InsertImplIntoImplIndex is possible
				//for an implementation. e.g. an implementaion is on different drives and only one of htem was removed.
				// The other one should now make it to the iImplIndex through InsertImplIntoImplIndex. If it wasn't 
				// removed, before the insert operation, it will fail since both the implementations hav the same
				// impl UID.
				RemoveImplByAddrFromImplIndex((*interface->iImplementations)[i]);
				}
			if(impDataArray->Count() == 0)
				{
				iInterfaceImplIndex[indexPos].Reset();
				iInterfaceImplIndex.Remove(indexPos);
				}
			}
		counter++;
		}
	}

/**
This method inserts implementations into the index
@param          aDriveData Drive data in which we are searching the implementations
@pre            This object is fully constructed.
@post           all implemementations found, have been inserted into the index.
*/
void CRegistryData::AddImplDataL(CDriveData* aDriveData)
	{
 	TBool checkIsNeeded = EFalse;
 	TInt dllCount=aDriveData->iDllList->Count();
 	for(TInt dllCounter=0; dllCounter < dllCount;dllCounter++)
 		{
 		CDllData* aDllData = (*aDriveData->iDllList)[dllCounter];
 		TInt interfaceCount=aDllData->iIfList->Count();
 		for(TInt ifListCounter=0; ifListCounter < interfaceCount; ifListCounter++)
 			{
 			// For each interface structure
 			CInterfaceData* interface = (*aDllData->iIfList)[ifListCounter];
 			TInt implementationCount=interface->iImplementations->Count();
 			for(TInt impNum = 0; impNum < implementationCount; ++impNum)
 				{
 				CImplementationData* implData = (*interface->iImplementations)[impNum];
 				TInt retValue = InsertIntoIndexL(implData, checkIsNeeded);
 				//we know that during internalize, all implementations are legitimate,
 				//as the check was already performed during discovery. No check is 
 				//performed at this stage, therefore ignore return value, as it will always
 				//KErrNone
 				}
 			}	
 		}	
 	}


/** This method does a search by address and removes the specified entry from iImplIndex.

@param  aPtr is the entry to remove
@return True if aPtr is removed from iImplIndex. False if aPtr is not
        in iImplIndex, i.e. nothing is removed.
*/
TBool CRegistryData::RemoveImplByAddrFromImplIndex(CImplementationData* aPtr) const
    {
    TInt aIdx = iImplIndex.Find(aPtr);
    if (aIdx != KErrNotFound)
        {
        // This linear pointer search ensures a safe removal of the impl from iImplIndex so that it is free from a dead object.
    
        // The array does not own the pointer. Do not delete!
        iImplIndex.Remove(aIdx);
        return ETrue;
        }
    return EFalse;
    }

/** This method inserts the entry aNewImpl into iImplIndex.

@param aNewImpl the item to add to iImplIndex.
@return KErrNone aNewImpl is successfully added to the index.
		KErrAlreadyExists iImplIndex has an entry with the same 
		implementation UID and same parent Interface UID.
		KErrNoMemory fail to insert due to out of memory.
		Other system wide errors.
@pre    aNewImpl is fully constructed.
*/
TInt CRegistryData::InsertImplIntoImplIndex(CImplementationData* aNewImpl) const
	{
	TLinearOrder<CImplementationData> ImplUidComparator(CImplementationData::CompareImplUid);

	return iImplIndex.InsertInOrder(aNewImpl, ImplUidComparator);
	}

/** This method checks whether the language downgrade path has been changed.
If it is, save language downgrade path information and set related flag true.

@param aLanguageChanged the returned value to indicate language changed.
*/
void CRegistryData::LanguageChangedL(TBool& aLanguageChanged)
	{
	iLanguageChanged = RDowngradePath::HasChangedL(iFs);
	aLanguageChanged = iLanguageChanged;
	}

/** setter
If need to unset the callback, use a TCallBackWithArg constructed with no
arguments.
*/
void CRegistryData::SetImplUpgradeCallBack(const TCallBackWithArg& aCallBack)
	{
	iImplUpgradeCallBack = aCallBack;
	}

#ifdef __ECOM_SERVER_PERFORMANCE__
/**
This method calculates the drive, plugins, interfaces, implementations counts for the drive
type set
@param			aType The drive type for which the counts should be calculated 
@param			aCounts Holds the calculated counts
@pre			This object is constructed
*/
void CRegistryData::GetRegistryCountsL(TInt aType, RegistryCounts::TRegistryCounts& aCounts) const
	{
	aCounts.iDrives = 0;
	aCounts.iImplementations = 0;
	aCounts.iInterfaces = 0;
	aCounts.iDlls = 0;
	
	for(TInt driveIndex = 0; driveIndex< iRegistrations->Count(); driveIndex++)
		{
		CDriveData* drive = (*iRegistrations)[driveIndex];
		TBool isReadOnly = iCachedDriveInfo->DriveIsReadOnlyInternalL(drive->iDrive);

		if((aType == RegistryCounts::TRegistryCounts::EAll) ||
			(aType == RegistryCounts::TRegistryCounts::ERoInternal && isReadOnly) ||
			(aType == RegistryCounts::TRegistryCounts::ENonRoInternal && !isReadOnly))
			{
			aCounts.iDrives++;
			aCounts.iDlls += drive->iDllList->Count();
			for(TInt dllIndex = 0; dllIndex < drive->iDllList->Count(); dllIndex++)
				{
				CDllData* dllList = (*drive->iDllList)[dllIndex];
				aCounts.iInterfaces += dllList->iIfList->Count();
				for(TInt ifIndex = 0; ifIndex < dllList->iIfList->Count(); ifIndex++)
					{
					CInterfaceData* ifList = (*dllList->iIfList)[ifIndex];
					aCounts.iImplementations += ifList->iImplementations->Count();
					}
				}
			}
		}
	}

#endif // __ECOM_SERVER_PERFORMANCE__
