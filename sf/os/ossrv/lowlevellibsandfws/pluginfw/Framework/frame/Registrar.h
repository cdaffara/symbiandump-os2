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
// Contains the definition of the CRegistrar class.
// 
//

/**
 @internalComponent
 @file 
*/

#ifndef __REGISTRAR_H__
#define __REGISTRAR_H__

#include <f32file.h>
#include <barsread2.h> // RResourceReader
#include "DiscovererObserver.h"
#include "BackupNotifierObserver.h"
#include "RegistryData.h"
#include "StartupStateObserver.h"
#include "callback.h"
class CDiscoverer;
class CBackupNotifier;
class MRegistrarObserver;
class CEComCachedDriveInfo;

/**
	@internalComponent
	@since 7.0
	Responsible for managing Interface Implementaion Collection registration
	(adding and removing entries).
 */

class CRegistrar : public CBase, public MStartupStateObserver, private MDiscovererObserver, private MBackupNotifierObserver
{
public:

/** State declaration for CRegistrar */	
	enum TRegistrarState
	{
	/** Undefined state*/
	EReg_Undefined,
	/** Internalization/Externalization and Discovery in progress */
	EReg_StartupInProgess,
	/** Internalization/Externalization and Discovery complete */
	EReg_StartupComplete
	};
	
/**
	@fn				static CRegistrar* NewL(CRegistryData& aRegistry, MRegistrarObserver& aRegistrarObserver, RFs& aFs)
	Intended Usage	: Standardised safe construction which leaves nothing 
					on the cleanup stack.
	Error Condition	: Cannot fully construct because of memory limitations.	
	@leave			KErrNoMemory
	@since			7.0
	@param			aRegistry A reference to the registry data management class.
	@param			aRegistrarObserver A reference to the registrar observer class.
	@param			aFs A reference to an connected file server session.
	@return			CRegistrar* A pointer to the new class
	@pre 			None
	@post			CRegistrar is fully constructed, 
					and initialised.
 */
	
	static CRegistrar* NewL(CRegistryData& aRegistry, MRegistrarObserver& aRegistrarObserver, RFs& aFs);

/**
	@fn				~CRegistrar()
	Intended Usage	: Standard default d'tor
	Error Condition	: None
	@since			7.0
	@pre 			None
	@post			CRegistrar is totally destroyed
 */
	
	~CRegistrar();

/**
	@fn				Enable(TUid aImplementationUid)
	Intended Usage	:Allow the specified Interface Implementation to be used.
	Error Condition	: KErrEnableFailed The enable call failed.
	@since			7.0	:
	@param			aImplementationUid The Interface Implementation to enable.
	@return			KErrNone on success, KErrEnableFailed otherwise.
	@pre 			CRegistrar is fully constructed
	@post			The interface implementation identified by 
					the UID	is made available for use.
 */
	
	TInt Enable(TUid aImplementationUid);

/**
	@fn				Disable(TUid aImplementationUid)
	Intended Usage	:Stop the specified interface implementation 
					from being used.
	Error Condition	: KErrDisableFailed The disable call failed.
	@since			7.0	:
	@param			aImplementationUid The Interface Implementation to disable.
	@return			KErrNone on success, KErrDisableFailed otherwise.
	@pre 			CRegistrar is fully constructed
	@post			The interface implementation identified by 
					the UID	is no longer available for use.
 */	
	
	TInt Disable(TUid aImplementationUid);

/**
	@fn				Resume()
	Intended Usage	: Overload of the MBackupNotifierObserver method.
					Allow registration of new Interface Implementation Collections
					to begin again.
	Error Condition	: KErrResumeFailed The registration resume failed.
	@since			7.0	:
	@return			KErrNone on success, KErrResumeFailed otherwise.
	@pre 			CRegistrar is fully constructed
	@post			Registration suspension is cancelled.
 */	
	
	TInt Resume();

/**
	@fn				Suspend()
	Intended Usage	: Overload of the MBackupNotifierObserver method.
					Suspend registration of new Interface Implementation Collections.
	Error Condition	: KErrSuspendFailed The registration suspension failed.
	@since			7.0	:
	@return			KErrNone on success, KErrSuspendFailed otherwise.
	@pre 			CRegistrar is fully constructed
	@post			Registration is suspended.
 */	
	
