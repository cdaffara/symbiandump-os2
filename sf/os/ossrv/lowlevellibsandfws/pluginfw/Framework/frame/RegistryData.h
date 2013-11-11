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
// Implements Registry data handling.
// Registry structure...
// iRegistrations
// -> CDriveData					(Drive number 1)
// -> CDllData						(Dll number 1)
// |	 -> CInterfaceData				(Interface number 1)
// |	|	 -> CImplementationData -> CImplementationInformation	(Implementation 1)
// |	|	 -> CImplementationData -> CImplementationInformation	(Implementation 2)
// |	 -> CInterfaceData			
// -> CDllData						(Dll number 2)
// -> CInterfaceData				(Interface number 1)
// -> CImplementationData -> CImplementationInformation	(Implementation 1)
// 
//

/**
 @code
 @endcode
*/

#ifndef __REGISTRYDATA_H__
#define __REGISTRYDATA_H__

#include <e32base.h>
#include <f32file.h>
#include <barsread2.h> // RResourceReader
#include <s32std.h>
#include <s32file.h>
#include <badesca.h>
#include "clientrequest.h"
#include <ecom/ecom.h>
#include <ecom/ecomerrorcodes.h>
#include <ecom/ecomresolverparams.h>
#include <ecom/implementationinformation.h>
#include <ecom/publicregistry.h>
#include "EComEntry.h"
#include "DiscovererObserver.h"
#include "EComPerformance.h"
#include "callback.h"

/** 
The index of the uid which is used for matching dlls 
@internalComponent
*/
const TInt KTEntryDllUniqueIdIndex = 2;

/**
Default path to Interface Implementation Collection resource files.
@internalComponent
*/
_LIT(KEComResourceFilePath,"\\resource\\plugins\\");

/** The granularities of two arrays, iInterfaceImplIndex and
iImplIndex, have huge impact on
discovery time on boot up. Larger granularity does not
always shorten discovery time. The two default granularities
below are chosen empirically to optimize discovery time
while as much as possible, not to waste too much memory.

@internalComponent
*/
const TInt KDefaultInterfaceImplIndexGranularity = 23;

/** @internalComponent */
const TInt KDefaultImplIndexGranularity = 29;

// Forward declaration
class CEComCachedDriveInfo;

//
// CRegistryData class