	TInt Suspend();
	
/** 
	@fn				ProcessSSAEventL()
	Intended Usage	:Overload of MStartupStateObserver method.
					Used by CServerStratupMgr to notify the objects 
					of the start-up state changes.
	@param 			aKnownState the startup state passing into the CRegistar.
	@pre 			CRegistrar is fully constructed.
	@post 			CRegistrar is notified of the start-up state changes.
*/
	void ProcessSSAEventL(TStartupStateIdentifier aKnownState);

/** Install a callback function to receive notification of SWI status
changes. Note that the callback is passed along to CDsicoverer which
does the SWI monitoring.
@param aCallBack the callback function to install.
*/
	void InstallSwiEventCallBack(const TCallBackWithArg& aCallBack);

/** Install a callback function to receive notification of BUR status
changes. Note that the callback is passed along to CDsicoverer which
does the BUR monitoring.
@param aCallBack the callback function to install.
*/
	void InstallBurEventCallBack(const TCallBackWithArg& aCallBack);

private:
// MDiscovererObserver method implementations
/**
	@fn				DiscoveriesBegin()
	Intended Usage	:Overload of the MDiscovererObserver callback method.
					The cue that a registry update is about to occur.
	Error Condition	: None.
	@since			7.0	:
	@pre 			CRegistrar is fully constructed
	@post			The Registry data manager has been informed that its internal
					data is out of date.
 */
	
	void DiscoveriesBegin();

/**
	@fn				RegisterDiscoveryL(const TDriveName& aDrive,CPluginBase*& aDirEntry,TBool aAnyDllDiscovered)
	Intended Usage	:Overload of the MDiscovererObserver callback method.
					Adds the specified Interface Implementation Collection to the 
					registry.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory, KErrRegistrationFailed.
	@since			9.1	:
	@param			aDrive, the drive containing this entry
	@param			aDirEntry The directory entry to register.
	@param			aAnyDllDiscovered A flag to tell if any DLL is discovered in the drive.
	@pre 			CRegistrar is fully constructed
	@post			The Interface Implementation Collection identified by the directory entry 
					is registered.
 */
	
	void RegisterDiscoveryL(const TDriveName& aDrive,CPluginBase*& aDirEntry, TBool aAnyDllDiscovered);

/**
	@fn				UpdateRegistryL(TDriveUnit& driveUnit, TUid aDllUid, const TTime& aModified, TInt& aRegistryDriveIndex)
	Intended Usage	:Checks if the Registry must be updated or not.
	Error Condition	: Leaves with an error code.
	@leave			KErrNoMemory, KErrRegistrationFailed.
	@since			9.0	:
	@param			aDrive The drive the registry data has been found on
	@param			aDllUid The interface implementation collection data to add. 
					CRegistryData takes ownership of this object
					so it should not be on the CleanupStack.
	@param			aModified The date of the last modification of the Interface 
					Implementation Collection. 
	@param			aRegistryDriveIndex An output providing index within the registry data 
					for the branch that the registry entry was found within. 
	@return			ETrue if the Interface Implementation Collection already has a registry entry and
					must be updated.
					EFalse otherwise.
	@pre 			CRegistrar is fully constructed
	@post			No change to object..
 */
	TBool UpdateRegistryL(TDriveUnit& driveUnit, TUid aDllUid, const TTime& aModified, TInt& aRegistryDriveIndex);

/**
	@fn				DiscoveriesComplete(TBool aSuccessful,TPluginProcessingTypeIdentifier aProcessingType)
	Intended Usage	:Overload of the MDiscovererObserver callback method.
	Error Condition	: None.
	@since			7.0	:
	@param			aSuccessful ETrue indicates the scan completed successfully,
					EFalse, that the scan was only partial.
	@param			aProcessingType indicates the type of processing for plugins
					for ensuring that plugins are not processed multiple times
					during start-up phase
	@pre 			CRegistrar is fully constructed
	@post			The current registry data and the 
					internal access indexes have been rebuilt.
 */
	
	void DiscoveriesComplete(TBool aSuccessful, TPluginProcessingTypeIdentifier aProcessingType);
/**
	@fn 			SetDiscoveryFlagL(const TDriveUnit aDrive)
	Intended Usage	:Overload of the MDiscovererObserver callback method
					to notify the observer to find the drive has changed.
	Error Condition	: None.
	@param 			aDrive The identifier of the drive changed.
	@pre 			The MDiscovererObserver must be fully constructed
	@post			The flag is set.				 
*/	
	void SetDiscoveryFlagL(const TDriveUnit& aDriveUnit);
/**
	@fn				DriveRemovedL(TDriveUnit aDrive)
	Intended Usage	:Overload of the MDiscovererObserver callback method.
					The cue to signal the registry that 
					the registered Interface Implementation Collections
					stored upon the specified drive should no 
					longer available for use.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory
	@since			7.0	:
	@param			aDrive	The drive identifier.
	@pre 			CRegistrar is fully constructed.
	@post			The registered Interface Implementation Collections
					stored upon the specified drive are no 
					longer available for use.
 */
	
	void DriveRemovedL(TDriveUnit aDrive);

/**
	@fn				DriveReinstatedL(TDriveUnit aDrive)
	Intended Usage	:Overload of the MDiscovererObserver callback method.
					The cue to signal the registry that 
					the registered Interface Implementation Collections
					stored upon the specified drive are again
					available for use.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory, KErrRegistrationFailed.
	@since			7.0	:
	@param			aDrive	The drive identifier.
	@pre 			CRegistrar is fully constructed.
	@post			The registered Interface Implementation Collections
					stored upon the specified drive are again 
					made available for use.
 */
	
	void DriveReinstatedL(TDriveUnit aDrive);

/**
	@fn				NotifiedWithErrorCode(TInt aError)
	Intended Usage	:Overload of the MDiscovererObserver callback method.
					The notification that the notifier has been activated
					with an error status code. The observer should instruct
					the notification object how to proceed.
	Error Condition	: None.	
	@since			7.0	:
	@param			aError	The error value recieved by the notifier.
	@return			ETrue for ignore error, EFalse otherwise. 
	@pre 			CRegistrar is fully constructed.
	@post			The status code should be used by the observer to instruct 
					the notifier to cease recieving notifications or not.
 */
	
	TBool NotifiedWithErrorCode(TInt aError);
	
/*	
	@fn				LanguageChangedL(TBool& aLanguageChanged)
	Intended Usage	:Overload of the MDiscovererObserver callback method.
					It is used to notify the observer that the language switch notifier has been activated.
					The observer should instruct the notification object how to proceed.
	Error Condition	: Leaves with an error code.
	@leave  		KErrNoMemory
	@since			9.3	:
	@param			aLanguageChanged the flag returned to indicate whether language downgrade path is changed.
	@pre 			The MDiscovererObserver must be fully constructed, and
					capable of accepting discoveries for registration.
	@post			The CRegistryData will have its iLanguageChanged 
					flag set to True when language switch happens.

*/
	void LanguageChangedL(TBool& aLanguageChanged);

// Internal methods
/**
	@internalComponent
	@fn				CRegistrar(CRegistryData& aRegistry, MRegistrarObserver& aRegistrarObserver, RFs& aFs)
	Intended Usage	: Standardised default c'tor
	Error Condition	: None
	@since			7.0
	@param			aRegistry A reference to the registry data management class.
	@param			aRegistrarObserver A reference to the registrar observer class.
	@param			aFs A reference to an connected file server session.
	@pre 			None
	@post			CRegistrar is fully constructed
 */
	
	CRegistrar(CRegistryData& aRegistry, MRegistrarObserver& aRegistrarObserver, RFs& aFs);

/**
	@fn				void ConstructL()
	Intended Usage	: Standardised 2nd, (Initialisation) phase of two phase construction.
	Error Condition	: None
	@leave			None
	@since			7.0
	@pre 			CRegistrar is fully constructed.
	@post			CRegistrar is fully initialised.
 */
	