/**
@internalComponent
class CRegistryData.
This class manages the entire data of the registry.
*/
class CRegistryData : public CBase
	{
public:
	// Make the test State Accessor a friend
	friend class TRegistryData_StateAccessor;
	friend class CRegistrar;
	
	class CDriveData;
	class CDllData;
	class CInterfaceData;
	class CImplementationData;	

	// The implemented structure for the registry data 
	typedef CRegistryData::CImplementationData* CImplementationDataPtr;
	
	typedef RArray<CImplementationDataPtr> RImplDataArray;
	/** whether the implementation to be added to the registry data
	is a newcomer, or upgrade of an existing implementation, or
	downgrade of an existing implementation. 
 	*/
	enum TInsertImplMode
		{
		EInsertImplUndefinedMode,
		EInsertImplAsNewcomerOfInterface,
		EInsertImplAsUpgradeOfExistingImpl,
		EInsertImplAsUnusedImpl
		};
		
public:	
	static CRegistryData* NewL(RFs& aFs);

	/** This overload is provided for ECom performance test to find
	the optimum granularity settings.
	@see CEComImplIndexPerfTest
	*/
	static CRegistryData* NewL(RFs& aFs, 
		TInt aInterfaceImplIndexGranularity, TInt aImplIndexGranularity);

	virtual ~CRegistryData();
	void ListImplementationsL(TUid aInterfaceUid,
							  RImplDataArray& aImplementationData) const;
	TInt SetEnabledState(TUid aImplementationUid, TBool aState);
	void TemporaryUninstallL(const TDriveUnit& aDrive);
	void UndoTemporaryUninstallL(const TDriveUnit& aDrive);
	TBool IsRegisteredWithDate(TUid aDllUid, 
								const TTime& aModified, 
								TBool& aUpdate, 
								CDriveData* aDriveData);
	void AddDllDataL(const TDriveUnit& aDrive, TInt aFoundDriveIndex, CDllData* aDllData);
	void UpdateDllDataL(const TDriveUnit& aDrive,TInt aFoundDriveIndex, CDllData* aDllData);
	void DiscoveriesBeginning() const;
	void DiscoveriesCompleteL(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType, TBool& aHasRegistryChanged);
	TBool IndexValid() const;
	TInt GetImplementationDllInfoForServer(	const TCapabilitySet& aCapabilitySet,
											const TUid aImplUid,
											const TUid aInterfaceUid,
											TEntry& aEntry,
											CImplementationInformation*& aImplInfo,
											TBool& aIsOnRWDrive) const;
	TInt GetImplementationDllInfoForClientL(const TClientRequest& aClientRequest,
											const TUid aImplUid,
											const TUid aInterfaceUid,
											TEntry& aEntry,
											CImplementationInformation*& aImplInfo,
											TBool aSecurityCheckNeeded)const;
	TBool HasLanguageChanged() const;
	TBool InsertIntoIndexL(CImplementationData* aImplPtr, TBool aCheckIsNeeded);
	void RemoveFromIndexL(CDllData* aDllData) const;
	TBool IsAnyDllRegisteredWithDriveL(const TDriveUnit& aDrive)const;
	TInt FindDriveL(const TDriveUnit& aDrive, CDriveData*& aDriveData)const;
	void SetDiscoveryFlagL(const TDriveUnit& aDriveUnit);
	void LanguageChangedL(TBool& aLanguageChanged);
	void SetImplUpgradeCallBack(const TCallBackWithArg& aCallBack);
	
#ifdef __ECOM_SERVER_PERFORMANCE__
	void GetRegistryCountsL(TInt aType, RegistryCounts::TRegistryCounts& aCounts) const;
#endif // __ECOM_SERVER_PERFORMANCE__

private:
	typedef RPointerArray<CDriveData> TRegistration;

	/** 
	Implementation structure containing the current in use implementation and
	a list of unused implementations with the same uid held in the registry tree.
	The unused list is used to allow efficient rollback to previous impl in the event
	that the current impl is removed.
	*/	
	typedef struct TImplStruct
		{
		/** Constructor */
		TImplStruct();
		/** Resets iUnusedImpls*/
		inline void Reset();
		/** Comparer*/
		static TInt CompareImplStructUid(const TImplStruct& aEntry1,const TImplStruct& aEntry2);
		/** Comparer to search TImplStruct with just the Impl. UID */
		static TInt CompareUidAgainstImplStruct(const TUid* aUid, const TImplStruct& aEntry);
		/**
		Current Impl. This is not held in an array with the unused implementations because the array is not
		sorted. See comment for iUnusedImpls.
		*/
		CImplementationDataPtr iCurrentImpl;
		/**
		List of additional unused implementations. This list is not ordered as the processing required to sort
		the list when adding is more than that required to determine which should be used on rollback.
		Also this additional processing is not wanted during startup.
		*/
		RImplDataArray iUnusedImpls;
		}TImplContainer;
	
	typedef RArray<TImplContainer> TImplContainerArray;

	/** 
	Interface used by all implementation index
	*/
	typedef struct TInterfaceStruct
		{
		/** Constructor */
		TInterfaceStruct();
		/** Resets iImplementationInfo */
		inline void Reset();
		/** Comparer*/
		static TInt CompareInfUid(const TInterfaceStruct& aIndexEntry1,const TInterfaceStruct& aIndexEntry2);
		/** Unique Id of this interface */
		TUid	iInterfaceUid;
		/** List of the implementations of this interface */
		TImplContainerArray iImpData;
		}TInterfaceIndex;

private:
	explicit CRegistryData(RFs& aFs, TInt aInterfaceImplIndexGranularity, TInt aImplIndexGranularity);
	void ConstructL();

	TInt IndexedFind(TUid aInterfaceUid) const;
	static TBool MatchOnDrive(const CRegistryData::CDriveData& aIndexOne, 
							  const CRegistryData::CDriveData& aIndexTwo);

	TInt FindImplementation(const TUid aImplUid, const TUid aInterfaceUid, CImplementationData*& aImplData) const;
	CImplementationData* SelectDuplicatedImpl(const CImplementationData* aImpl1, const CImplementationData* aImpl2) const;

	void ValidateRegistryL();
	
	CImplementationData* SelectPreferredImplL(CImplementationData* aOldImpl,
											CImplementationData* aNewImpl,
											TBool& aLigitimateImpl,
											TBool aCheckIsNeeded) const;

	void FilterForLatestLegitimateImplL(TImplContainerArray& aIdxArray,
										CImplementationData* aNewImpl,
										TInsertImplMode& aInsertMode,
										TInt&  aPosOfImplInArray,
										TBool& aLigitimateImpl,
										TBool aCheckIsNeeded);
	void DeleteDllL(CDllData* aDllData) const;
	void AddImplDataL(CDriveData* aDriveData);

	void InsertImplInIndexesL(TInsertImplMode	aInsertMode,
							  TInt				aIfPosInInterfaceImplIndex,
							  TInterfaceIndex&  aNewIfIndexEl,
							  TInt				aImplPosInContainerArray,
							  CImplementationData* aNewImpl,
							  TBool aLegitimateImpl);
	static void ResetTInterfaceIndex(TAny* aObject);

	/** Remove the pointer from iImplIndex based on the impl address*/
    TBool RemoveImplByAddrFromImplIndex(CImplementationData* aPtr) const;

    /** Used to restore the impIndex sanity in case of a leave */
	static void RemoveImplFromImplIndexCleanUp(TAny* aPtr);
	
	/** Insert aNewImpl into iImplIndex. */
	TInt InsertImplIntoImplIndex(CImplementationData* aNewImpl) const;
	/** Store change in drive state - addition or removal*/
	void DriveChanged(const TDriveUnit& aDrive, TBool aDriveRemoved);

// Attributes / Properties
private:
	/** A reference to a connected file server instance */
	RFs& iFs;
	/** The flag to indicate that we are in the middle of discoveries and therefore
	the index list is probably out of date */
	mutable TBool iCurrentlyDiscovering;
	/** A boolean, indicating if the language downgrade path has changed */
	TBool iLanguageChanged;
	/** The entire registration data */ 
	TRegistration* iRegistrations;
	/** The index of all registered implementations sorted by interface UID.*/
	mutable RArray<TInterfaceIndex> iInterfaceImplIndex;
	/** Index of all implementations available to clients sorted by
	 Implementation UID.
	 Majority of clients do not specify the interface UID in 
	 their create requests. Thus ECOM needs this auxiliary index.
	 Note that if multiple entries have the same Impl. UID, they are
	 ordered by the interface UID.
	*/
	mutable RPointerArray<CImplementationData> iImplIndex;
	/**Bitmap indicating any drives removed */
	TInt iRemovedDrives;

	TCallBackWithArg iImplUpgradeCallBack;

public:
	/** System Drive cache to allow access by CEComServer*/
	TDriveNumber iSystemDrive;	

	/** cached info on drivelist */
	CEComCachedDriveInfo*  iCachedDriveInfo;	
	};  // End of CRegistryData definition
	

//
// CRegistryData::CImplementationData class
/**
This wrapper class is needed, to create a 2 way relationship between CInterfaceData and the public 
CImplementationInformation objects. It holds a reference to an implementation and a reference to its 
parent interface data. 
@internalComponent
*/
class CRegistryData::CImplementationData : public CBase
	{
public:
	static CImplementationData* NewLC(CInterfaceData* aParent);
	static CImplementationData* NewL(CInterfaceData* aParent,
											TUid	aUid,
											TInt	aVersion, 
											HBufC*  aName,
											HBufC8* aDataType,
											HBufC8* aOpaqueData,
											TDriveUnit aDrive,
											TBool aRomOnly,
											TBool aRomBased);
	static CImplementationData* NewL(CInterfaceData* aParent,
											TUid	aUid,
											TInt	aVersion, 
											HBufC*  aName,
											HBufC8* aDataType,
											HBufC8* aOpaqueData,
											TDriveUnit aDrive,
											TBool aRomOnly,
											TBool aRomBased,
											RExtendedInterfacesArray* aExtendedInterfaces);											
	~CImplementationData();
	void ExternalizeL(RWriteStream& aStore) const;
	void InternalizeL(RReadStream& aStore);
	static TInt CompareImplUid(const CImplementationData& aImpl1,const CImplementationData& aImpl2);
	static TInt CompareImplUidIgnoreIfUid(const CImplementationData& aImpl1,const CImplementationData& aImpl2);
	static TInt CompareUidAgainstImplData(const CImplementationData& aUid, const CImplementationData& aImplData);
private:
	CImplementationData(CInterfaceData* aParent);
	void ConstructL(TUid	aUid,
					TInt	aVersion, 
					HBufC*  aName,
					HBufC8* aDataType,
					HBufC8* aOpaqueData,
					TDriveUnit aDrive,
					TBool aRomOnly,
					TBool aRomBased);
	void ConstructL(TUid	aUid,
					TInt	aVersion, 
					HBufC*  aName,
					HBufC8* aDataType,
					HBufC8* aOpaqueData,
					TDriveUnit aDrive,
					TBool aRomOnly,
					TBool aRomBased,
					RExtendedInterfacesArray* aExtendedInterfaces);				
public:
	/** A pointer to the implementation */
	CImplementationInformation* iImplInfo;
	/**Pointer to the parent CInterfaceData */
	CInterfaceData* iParent;
	};

//
// CRegistryData::CInterfaceData class