	void ConstructL();

/**
	@fn				ParseRegistrationDataL(CPluginBase*& aEntry, 
										   const TDriveUnit& aDrive, 
										   TBool aUpdate, 
										   TInt aRegistryDriveIndex)
	Intended Usage	: To determine if the directory entry for an Interface Implementation 
					Collection entry requires registration.
					Parsing the associated resource file information where necessary.
	Error Condition	: Parsing fails due to a lack of memory, or a parsing error.
	@leave  		KErrNoMemory
	@since			9.1
	@param			aEntry The directory entry data for the Interface Impplementation Collection.
	@param			aDrive The drive the registry data has been found on 
	@param			aUpdate A flag indicating if this is an update, or new entry.
	@param			aRegistryDriveIndex The index within the registry data 
					for the branch that the registry entry was found within. 
	@param			aDriveData Output value, which delivers the CDriveData object, needed for the
					CDllData object construction.
	@pre 			CRegistrar is fully constructed.
	@post			The Interface Implementation Collection entry has been processed appropriately.
*/
	
	void ParseRegistrationDataL(CPluginBase*& aEntry,
								const TDriveUnit& aDrive, 
								TBool aUpdate, 
								TInt aRegistryDriveIndex, 
								CRegistryData::CDriveData* aDriveData);

/**
	@fn				ParseL(CPluginBase*& aEntry, CRegistryData::CDllData& aDll)
	Intended Usage	: Parsing the resource file information associated with an
					Interface Implementation Collection for entry into the registry
					information.	
	Error Condition	: Parsing fails due to a lack of memory, or a parsing error.
	@leave  		KErrNoMemory
	@since			9.1
	@param			aEntry The directory entry data for the Interface Impplementation Collection.
	@param			aDll The CDllData information to fill
	@pre 			CRegistrar is fully constructed.
	@post			The resource file has been parsed and the data added to 'aDll'.
 */
	
	void ParseL(CPluginBase*& aEntry,CRegistryData::CDllData& aDll);
/**
	Intended Usage	: Get resource format version from the resource file
	@param			aReader The resource file reader.
	@param			aResourceFormatVersion The resource file format version 
	@pre 			CRegistrar is fully constructed.
 */
	void GetResourceFormatVersionL(RResourceReader& aReader, TInt& aResourceFormatVersion);
/**
	@fn 			State()
	Intended Usage	:Retrieve the object's current state.
	@return 		TCRegistarState the CRegistrar's current state.
	@pre			CRegistrar is fully constructed.
	@post 			the CRegistrar's current state has been returned.
*/
	TRegistrarState State() const;
	
/**
	@fn 			InitialiseEvent()
	Intended Usage	: change the state during the CRegistrar's construction.
	@post			: state is changed to EReg_StartupInProgess.
*/
	void InitialiseEvent();
	
/** 
	@fn				IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive)
	Intended Usage	Find if any Dll is registered in the drive unit.
	@param 			aDrive the identifier of the drive to find if any Dll is registered.
	@return			ETrue if any Dll is registered in the drive, otherwise EFalse.
	@pre			CRegistrar is fully constructed.
	@post			If any Dll registered in the drive is returned.
*/
	TBool IsAnyDllRegisteredWithDriveL(const TDriveUnit aDrive) const;
	

	
//Attributes
private:

	/** The reference to the registry data manager class */
	
	CRegistryData&		iRegistry;
	/** The owned discovery notification handling class */
	
	CDiscoverer*		iDiscoverer;
	/** The owned backup/restore notification handling class */
	
	CBackupNotifier*	iBackupNotifier;
	/** 
		The reference to the observer of the 
		Registrar for change notification. 
	*/
	
	MRegistrarObserver& iRegistrarObserver;
	/** A reference to a conncted file session */
	
	RFs&			iFs;
	
	/** The current state of this object */
	TRegistrarState iState;

	/** cached info on drivelist */
	CEComCachedDriveInfo*  iCachedDriveInfo;

	// Declare the test accessor as a friend
	friend class TRegistrar_StateAccessor;
};

#endif /* __REGISTRAR_H__ */