/**
This class manages the entire data for an interface of a dll.
@internalComponent
*/
class CRegistryData::CInterfaceData : public CBase
	{
public:
	static CInterfaceData* NewLC(CDllData* aParent);
	static CInterfaceData* NewLC(TUid aInterfaceUid, CDllData* aParent);
	~CInterfaceData();
	void AddL(const CImplementationData* aImplementation);
	void SetInterfaceUid(TUid aInterfaceUid);
	void ExternalizeL(RWriteStream& aStore) const;
	void InternalizeL(RReadStream& aStore);
private:
	CInterfaceData(CDllData* aParent);
	CInterfaceData(TUid aInterfaceUid,CDllData* aParent);
	void ConstructL();
public:
	/** The interface UID */
	TUid		iInterfaceUid;
	/** The implementations related to this interface */
	RPointerArray<CImplementationData>* iImplementations;
	/** Pointer to the parent DLL data */
	CDllData* iParent;
	};	// End CRegistryData::CInterfaceData

/**
@internalComponent
*/
typedef RPointerArray<CRegistryData::CInterfaceData> RInterfaceList ;

//
// CRegistryData::CDllData class

/**
This class manages the entire data of a registered dll.
@internalComponent
*/
class CRegistryData::CDllData : public CBase
	{
public:
	static CDllData* NewLC( CDriveData* iParent);//
	static CDllData* NewLC(const TDesC& aDllName,const TTime& aDllModTime,const TUid& aSecondUid,const TUid& aThirdUid, CDriveData* iParent);
	~CDllData();
	void AddL(const CInterfaceData* aInterface);
	void ExternalizeL(RWriteStream& aStore) const;
	void InternalizeL(RReadStream& aStore);
	void PopulateAnEntry(TEntry& aEntry) const;
	TBool ProcessSecurityCheckL();
	TBool SaveSecurityInfoL();
	void SetResourceExtL(const TDesC& aExt);
private:
	CDllData( CDriveData* iParent);//
	void ConstructL();//
	/** See CDllData* NewLC */
	void ConstructL(const TDesC& aDllName,const TTime& aDllModTime,const TUid& aSecondUid,const TUid& aThirdUid);

public:
	/** Information on the DLL */
	CEComEntry* iDllEntry;
	/** The list of interfaces within the dll */
	RInterfaceList*	iIfList;
	/** Capability set of this dll */
	TCapabilitySet iCapSet;
	/** A flag to tell if the DLL's security has been previously checked or not.*/
	TBool iSecurityChecked;
	/** A pointer to the parent drive data. */
	CDriveData* iParent;
	/** The VendorId of the DLL. */
	TVendorId iVid;
	/** The plugin resource file extension  - not set for plugins residing on RO Internal drives*/
	HBufC* iRscFileExtension;
	}; // End CRegistryData::CDllData

/**
@internalComponent
*/
typedef RPointerArray<CRegistryData::CDllData> TDll ;

//
// CRegistryData::CDriveData class

/**
This class manages the entire data of a registered dll.
*/
class CRegistryData::CDriveData : public CBase
	{
public:
	static CDriveData* NewLC(TDriveUnit aDrive,CRegistryData* aParent);
	~CDriveData();
	void AddL(const CDllData* aDll);
	void ExternalizeL(RFs& aFs,const TDesC& aDatFileName);
	void InternalizeL(RFs& aFs,const TDesC& aDatFileName);
	TInt FindDllIndex(const TUid aDllUid) const;
private:
	void DoInternalizeL(RFs& aFs, const TDesC& aFileName);
	CDriveData(TDriveUnit aDrive,CRegistryData* aParent);
	void ConstructL();

public:
	/** A reference to the drive. */
	TDriveUnit	iDrive;
	/** The list of Interface Implementation Collections upon the drive */
	TDll*		iDllList;
	/** The registry data. */
    CRegistryData* iParent;
    /**indicate the drive has change */
    TBool iDriveChanged;
    /**indiacate the registry data on the drive has changed */
    TBool iRegistryChanged;

	}; // End CRegistryData::CDriveData

void CRegistryData::TInterfaceIndex::Reset()
 	{
 	TInt count = iImpData.Count();
 	while(count > 0)
 		{
 		--count;
 		iImpData[count].Reset();
 		}
 	iImpData.Reset();
   	}

void CRegistryData::TImplContainer::Reset()
 	{
 	iUnusedImpls.Reset();
   	}

    
    class TCleanupImplIndexEntry
        {
        public:
        TCleanupImplIndexEntry(CRegistryData* aRegData, CRegistryData::CImplementationData* aImpl)
        : iRegistryData(aRegData), iImplEntry(aImpl)   
        {}
        CRegistryData*         iRegistryData;
        CRegistryData::CImplementationData*   iImplEntry;
        };
#endif //__REGISTRYDATA_H__
